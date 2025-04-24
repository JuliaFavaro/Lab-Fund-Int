# Julia Favaro's Project - Fundamental Interactions Laboratory 2024-2025

## Muon decay lab experience
Decode data stored in an output file produced by FIFOread.c and print a table in a file.
The output file contains (from left to right):
- timestamp: number of clock cycle since FPGA deassertion (perodical clock counter resets are resolved and removed)
- digital channels: logic value of the 12 digital channel (from ch 0 to ch 11)
- place holder: fixed to "0 0 0"
- analog flag: 1 if the analog value is valid in this entry (the reading was not performed at this timestamp)
- analog id: identifier of the analog reading
- analag value: digitalized value of the analog signal
Usage: ./decoder inputFile outputFile

### Folder organization:

### Objectives of the experience
1. Misurare con la maggiore precisione possibile la vita media dei muoni che si arrestano nel
bersaglio, realizzando un sistema per misurare il tempo che intercorre tra il loro arrivo e il
successivo decadimento.
2. Determinare con la maggiore precisione possibile la massa di tali particelle, sulla base della
distribuzione di energia rilasciata dai prodotti di decadimento.

#### Installation and usage
Most of the code was developed using the C++ language and the ROOT library. My partner Arianna and I obtained the datasets during our lab lectures. This is available on a private Google Drive because of GitHub space limitation. Documentation is embedded in the code. 

Some useful information on how to install ROOT:[ROOT on WSL](https://root-forum.cern.ch/t/complete-root-installation-instructions-for-wsl-ubuntu-18-04-on-windows-10/35148/3), [ROOT official installation page](https://root.cern/install/)

Disclaimer: I'm bilingual and in many cases you will find comments in both languages, as this was written on the spot. This way of thinking helped me develop my project faster.

### Authors and acknowledgment
Julia Favaro's bio: I'm a master's student from the University of Pisa (Italy). 
Supervisors for this lab experience: Prof. Giovanni Punzi and Prof. Francesco Forti.

### Support
For more explanation or any suggestions, write me at j.favaro@studenti.unipi.it .

### License
GNU General Public License v3.0 or later

### Project status
April 2025- May 2025
