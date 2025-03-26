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
#include <TVirtualFitter.h>

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

TF1* calibration_exp(){
    std::string filename_Na = "Dati/Giorno_0403/calibrazioneCs_T2501_0403_histo.dat";
    
    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Cs;
    readData(filename_Na, data_Cs);

    std::string timestamp_Cs= "04.03.2025 14:30";

    TH1D* hCs=histogram(data_Cs, "hCs", "Calibrazione 137Cs", kGreen-2);
    TCanvas* cCs = new TCanvas("cCs","Fondo esponenziale");
    cCs->cd();
    hCs->Draw();
    addTimestamp(cCs, timestamp_Cs);

    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)", 1650, 1980);
    f1->SetParameters(10, 0.001);
    hCs->Fit("f1","L","", 1650, 1980);
    gStyle->SetOptFit(15);
    
    // Estrai i parametri del fondo
    double p0 = f1->GetParameter(0);
    double p1 = f1->GetParameter(1);

    // Fit 2: Fondo esponenziale + prima gaussiana
    TF1* fExp1Gaus = new TF1("fExp1Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", 1650, 2800);
    fExp1Gaus->SetParameters(p0, p1, 9648, 2420, 81);

    fExp1Gaus->SetParLimits(2, 9000, 9700); // Vincola la ampiezza del secondo picco
    fExp1Gaus->SetParLimits(4, 60, 85); // Vincola la larghezza del secondo picco

    fExp1Gaus->SetParName(2, "A_1"); //ampiezza gaussiana
    fExp1Gaus->SetParName(3, "#mu_1"); 
    fExp1Gaus->SetParName(4, "#sigma_1");
    hCs->Fit("fExp1Gaus","L","", 1650, 2800);
    gStyle->SetOptFit(1111);

    return fExp1Gaus;
}

TF1* calibration_pol4(){
    std::string filename_Na = "Dati/Giorno_0403/calibrazioneCs_T2501_0403_histo.dat";
    
    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Cs;
    readData(filename_Na, data_Cs);

    std::string timestamp_Cs= "04.03.2025 14:30";

    TH1D* hCs=histogram(data_Cs, "hCs", "Calibrazione 137Cs", kGreen-2);
    
    TCanvas* cCs = new TCanvas("cCs","Fondo pol5");
    cCs->cd();
    hCs->Draw();
    addTimestamp(cCs, timestamp_Cs);

    // Fit 1: Solo fondo polinomiale di 4° grado
    TF1* fBkg = new TF1("fBkg", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x ", 1650, 1980);
    fBkg->SetParameters(5, -0.001, 1e-7, -1e-11, 1e-15); // Stima iniziale
    hCs->Fit("fBkg","L","", 1650, 1980);

    // Estrai i parametri del fondo
    double p0 = fBkg->GetParameter(0);
    double p1 = fBkg->GetParameter(1);
    double p2 = fBkg->GetParameter(2);
    double p3 = fBkg->GetParameter(3);
    double p4 = fBkg->GetParameter(4);

    // Fit 2: Fondo polinomiale + prima gaussiana
    TF1* fPol1Gaus = new TF1("fPol1Gaus", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*exp(-0.5*((x-[6])/[7])**2)", 1650, 2800);
    fPol1Gaus->SetParameters(p0, p1, p2, p3, p4, 9648, 2420, 81); // Ampiezza da immagine intorno a 70, centro 1800, sigma 100

    fPol1Gaus->SetParLimits(5, 9000, 9700); // Vincola la media del ampiezza picco
    fPol1Gaus->SetParLimits(7,  60, 85); // Vincola la media del larghezza picco
    
    fPol1Gaus->SetParName(5, "A_1"); //ampiezza gaussiana
    fPol1Gaus->SetParName(6, "#mu_1"); 
    fPol1Gaus->SetParName(7, "#sigma_1");
    hCs->Fit("fPol1Gaus","L","", 1650, 2800);

    gStyle->SetOptFit(1111);

    return fPol1Gaus;
}

TF1* calibration_pol5(){
    std::string filename_Na = "Dati/Giorno_0403/calibrazioneCs_T2501_0403_histo.dat";
    
    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Cs;
    readData(filename_Na, data_Cs);

    std::string timestamp_Cs= "04.03.2025 14:30";

    TH1D* hCs=histogram(data_Cs, "hCs", "Calibrazione 137Cs", kGreen-2);
    
    TCanvas* cCs = new TCanvas("cCs","Fondo pol5");
    cCs->cd();
    hCs->Draw();
    addTimestamp(cCs, timestamp_Cs);

    // Fit 1: Solo fondo polinomiale di 5° grado
    TF1* fBkg = new TF1("fBkg", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x", 1650, 1980);
    fBkg->SetParameters(5, -0.001, 1e-7, -1e-11, 1e-15, -1e-19); // Stima iniziale
    hCs->Fit("fBkg","L","", 1650, 1980);

    // Estrai i parametri del fondo
    double p0 = fBkg->GetParameter(0);
    double p1 = fBkg->GetParameter(1);
    double p2 = fBkg->GetParameter(2);
    double p3 = fBkg->GetParameter(3);
    double p4 = fBkg->GetParameter(4);
    double p5 = fBkg->GetParameter(5);

    // Fit 2: Fondo polinomiale + prima gaussiana
    TF1* fPol1Gaus = new TF1("fPol1Gaus", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x + [6]*exp(-0.5*((x-[7])/[8])**2)", 1650, 2800);
    fPol1Gaus->SetParameters(p0, p1, p2, p3, p4, p5, 9648, 2420, 81); // Ampiezza da immagine intorno a 70, centro 1800, sigma 100

    fPol1Gaus->SetParLimits(6, 9000, 9700); // Vincola la media del ampiezza picco
    fPol1Gaus->SetParLimits(8,  60, 85); // Vincola la media del larghezza picco
    
    fPol1Gaus->SetParName(6, "A_1"); //ampiezza gaussiana
    fPol1Gaus->SetParName(7, "#mu_1"); 
    fPol1Gaus->SetParName(8, "#sigma_1");
    hCs->Fit("fPol1Gaus","L","", 1650, 2800);

    gStyle->SetOptFit(1111);

    return fPol1Gaus;
}

double calculateAIC(TF1* fitFunction) {
    TVirtualFitter* fitter = TVirtualFitter::GetFitter();
    Double_t amin, edm, errdef;
    Int_t nvpar, nparx;
    fitter->GetStats(amin, edm, errdef, nvpar, nparx);
    
    int k = fitFunction->GetNpar();
    double AIC = 2 * k + 2 * amin;
    
    return AIC;
}

void compare_fits_AIKAKE() {
    TF1* fExp1Gaus = calibration_exp();
    TF1* fPol4Gaus = calibration_pol4();
    TF1* fPol5Gaus = calibration_pol5();

    double AIC_exp = calculateAIC(fExp1Gaus);
    double AIC_pol4 = calculateAIC(fPol4Gaus);
    double AIC_pol5 = calculateAIC(fPol5Gaus);

    std::cout << "AIC modello esponenziale: " << AIC_exp << std::endl;
    std::cout << "AIC modello polinomiale di 4° grado: " << AIC_pol4 << std::endl;
    std::cout << "AIC modello polinomiale di 5° grado: " << AIC_pol5 << std::endl;

    if (AIC_pol4 < AIC_exp && AIC_pol4 < AIC_pol5) {
        std::cout << "Il modello polinomiale di 4° grado è il migliore secondo l'AIC." << std::endl;
    } else if (AIC_pol5 < AIC_exp && AIC_pol5 < AIC_pol4) {
        std::cout << "Il modello polinomiale di 5° grado è il migliore secondo l'AIC." << std::endl;
    } else {
        std::cout << "Il modello esponenziale è il migliore secondo l'AIC." << std::endl;
    }
}