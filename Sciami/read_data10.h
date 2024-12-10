#ifndef READ_DATA10_H
#define READ_DATA10_H

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <iomanip>

// Funzione per leggere e formattare i dati dal file da De0Nano
void readData(const std::string& filename, std::vector<int>& decimalNumbers, std::vector<double>& tempi) {
    // Apri il file
    std::ifstream file(filename);

    // Verifica che il file sia stato aperto correttamente
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file " << filename << std::endl;
        exit(1); // Uscita dal programma in caso di errore
    }

    int number;
    double tempo;

    // Leggi i dati dal file
    while (file >> number >> tempo) {  //risoluzione temporale di 5 ns
        decimalNumbers.push_back(number);
        tempi.push_back(tempo);
    }

    // Chiudi il file
    file.close();
}

// Funzione per determinare i canali attivati dato un numero decimale e per sistemare i tempi dopo il reset counter
std::vector<int> getActiveChannels(int decimalNumber) { //reset counter del clock a 20^30 ns-> 5 s
    std::vector<int> activeChannels;
    std::bitset<7> bits(decimalNumber); // Assumiamo un sistema con 7 canali

    for (int i = 0; i < 7; ++i) {
        if (bits[i]) {
            activeChannels.push_back(i);
        }
    }

    return activeChannels;
}

#endif // READ_DATA10_H
