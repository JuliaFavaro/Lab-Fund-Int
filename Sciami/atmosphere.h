#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct Dati { //memorizziamo in modo 
    std::string data;
    std::string ora;
    double tempOut;
    double umiditaOut;
    double pressione;
};

int main() {
    std::ifstream file("Dati/2024-12-15_downld02.txt");
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file." << std::endl;
        return 1;
    }

    // Salta le righe dell'header
    std::string riga;
    for (int i = 0; i < 3; ++i) {
        std::getline(file, riga);
    }

    std::vector<Dati> dati;

    while (std::getline(file, riga)) { // leggere l'intera riga dall'input standard (tastiera) e assegnarla a una variabile 
        std::istringstream iss(riga); //creazione di un flusso di stringhe basato sulla riga letta 
        Dati d;

        // Estrarre i valori delle colonne specifiche
        std::string campo;
        int indiceCampo = 0;
        while (iss >> campo) {
            switch (indiceCampo) {
                case 0: // Date
                    d.data = campo;
                    break;
                case 1: // Time
                    d.ora = campo;
                    break;
                case 5: // Temp Out
                    d.tempOut = std::stod(campo);
                    break;
                case 7: // Out Hum
                    d.umiditaOut = std::stod(campo);
                    break;
                case 18: // Bar
                    d.pressione = std::stod(campo);
                    break;
            }
            indiceCampo++;
        }

        dati.push_back(d);
    }

    file.close();

    // Visualizzare i dati estratti
    for (const auto& d : dati) {
        std::cout << "Data: " << d.data << ", Ora: " << d.ora << ", Temp Out: " << d.tempOut
                  << ", Umidità Out: " << d.umiditaOut << ", Pressione: " << d.pressione << std::endl;
    }

    return 0;
}
