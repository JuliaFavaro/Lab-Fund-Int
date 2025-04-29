#ifndef READ_DATA10_H
#define READ_DATA10_H

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <iomanip>
#include <cmath>


// Funzione per leggere e formattare i dati dal file da De0Nano
void readData(const std::string& filename, std::vector<int>& decimalNumbers, std::vector<double>& counts) {
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
        counts.push_back(tempo);
    }

    // Chiudi il file
    file.close();
}

void reset_clock( std::vector<int>& decimalNumbers, std::vector<double>& counts,std::vector<std::vector<double>> & tempi_sistemati){
    int numero_reset=0;
    int inizio_file=0;
    int it=0;

    //trovo il primo reset
    while (true){
        if (decimalNumbers[it]==static_cast<int>(2147483648)){
            inizio_file=it;
            break;
        }
        ++it;
    }

    int i=0;
    for (i=inizio_file+1; i<decimalNumbers.size(); ++i){
        if (decimalNumbers[i]==static_cast<int>(2147483648)){
            ++numero_reset; //clock resettato
            ++i; //salti questo elemento
        }
        else{
            tempi_sistemati[0].push_back(counts[i]*5e-9+numero_reset*5.36870912); //converto i counts in secondi
            tempi_sistemati[1].push_back(counts[i]*5e-9+numero_reset*5.36870912); //converto i counts in secondi
       
        }
    }
}

#endif // READ_DATA10_H