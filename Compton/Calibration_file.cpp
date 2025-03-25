#include <vector>
#include <string>
#include <iostream>
#include <fstream>
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

int calibration() {
    std::string filename_Cs = "Dati/calibrazioneCs_T24_2702.dat";
    std::string filename_Co = "Dati/calibrazioneCo_T24_2702.dat";
    std::string filename_Na = "Dati/calibrazioneNa_T24_2702.dat";
    
    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Cs;
    std::vector<double> data_Co;
    std::vector<double> data_Na;
    readData(filename_Cs, data_Cs);
    readData(filename_Co, data_Co);
    readData(filename_Na, data_Na);

    // Ottenere il timestamp di creazione dei file
    std::string timestamp_Cs = "27.02.2025 17:00";
    std::string timestamp_Co = "27.02.2025 17:00";
    std::string timestamp_Na = "27.02.2025 17:00";

    //vedere i nostri grafici uno dopo l'altro
    TH1F* hCs = histogram(data_Cs, "hCs", "Calibrazione 137Cs", kGreen+2);
    TH1F* hCo = histogram(data_Co, "hCo", "Calibrazione 60Co", kBlue+2);
    TH1F* hNa = histogram(data_Na, "hNa", "Calibrazione 22Na", kRed+2);

    // Visualizzazione separata per ogni istogramma
    TCanvas* cCs = new TCanvas("cCs","Istogrammi Occorrenze Canali Cs");
    cCs->cd();
    hCs->Draw();
    addTimestamp(cCs, timestamp_Cs);
    
    TF1* f1 = new TF1("f1", "[0]*exp(-x/[1]) + [2]*TMath::Gaus(x, [3], [4])", 1650, 2700);

    // Parametri iniziali
    f1->SetParameter(0, 1e2);   // Ampiezza esponenziale
    f1->SetParameter(1, 1e2);    // Tau (vita media esponenziale)
    f1->SetParameter(2, 1200);
    f1->SetParameter(3, 2400);
    f1->SetParameter(4, 100);
    f1->SetParName(2, "A_1"); //ampiezza gaussiana
    f1->SetParName(3, "#mu_1"); 
    f1->SetParName(4, "#sigma_1"); 
    hCs->Fit("f1","L","", 1650, 2700);
    gStyle->SetOptFit(1111);

    // Aggiungi il valore di mu alla lista
    double mu_Cs = f1->GetParameter(3);
    double mu_err_Cs = f1->GetParError(3);
    double energy_Cs = 662e3;

    /*-------------------------------------------------------------------------*/

    TCanvas* cCo = new TCanvas("cCo","Istogrammi Occorrenze Canali Co");
    cCo->cd();
    hCo->Draw();
    addTimestamp(cCo, timestamp_Co);

    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5 + [6]*TMath::Gaus(x, [7], [8]) + [9]*TMath::Gaus(x, [10], [11])", 3250, 4950);

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

    hCo->Fit("f2","L","",3250, 4950); 
    gStyle->SetOptFit(1111);

    // Aggiungi i valori di mu alla lista
    double mu_Co1 = f2->GetParameter(7);
    double mu_err_Co1 = f2->GetParError(7);
    double energy_Co1 = 1.17e6;
    double mu_Co2 = f2->GetParameter(10);
    double mu_err_Co2 = f2->GetParError(10);
    double energy_Co2 = 1.33e6;

    /*-------------------------------------------------------------------------*/

    TCanvas* cNa = new TCanvas("cNa","Istogrammi Occorrenze Canali Na");
    cNa->cd();
    hNa->Draw();
    addTimestamp(cNa, timestamp_Na);

    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *fa= new TF1("fa", "[0]*exp(-[1]*x)", 1000, 1600);
    fa->SetParameters(10, 0.001);
    hNa->Fit("fa","","", 2100, 3700);
    
    // Estrai i parametri del fondo
    double p0 = fa->GetParameter(0);
    double p1 = fa->GetParameter(1);

    // Fit 2: Fondo esponenziale + prima gaussiana
    TF1* fExp1Gaus = new TF1("fExp1Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", 1000, 2000);
    fExp1Gaus->SetParameters(p0, p1, 60, 1850, 50);
    hNa->Fit("fExp1Gaus","","", 1000, 2000);

    double a0 = fExp1Gaus->GetParameter(0);
    double a1 = fExp1Gaus->GetParameter(1);
    double a2 = fExp1Gaus->GetParameter(2);
    double a3 = fExp1Gaus->GetParameter(3);
    double a4 = fExp1Gaus->GetParameter(4);

    TF1* f3 = new TF1("f3", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2) + [5]*exp(-0.5*((x-[6])/[7])**2)", 1000, 5000);
    f3->SetParameters(a0, a1, a2, a3, a4, 20, 4500, 75);
    f3->SetParLimits(6, 4200, 4800); // Vincola la media del secondo picco
    f3->SetParLimits(7, 50, 80); // Vincola la media del secondo picco
    hNa->Fit("f3","L","", 1000, 5000);
    gStyle->SetOptFit(1111);

    // Aggiungi i valori di mu alla lista
    double mu_Na1 = f3->GetParameter(3);
    double mu_err_Na1 = f3->GetParError(3);
    double energy_Na1 = 551e3;
    double mu_Na2 = f3->GetParameter(6);
    double mu_err_Na2 = f3->GetParError(6);
    double energy_Na2 = 1274e3;

    /*-------------------------------------------------------------------------*/
    // Creare grafici separati per Cs, Co e Na
    TCanvas* cFit = new TCanvas("cFit","Funzione di calibrazione");
    TMultiGraph* mg = new TMultiGraph();

    TGraphErrors* graphCs = new TGraphErrors(1);
    graphCs->SetPoint(0, energy_Cs, mu_Cs);
    graphCs->SetPointError(0, 0, mu_err_Cs);
    graphCs->SetMarkerColor(kGreen + 2);
    graphCs->SetMarkerStyle(20);
    graphCs->SetMarkerSize(1.0);
    mg->Add(graphCs);

    TGraphErrors* graphCo = new TGraphErrors(2);
    graphCo->SetPoint(0, energy_Co1, mu_Co1);
    graphCo->SetPointError(0, 0, mu_err_Co1);
    graphCo->SetPoint(1, energy_Co2, mu_Co2);
    graphCo->SetPointError(1, 0, mu_err_Co2);
    graphCo->SetMarkerColor(kBlue + 2);
    graphCo->SetMarkerStyle(20);
    graphCo->SetMarkerSize(1.0);
    mg->Add(graphCo);

    TGraphErrors* graphNa = new TGraphErrors(2);
    graphNa->SetPoint(0, energy_Na1, mu_Na1);
    graphNa->SetPointError(0, 0, mu_err_Na1);
    graphNa->SetPoint(1, energy_Na2, mu_Na2);
    graphNa->SetPointError(1, 0, mu_err_Na2);
    graphNa->SetMarkerColor(kRed + 2);
    graphNa->SetMarkerStyle(20);
    graphNa->SetMarkerSize(1.0);
    mg->Add(graphNa);

    mg->Draw("AP");

    mg->SetTitle("Funzione di calibrazione");

    // Fit lineare
    TF1* fit = new TF1("fit", "pol1", std::min({energy_Cs, energy_Co1, energy_Co2, energy_Na1, energy_Na2}), std::max({energy_Cs, energy_Co1, energy_Co2, energy_Na1, energy_Na2}));
    mg->Fit(fit);
    gStyle->SetOptFit(1111);

    mg->GetXaxis()->SetTitle("Energie [eV]");
    mg->GetYaxis()->SetTitle("Canali dei picchi");

    // Legenda
    TLegend* legend = new TLegend(0.1, 0.7, 0.3, 0.9);
    legend->AddEntry(graphCs, "137Cs");
    legend->AddEntry(graphCo, "60Co");
    legend->AddEntry(graphNa, "22Na");
    legend->Draw();

    return 0;
}