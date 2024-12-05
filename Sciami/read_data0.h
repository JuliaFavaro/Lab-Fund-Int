#ifndef READ_DATA0_H
#define READ_DATA0_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
// Funzione per leggere e formattare i dati dal file da De0Nano
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

void correctTimes(const std::vector<double>& tempi, std::vector<double>& new_time, double dt) {
    double time_start = 0.0;

    if (tempi.size() > 1) {
        for (size_t i = 1; i < tempi.size(); ++i) {
            if (tempi[i-1] <= tempi[i]) {
                new_time.push_back(tempi[i-1] + time_start);
            } else {
                new_time.push_back(tempi[i-1] + time_start);
                time_start += tempi[i-1] + dt - tempi[i]; // per permettere di avere una dipendenza lineare dal tempo senza discontinuità
            }
        }
        new_time.push_back(tempi.back() + time_start);
    }
}


#endif // READ_DATA0_H
