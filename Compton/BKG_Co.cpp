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
#include <TDatime.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>
#include <sys/stat.h>

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

TH1F* histogram(std::vector<double>& channels, const char* hist_name, const char* title, Color_t color) {
    TH1F* h1 = new TH1F(hist_name, title, channels.size(), 0, channels.size());

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i+1, channels[i]);
    }
    h1->SetLineColor(color);
    h1->GetXaxis()->SetTitle("Canali");
    h1->GetYaxis()->SetTitle("Conteggi");
    return h1;
}

void addTimestamp(TCanvas* canvas, const std::string& timestamp) {
    canvas->cd();
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.03);
    latex->DrawLatex(0.1, 0.92, timestamp.c_str()); // Posizionamento sotto il titolo
}

void addFitFunction(TCanvas* canvas, std::string fitInfo) {
    canvas->cd();
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.03);

    fitInfo += "+ A_{1} \\cdot \\exp \\left( -\\frac{1}{2} \\left( \\frac{x - \\mu_{1}}{\\sigma_{1}} \\right)^{2} \\right) + ";
    fitInfo += "A_{2} \\cdot \\exp \\left( -\\frac{1}{2} \\left( \\frac{x - \\mu_{2}}{\\sigma_{2}} \\right)^{2} \\right)";
    
    latex->DrawLatex(0.1, 0.13, fitInfo.c_str()); // Posizionamento sotto la timestamp
}

int calibration_exp(){
    std::string filename_Co = "Dati/calibrazioneCo_T24_2702.dat";
    
    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co;
    readData(filename_Co, data_Co);

    std::string timestamp_Co = "27.02.2025 17:00";

    TH1F* hCo=histogram(data_Co, "hCo", "Calibrazione 60Co", kBlue+2);
    
    TCanvas* cCo = new TCanvas("cCo","Fondo esponenziale");
    cCo->cd();
    hCo->Draw();
    addTimestamp(cCo, timestamp_Co);

    /*
    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)", 3250, 3900);
    f1->SetParameter(0, 29631.1);
    f1->SetParameter(1, 9e-4);
    gStyle->SetOptFit(15);*/

    TF1* f2 = new TF1("f2", "[0]*TMath::Exp(-[1]*x) + [2]*TMath::Gaus(x, [3], [4]) + [5]*TMath::Gaus(x, [6], [7])", 3250, 4950);
    f2->SetParameter(0, 2073); //ci metto quelli trovati dal primo fit esponenziale
    f2->SetParameter(1, 8e-4);
    f2->SetParameter(2, 300);
    f2->SetParameter(3, 4150);
    f2->SetParameter(4, 70);
    f2->SetParameter(5, 196);
    f2->SetParameter(6, 4715); //media ricavata dal fit singolo
    f2->SetParameter(7, 108);
   
    f2->SetParName(2, "A_1"); //ampiezza gaussiana
    f2->SetParName(3, "#mu_1"); 
    f2->SetParName(4, "#sigma_1");  
    f2->SetParName(5, "A_2"); //ampiezza gaussiana
    f2->SetParName(6, "#mu_2"); 
    f2->SetParName(7, "#sigma_2");

    hCo->Fit("f2","","",3250, 4950); 
    gStyle->SetOptFit(1111);

    return 0;
}

int calibration_pol5(){
    std::string filename_Co = "Dati/calibrazioneCo_T24_2702.dat";
    
    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co;
    readData(filename_Co, data_Co);

    std::string timestamp_Co = "27.02.2025 17:00";

    TH1F* hCo=histogram(data_Co, "hCo", "Calibrazione 60Co", kBlue+2);
    
    TCanvas* cCo = new TCanvas("cCo","Fondo polinomio di 5 grado");
    cCo->cd();
    hCo->Draw();
    addTimestamp(cCo, timestamp_Co);

    //Provo a fare fit esponenziale solo nella parte di fondo
    /*
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5", 3250, 3900);
    f1->SetParameter(0, 1);
    f1->SetParameter(1, 1e-3);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-8);
    f1->SetParameter(4, 1e-11);
    f1->SetParameter(5, 1e-15);
    hCo->Fit("f1","","",3250, 3900); 
    gStyle->SetOptFit(1111);*/

    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5 + [6]*TMath::Gaus(x, [7], [8]) + [9]*TMath::Gaus(x, [10], [11])", 3250, 4950);

    addFitFunction(cCo, "pol5(x)");
    f2->SetParameter(0, 881); //ci metto quelli trovati dal primo fit esponenziale
    f2->SetParameter(1, -0.05);
    f2->SetParameter(2, -6.399e-5);
    f2->SetParameter(3, -1.337e-8);
    f2->SetParameter(4, 4e-12);
    f2->SetParameter(5, 1e-16);
   
    f2->SetParameter(6, 300);
    f2->SetParameter(7, 4150);
    f2->SetParameter(8, 70);
    f2->SetParameter(9, 196);
    f2->SetParameter(10, 4715);
    f2->SetParameter(11, 108);

    f2->SetParName(6, "A_1"); //ampiezza gaussiana
    f2->SetParName(7, "#mu_1"); 
    f2->SetParName(8, "#sigma_1");  
    f2->SetParName(9, "A_2"); //ampiezza gaussiana
    f2->SetParName(10, "#mu_2"); 
    f2->SetParName(11, "#sigma_2");

    hCo->Fit("f2","","",3250, 4950); 
    gStyle->SetOptFit(1111);

    return 0;
}
