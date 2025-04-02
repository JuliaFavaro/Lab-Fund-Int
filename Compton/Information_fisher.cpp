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
    // Open the file
    std::ifstream file(filename);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        exit(1); // Exit the program in case of error
    }

    double number;

    // Read data from the file
    while (file >> number) {
        channels.push_back(number);
    }
    // Close the file
    file.close();
}

TH1D* histogram_nonrebin(const std::vector<double>& channels, const char* title, Color_t color) {
    TH1D* h1 = new TH1D("h1_nonrebin", title, channels.size(), 0, channels.size());

    // Fill the histogram with data
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i + 1, channels[i]);
    }

    h1->SetLineColor(color);
    h1->GetXaxis()->SetTitle("Canali");
    h1->GetYaxis()->SetTitle("Conteggi");
    return h1;
}

TH1D* histogram(const std::vector<double>& channels, const char* hist_name, const char* title, Color_t color) {
    TH1D* h1 = new TH1D(hist_name, title, channels.size(), 0, channels.size());

    // Fill the histogram with data
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i + 1, channels[i]);
    }

    TH1D* h1Rebinned2 = dynamic_cast<TH1D*>(h1->Rebin(4, hist_name));
    h1Rebinned2->SetLineColor(color);
    h1Rebinned2->GetXaxis()->SetTitle("Canali");
    h1Rebinned2->GetYaxis()->SetTitle("Conteggi");
    return h1Rebinned2;
}

TH1D* histogram_piubin(const std::vector<double>& channels, const char* hist_name, const char* title, Color_t color) {
    TH1D* h1 = new TH1D(hist_name, title, channels.size(), 0, channels.size());

    // Fill the histogram with data
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i + 1, channels[i]);
    }

    TH1D* h1Rebinned2 = dynamic_cast<TH1D*>(h1->Rebin(8, hist_name));
    h1Rebinned2->SetLineColor(color);
    h1Rebinned2->GetXaxis()->SetTitle("Canali");
    h1Rebinned2->GetYaxis()->SetTitle("Conteggi");
    return h1Rebinned2;
}

// Funzione per calcolare la divergenza di Kullback-Leibler
double calcolaKLDivergenza(TH1D* P, TH1D* Q) {
    // Normalizza gli istogrammi
    P->Scale(1.0 / P->Integral(), "width");
    Q->Scale(1.0 / Q->Integral(), "width");

    double klDivergenza = 0.0;
    for (int i = 0; i < P->GetNbinsX(); i++) {
        double p = P->GetBinContent(i);
        double q = Q->GetBinContent((i / 4)); // Utilizza il bin corrispondente in Q
        if (p > 0 && q > 0) {
            klDivergenza += p * TMath::Log(p / q);
        }
    }

    return klDivergenza;
}

int main() {
    std::string filename = "Dati/Acquisizione_notte_1203_47cm_histo.dat";

    // Vectors to store the pulse amplitudes recorded in the channels
    std::vector<double> data_Co;
    readData(filename, data_Co);


    TH1D* hCo_nonrebin = histogram_nonrebin(data_Co, "Spettro ^{60}Co. Angolo 22#circ. Distanza 47 cm", kBlue + 2);

    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 22#circ. Distanza 47 cm", kBlue + 2);
    
    // Calcola la divergenza di Kullback-Leibler
    double kl = calcolaKLDivergenza(hCo_nonrebin, hCo);
    std::cout << "Divergenza di Kullback-Leibler: " << kl << std::endl;

    // Disegna gli istogrammi
    TCanvas* c1 = new TCanvas("c1", "Confronto Istogrammi");
    hCo_nonrebin->Draw();

    TCanvas* c2 = new TCanvas("c2", "Confronto Istogrammi");
    hCo->Draw();

    std::string filename2 = "Dati/Acquisizione_notte_2703_67deg.dat";

    // Vectors to store the pulse amplitudes recorded in the channels
    std::vector<double> data_Co2;
    readData(filename2, data_Co2);


    TH1D* hCo_nonrebin2 = histogram_nonrebin(data_Co2, "Spettro ^{60}Co. Angolo 26#circ. Distanza 47 cm", kBlue + 2);

    TH1D* hCo2 = histogram(data_Co2, "hCo", "Spettro ^{60}Co. Angolo 26#circ. Distanza 47 cm", kBlue + 2);
    
    // Calcola la divergenza di Kullback-Leibler
    double kl2 = calcolaKLDivergenza(hCo_nonrebin2, hCo2);
    std::cout << "Divergenza di Kullback-Leibler: " << kl2 << std::endl;
    return 0;
}