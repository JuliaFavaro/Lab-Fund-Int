#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

int main() {
    // Nome del file .dat
    std::string filename = "Set0408_1.dat";

    // Apri il file
    std::ifstream file(filename);

    // Verifica che il file sia stato aperto correttamente
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file " << filename << std::endl;
        return 1;
    }

    // Vettori per memorizzare i numeri dei canali e i tempi
    std::vector<int> canali;
    std::vector<double> tempi;

    int canale;
    double tempo;

    // Leggi i dati dal file
    while (file >> canale >> std::fixed >> std::setprecision(8) >> tempo) { //questo mantiene la precisione di almeno 8 cifre significative
        canali.push_back(canale);
        tempi.push_back(tempo);
    }

    // Chiudi il file
    file.close();

    //Let's correct for the random resets in the De0Nano

    std::vector<double> new_time;

    // Intervallo di tempo
    double dt = 0.01;
    // Contatore time_start inizialmente 0
    double time_start = 0.0;

    // Assicurati che ci siano almeno 2 elementi per poter fare il confronto
    if (tempi.size() > 1) {
        // Iniziare il ciclo da 1 perché compariamo con t[i-1]
        for (size_t i = 1; i < tempi.size(); ++i) {
            if (tempi[i-1] < tempi[i]) {
                new_time.push_back(tempi[i-1] + time_start);
            } else if (tempi[i-1] > tempi[i]) {
                new_time.push_back(tempi[i-1]+time_start);
                std::cout << "Reset, time_start" << time_start<< std::endl;
                time_start = time_start+tempi[i-1] + dt;
                new_time.push_back(time_start);
                i++;
            }
        }
    }

    // Stampa i nuovi tempi per verificare
    for (size_t j= 0; j < new_time.size(); ++j){
        std::cout << std::fixed << std::setprecision(8) <<"new_time[" << j << "] = " << new_time[j] << std::endl;
    }

    // Dividere i nuovi tempi in base ai canali 
    std::vector<double> t1, t2; 
    for (size_t i = 0; i < new_time.size(); ++i) {
         if (canali[i] == 1) { 
            t1.push_back(new_time[i]); 
            } 
        else if (canali[i] == 2) { 
            t2.push_back(new_time[i]); 
        } 
    }

    return 0;
}
