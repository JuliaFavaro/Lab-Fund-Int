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
            analogMeasurements.push_back(analogValue);
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

int main() {
    const std::string filename = "Dati/output_20250506-mulife.txt";
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

    std::string timestamp = "06.05.2025 18:30";
    std::string duration = "Durata: 8.5 ore";
    /*
    TCanvas *c1 = new TCanvas("c1", "Differenze di tempo tra start e stop", 1500, 1500);
    c1->SetGrid();


    TH1F *histogram1 = new TH1F("time_differences_stop1", "Time Differences (Start-Stop1)", 1000, 0, 500);
    TH1F *histogram2 = new TH1F("time_differences_stop2", "Time Differences (Start-Stop2)", 1000, 0, 500);

    int matchedStartCount1 = calculateTimeDifferencesAndFillHistogram(startTimes, stop1Times, histogram1);
    int matchedStartCount2 = calculateTimeDifferencesAndFillHistogram(startTimes, stop2Times, histogram2);

    histogram1->SetLineColor(kRed);
    histogram1->SetLineWidth(2);
    histogram1->SetXTitle("Differenza di tempo (ns)");
    histogram1->SetYTitle("Conteggi");
    histogram1->Draw();

    histogram2->SetLineColor(kBlue);
    histogram2->SetLineWidth(2);
    histogram2->Draw("SAME");

    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(histogram1, "Start - Stop1", "l");
    legend->AddEntry(histogram2, "Start - Stop2", "l");
    legend->Draw();

    addTimestamp(c1, timestamp, duration);*/

    // Canvas per le misure analogiche
    TCanvas *c2 = new TCanvas("c2", "Distribuzione delle misure analogiche", 1500, 1500);
    c2->SetGrid();

    TH1F *analogHistogram = new TH1F("analog_measurements", "Distribuzione dei valori analogici", 100, 2, *std::max_element(analogMeasurements.begin(), analogMeasurements.end()));
    for (double value : analogMeasurements) {
        analogHistogram->Fill(value);
    }

    analogHistogram->SetLineColor(kGreen + 2);
    analogHistogram->SetLineWidth(2);
    analogHistogram->SetXTitle("Valore analogico");
    analogHistogram->SetYTitle("Conteggi");
    analogHistogram->Draw();

    addTimestamp(c2, timestamp, duration);

    //std::cout << "Numero di start con almeno uno stop1 corrispondente: " << matchedStartCount1 << std::endl;
    //std::cout << "Numero di start con almeno uno stop2 corrispondente: " << matchedStartCount2 << std::endl;
    std::cout << "Numero di misure analogiche: " << analogMeasurements.size() << std::endl;

    return 0;
}