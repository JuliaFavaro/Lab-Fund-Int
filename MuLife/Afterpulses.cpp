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
#include <TPad.h> 

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
int calculateTimeDifferencesAndFillHistogram(
    const std::vector<double>& startTimes,
    const std::vector<double>& stopTimes,
    TH1F* histogram,
    int& totalStopsUsed) { // Aggiunto parametro per contare il numero totale di stop

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
    latex->DrawLatex(0.1, 0.79, duration.c_str()); // Posizionamento sotto il timestamp
}
int main() {
    const std::string filename = "Dati/prova.txt";
    std::vector<double> startTimes;
    std::vector<double> stop1Times;
    std::vector<double> stop2Times;

    try {
        readFileAndPopulateArrays(filename, startTimes, stop1Times, stop2Times);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::string timestamp = "29.04.2025 15:00";
    std::string duration = "Durata: 2.5 ore";

    TCanvas* c1 = new TCanvas("c1", "Differenze di tempo tra start e stop", 1500, 1500);
    c1->SetGrid();

    // Crea gli istogrammi
    TH1F* histogram1 = new TH1F("time_differences_stop1", "Differenze di tempo tra start e stop, con start casuale", 100, 0, 4000);
    TH1F* histogram2 = new TH1F("START senza veto", "Differenze di tempo tra start e stop, con start casuale", 50, 0, 4000);

    // Variabili per il conteggio totale di stop usati
    int totalStopsUsed1 = 0;
    int totalStopsUsed2 = 0;

    // Calcola le differenze temporali per stop1 e stop2
    int matchedStartCount1 = calculateTimeDifferencesAndFillHistogram(startTimes, stop1Times, histogram1, totalStopsUsed1);
    int matchedStartCount2 = calculateTimeDifferencesAndFillHistogram(startTimes, stop2Times, histogram2, totalStopsUsed2);

    // Disegna gli istogrammi sovrapposti
    histogram2->SetLineColor(kRed);
    histogram2->SetLineWidth(2);
    histogram2->SetXTitle("#Delta t (ns)");
    histogram2->SetYTitle("Conteggi");
    histogram2->Draw();
    // Aggiungi timestamp e durata
    addTimestamp(c1, timestamp, duration);

    // Aggiungi un TPad per lo zoom
    TPad *zoomPad = new TPad("zoomPad", "Zoom", 0.6, 0.5, 0.9, 0.8); // Specifica la posizione e le dimensioni del TPad
    zoomPad->SetFillColor(0); // Sfondo trasparente
    zoomPad->Draw();
    zoomPad->cd(); // Cambia il contesto grafico al TPad

    // Creare un istogramma per lo zoom
    TH1F *zoomHistogram2 = (TH1F *)histogram2->Clone("zoom_time_differences_stop2");
    zoomHistogram2->SetTitle(""); // Rimuovi il titolo per lo zoom
    zoomHistogram2->GetXaxis()->SetRangeUser(0, 300); // Limita il range dell'asse X
    zoomHistogram2->SetLineColor(kRed);
    zoomHistogram2->Draw();

    // Torna alla canvas principale
    c1->cd();


    return 0;
}