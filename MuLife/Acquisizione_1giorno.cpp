#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept> // Per gestire gli errori

#include <TCanvas.h>
#include <TH1F.h>
#include <TStyle.h>

#include "read_data10.h"

// Funzione per calcolare le differenze temporali e riempire l'istogramma
void calculateTimeDifferencesAndFillHistogram(
    const std::vector<double>& startTimes,
    const std::vector<double>& stopTimes,
    TH1F* histogram) {
    
    if (startTimes.empty() || stopTimes.empty()) {
        throw std::runtime_error("I vettori startTimes o stopTimes sono vuoti!");
    }

    size_t stopIndex = 0;

    for (size_t i = 0; i < startTimes.size(); ++i) {
        double startTime = startTimes[i];

        // Itera sugli STOP successivi fino al prossimo START
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < startTime) {
            ++stopIndex;
        }

        while (stopIndex < stopTimes.size() && 
               (i + 1 == startTimes.size() || stopTimes[stopIndex] < startTimes[i + 1])) {
            
            double timeDifference = stopTimes[stopIndex] - startTime;
            histogram->Fill(timeDifference);
            ++stopIndex;
        }
    }
}

int acquisizione1604() {
    std::cout << "Inizio" << std::endl;

    std::string filename = "Dati/FIFOread_20250416-104849.txt";

    std::vector<int> decimalNumbers;
    std::vector<double> counts;
    readData(filename, decimalNumbers, counts);

    if (decimalNumbers.empty() || counts.empty()) {
        std::cerr << "Errore: i dati non sono stati letti correttamente dal file!" << std::endl;
        return -1;
    }

    // Vettore di vettori per memorizzare i tempi di arrivo per ciascun canale
    std::vector<std::vector<double>> channelTimes(2);

    // Sistema i tempi con i reset e dividi nei canali
    reset_clock(decimalNumbers, counts, channelTimes);

    if (channelTimes[0].empty()|| channelTimes[1].empty()) {
        std::cerr << "Errore: nessun dato valido nei canali START o STOP!" << std::endl;
        return -1;
    }

    std::cout << "Diviso in canali" << std::endl;

    // Calcola il tempo totale di acquisizione
    double time_start_total = *std::min_element(channelTimes[0].begin(), channelTimes[0].end());
    double time_end_total = *std::max_element(channelTimes[1].begin(), channelTimes[1].end());
    double total_time = time_end_total - time_start_total;

    std::cout << "Tempo totale in ore: " << total_time / 3600 << std::endl;
    /*
    // Creazione dell'istogramma
    TH1F* histogram = new TH1F("time_differences", "Distribuzione delle differenze temporali;Tempo di differenza (s);Conteggi", 100, 0, 10e-6); 

    // Calcola le differenze temporali e riempi l'istogramma
    try {
        calculateTimeDifferencesAndFillHistogram(channelTimes[0], channelTimes[1], histogram);
    } catch (const std::exception& e) {
        std::cerr << "Errore durante il calcolo delle differenze temporali: " << e.what() << std::endl;
        delete histogram;
        return -1;
    }

    // Disegna l'istogramma su un canvas
    TCanvas* canvas = new TCanvas("canvas", "Istogramma delle differenze temporali", 800, 600);
    histogram->Draw();

    std::cout << "Istogramma creato e salvato come time_differences_histogram.png" << std::endl;*/
    return 0;
}