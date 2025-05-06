#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept> // Per gestire gli errori

#include <TCanvas.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TLegend.h>

// Funzione per leggere il file e popolare gli array startTimes, stop1Times e stop2Times
void readFileAndPopulateArrays(const std::string& filename, std::vector<double>& startTimes, std::vector<double>& stop1Times, std::vector<double>& stop2Times) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Errore: impossibile aprire il file " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);

        double timestamp;
        int ch0, ch1, ch2;
        std::vector<int> digitalChannels(12);
        int placeholder0, placeholder1, placeholder2;
        int analogFlag, analogId;
        double analogValue;

        // Leggi i dati dal file
        if (!(iss >> timestamp >> digitalChannels[0] >> digitalChannels[1] >> digitalChannels[2]
                  >> digitalChannels[3] >> digitalChannels[4] >> digitalChannels[5]
                  >> digitalChannels[6] >> digitalChannels[7] >> digitalChannels[8]
                  >> digitalChannels[9] >> digitalChannels[10] >> digitalChannels[11]
                  >> placeholder0 >> placeholder1 >> placeholder2 >> analogFlag 
                  >> analogId >> analogValue)) {
            std::cerr << "Errore nel parsing della linea: " << line << std::endl;
            continue;
        }

        // Controlla i valori di ch0, ch1, ch2
        ch0 = digitalChannels[0];
        ch1 = digitalChannels[1];
        ch2 = digitalChannels[2];

        if (ch0 == 1 && ch1 == 0 && ch2 == 0) {
            startTimes.push_back(timestamp * 5); // Converti in nanosecondi
        } else if (ch0 == 0 && ch1 == 1 && ch2 == 0) {
            stop1Times.push_back(timestamp * 5); // Converti in nanosecondi
        } else if (ch0 == 0 && ch1 == 0 && ch2 == 1) {
            stop2Times.push_back(timestamp * 5); // Converti in nanosecondi
        }
    }

    file.close();
}

// Funzione per calcolare le differenze temporali e riempire l'istogramma
void calculateTimeDifferencesAndFillHistogram(
    const std::vector<double>& startTimes,
    const std::vector<double>& stopTimes,
    TH1F* histogram) {

    if (startTimes.empty()) {
        throw std::runtime_error("Il vettore startTimes è vuoto!");
    }

    if (stopTimes.empty()) {
        throw std::runtime_error("Il vettore stopTimes è vuoto!");
    }

    size_t stopIndex = 0;

    // Itera sugli START consecutivi
    for (size_t i = 0; i < startTimes.size() - 1; ++i) {
        double currentStart = startTimes[i];
        double nextStart = startTimes[i + 1];
        
        // Avanza il puntatore degli STOP al primo con timestamp maggiore del currentStart
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < currentStart) {
            ++stopIndex;
        }

        // Calcola le differenze di tempo per tutti gli STOP compresi tra currentStart e nextStart
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < nextStart) {
            double timeDifference = stopTimes[stopIndex] - currentStart;
            if (timeDifference>300e-9) {    
            histogram->Fill(timeDifference);
            ++stopIndex;
            }
        }
    }
}

int main() {
    // File di input
    const std::string file0424 = "Dati/output_20250424-mulife.txt";
    const std::string file0430 = "Dati/output_20250430-mulife.txt";

    // Vettori per i dati di ciascun file
    std::vector<double> startTimes0424, stop1Times0424, stop2Times0424;
    std::vector<double> startTimes0430, stop1Times0430, stop2Times0430;

    try {
        readFileAndPopulateArrays(file0424, startTimes0424, stop1Times0424, stop2Times0424);
        readFileAndPopulateArrays(file0430, startTimes0430, stop1Times0430, stop2Times0430);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Istogrammi per i file separati
    TH1F* histogram0424 = new TH1F("histogram0424", "#Delta t (Start-Stop)", 100, 0, 5000);
    TH1F* histogram0430 = new TH1F("histogram0430", "#Delta t (Start-Stop)", 100, 0, 5000);

    // Istogramma combinato
    TH1F* combinedHistogram = new TH1F("combinedHistogram", "#Delta t combinato (24/04 e 30/04)", 500, 0, 8000);

    // Riempi gli istogrammi
    calculateTimeDifferencesAndFillHistogram(startTimes0424, stop2Times0424, histogram0424);
    calculateTimeDifferencesAndFillHistogram(startTimes0430, stop1Times0430, histogram0430);
    calculateTimeDifferencesAndFillHistogram(startTimes0424, stop2Times0424, combinedHistogram);
    calculateTimeDifferencesAndFillHistogram(startTimes0430, stop1Times0430, combinedHistogram);

    // Canvas per istogrammi separati
    TCanvas* canvasSeparate = new TCanvas("canvasSeparate", "Istogrammi separati Delta t", 1500, 1500);
    canvasSeparate->SetGrid();
    
    double binWidth = combinedHistogram->GetBinWidth(1); 
    std::cout << "La larghezza di un bin è: " << binWidth << " ns" << std::endl; //non deve essere più piccola della risoluzione di 5 ns per ovvi motivi


    // Disegna gli istogrammi sovrapposti
    histogram0424->SetLineColor(kRed);
    histogram0424->SetLineWidth(2);
    histogram0424->SetXTitle("#Delta t (ns)");
    histogram0424->SetYTitle("Conteggi");
    histogram0424->Draw();

    histogram0430->SetLineColor(kBlue);
    histogram0430->SetLineWidth(2);
    histogram0430->Draw("SAME");

    // Legenda per il canvas separato
    TLegend* legendSeparate = new TLegend(0.7, 0.7, 0.9, 0.9);
    legendSeparate->AddEntry(histogram0424, "Start - Stop (24/04)", "l");
    legendSeparate->AddEntry(histogram0430, "Start - Stop (30/04)", "l");
    legendSeparate->Draw();

    canvasSeparate->Update();

    // Canvas per istogramma combinato
    TCanvas* canvasCombined = new TCanvas("canvasCombined", "Istogramma combinato Delta t", 1500, 1500);
    canvasCombined->SetGrid();

    // Disegna l'istogramma combinato
    combinedHistogram->SetLineColor(kGreen + 2);
    combinedHistogram->SetLineWidth(2);
    combinedHistogram->SetXTitle("#Delta t (ns)");
    combinedHistogram->SetYTitle("Conteggi");
    combinedHistogram->Draw();

    canvasCombined->Update();

    return 0;
}