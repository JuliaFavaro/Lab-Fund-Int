#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <TCanvas.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TLegend.h>

// Funzione per leggere il file e popolare gli array startTimes, stop1Times, stop2Times e analogMeasurements
void readFileAndSeparateMeasurements(const std::string &filename, std::vector<double> &startTimes, std::vector<double> &stop1Times, std::vector<double> &stop2Times, std::vector<double> &analogMeasurements) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Errore: impossibile aprire il file " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);

        double timestamp;
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

        // Controlla se la misura è analogica
        if (analogFlag != 0) {
            if (analogId==0){
                analogMeasurements.push_back(analogValue);
            }
        } else {
            // Se la misura è digitale, determina il tipo
            if (digitalChannels[0] == 1 && digitalChannels[1] == 0 && digitalChannels[2] == 0) {
                startTimes.push_back(timestamp * 5); // Converti in nanosecondi
            } else if (digitalChannels[0] == 0 && digitalChannels[1] == 1 && digitalChannels[2] == 0) {
                stop1Times.push_back(timestamp * 5); // Converti in nanosecondi
            } else if (digitalChannels[0] == 0 && digitalChannels[1] == 0 && digitalChannels[2] == 1) {
                stop2Times.push_back(timestamp * 5); // Converti in nanosecondi
            }
        }
    }

    file.close();
}

// Funzione per calcolare le differenze temporali e riempire l'istogramma
int calculateTimeDifferencesAndFillHistogram(const std::vector<double> &startTimes, const std::vector<double> &stopTimes, TH1F *histogram) {
    if (startTimes.size() < 2) {
        throw std::runtime_error("Il vettore startTimes deve contenere almeno due valori!");
    }

    if (stopTimes.empty()) {
        throw std::runtime_error("Il vettore stopTimes è vuoto!");
    }

    size_t stopIndex = 0;
    int matchedStartCount = 0;

    for (size_t i = 0; i < startTimes.size() - 1; ++i) {
        double currentStart = startTimes[i];
        double nextStart = startTimes[i + 1];

        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < currentStart) {
            ++stopIndex;
        }

        bool hasMatch = false;
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < nextStart) {
            double timeDifference = stopTimes[stopIndex] - currentStart;
            histogram->Fill(timeDifference);
            hasMatch = true;
            ++stopIndex;
        }

        if (hasMatch) {
            ++matchedStartCount;
        }
    }

    return matchedStartCount;
}

void addTimestamp(TCanvas *canvas, const std::string &timestamp, const std::string &duration) {
    TLatex *latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.03);
    latex->DrawLatex(0.1, 0.86, timestamp.c_str());
    latex->DrawLatex(0.1, 0.81, duration.c_str());
}

TF1* fit_gaus(TH1D* hist){
    // Fit: due gaussiane più fondo costante
    TF1* fGaus = new TF1("fGaus", "[0]*exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2)+[6]", 0, 3500);
    fGaus->SetParameters(500, 500, 100, 3500, 1500,200, 0);

    fGaus->SetParName(0, "A_1"); //ampiezza gaussiana
    fGaus->SetParName(1, "#mu_1"); 
    fGaus->SetParName(2, "#sigma_1");  
    fGaus->SetParName(3, "A_2"); //ampiezza gaussiana
    fGaus->SetParName(4, "#mu_2"); 
    fGaus->SetParName(5, "#sigma_2");
    fGaus->SetParName(6, "c");
    
    fGaus->SetParLimits(0, 400, 600);
    fGaus->SetParLimits(1, 450, 550);
    fGaus->SetParLimits(2, 75, 100);
    fGaus->SetParLimits(3, 3300, 3600);

    hist->Fit("fGaus","LM+","", 0, 3500);
    gStyle->SetOptFit(11111);
    return fGaus;
}

int main() {
    const std::string filename = "Dati/output_20250507-mumass.txt";
    std::vector<double> startTimes;
    std::vector<double> stop1Times;
    std::vector<double> stop2Times;
    std::vector<double> analogMeasurements;

    try {
        readFileAndSeparateMeasurements(filename, startTimes, stop1Times, stop2Times, analogMeasurements);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::string timestamp = "07.05.2025 12:30";
    std::string duration = "Durata: 26 ore";
    
    // Canvas per le misure analogiche
    TCanvas *c1 = new TCanvas("c1", "Distribuzione delle misure analogiche", 1500, 1500);
    c1->SetGrid();

    TH1D *analogHistogram = new TH1D("analog_measurements", "Distribuzione dei valori analogici", 100, 0, *std::max_element(analogMeasurements.begin(), analogMeasurements.end()));
    for (double value : analogMeasurements) {
        analogHistogram->Fill(value);
    }
    
    // Fit principale 
    TF1* fitFunction = fit_gaus(analogHistogram);

    analogHistogram->SetLineColor(kGreen + 2);
    analogHistogram->SetLineWidth(2);
    analogHistogram->SetXTitle("mV");
    analogHistogram->SetYTitle("Conteggi");
    analogHistogram->Draw("E");

    // Disegna il fit principale sul grafico
    fitFunction->SetLineColor(kRed);
    fitFunction->Draw("same");

    addTimestamp(c1, timestamp, duration);

    return 0;
}
