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
#include <TArrow.h> // Per aggiungere la freccia

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
int calculateTimeDifferencesAndFillHistogram(    const std::vector<double>& startTimes,    const std::vector<double>& stopTimes,    TH1F* histogram,
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

// Funzione per calcolare la distribuzione del numero di stop per start
void calculateStopDistribution(
    const std::vector<double>& startTimes,
    const std::vector<double>& stopTimes,
    TH1F* stopDistributionHistogram) {

    if (startTimes.size() < 2) {
        throw std::runtime_error("Il vettore startTimes deve contenere almeno due valori!");
    }

    if (stopTimes.empty()) {
        throw std::runtime_error("Il vettore stopTimes è vuoto!");
    }

    size_t stopIndex = 0;

    // Itera sugli START consecutivi
    for (size_t i = 0; i < startTimes.size() - 1; ++i) {
        double currentStart = startTimes[i];
        double nextStart = startTimes[i + 1];
        
        // Conta quanti stop sono associati a questo start
        int stopCount = 0;
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < currentStart) {
            ++stopIndex;
        }

        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < nextStart) {
            ++stopCount;
            ++stopIndex;
        }

        // Riempie l'istogramma con il numero di stop per questo start
        stopDistributionHistogram->Fill(stopCount);
    }
}

int main() {
    const std::string filename = "Dati/output_20250424-mulife.txt";
    std::vector<double> startTimes;
    std::vector<double> stop1Times;
    std::vector<double> stop2Times;
    
    const std::string filename2 = "Dati/output_20250506-mulife.txt";
    std::vector<double> start3Times;
    std::vector<double> stop3Times;

    try {
        readFileAndPopulateArrays(filename, startTimes, stop1Times, stop2Times);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    try {
        readFileAndPopulateArrays(filename2, start3Times, stop3Times, stop3Times);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    TCanvas* c1 = new TCanvas("c1", "Differenze di tempo tra start e stop", 1500, 1500);
    c1->Divide(3, 2); // Divide il canvas in 4 sezioni
    c1->SetGrid();

    // Crea gli istogrammi
    TH1F* histogram1 = new TH1F("time_differences_stop1", "#Delta t usando Stop 1", 200, 0, 8000);
    TH1F* histogram2 = new TH1F("time_differences_stop2", "#Delta t usando Stop 2", 200, 0, 8000);
    TH1F* histogram3= new TH1F("time_differences_stop3", "#Delta t usando Stop 3", 200, 0, 8000);  
    TH1F* stopDistribution1 = new TH1F("stop_distribution1", "Distribuzione numero di Stop1 per Start", 5, 0, 5);
    TH1F* stopDistribution2 = new TH1F("stop_distribution2", "Distribuzione numero di Stop2 per Start", 5, 0, 5);
    TH1F* stopDistribution3 = new TH1F("stop_distribution3", "Distribuzione numero di Stop3 per Start", 5, 0, 5);

    double binWidth = histogram2->GetBinWidth(1); 
    std::cout << "La larghezza di un bin è: " << binWidth << " ns" << std::endl;

    // Variabili per il conteggio totale di stop usati
    int totalStopsUsed1 = 0;
    int totalStopsUsed2 = 0;
    int totalStopsUsed3 = 0;

    // Calcola le differenze temporali per stop1 e stop2
    int matchedStartCount1 = calculateTimeDifferencesAndFillHistogram(startTimes, stop1Times, histogram1, totalStopsUsed1);
    int matchedStartCount2 = calculateTimeDifferencesAndFillHistogram(startTimes, stop2Times, histogram2, totalStopsUsed2);
    int matchedStartCount3 = calculateTimeDifferencesAndFillHistogram(start3Times, stop3Times, histogram3, totalStopsUsed3);

    // Calcola la distribuzione del numero di stop per start
    calculateStopDistribution(startTimes, stop1Times, stopDistribution1);
    calculateStopDistribution(startTimes, stop2Times, stopDistribution2);
    calculateStopDistribution(start3Times, stop3Times, stopDistribution3);

    // Disegna gli istogrammi
    c1->cd(1);
    histogram1->SetLineColor(kBlue);
    histogram1->SetLineWidth(2);
    histogram1->SetXTitle("#Delta t (ns)");
    histogram1->SetYTitle("Conteggi");
    histogram1->SetMaximum(30);
    histogram1->Draw();
    gPad->SetLogy(); // Imposta la scala logaritmica sull'asse Y per questa sezione

    c1->cd(2);
    histogram2->SetLineColor(kRed);
    histogram2->SetLineWidth(2);
    histogram2->SetXTitle("#Delta t (ns)");
    histogram2->SetYTitle("Conteggi");
    histogram2->Draw();
    gPad->SetLogy(); // Imposta la scala logaritmica sull'asse Y per questa sezione
    
    c1->cd(3);
    histogram3->SetLineColor(kGreen);
    histogram3->SetLineWidth(2);
    histogram3->SetXTitle("#Delta t (ns)");
    histogram3->SetYTitle("Conteggi");
    histogram3->Draw();
    gPad->SetLogy(); // Imposta la scala logaritmica sull'asse Y per questa sezione

    c1->cd(4);
    stopDistribution1->SetLineColor(kBlue);
    stopDistribution1->SetLineWidth(2);
    stopDistribution1->SetXTitle("Numero di Stop1 per Start");
    stopDistribution1->SetYTitle("Conteggi");
    stopDistribution1->Draw();
    
    // Crea uno zoom su bin 1, 2 e 3 come TPad
    TPad *zoomPad1 = new TPad("zoomPad1", "Zoom sui bin 1-3", 0.5, 0.5, 0.9, 0.9); // Posizione del TPad
    zoomPad1->SetFillColor(0); // Colore di sfondo trasparente
    zoomPad1->Draw();
    zoomPad1->cd(); // Attiva il TPad
    TH1F* zoomHist1 = (TH1F*) stopDistribution1->Clone("zoomHist1"); // Clona l'istogramma originale
    zoomHist1->SetAxisRange(1, 3, "X"); // Zoom sui bin 1, 2 e 3
    zoomHist1->SetAxisRange(0, 500, "Y"); // Zoom sull'asse Y
    zoomHist1->SetStats(false); // Disabilita il box delle statistiche
    zoomHist1->SetTitle(""); // Rimuove il titolo
    zoomHist1->Draw();

    c1->cd(5);
    stopDistribution2->SetLineColor(kRed);
    stopDistribution2->SetLineWidth(2);
    stopDistribution2->SetXTitle("Numero di Stop2 per Start");
    stopDistribution2->SetYTitle("Conteggi");
    stopDistribution2->Draw();

    // Crea uno zoom su bin 1, 2 e 3 come TPad
    TPad *zoomPad2 = new TPad("zoomPad2", "Zoom sui bin 1-3", 0.5, 0.5, 0.9, 0.9); // Posizione del TPad
    zoomPad2->SetFillColor(0); // Colore di sfondo trasparente
    zoomPad2->Draw();
    zoomPad2->cd(); // Attiva il TPad
    TH1F* zoomHist2 = (TH1F*) stopDistribution2->Clone("zoomHist2"); // Clona l'istogramma originale
    zoomHist2->SetAxisRange(1, 3, "X"); // Zoom sui bin 1, 2 e 3
    zoomHist2->SetAxisRange(0, 100000, "Y"); // Zoom sull'asse Y
    zoomHist2->SetStats(false); // Disabilita il box delle statistiche
    zoomHist2->SetTitle(""); // Rimuove il titolo
    zoomHist2->Draw();

    c1->cd(6);
    stopDistribution3->SetLineColor(kGreen);
    stopDistribution3->SetLineWidth(2);
    stopDistribution3->SetXTitle("Numero di Stop3 per Start");
    stopDistribution3->SetYTitle("Conteggi");
    stopDistribution3->Draw();

    // Crea uno zoom su bin 1, 2 e 3 come TPad
    TPad *zoomPad3 = new TPad("zoomPad3", "Zoom sui bin 1-3", 0.5, 0.5, 0.9, 0.9); // Posizione del TPad
    zoomPad3->SetFillColor(0); // Colore di sfondo trasparente
    zoomPad3->Draw();
    zoomPad3->cd(); // Attiva il TPad
    TH1F* zoomHist3 = (TH1F*) stopDistribution3->Clone("zoomHist3"); // Clona l'istogramma originale
    zoomHist3->SetAxisRange(1, 3, "X"); // Zoom sui bin 1, 2 e 3
    zoomHist3->SetAxisRange(0, 1000, "Y"); // Zoom sull'asse Y
    zoomHist3->SetStats(false); // Disabilita il box delle statistiche
    zoomHist3->SetTitle(""); // Rimuove il titolo
    zoomHist3->Draw();

    c1->Update();

    // Stampa i risultati
    std::cout << "Numero di start con almeno uno stop1 corrispondente: " << matchedStartCount1 << std::endl;
    std::cout << "Numero di stop1 totali utilizzati: " << totalStopsUsed1 << std::endl;
    std::cout << "Numero di start con almeno uno stop2 corrispondente: " << matchedStartCount2 << std::endl;
    std::cout << "Numero di stop2 totali utilizzati: " << totalStopsUsed2 << std::endl;
    std::cout << "Numero di start con almeno uno stop3 corrispondente: " << matchedStartCount3 << std::endl;
    std::cout << "Numero di stop3 totali utilizzati: " << totalStopsUsed3 << std::endl;
    return 0;
}