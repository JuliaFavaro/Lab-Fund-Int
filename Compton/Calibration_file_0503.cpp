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

TH1D* histogram(std::vector<double>& channels, const char* hist_name, const char* title, Color_t color) {
    TH1D* horiginal = new TH1D(hist_name, title, channels.size(), 0, channels.size());

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < channels.size(); ++i) {
        horiginal->SetBinContent(i+1, channels[i]);
    }
    TH1D* h1 = dynamic_cast<TH1D*>(horiginal->Rebin(4, hist_name));
    h1->SetLineColor(color);
    h1->GetXaxis()->SetTitle("Canali");
    h1->GetYaxis()->SetTitle("Conteggi");

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
    std::string filename_Cs = "Dati/Giorno_0503/calibrazioneCs_T243_0503_histo.dat";
    std::string filename_Co = "Dati/Giorno_0503/calibrazioneCo_T240_0503_histo.dat";
    std::string filename_Na = "Dati/Giorno_0503/CalibrazioneNa_T237_0503_histo.dat";
    
    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Cs;
    std::vector<double> data_Co;
    std::vector<double> data_Na;
    readData(filename_Cs, data_Cs);
    readData(filename_Co, data_Co);
    readData(filename_Na, data_Na);

    // Ottenere il timestamp di creazione dei file
    std::string timestamp_Cs = "05.03.2025 18:00";
    std::string timestamp_Co = "05.03.2025 18:05";
    std::string timestamp_Na = "05.03.2025 18:10";

    //vedere i nostri grafici uno dopo l'altro
    TH1D* hCs = histogram(data_Cs, "hCs", "Calibrazione 137Cs", kGreen+2);
    TH1D* hCo = histogram(data_Co, "hCo", "Calibrazione 60Co", kBlue+2);
    TH1D* hNa = histogram(data_Na, "hNa", "Calibrazione 22Na", kRed+2);
    TCanvas* cCs = new TCanvas("cCs","Fondo pol5");
    cCs->cd();
    hCs->Draw();
    addTimestamp(cCs, timestamp_Cs);
    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)", 1650, 1980);
    f1->SetParameters(1e7, 0.005);
    hCs->Fit("f1","L","", 1650, 1980);
    gStyle->SetOptFit(15);
    
    // Estrai i parametri del fondo
    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);

    // Fit 2: Fondo esponenziale + prima gaussiana
    TF1* fExp1Gaus = new TF1("fExp1Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", 1650, 2800);
    //fExp1Gaus->SetParameters(1e7, 0.005, 16000, 2420, 81);
    fExp1Gaus->SetParameters(1e7, 0.005, 8000, 2420, 81);

    fExp1Gaus->SetParLimits(4, 60, 85); // Vincola la larghezza del secondo picco

    fExp1Gaus->SetParName(2, "A_1"); //ampiezza gaussiana
    fExp1Gaus->SetParName(3, "#mu_1"); 
    fExp1Gaus->SetParName(4, "#sigma_1");
    hCs->Fit("fExp1Gaus","L","", 1650, 2800);
    gStyle->SetOptFit(1111);

    // Aggiungi il valore di mu alla lista
    double mu_Cs = fExp1Gaus->GetParameter(3);
    double mu_err_Cs = fExp1Gaus->GetParError(3);
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
   
    f2->SetParameter(6, 450);
    f2->SetParameter(7, 4150);
    f2->SetParameter(8, 90);
    f2->SetParameter(9, 380);
    f2->SetParameter(10, 4700);
    f2->SetParameter(11, 88);

    f2->SetParName(6, "A_1"); //ampiezza gaussiana
    f2->SetParName(7, "#mu_1"); 
    f2->SetParName(8, "#sigma_1");  
    f2->SetParName(9, "A_2"); //ampiezza gaussiana
    f2->SetParName(10, "#mu_2"); 
    f2->SetParName(11, "#sigma_2");

    hCo->Fit("f2","L","",3250, 5000); 
    gStyle->SetOptFit(1111);

    // Aggiungi i valori di mu alla lista
    double mu_Co1 = f2->GetParameter(7);
    double mu_err_Co1 = f2->GetParError(7);
    double energy_Co1 = 1.17e6;
    double mu_Co2 = f2->GetParameter(10);
    double mu_err_Co2 = f2->GetParError(10);
    double energy_Co2 = 1.33e6;

    /*-------------------------------------------------------------------------*/

    TCanvas* cNa = new TCanvas("cNa","Fondo esponenziale");
    cNa->cd();
    hNa->Draw();
    addTimestamp(cNa, timestamp_Na);

    // Fit 1: Solo fondo polinomiale di 5° grado
    TF1* fBkg_Na= new TF1("fBkg_Na", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x", 2000, 3000);
    fBkg_Na->SetParameters(5, -0.001, 1e-7, -1e-11, 1e-15, -1e-19); // Stima iniziale
    hNa->Fit("fBkg_Na","L","", 2000, 3000);

    // Estrai i parametri del fondo
    double p0 = fBkg_Na->GetParameter(0);
    double p1 = fBkg_Na->GetParameter(1);
    double p2 = fBkg_Na->GetParameter(2);
    double p3 = fBkg_Na->GetParameter(3);
    double p4 = fBkg_Na->GetParameter(4);
    double p5 = fBkg_Na->GetParameter(5);

    // Fit 2: Fondo polinomiale + prima gaussiana
    TF1* fPol1Gaus_Na = new TF1("fPol1Gaus_Na", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x + [6]*exp(-0.5*((x-[7])/[8])**2)", 1150, 2000);
    fPol1Gaus_Na->SetParameters(p0, p1, p2, p3, p4, p5, 200, 1850, 50); // Ampiezza da immagine intorno a 70, centro 1800, sigma 100
    hNa->Fit("fPol1Gaus_Na","L","", 1150, 2000);

    // Fit 3: Fondo polinomiale + due gaussiane
    TF1* fFinal = new TF1("fFinal", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x + [6]*exp(-0.5*((x-[7])/[8])**2) + [9]*exp(-0.5*((x-[10])/[11])**2)", 1150, 5000);
    fFinal->SetParameter(0, p0);
    fFinal->SetParameter(1, p1);
    fFinal->SetParameter(2, p2);
    fFinal->SetParameter(3, p3);
    fFinal->SetParameter(4, p4);
    fFinal->SetParameter(5, p5);
    fFinal->SetParameter(6, 200);
    fFinal->SetParameter(7, 1850);
    fFinal->SetParameter(8, 50);
    fFinal->SetParameter(9,10);
    fFinal->SetParameter(10, 4450);
    fFinal->SetParameter(11, 75);
    
    fFinal->SetParName(6, "A1");
    fFinal->SetParName(7, "#mu_1");
    fFinal->SetParName(8, "#sigma_1");
    fFinal->SetParName(9, "A2");
    fFinal->SetParName(10, "#mu_2");
    fFinal->SetParName(11, "#sigma_2");
    
    fFinal->SetParLimits(9, 30, 50);
    fFinal->SetParLimits(11, 70, 90);
    hNa->Fit("fFinal","L","", 1250, 4820);

    gStyle->SetOptFit(1111);

    // Aggiungi i valori di mu alla lista
    double mu_Na1 = fFinal->GetParameter(7);
    double mu_err_Na1 = fFinal->GetParError(7);
    double energy_Na1 = 551e3; //keV
    
    double mu_Na2 = fFinal->GetParameter(10);
    double mu_err_Na2= fFinal->GetParError(10);
    double energy_Na2 =1274e3; //keV

    /*-------------------------------------------------------------------------*/
    
    double min_mu = std::min({mu_Cs, mu_Co1, mu_Co2, mu_Na1, mu_Na2});
    double max_mu = std::max({mu_Cs, mu_Co1, mu_Co2, mu_Na1, mu_Na2});

    // Creare grafici separati per Cs, Co e Na    
    TCanvas* cFit = new TCanvas("cFit", "Funzione di calibrazione");
    TGraphErrors* mg = new TGraphErrors(4);
    mg->SetMarkerStyle(21);

    mg->SetPoint(0, mu_Cs, energy_Cs / 1e3);
    mg->SetPointError(0, mu_err_Cs, 0.);

    mg->SetPoint(1, mu_Co1, energy_Co1 / 1e3);
    mg->SetPointError(1, mu_err_Co1, 0.);
    mg->SetPoint(2, mu_Co2, energy_Co2 / 1e3);
    mg->SetPointError(2, mu_err_Co2, 0.);

    mg->SetPoint(3, mu_Na1, energy_Na1 / 1e3);
    mg->SetPointError(3, mu_err_Na1, 0.);
    mg->SetPoint(4, mu_Na2, energy_Na2 / 1e3);
    mg->SetPointError(4, mu_err_Na2, 0.);


    std::cout<< mu_Cs<< " "<< mu_err_Cs<< std::endl;
    std::cout<< mu_Co1<< " "<< mu_err_Co1<< std::endl;
    std::cout<< mu_Co2<< " "<< mu_err_Co2<< std::endl;
    std::cout<< mu_Na1<< " "<< mu_err_Na1<< std::endl;
    std::cout<< mu_Na2<< " "<< mu_err_Na2<< std::endl;

    mg->SetMarkerStyle(8);
    mg->SetMarkerSize(1);
    mg->SetMarkerColor(kAzure -9);
    mg->SetLineColor(kAzure -9);
    mg->Draw("AP");

    TF1* fit = new TF1("fit", "pol1", min_mu, max_mu); // Adjust range as needed
    
    fit->SetParameters(0,1/3);
    mg->Fit(fit, "WQN");
    mg->Fit(fit); // "final fit"

    // Draw fit line
    fit->SetLineColor(kRed);
    fit->SetLineStyle(1);
    fit->Draw("same");

    mg->SetTitle("Funzione di calibrazione");
    addTimestamp(cFit, timestamp_Na);
    mg->GetXaxis()->SetTitle("Canali dei picchi");
    mg->GetYaxis()->SetTitle("Energie [keV]");

    return 0;
}