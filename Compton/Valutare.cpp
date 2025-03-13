#include <vector>
#include <string>
#include <iostream>
#include <fstream>
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
    TH1D* h1 = new TH1D("h1_nonrebin", title, channels.size(), 0, channels.size());

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i+1, channels[i]);
    }
    
    h1->SetLineColor(color);
    h1->GetXaxis()->SetTitle("Canali");
    h1->GetYaxis()->SetTitle("Conteggi");
    return h1;
}

TH1D* histogram(std::vector<double>& channels, const char* hist_name, const char* title, Color_t color) {
    TH1D* h1 = new TH1D(hist_name, title, channels.size(), 0, channels.size());

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i+1, channels[i]);
    }
    
    TH1D* h1Rebinned2 = dynamic_cast<TH1D*>(h1->Rebin(8, hist_name));
    h1Rebinned2->SetLineColor(color);
    h1Rebinned2->GetXaxis()->SetTitle("Canali");
    h1Rebinned2->GetYaxis()->SetTitle("Conteggi");
    return h1Rebinned2;
}

void addTimestamp(TCanvas* canvas, const std::string& timestamp, const std::string& duration) {
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.03);
    latex->DrawLatex(0.1, 0.87, timestamp.c_str()); // Posizionamento in alto a sinistra del grafico
    latex->DrawLatex(0.1, 0.83, duration.c_str()); // Posizionamento sotto il timestamp
}

TF1* fit_exp(TH1D* hCo){
    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)", 2620, 3150);
    f1->SetParameter(0, 230); 
    f1->SetParameter(1, 9e-4); // Tasso di decadimento per la regione 2800-3100
    hCo->Fit("f1","L","",2640,3150); 

    // Estrai i parametri del fondo
    double p0 = f1->GetParameter(0);
    double p1 = f1->GetParameter(1);
    
    // Fit 2: Fondo esponenziale + prima gaussiana
    TF1* fExp1Gaus = new TF1("fExp1Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", 2620, 3600);
    fExp1Gaus->SetParameters(p0, p1, 70, 3450, 50);
    hCo->Fit("fExp1Gaus","L","", 2620, 3600);

    double a0 = fExp1Gaus->GetParameter(0);
    double a1 = fExp1Gaus->GetParameter(1);
    double a2 = fExp1Gaus->GetParameter(2);
    double a3 = fExp1Gaus->GetParameter(3);
    double a4 = fExp1Gaus->GetParameter(4); 

    // Fit 3: Fondo esponenziale + prima gaussiana + seconda gaussiana
    TF1* fExp2Gaus = new TF1("fExp2Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)+[5]*exp(-0.5*((x-[6])/[7])**2)", 2620, 4400);
    fExp2Gaus->SetParameters(23685, 2e-3, 40, 3483, 199, 51, 3875, 142);
    hCo->Fit("fExp2Gaus","L","", 2620, 4400);

    fExp2Gaus->SetParName(2, "A_1"); //ampiezza gaussiana
    fExp2Gaus->SetParName(3, "#mu_1"); 
    fExp2Gaus->SetParName(4, "#sigma_1");  
    fExp2Gaus->SetParName(5, "A_2"); //ampiezza gaussiana
    fExp2Gaus->SetParName(6, "#mu_2"); 
    fExp2Gaus->SetParName(7, "#sigma_2");
    
    gStyle->SetOptFit(1111);
    return fExp2Gaus;
}

TF1* fit_pol5(TH1D* hCo){
    // Fit a fifth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5", 2620, 3150);
    f1->SetParameter(0, 50);
    f1->SetParameter(1, 1e-2);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-7);
    f1->SetParameter(4, 1e-11);
    f1->SetParameter(5, 1e-14);
    hCo->Fit("f1","L","",2640, 3150); 
    gStyle->SetOptFit(1111);

    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4); 
    double a5 = f1->GetParameter(5); 

    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5 + [6]*TMath::Gaus(x, [7], [8]) + [9]*TMath::Gaus(x, [10], [11])", 2620, 4400);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, a5);

    f2->SetParameter(6, 40);
    f2->SetParameter(7, 3483);
    f2->SetParameter(8, 199);
    f2->SetParameter(9, 51);
    f2->SetParameter(10, 3875);
    f2->SetParameter(11, 142);

    f2->SetParName(6, "A_1"); // Gaussian amplitude
    f2->SetParName(7, "#mu_1");
    f2->SetParName(8, "#sigma_1");
    f2->SetParName(9, "A_2"); // Gaussian amplitude
    f2->SetParName(10, "#mu_2");
    f2->SetParName(11, "#sigma_2");

    // Fit with Minuit and iterate to improve results
    for (int i = 0; i < 3; ++i) {  // Iterate 5 times to improve fit
        hCo->Fit("f2","L","",2620, 4400);
    }
    
    gStyle->SetOptFit(1111);
    return f2;
}

TF1* fit_pol9(TH1D* hCo){
    // Fit a ninth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5 + [6]*x^6 + [7]*x^7 + [8]*x^8 + [9]*x^9", 2620, 3150);
    f1->SetParameter(0, 60);
    f1->SetParameter(1, -0.7);
    f1->SetParameter(2, 7e-4);
    f1->SetParameter(3, -3e-7);
    f1->SetParameter(4, 8.79e-11);
    f1->SetParameter(5, 1e-14);
    f1->SetParameter(6, 1e-18);
    f1->SetParameter(7, 1e-22);
    f1->SetParameter(8, 1e-27);
    f1->SetParameter(9, 1e-30);
    hCo->Fit("f1","L","",2640, 3150); 
    gStyle->SetOptFit(1111);
/*
    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4); 
    double a5 = f1->GetParameter(5);
    double a6 = f1->GetParameter(6);
    double a7 = f1->GetParameter(7);
    double a8 = f1->GetParameter(8);
    double a9 = f1->GetParameter(9);

    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5 + [6]*x^6 + [7]*x^7 + [8]*x^8 + [9]*x^9 + [10]*TMath::Gaus(x, [11], [12]) + [13]*TMath::Gaus(x, [14], [15])", 2620, 4400);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, a5);
    f2->SetParameter(6, a6);
    f2->SetParameter(7, a7);
    f2->SetParameter(8, a8);
    f2->SetParameter(9, a9);

    f2->SetParameter(10, 40);
    f2->SetParameter(11, 3483);
    f2->SetParameter(12, 199);
    f2->SetParameter(13, 51);
    f2->SetParameter(14, 3875);
    f2->SetParameter(15, 142);

    f2->SetParName(10, "A_1"); // Gaussian amplitude
    f2->SetParName(11, "#mu_1");
    f2->SetParName(12, "#sigma_1");
    f2->SetParName(13, "A_2"); // Gaussian amplitude
    f2->SetParName(14, "#mu_2");
    f2->SetParName(15, "#sigma_2");

    // Fit with Minuit and iterate to improve results
    for (int i = 0; i < 3; ++i) {  // Iterate 3 times to improve fit
        hCo->Fit("f2","L","",2620, 4400);
    }
    
    gStyle->SetOptFit(1111);*/
    return f1;
}

int miglior_fit(){
    std::string filename = "Dati/Acquisizione_notte_0503_51cm_histo.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co = "04.03.2025 18:20";
    std::string duration_Co = "Durata: 96651319 ms #approx 26.8h";

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co;
    readData(filename, data_Co);

    TH1D* hCo_nonrebin = histogram_nonrebin(data_Co, "Spettro ^{60}Co. Angolo 25#circ. Distanza 51.5 cm", kBlue+2);
    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 25#circ. Distanza 51.5 cm", kBlue+2);

    //TF1* fitFunction = fit_exp(hCo_nonrebin);
    //TF1* fitFunctionpol5 = fit_pol5(hCo_nonrebin);
    TF1* fitFunctionpol9 = fit_pol9(hCo_nonrebin);

    TCanvas* cCo = new TCanvas("cCo","Istogrammi Occorrenze Canali Co");
    cCo->cd();
    hCo_nonrebin->Draw();
    addTimestamp(cCo, timestamp_Co, duration_Co);

    //fitFunction->SetLineColor(kRed);
    //fitFunction->Draw("same"); // Disegna la funzione di fit sovrapposta sul grafico

    //fitFunctionpol5->SetLineColor(kGreen);
    //fitFunctionpol5->Draw("same"); // Disegna la funzione di fit sovrapposta sul grafico

    fitFunctionpol9->SetLineColor(kMagenta);
    fitFunctionpol9->Draw("same"); // Disegna la funzione di fit sovrapposta sul grafico

    // Creazione della legenda
    TLegend* legend = new TLegend(0.75,0.75,0.9,0.9);
    //legend->AddEntry(fitFunction, "Fit esponenziale", "l");
    //legend->AddEntry(fitFunctionpol5, "Fit polinomiale 5 grado", "l");
    legend->AddEntry(fitFunctionpol9, "Fit polinomiale 9 grado", "l");
    legend->Draw();

    return 0;
}