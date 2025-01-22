#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>


/*E' possibile migliorare questo codice usando delle struct, 
    ma per il momento utilizziamo semplicemente dei vettori separati per prendere un po' di pratica con C++*/
void read_atmData(std::vector<std::string> & dates,std::vector<std::string>& times,std::vector<double>& temperature,std::vector<double>& humidity,std::vector<double>& pression, double& total_time){
    std::ifstream file("Dati/2024-12-15_downld02.txt");
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file." << std::endl;
        return;
    }

    // Salta le righe dell'header
    std::string line;
    for (int i = 0; i < 3; ++i) {
        std::getline(file, line);
    }

    // Definisci l'intervallo temporale di interesse
    std::tm dataInizio = {};
    dataInizio.tm_year = 2024 - 1900; // Anno 2024
    dataInizio.tm_mon = 11; // Dicembre (0-based)
    dataInizio.tm_mday = 13; // Giorno 13
    dataInizio.tm_hour = 10; // Ora 09
    dataInizio.tm_min = 14; // Minuto 14

    time_t tempoInizio = mktime(&dataInizio);
    time_t tempoFine = tempoInizio + static_cast<time_t>(total_time);

    while (std::getline(file, line)) { //leggo una riga per volta
            std::istringstream iss(line); //flusso di parole nella riga
            std::string campo; //soggetto della colonna
            int indiceCampo = 0; //indice della colonna che sto leggendo

            std::string date;
            std::string time;
            double temp = 0.0;
            double hum = 0.0;
            double press = 0.0;

            while (iss >> campo) {
                switch (indiceCampo) { //in base alla posizione della mia  parola, deduco che tipo di oggetto sia 
                    case 0: // Date
                        date = campo;
                        break;
                    case 1: // Time
                        time = campo;
                        break;
                    case 2: // Temp Out
                        temp = std::stod(campo); //conversione da stringa di caratteri a double
                        break;
                    case 5: // Out Hum
                        hum = std::stod(campo);
                        break;
                    case 16: // Bar
                        press= std::stod(campo);
                        break;
                }
                indiceCampo++;
            }

            // Converti la data e l'ora in time_t
            std::tm dataCorrente = {};
            strptime((date + " " + time).c_str(), "%d/%m%Y %H:%M", &dataCorrente);
            time_t tempoCorrente = mktime(&dataCorrente);

            // Verifica se la data e l'ora rientrano nell'intervallo temporale
            if (tempoCorrente >= tempoInizio && tempoCorrente <= tempoFine) {
                // Aggiungi i dati ai vettori
                dates.push_back(date);
                times.push_back(time);
                temperature.push_back(temp);
                humidity.push_back(hum);
                pression.push_back(press);
            }
        }
    file.close();

}

#endif 