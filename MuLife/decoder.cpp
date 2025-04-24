#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <argp.h>

// const char *argp_program_version = "0";
const char *argp_program_bug_address = "<federico.lazzari@df.unipi.it>";
static char doc[] = "Decode a file produced by DE10-Nano in a human readable file.";
static char args_doc[] = "inputFile outputFile";
static struct argp_option options[] = {
  { "digital", 'd', 0, 0, "Fall back to old only-digital data format."},
  { 0 }
};

struct arguments {
  std::string inFile;
  std::string outFile;
  int reset_bit = 31;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = static_cast<struct arguments*>(state->input);
    switch (key) {
    case 'd': arguments->reset_bit = 19; break;
    case ARGP_KEY_ARG:
      switch (state->arg_num) {
        case 0: arguments->inFile = arg; break;
        case 1: arguments->outFile = arg; break;
        default: return ARGP_ERR_UNKNOWN;
      }
      break;
    default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char* argv[])
{

  struct arguments arguments;
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  // Open files
  std::ifstream inFile;
  inFile.open(arguments.inFile);
  if (!inFile.is_open()) {
    std::cerr << "Error while opening file: " << arguments.inFile << std::endl;
    return 2;
  }

  std::ofstream outFile;
  outFile.open(arguments.outFile);
  if (!outFile.is_open()) {
    std::cerr << "Error while opening file: " << arguments.outFile << std::endl;
    return 3;
  }

  uint32_t chWord = 0;
  uint32_t clkWord = 0;
  uint64_t loopCounter = 0; // PAY ATTENTION TO THE TYPE
  uint64_t clkCounter = 0; // PAY ATTENTION TO THE TYPE

  // Loop over the file
  while (inFile >> chWord) {
    inFile >> clkWord;

    const bool chWordReset = (chWord >> arguments.reset_bit) == 1;
    const bool clkWordReset = (clkWord >> 31) == 1;
    // Check if chWord or clkWord marks a FPGA counter reset
    if (chWordReset || clkWordReset) {
      // Throw an error if only one marks a FPGA counter reset
      if (!chWordReset || !clkWordReset) {
        std::cerr << "FIFO misaligned. chWord: " << chWord << ", clkWord: " << clkWord << std::endl;
        return 3;
      }

      // Throw an error if the loopCounter coded in clkWord do not match the program loopCounter
      if ((clkWord & 0x7FFFFFFF) != loopCounter) {
        std::cerr << "loopCounter misaligned. Loop in clkWord: " << (clkWord & 0x7FFFFFFF) << ", loopCounter: " << loopCounter << std::endl;
        return 4;
      }

      // Increase loopCounter and put clkWord to 0 (during reset clkWord store the FPGA loopCounter)
      loopCounter++;
      clkWord = 0;
    }

    // Calculate clkCounter taking in account the FPGA counter reset
    // ATTENTION if loopCounter is not a uint64_t an explicit cast is required
    clkCounter = (loopCounter << 30) + clkWord;

    if (chWord & (0xffffffff ^ (1 << arguments.reset_bit))) {
      // write clkCounter
      outFile << std::setw(15) << std::setfill(' ') << clkCounter << "  ";

      // write digital signals status
      for (int i=0; i<12; ++i) {
        outFile << " " << (chWord & 0x1);
        chWord = chWord >> 1;
      }

     // write padding bits
     outFile << "  ";
     for (int i=0; i<3; ++i) {
       outFile << " " << (chWord & 0x1);
       chWord = chWord >> 1;
     }

      // write analog acquisition status, id, value
      outFile << "   " << (chWord & 0x1);
      chWord = chWord >> 1;
      outFile << " " << ((chWord >> 12) & 0x7);
      outFile << " " << std::setw(4) << std::setfill(' ') << (chWord & 0xfff) << "\n";
    }
  }

  outFile.close();
  inFile.close();

  return 0;
}
