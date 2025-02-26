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
#include "flux_coincidences.h"

int coincidenze() {
    std::cout<<"Inizio"<<std::endl;

    // Nome del file di input
    std::string filename = "Dati/FIFOread_20241213-091357_7day.txt"; 

    // Vettori per memorizzare i numeri dei canali e i tempi del file
    std::vector<int> decimalNumbers;
    std::vector<double> counts;
    readData(filename, decimalNumbers, counts);

    // Sistema i tempi con i reset
    std::vector<int> Numbers;
    std::vector<double> tempi;
    reset_clock(decimalNumbers, counts, Numbers, tempi);

    // Calcola il tempo totale di acquisizione
    double time_start_total = *std::min_element(tempi.begin(), tempi.end());
    double time_end_total = *std::max_element(tempi.begin(), tempi.end());
    double total_time = time_end_total - time_start_total; //secondi
    std::cout << "Tempo totale in ore: " << total_time / (3600) << std::endl;

    // Vettore di vettori per memorizzare i tempi di arrivo per ciascun canale
    std::vector<std::vector<double>> channelTimes(7);

    // Processa ciascun numero decimale per determinare i canali attivati
    for (size_t i = 0; i < decimalNumbers.size(); ++i) {
        int Number = Numbers[i];
        std::vector<int> activeChannels = getActiveChannels(Number);

        // Aggiungi il tempo corrispondente ai canali attivati
        for (int channel : activeChannels) {
            channelTimes[channel].push_back(tempi[i]);
        }
    }
    std::cout<<"Diviso in canali"<<std::endl;

    /*Legenda dei telescopi: il canale 2 è il canale del Setup06, il canale 4
    è il Setup08, mentre il canale 7 è il telescopio in Stanza2004.*/
    std::cout<<"Coincidenze 06-08"<<std::endl;
    double minTime13=150e-9;
    double maxTime13=230e-9;
    std::vector<std::pair<double, double>> coincidences = findCoincidences(channelTimes[1],channelTimes[6],minTime13, maxTime13);

    double interval = 1000; //mi aspetto di avere un rate in mHz dividendo il valore medio per 1000
    int num_intervals=static_cast<int>(total_time / interval);
    histogram(interval, num_intervals, coincidences, "coincidence_histogram", "Coincidenze Setup04 e Setup06",kBlue + 2);

    histogram_exponential(coincidences,"Dt Setup06-04","Coincidenze Setup04 e Setup06",kBlue + 2);

    Rategraph(3600,static_cast<int>(total_time / interval),);

    return 0;
}
