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

int acquisizionebreve() {
    std::cout<<"Inizio"<<std::endl;

    // Nome del file di input
    std::string filename = "Dati/FIFOread_20241203-172807.txt"; // Questo funziona solo se "Dati" si trova in una sottocartella della principale

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
    double total_time = time_end_total - time_start_total;
    std::cout << "Tempo totale in giorni: " << total_time / (3600 * 24) << std::endl;

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

    /*Legenda dei telescopi: il canale 2 è il canale del Setup06, il canale 5
    è il Setup08, mentre il canale 6 è il telescopio in Stanza2004.*/
    double interval = 30*60; //30 minuti
    int num_intervals=static_cast<int>(total_time / interval);
    Rategraph3(interval,num_intervals,channelTimes[1],channelTimes[3], channelTimes[6]);

    //1-2-6: 1: 3-2-11
    //2-3-6: 2-0-11
    std::cout<<"Calcolato la variazione di flusso nel tempo per i vari set "<<std::endl;

    /*Consideriamo la variazione dell'efficienza nel tempo del Setup06:
    Doppie 2 e 3: CH0, Doppie 1 e 2 CH1, Triple: CH2*/

    std::vector<double> counts23 = channelTimes[0];
    std::vector<double> counts12 = channelTimes[1];
    std::vector<double> triple_06 = channelTimes[2];
    
    double interval_eff=3600;
    int num_intervals_eff=static_cast<int>(total_time/interval_eff);

    efficiency_set06(interval_eff, num_intervals_eff, counts23, counts12, triple_06);
    std::cout<<"Calcolata la variazione dell'efficienza Setup06 "<<std::endl;


    /*Consideriamo ora la distribuzione dei tempi di arrivo per un telescopio*/
    
    double interval_counts=10;
    int num_intervals_counts=static_cast<int>(total_time / interval_counts);
    double guess_rate;

    guess_rate=histogram_fitpoiss(interval_counts, num_intervals_counts,channelTimes[6]);

    histogram_fitexponential(channelTimes[6], guess_rate);
    std::cout<<"Calcolata distribuzione dei tempi di arrivo per i singoli telescopi"<<std::endl;

    return 0;
}
