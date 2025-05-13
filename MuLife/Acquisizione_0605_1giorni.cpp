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

// Funzione per leggere il file e popolare gli array startTimes, stopTimes, e analogMeasurements
void readFileAndSeparateMeasurements(const std::string &filename, std::vector<double> &startTimes, std::vector<double> &stop1Times, std::vector<double> &analogMeasurements) {
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
            // Se la misura è digitale, considera solo canali 0 e 1
            if (digitalChannels[0] == 1 && digitalChannels[1] == 0) {
                startTimes.push_back(timestamp * 5); // Converti in nanosecondi
            } else if (digitalChannels[0] == 0 && digitalChannels[1] == 1) {
                stop1Times.push_back(timestamp * 5); // Converti in nanosecondi
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
    latex->DrawLatex(0.1, 0.86, timestamp.c_str()); // Posizionamento in alto a sinistra del grafico
    latex->DrawLatex(0.1, 0.81, duration.c_str()); // Posizionamento sotto il timestamp
    latex->DrawLatex(0.1, 0.76, "Bin: 50"); // Posizionamento sotto il timestamp
}

// Funzione di fit: esponenziale + costante
Double_t exponentialPlusConstant(Double_t* x, Double_t* par) {
    Double_t t = x[0];
    Double_t amplitude = par[0];
    Double_t decayConstant = par[1];
    Double_t offset = par[2];
    return amplitude * exp(-t / decayConstant)+offset ;
}

void fitHistogram(TH1F* histogram) {
    /*
    // Definizione della funzione di fit
    TF1* f1 = new TF1("f1", "[0]+[1]*x", 300, 8000);
    histogram->Fit("f1","RL+N","",300,5000); 
    f1->SetParameters(2, -1); // Valori iniziali: ampiezza, costante di decadimento, offset
    double p0 = f1->GetParameter(0);
    double p1 = f1->GetParameter(1);*/
    TF1* fitFunction = new TF1("fitFunction", "[0]+[1]*exp(-x/[2])", 300, 8000);
    // Inizializzazione dei parametri
    fitFunction->SetParameters(2, 40, 2.2e-6); // Valori iniziali: ampiezza, costante di decadimento, offset

    histogram->Fit(fitFunction, "RLM+N","", 300, 5000); 

    fitFunction->SetParName(0, "A_1"); 
    fitFunction->SetParName(1, "#tau_1"); 
    fitFunction->SetParName(2, "c");

    // Disegna la funzione di fit sull'istogramma
    fitFunction->SetLineColor(kRed);
    fitFunction->SetLineWidth(2);
    fitFunction->Draw("same");
    
    gStyle->SetOptFit(1111);
}

int main() {
    const std::string filename = "Dati/output_20250506-mulife.txt";
    std::vector<double> startTimes;
    std::vector<double> stop1Times;
    std::vector<double> stop2Times;
    std::vector<double> analogMeasurements;

    try {
        readFileAndSeparateMeasurements(filename, startTimes, stop1Times, analogMeasurements);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::string timestamp = "06.05.2025 18:30";
    std::string duration = "Durata: 14.5 ore";

    TCanvas *c1 = new TCanvas("c1", "Differenze di tempo tra start e stop", 1500, 1500);
    c1->SetGrid();

    TH1F *histogram1 = new TH1F("time_differences_stop", "#Delta t tra Start e Stop", 50, 300, 7000);
    int matchedStartCount1 = calculateTimeDifferencesAndFillHistogram(startTimes, stop1Times, histogram1);

    double binWidth = histogram1->GetBinWidth(1); 
    std::cout << "La larghezza di un bin è: " << binWidth << " ns" << std::endl; //non deve essere più piccola della risoluzione di 5 ns per ovvi motivi

    histogram1->SetLineColor(kBlue);
    histogram1->SetLineWidth(2);
    histogram1->SetXTitle("#Delta t (ns)"); // Corretto il titolo
    histogram1->SetYTitle("Conteggi");
    histogram1->Draw();

    addTimestamp(c1, timestamp, duration);
    std::cout << "Numero di start con almeno uno stop corrispondente: " << matchedStartCount1 << std::endl;
    fitHistogram(histogram1); // Esegui il fit sull'istogramma

    return 0;
}