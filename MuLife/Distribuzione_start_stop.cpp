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

void computeAndPrintPercentages(
    const std::vector<double>& startTimes,
    const std::vector<double>& stopTimes,
    const std::string& stopLabel)
{
    if (startTimes.size() < 2) {
        std::cerr << "Il vettore startTimes deve contenere almeno due valori!" << std::endl;
        return;
    }
    if (stopTimes.empty()) {
        std::cerr << "Il vettore stopTimes è vuoto!" << std::endl;
        return;
    }

    std::vector<int> stopCounts;
    size_t stopIndex = 0;
    int totalStopsInWindow = 0;
    int stopsInInterval = 0; // Nuovo contatore

    // Percentuali per start
    for (size_t i = 0; i < startTimes.size() - 1; ++i) {
        double currentStart = startTimes[i];
        double nextStart = startTimes[i + 1];
        int stopCount = 0;

        // Avanza stopIndex oltre stop precedenti
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < currentStart) {
            ++stopIndex;
        }
        // Conta stop nel range e quelli nell'intervallo richiesto
        size_t savedStopIndex = stopIndex; // Salva posizione per secondo ciclo
        while (stopIndex < stopTimes.size() && stopTimes[stopIndex] < nextStart) {
            ++stopCount;
            ++totalStopsInWindow;
            double dt = stopTimes[stopIndex] - currentStart; // in ns
            if (dt >= 150 && dt < 300) {
                ++stopsInInterval;
            }
            ++stopIndex;
        }
        stopCounts.push_back(stopCount);
    }

    int totalStarts = stopCounts.size();
    int n0 = 0, n1 = 0, nMore = 0;
    for (int c : stopCounts) {
        if (c == 0) ++n0;
        else if (c == 1) ++n1;
        else if (c > 1) ++nMore;
    }

    double perc0 = 100.0 * n0 / totalStarts;
    double perc1 = 100.0 * n1 / totalStarts;
    double percMore = 100.0 * nMore / totalStarts;

    // Calcolo rate (Hz) per START e STOP
    double tMinStart = startTimes.front();
    double tMaxStart = startTimes.back();
    double duration_s = (tMaxStart - tMinStart) * 1e-9;
    double rateStart = startTimes.size() / duration_s;

    double tMinStop = stopTimes.front();
    double tMaxStop = stopTimes.back();
    double durationStop_s = (tMaxStop - tMinStop) * 1e-9;
    double rateStop = stopTimes.size() / durationStop_s;

    // Percentuale di stop nell'intervallo 200-300 ns
    double percStopInInterval = 0;
    if (totalStopsInWindow > 0) {
        percStopInInterval = 100.0 * stopsInInterval / totalStopsInWindow;
    }

    std::cout << "----- " << stopLabel << " -----" << std::endl;
    std::cout << "Percentuale di start senza stop: " << perc0 << " %" << std::endl;
    std::cout << "Percentuale di start con 1 stop: " << perc1 << " %" << std::endl;
    std::cout << "Percentuale di start con >1 stop: " << percMore << " %" << std::endl;

    std::cout << "Durata acquisizione (start): " << duration_s << " s" << std::endl;
    std::cout << "Rate start: " << rateStart << " Hz" << std::endl;
    std::cout << "Durata acquisizione (stop): " << durationStop_s << " s" << std::endl;
    std::cout << "Rate stop: " << rateStop << " Hz" << std::endl;

    std::cout << "Percentuale di stop che cadono nell'intervallo 200-300 ns dopo uno start: "
              << percStopInInterval << " %" << std::endl;
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

    const std::string filename2 = "Dati/output_20250506-mulife.txt";
    const std::string filename3 = "Dati/output_20250507-mumass.txt";
    std::vector<double> start3Times;
    std::vector<double> stop3Times;

    try {
        readFileAndPopulateArrays(filename2, start3Times, stop3Times, stop3Times);
        readFileAndPopulateArrays(filename3, start3Times, stop3Times, stop3Times);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    TCanvas* c1 = new TCanvas("c1", "Differenze di tempo tra start e stop", 1500, 1500);
    c1->Divide(2, 2); // Divide il canvas in 4 sezioni
    c1->SetGrid();

    // Crea gli istogrammi
    TH1F* histogram1 = new TH1F("time_differences_StopA", "#Delta t usando Stop A", 200, 0, 8000);
    TH1F* histogram2 = new TH1F("time_differences_StopB", "#Delta t usando Stop B", 200, 0, 8000);
    TH1F* histogram3= new TH1F("time_differences_StopC", "#Delta t usando Stop C", 200, 0, 20*1000);  
    TH1F* stopDistribution1 = new TH1F("stop_distributionA", "Distribuzione numero di StopA per Start", 6, 0, 6);
    TH1F* stopDistribution2 = new TH1F("stop_distributionB", "Distribuzione numero di StopB per Start", 6, 0, 6);
    TH1F* stopDistribution3 = new TH1F("stop_distributionC", "Distribuzione numero di StopC per Start", 6, 0, 6);

    double binWidth = histogram2->GetBinWidth(1); 
    std::cout << "La larghezza di un bin è: " << binWidth << " ns" << std::endl;
    binWidth = histogram3->GetBinWidth(1); 
    std::cout << "La larghezza di un bin è: " << binWidth << " ns" << std::endl;

    // Variabili per il conteggio totale di stop usati
    int totalStopsUsed1 = 0;
    int totalStopsUsed2 = 0;
    int totalStopsUsed3 = 0;

    // Calcola le differenze temporali per StopA e StopB
    int matchedStartCount1 = calculateTimeDifferencesAndFillHistogram(startTimes, stop1Times, histogram1, totalStopsUsed1);
    int matchedStartCount2 = calculateTimeDifferencesAndFillHistogram(startTimes, stop2Times, histogram2, totalStopsUsed2);
    int matchedStartCount3 = calculateTimeDifferencesAndFillHistogram(start3Times, stop3Times, histogram3, totalStopsUsed3);

    // Calcola la distribuzione del numero di stop per start
    calculateStopDistribution(startTimes, stop1Times, stopDistribution1);
    calculateStopDistribution(startTimes, stop2Times, stopDistribution2);
    calculateStopDistribution(start3Times, stop3Times, stopDistribution3);

    /*
    // Disegna gli istogrammi
    c1->cd(1);
    histogram1->SetLineColor(kBlue);
    histogram1->SetLineWidth(2);
    histogram1->SetXTitle("#Delta t (ns)");
    histogram1->SetYTitle("Conteggi");
    histogram1->SetMaximum(30);
    histogram1->Draw();
    gPad->SetLogy(); // Imposta la scala logaritmica sull'asse Y per questa sezione

    TLatex* texA = new TLatex();
    texA->SetNDC();
    texA->SetTextSize(0.03);
    texA->DrawLatex(0.15, 0.85, "24.04.2025, 120 ore");*/

    c1->cd(1);
    histogram2->SetLineColor(kRed);
    histogram2->SetLineWidth(2);
    histogram2->SetXTitle("#Delta t (ns)");
    histogram2->SetYTitle("Conteggi");
    histogram2->Draw();
    gPad->SetLogy(); // Imposta la scala logaritmica sull'asse Y per questa sezione

    TLatex* texB = new TLatex();
    texB->SetNDC();
    texB->SetTextSize(0.03);
    texB->DrawLatex(0.15, 0.85, "24.04.2025, 120 ore");

    c1->cd(2);
    histogram3->SetLineColor(kGreen);
    histogram3->SetLineWidth(2);
    histogram3->SetXTitle("#Delta t (ns)");
    histogram3->SetYTitle("Conteggi");
    histogram3->Draw();
    gPad->SetLogy(); // Imposta la scala logaritmica sull'asse Y per questa sezione

    TLatex* texC = new TLatex();
    texC->SetNDC();
    texC->SetTextSize(0.03);
    texC->DrawLatex(0.15, 0.85, "06.05.2025, 42 ore");
    /*
    c1->cd(3);
    stopDistribution1->SetLineColor(kBlue);
    stopDistribution1->SetLineWidth(2);
    stopDistribution1->SetXTitle("Numero di StopA per Start");
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
    zoomHist1->Draw();*/

    c1->cd(3);
    stopDistribution2->SetLineColor(kRed);
    stopDistribution2->SetLineWidth(2);
    stopDistribution2->SetXTitle("Numero di StopB per Start");
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

    c1->cd(4);
    stopDistribution3->SetLineColor(kGreen);
    stopDistribution3->SetLineWidth(2);
    stopDistribution3->SetXTitle("Numero di StopC per Start");
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
    
    std::cout << "Numero di start: " << startTimes.size() << std::endl;
    std::cout << "Numero di stopA: " << stop1Times.size() << std::endl;
    std::cout << "Numero di stopB: " << stop2Times.size() << std::endl;
    std::cout << "Numero di start1: " << start3Times.size() << std::endl;
    std::cout << "Numero di stopC: " << stop3Times.size() << std::endl;

    // Stampa i risultati
    computeAndPrintPercentages(startTimes, stop2Times, "StopB");
    computeAndPrintPercentages(start3Times, stop3Times, "StopC");

    return 0;
}