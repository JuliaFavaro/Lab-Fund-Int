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
#include "atmosphere.h"

int acquisizione7giorni() {
    std::cout<<"Inizio"<<std::endl;

    // Nome del file di input
    std::string filename = "Dati/FIFOread_20241213-091357_7day.txt"; // Questo funziona solo se "Dati" si trova in una sottocartella della principale

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
    double interval = 3600; //1 ora
    int num_intervals=static_cast<int>(total_time / interval);
    Rategraph3(interval,num_intervals,channelTimes[1],channelTimes[3], channelTimes[6]);

    
    // Vettore di AtmData per memorizzare i dati atmosferici
    std::vector<AtmData> atmDataList;
    //effettivi dati atmosferici utilizzati per l'operazione di correlazione
    std::vector<BinnedData> atmDataBins;

    std::cout<<"Letto parametri atmosferici"<<std::endl;
    // Leggi i parametri atmosferici che corrispondono a quest'acquisizione.
    read_atmData(atmDataList, total_time);
     //print_atmData(atmDataList); //sanity check

    atmDataBins=interpolateAndBin(atmDataList, interval);
        //print_binnedData(atmDataBins);

    std::cout<<"Correlazione tra parametri atmosferici e rate "<<std::endl;
    plotCorrelationTemp(atmDataBins, interval, num_intervals, channelTimes[1],channelTimes[3], channelTimes[6]);
    //plotCorrTemp1d(atmDataBins, interval, num_intervals, channelTimes[1],channelTimes[3], channelTimes[6]);
    plotCorrelationPress(atmDataBins, interval, num_intervals, channelTimes[1],channelTimes[3], channelTimes[6]);
    TCanvas* canvasHum = new TCanvas("canvasHum", "Correlazione tra Umidità e Rates", 800, 600);
    canvasHum->cd();
    plotCorrelationHum(atmDataBins, interval, num_intervals, channelTimes[1], channelTimes[3], channelTimes[6]);
    canvasHum->Update(); // Aggiorna per visualizzare il grafico

    std::cout << "Distribuzione poissoniana" << std::endl;

    TCanvas* canvas06 = new TCanvas("canvas06", "Fit Poissoniano Telescopio 06", 800, 600);
    canvas06->cd();
    double rate06 = histogram_fitpoiss(10, static_cast<int>(total_time / 10), channelTimes[1], "Conteggi06","Telescopio 06",kBlue + 2);
    canvas06->Update();
    canvas06->SaveAs("Risultati/Poisson06_7d.png");

    TCanvas* canvas08 = new TCanvas("canvas08", "Fit Poissoniano Telescopio 08", 800, 600);
    canvas08->cd();
    double rate08 = histogram_fitpoiss(10, static_cast<int>(total_time / 10), channelTimes[3], "Conteggi08","Telescopio 08",kRed + 2);
    canvas08->Update();
    canvas08->SaveAs("Risultati/Poisson08_7d.png");

    TCanvas* canvas04 = new TCanvas("canvas04", "Fit Poissoniano Telescopio 04", 800, 600);
    canvas04->cd();
    double rate04=histogram_fitpoiss(10, static_cast<int>(total_time / 10), channelTimes[6], "Conteggi04", "Telescopio 04",kGreen + 2);
    canvas04->Update();
    canvas04->SaveAs("Risultati/Poisson04_7d.png");

    return 0;
}
