#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept> // Per gestire gli errori
#include <TFile.h>
#include <TTree.h>

// Funzione per leggere il file e popolare gli array startTimes, stop1Times e stop2Times
void readFileAndPopulateArrays(const std::string& filename, std::vector<double>& startTimes, std::vector<double>& stop1Times) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Errore: impossibile aprire il file " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);

        double timestamp;
        int ch0, ch1, ch2;
        std::vector<int> digitalChannels(12);
        int placeholder0, placeholder1, placeholder2;
        int analogFlag, analogId;
        double analogValue;

        // Leggi i dati dal file
        if (!(iss >> timestamp >> digitalChannels[0] >> digitalChannels[1] >> digitalChannels[2]
                  >> digitalChannels[3] >> digitalChannels[4] >> digitalChannels[5]
                  >> digitalChannels[6] >> digitalChannels[7] >> digitalChannels[8]
                  >> digitalChannels[9] >> digitalChannels[10] >> digitalChannels[11]
                  >> placeholder0 >> placeholder1 >> placeholder2 >> analogFlag 
                  >> analogId >> analogValue)) {
            std::cerr << "Errore nel parsing della linea: " << line << std::endl;
            continue;
        }

        // Controlla i valori di ch0, ch1, ch2
        ch0 = digitalChannels[0];
        ch1 = digitalChannels[1];
        ch2 = digitalChannels[2];

        if (ch0 == 1 && ch1 == 0 && ch2 == 0) {
            startTimes.push_back(timestamp * 5); // Converti in nanosecondi
        } else if (ch0 == 0 && ch1 == 1 && ch2 == 0) {
            stop1Times.push_back(timestamp * 5); // Converti in nanosecondi
        }
    }
    
    file.close();
}

void calculateTimeDifferencesAndSaveToTree(
    const std::vector<double>& startTimes,
    const std::vector<double>& stopTimes,
    TTree* tree) {

    if (startTimes.empty()) {
        throw std::runtime_error("Il vettore startTimes è vuoto!");
    }

    if (stopTimes.empty()) {
        throw std::runtime_error("Il vettore stopTimes è vuoto!");
    }

    double timeDifference;
    tree->Branch("timeDifference", &timeDifference, "timeDifference/D");

    size_t stopIndex = 0;
    int count = 0; // Contatore per debug

    // Itera sugli START consecutivi
    for (size_t i = 0; i < startTimes.size() - 1; ++i) {
        double currentStart = startTimes[i];
        double nextStart = startTimes[i + 1];

        // Avanza il puntatore degli STOP al primo con timestamp maggiore del currentStart
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < currentStart) {
            ++stopIndex;
        }

        // Calcola le differenze di tempo per tutti gli STOP compresi tra currentStart e nextStart
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < nextStart) {
            timeDifference = stopTimes[stopIndex] - currentStart;
            if (timeDifference > 300e-9) {
                tree->Fill();
                ++count;
            }
            ++stopIndex;
        }
    }
    std::cout << "Numero totale di differenze salvate: " << count << std::endl; // Debug finale

}

int main() {
    // File di input
    const std::string file0424 = "Dati/output_20250506-mulife.txt";
    const std::string file0430 ="Dati/output_20250507-mumass.txt";

    // Vettori per i dati di ciascun file
    std::vector<double> startTimes0424, stop1Times0424;
    std::vector<double> startTimes0430, stop1Times0430;

    try {
        readFileAndPopulateArrays(file0424, startTimes0424, stop1Times0424);
        readFileAndPopulateArrays(file0430, startTimes0430, stop1Times0430);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Creazione del file ROOT e del TTree
    TFile* outputFile = new TFile("time_differences.root", "RECREATE");
    TTree* tree = new TTree("TimeDifferences", "Differenze temporali Start-Stop");

    // Calcolo delle differenze temporali e salvataggio nel TTree
    calculateTimeDifferencesAndSaveToTree(startTimes0424, stop1Times0424, tree);
    calculateTimeDifferencesAndSaveToTree(startTimes0430, stop1Times0430, tree);

    // Scrittura del file ROOT
    outputFile->cd(); // Cambia il contesto al file ROOT
    tree->Write();
    outputFile->Close();

    std::cout << "File ROOT salvato con successo!" << std::endl;
    return 0;
}