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

void readData(const std::string& filename, std::vector<double>& channels) {
    // Apri il file
    std::ifstream file(filename);

    // Verifica che il file sia stato aperto correttamente
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file " << filename << std::endl;
        exit(1); // Uscita dal programma in caso di errore
    }

    double number;

    // Leggi i dati dal file
    while (file >> number) {
        channels.push_back(number);
    }
    // Chiudi il file
    file.close();
}

void histogram(std::vector<double>& channels, const char* hist_name, const char* title, Color_t color) {
    TH1F* h1 = new TH1F(hist_name, title, channels.size(), 0, channels.size());

    // Riempimento dell'istogramma con le differenze tra i tempis
    for (double ampiezza : channels) {
        h1->Fill(ampiezza);
    }
    // Visualizzazione dell’istogamma su un canvas.
    TCanvas* c = new TCanvas("c","Istogrammi Occorrenze Canali");
    c->cd();
    h1->Draw();
}

int main(){
    std::cout<<"Inizio"<<std::endl;

    // Nome del file di input
    std::string filename = "Dati/histo-set01-70deg.dat";

    // Vettori per memorizzare le ampiezze registrate nei canali
    std::vector<double> channels;
    readData(filename, channels);
    std::cout<<"Lunghezza del file "<< channels.size()<<std::endl;
 
    histogram(channels, "Spectro Co", "PMT01 500V", kBlue+2);

    filename = "Dati/PrelimPMT01_700V.dat"; // Questo funziona solo se "Dati" si trova in una sottocartella della principale
    // Vettori per memorizzare i numeri dei canali e i tempi del file
    std::vector<double> channels1;
    readData(filename, channels1);
    std::cout<<"Lunghezza del file "<< channels1.size()<<std::endl;

}
