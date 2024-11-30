#ifndef READ_DATA_H
#define READ_DATA_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
// Funzione per leggere e formattare i dati dal file
void readData(const std::string& filename, std::vector<int>& canali, std::vector<double>& tempi) {
    // Apri il file
    std::ifstream file(filename);

    // Verifica che il file sia stato aperto correttamente
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file " << filename << std::endl;
        exit(1); // Uscita dal programma in caso di errore
    }

    int canale;
    double tempo;

    // Leggi i dati dal file
    while (file >> canale >> std::fixed >> std::setprecision(8) >> tempo) { // mantiene la precisione di almeno 8 cifre significative
        canali.push_back(canale);
        tempi.push_back(tempo);
    }

    // Chiudi il file
    file.close();
}
#endif // READ_DATA_H
