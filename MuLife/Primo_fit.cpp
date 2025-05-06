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
int calculateTimeDifferencesAndFillHistogram(    const std::vector<double>& startTimes,    const std::vector<double>& stopTimes,
    TH1F* histogram,    int& totalStopsUsed) {
    if (startTimes.size() < 2) {
        throw std::runtime_error("Il vettore startTimes deve contenere almeno due valori!");
    }

    if (stopTimes.empty()) {
        throw std::runtime_error("Il vettore stopTimes è vuoto!");
    }

    size_t stopIndex = 0;
    int matchedStartCount = 0;
    totalStopsUsed = 0; // Inizializza il contatore totale degli stop usati

    // Itera sugli START consecutivi
    for (size_t i = 0; i < startTimes.size() - 1; ++i) {
        double currentStart = startTimes[i];
        double nextStart = startTimes[i + 1];
        
        // Avanza il puntatore degli STOP al primo con timestamp maggiore del currentStart
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < currentStart) {
            ++stopIndex;
        }

        // Calcola le differenze di tempo per tutti gli STOP compresi tra currentStart e nextStart
        bool hasMatch = false;
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < nextStart) {
            double timeDifference = stopTimes[stopIndex] - currentStart;
            histogram->Fill(timeDifference);
            hasMatch = true; // Indica che questo start ha almeno una corrispondenza
            ++totalStopsUsed; // Incrementa il conteggio totale degli stop usati
            ++stopIndex;
        }

        if (hasMatch) {
            ++matchedStartCount;
        }
    }

    return matchedStartCount;
}

void addTimestamp(TCanvas* canvas, const std::string& timestamp, const std::string& duration) {
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.03);
    latex->DrawLatex(0.1, 0.86, timestamp.c_str()); // Posizionamento in alto a sinistra del grafico
    latex->DrawLatex(0.1, 0.81, duration.c_str()); // Posizionamento sotto il timestamp
}

// Funzione di fit: esponenziale + costante
Double_t exponentialPlusConstant(Double_t* x, Double_t* par) {
    Double_t t = x[0];
    Double_t amplitude = par[0];
    Double_t decayConstant = par[1];
    Double_t offset = par[2];
    return amplitude * exp(-t / decayConstant) ;
}

void fitHistogram(TH1F* histogram) {
    // Definizione della funzione di fit
    TF1* fitFunction = new TF1("fitFunction", exponentialPlusConstant, 700, 8000, 3);
    fitFunction->SetParNames("Amplitude", "Decay Constant", "Offset");

    // Inizializzazione dei parametri
    fitFunction->SetParameters(60, 2.2e-6); // Valori iniziali: ampiezza, costante di decadimento, offset
    fitFunction->SetParLimits(0, 0, 100);
    fitFunction->SetParLimits(1, 1e-6, 3e-6);

    // Forzare il fit con log-likelihood
    ROOT::Fit::Fitter fitter;
    histogram->Fit(fitFunction, "RLI+"); 

    // Stampa dei risultati del fit
    std::cout << "Risultati del fit:" << std::endl;
    std::cout << "Ampiezza: " << fitFunction->GetParameter(0) << " ± " << fitFunction->GetParError(0) << std::endl;
    std::cout << "Costante di decadimento: " << fitFunction->GetParameter(1) << " ± " << fitFunction->GetParError(1) << std::endl;
    //std::cout << "Offset: " << fitFunction->GetParameter(2) << " ± " << fitFunction->GetParError(2) << std::endl;

    // Disegna la funzione di fit sull'istogramma
    fitFunction->SetLineColor(kRed);
    fitFunction->SetLineWidth(2);
    fitFunction->Draw("same");
    
    gStyle->SetOptFit(1111);
}

int main() {
    const std::string filename = "Dati/output_20250424-mulife.txt";
    std::vector<double> startTimes;
    std::vector<double> stop1Times;
    std::vector<double> stop2Times;

    try {
        readFileAndPopulateArrays(filename, startTimes, stop1Times, stop2Times);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::string timestamp = "24.04.2025 18:30";
    std::string duration = "Durata: 120 ore";

    TCanvas* c1 = new TCanvas("c1", "Differenze di tempo tra start e stop", 1500, 1500);
    c1->SetGrid();

    // Crea gli istogrammi
    TH1F* histogram2 = new TH1F("time_differences_stop", "Time Differences (Start-Stop)", 50, 700, 8000); 

    double binWidth = histogram2->GetBinWidth(1); 
    std::cout << "La larghezza di un bin è: " << binWidth << " ns" << std::endl; //non deve essere più piccola della risoluzione di 5 ns per ovvi motivi

    // Variabili per il conteggio totale di stop usati
    int totalStopsUsed2 = 0;

    // Calcola le differenze temporali per stop1 e stop2
    int matchedStartCount2 = calculateTimeDifferencesAndFillHistogram(startTimes, stop2Times, histogram2, totalStopsUsed2);

    histogram2->SetLineColor(kBlue);
    histogram2->SetLineWidth(2);
    histogram2->Draw("E");
    histogram2->SetXTitle("Differenza di tempo (ns)");
    histogram2->SetYTitle("Conteggi");

    // Aggiungi timestamp e durata
    addTimestamp(c1, timestamp, duration);

    // Stampa i risultati
    std::cout << "Numero di start con almeno uno stop2 corrispondente: " << matchedStartCount2 << std::endl;
    std::cout << "Numero di stop2 totali utilizzati: " << totalStopsUsed2 << std::endl;
    // Effettua il fit sull'istogramma
    fitHistogram(histogram2);
    return 0;
}