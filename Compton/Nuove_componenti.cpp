#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <TCanvas.h>
#include <cmath>
#include <iomanip>
#include <sstream>

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

TH1D* histogram(std::vector<double>& channels, const char* hist_name, const char* title, Color_t color) {
    TH1D* h1 = new TH1D(hist_name, title, channels.size(), 0, channels.size());

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i+1, channels[i]);
    }
    
    TH1D* h1Rebinned2 = dynamic_cast<TH1D*>(h1->Rebin(4, hist_name));
    h1Rebinned2->SetLineColor(color);
    h1Rebinned2->GetXaxis()->SetTitle("Canali");
    h1Rebinned2->GetYaxis()->SetTitle("Conteggi");
    return h1Rebinned2;
}

void fitBackgroundAndPeak(TH1D* hCo, TF1*& fit) {
    // Fit a fifth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4", 2620, 3150);
    f1->SetParameter(0, 50);
    f1->SetParameter(1, 1e-2);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-7);
    f1->SetParameter(4, 1e-11);
    hCo->Fit("f1","RLM+N","",2800, 3150); 

    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4); 

    // Fit 2
    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*exp(-0.5*((x-[6])/[7])**2)", 2800, 3683);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, 40);
    f2->SetParameter(6, 3283);
    f2->SetParameter(7, 199);
    hCo->Fit("f2","RLMI+N","", 2800, 3683);

    double b0 = f2->GetParameter(0);
    double b1 = f2->GetParameter(1);
    double b2 = f2->GetParameter(2);
    double b3 = f2->GetParameter(3);
    double b4 = f2->GetParameter(4); 
    double b5 = f2->GetParameter(5); 
    double b6 = f2->GetParameter(6); 
    double b7 = f2->GetParameter(7); 

    //Fit totale
    fit = new TF1("f3", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*TMath::Gaus(x, [6], [7]) + [8]*TMath::Gaus(x, [9], [10])", 2800, 4400);
    fit->SetParameter(0, b0);
    fit->SetParameter(1, b1);
    fit->SetParameter(2, b2);
    fit->SetParameter(3, b3);
    fit->SetParameter(4, b4);
    fit->SetParameter(5, b5);
    fit->SetParameter(6, b6);
    fit->SetParameter(7, b7);

    fit->SetParameter(8, 51);
    fit->SetParameter(9, 3875);
    fit->SetParameter(10, 142);
    fit->SetParLimits(10, 140, 150);

    fit->SetParName(5, "A_1"); // Gaussian amplitude
    fit->SetParName(6, "#mu_1");
    fit->SetParName(7, "#sigma_1");
    fit->SetParName(8, "A_2"); // Gaussian amplitude
    fit->SetParName(9, "#mu_2");
    fit->SetParName(10, "#sigma_2");

    hCo->Fit("f3","RLI+","",2800, 4400);   
    gStyle->SetOptFit(1111);
}

void fitBackgroundAndGaussians(TH1D* hCo_fondo, TF1*& fit) {
    fit = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5 + [6]*TMath::Gaus(x, [7], [8]) + [9]*TMath::Gaus(x, [10], [11])", 3250, 4950);
    fit->SetParameter(0, 881); //ci metto quelli trovati dal primo fit esponenziale
    fit->SetParameter(1, -0.05);
    fit->SetParameter(2, -6.399e-5);
    fit->SetParameter(3, -1.337e-8);
    fit->SetParameter(4, 4e-12);
    fit->SetParameter(5, 1e-16);
   
    fit->SetParameter(6, 450);
    fit->SetParameter(7, 4150);
    fit->SetParameter(8, 90);
    fit->SetParameter(9, 380);
    fit->SetParameter(10, 4700);
    fit->SetParameter(11, 88);

    fit->SetParName(6, "A_1"); // Gaussian amplitude
    fit->SetParName(7, "#mu_1"); 
    fit->SetParName(8, "#sigma_1");  
    fit->SetParName(9, "A_2"); // Gaussian amplitude
    fit->SetParName(10, "#mu_2"); 
    fit->SetParName(11, "#sigma_2");

    hCo_fondo->Fit("f2","L","",3250, 5000); 
    gStyle->SetOptFit(1111);
}

void overlayHistograms(TH1D* hCo, TH1D* hCo_fondo) {
    TCanvas* c = new TCanvas("c", "Overlay of Spectra", 800, 600);
    hCo->Draw("HIST");
    hCo_fondo->Draw("HIST SAME");

    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hCo, "Spettro ^{60}Co", "l");
    legend->AddEntry(hCo_fondo, "Spettro ^{60}Co Fondo", "l");
    legend->Draw();
}

int main() {
    std::string filename_fondo= "Dati/Giorno_0503/calibrazioneCo_T240_0503_histo.dat";
    std::vector<double> data_Co_fondo;
    readData(filename_fondo, data_Co_fondo);
    TH1D* hCo_fondo = histogram(data_Co_fondo, "hCo_fondo", "Spettro ^{60}Co Fondo. Angolo 22#circ. Distanza 47 cm", kBlue + 2);

    std::string filename = "Dati/Acquisizione_notte_0503_51cm_histo.dat";
    std::vector<double> data_Co;
    readData(filename, data_Co);
    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 22#circ. Distanza 51.5 cm", kRed);

    // Overlay the histograms
    overlayHistograms(hCo, hCo_fondo);

    // Fit the histograms
    TF1* fitCo;
    TF1* fitCo_fondo;
    fitBackgroundAndPeak(hCo, fitCo);
    fitBackgroundAndGaussians(hCo_fondo, fitCo_fondo);

    // Find the mean shift
    double meanCo = fitCo->GetParameter(6);
    double meanCo_fondo = fitCo_fondo->GetParameter(7);
    double meanShift = meanCo - meanCo_fondo;

    std::cout << "Mean of hCo: " << meanCo << std::endl;
    std::cout << "Mean of hCo_fondo: " << meanCo_fondo << std::endl;
    std::cout << "Shift in means: " << meanShift << std::endl;

    return 0;
}