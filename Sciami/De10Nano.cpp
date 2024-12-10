#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>

#include "read_data10.h"
#include "flux_graphs_10.h"

int main() {
    // Nome del file di input
    std::string filename = "Dati/FIFOread_20241203-172807.txt"; //questo funziona solo se Dati si trova in una sottocartella della principale

    // Vettori per memorizzare i numeri dei canali e i tempi
    std::vector<int> decimalNumbers;
    std::vector<double> tempi;

    // Leggi i dati dal file
    readData(filename, decimalNumbers, tempi);
    
    // Tempo totale di acquisizione
    double time_start_total = *std::min_element(tempi.begin(), tempi.end());
    double time_end_total = *std::max_element(tempi.begin(), tempi.end());
    double total_time = time_end_total - time_start_total;
    std::cout<<"Tempo totale in giorni "<<total_time/(3600*24)<<std::endl;

    // Vettore di vettori per memorizzare i tempi di arrivo per ciascun canale
    std::vector<std::vector<double>> channelTimes(7);

    // Processa ciascun numero decimale per determinare i canali attivati
    for (size_t i = 0; i < decimalNumbers.size(); ++i) {
        int decimalNumber = decimalNumbers[i];
        std::vector<int> activeChannels = getActiveChannels(decimalNumber);

        // Aggiungi il tempo corrispondente ai canali attivati
        for (int channel : activeChannels) {
            channelTimes[channel].push_back(tempi[i]);
        }
    }

    /*Legenda dei telescopi: il canale 2 è il canale del Setup06, il canale 5
    è il Setup08, mentre il canale 6 è il telescopio in Stanza2004.*/
    // Intervallo di 10s
    double interval = 10;
    return 0;
}
