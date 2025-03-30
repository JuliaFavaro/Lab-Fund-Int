#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <TCanvas.h>
#include <cmath>
#include <iomanip>
#include <sstream>

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>
#include <sys/stat.h>
#include <TMinuit.h>

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

TH1D* histogram_nonrebin(std::vector<double>& channels, const char* title, Color_t color) {
    TH1D* h1 = new TH1D(" ", " ", channels.size(), 0, channels.size());

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i+1, channels[i]);
    }
    
    h1->SetLineColor(color);
    h1->GetXaxis()->SetTitle("Canali");
    h1->GetYaxis()->SetTitle("Conteggi");
    return h1;
}

// Funzione per aggiungere il timestamp
void addTimestamp(TCanvas* c, const std::string& timestamp, Color_t color) {
    TText* text = new TText(0.5, 0.9, timestamp.c_str());
    text->SetTextSize(0.08);
    text->SetTextColor(color);
    text->SetNDC(); // Utilizza coordinate normalizzate del dispositivo
    text->Draw();
}

void addMainTitle(TCanvas* canvas, const std::string& title) {
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.05);
    latex->SetTextAlign(22); // Center alignment
    latex->DrawLatex(0.5, 0.95, title.c_str()); // Centered and at the top of the canvas
}

int main(){
    // File names and timestamps
    std::vector<std::string> filenames = {
        "Dati/Prelim/PrelimPMT01_550V.dat",
        "Dati/Prelim/PrelimPMT01_650V.dat",
        "Dati/Prelim/PrelimPMT01_700V.dat"
    };
    std::vector<std::string> timestamps = {
        "550V",
        "650V",
        "700V"
    };

    // Colors for each histogram
    std::vector<Color_t> colors = {
        kGreen+2,
        kMagenta+2,
        kCyan+2
    };

    TCanvas* c = new TCanvas("Acquisizione spettro PMT1 12/02","Acquisizione spettro PMT1 12/02. Angolo #theta = 0. Distanza 40.0 #pm 0.2 cm.",1500,500);
    c->SetTitle("Acquisizione spettro PMT1 12/02. Angolo #theta = 0. Distanza 40.0 #pm 0.2 cm.");
    c->Divide(3,1,0.01,0.01); // Add padding between pads

    // Loop over files and create histograms
    for (size_t i = 0; i < filenames.size(); ++i) {
        std::vector<double> data;
        readData(filenames[i], data);
        TH1D* h = histogram_nonrebin(data, filenames[i].c_str(), colors[i]);
        c->cd(i+1);
        h->Draw("E");
        addTimestamp(c, timestamps[i], colors[i]);
    }

    return 0;
}