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
    
    TH1D* h1Rebinned2 = dynamic_cast<TH1D*>(h1->Rebin(4, hist_name));
    h1Rebinned2->SetLineColor(color);
    h1Rebinned2->GetXaxis()->SetTitle("Canali");
    h1Rebinned2->GetYaxis()->SetTitle("Conteggi");
    return h1Rebinned2;
}

void addTimestamp(TCanvas* canvas, const std::string& timestamp, const std::string& duration) {
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);
    latex->DrawLatex(0.1, 0.87, timestamp.c_str()); // Posizionamento in alto a sinistra del grafico
    latex->DrawLatex(0.1, 0.83, duration.c_str()); // Posizionamento sotto il timestamp
}

double integrateHistogram(TH1D* histogram) {
    int minBin = histogram->GetMinimumBin();
    int maxBin = histogram->GetMaximumBin();
    double integral = histogram->Integral(minBin, maxBin);
    return integral;
}

void addIntegral(TCanvas* canvas, double integral) {
    // Function to add integral value to the canvas
    TText* text = new TText();
    text->SetNDC();
    text->SetTextSize(0.04);
    std::string integralText = "Integrale: " +std::to_string(static_cast<int>(integral)) + " counts";
    text->DrawText(0.1, 0.78, integralText.c_str());
}

TF1* fit_exp(TH1D* hCo ){
    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)", 2900, 3150);
    f1->SetParameter(0, 180); 
    f1->SetParameter(1, 9e-4); // Tasso di decadimento per la regione 2800-3100
    hCo->Fit("f1","RL+N","",2900,3150); 

    // Estrai i parametri del fondo
    double p0 = f1->GetParameter(0);
    double p1 = f1->GetParameter(1);
    
    // Fit 2: Fondo esponenziale + prima gaussiana
    TF1* fExp1Gaus = new TF1("fExp1Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", 2900, 3600);
    fExp1Gaus->SetParameters(p0, p1, 70, 3450, 50);
    hCo->Fit("fExp1Gaus","RLM+N","", 2900, 3600);

    double a0 = fExp1Gaus->GetParameter(0);
    double a1 = fExp1Gaus->GetParameter(1);
    double a2 = fExp1Gaus->GetParameter(2);
    double a3 = fExp1Gaus->GetParameter(3);
    double a4 = fExp1Gaus->GetParameter(4); 

    // Fit 3: Fondo esponenziale + prima gaussiana + seconda gaussiana
    TF1* fExp2Gaus = new TF1("fExp2Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)+[5]*exp(-0.5*((x-[6])/[7])**2)", 2900, 5000);
    fExp2Gaus->SetParameters(p0, p1, 40, 3483, 199, 51, 3875, 142);
    hCo->Fit("fExp2Gaus","LN","", 2900, 5000);

    fExp2Gaus->SetParName(2, "A_1"); //ampiezza gaussiana
    fExp2Gaus->SetParName(3, "#mu_1"); 
    fExp2Gaus->SetParName(4, "#sigma_1");  
    fExp2Gaus->SetParName(5, "A_2"); //ampiezza gaussiana
    fExp2Gaus->SetParName(6, "#mu_2"); 
    fExp2Gaus->SetParName(7, "#sigma_2");
    
    gStyle->SetOptFit(1111);
    return fExp2Gaus;
}

TF1* fit_pol4(TH1D* hCo ){
    // Fit a fifth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4", 2800, 3200);
    f1->SetParameter(0, 50);
    f1->SetParameter(1, 1e-2);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-7);
    f1->SetParameter(4, 1e-11);
    hCo->Fit("f1","RLMI+N","",2800, 3200); 

    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4); 

    // Fit 2
    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*exp(-0.5*((x-[6])/[7])**2)", 2800, 3850);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, 40);
    f2->SetParameter(6, 3283);
    f2->SetParameter(7, 199);
    hCo->Fit("f2","RLMI+N","", 2800, 3850);

    double b0 = f2->GetParameter(0);
    double b1 = f2->GetParameter(1);
    double b2 = f2->GetParameter(2);
    double b3 = f2->GetParameter(3);
    double b4 = f2->GetParameter(4); 
    double b5 = f2->GetParameter(5); 
    double b6 = f2->GetParameter(6); 
    double b7 = f2->GetParameter(7); 

    //Fit totale
    TF1* f3 = new TF1("f3", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*TMath::Gaus(x, [6], [7]) + [8]*TMath::Gaus(x, [9], [10])", 2800, 4520);
    f3->SetParameter(0, a0);
    f3->SetParameter(1, a1);
    f3->SetParameter(2, a2);
    f3->SetParameter(3, a3);
    f3->SetParameter(4, a4);
    f3->SetParameter(5, b5);
    f3->SetParameter(6, b6);
    f3->SetParameter(7, b7);

    f3->SetParameter(8, 51);
    f3->SetParameter(9, 3875);
    f3->SetParameter(10, 142);

    f3->SetParName(5, "A_1"); // Gaussian amplitude
    f3->SetParName(6, "#mu_1");
    f3->SetParName(7, "#sigma_1");
    f3->SetParName(8, "A_2"); // Gaussian amplitude
    f3->SetParName(9, "#mu_2");
    f3->SetParName(10, "#sigma_2");

    hCo->Fit("f3","L","",2800, 4520);   
    gStyle->SetOptFit(1111);
    return f3;
}

TF1* fit_pol5(TH1D* hCo){

    // Fit a fifth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5", 2900, 3150);
    f1->SetParameter(0, 50);
    f1->SetParameter(1, 1e-2);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-7);
    f1->SetParameter(4, 1e-11);
    f1->SetParameter(5, 1e-14);
    hCo->Fit("f1","RLM+N","",2900, 3150); 

    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4); 
    double a5 = f1->GetParameter(5); 

    // Fit 2
    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5+ [6]*exp(-0.5*((x-[7])/[8])**2)", 2900, 3683);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, a5);
    f2->SetParameter(6, 40);
    f2->SetParameter(7, 3283);
    f2->SetParameter(8, 199);
    hCo->Fit("f2","RLM+N","", 2900, 3683);

    double b0 = f2->GetParameter(0);
    double b1 = f2->GetParameter(1);
    double b2 = f2->GetParameter(2);
    double b3 = f2->GetParameter(3);
    double b4 = f2->GetParameter(4); 
    double b5 = f2->GetParameter(5); 
    double b6 = f2->GetParameter(6); 
    double b7 = f2->GetParameter(7); 
    double b8 = f2->GetParameter(8); 

    //Fit totale
    TF1* f3 = new TF1("f3", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5 + [6]*TMath::Gaus(x, [7], [8]) + [9]*TMath::Gaus(x, [10], [11])", 2900, 4480);
    f3->SetParameter(0, b0);
    f3->SetParameter(1, b1);
    f3->SetParameter(2, b2);
    f3->SetParameter(3, b3);
    f3->SetParameter(4, b4);
    f3->SetParameter(5, b5);
    f3->SetParameter(6, b6);
    f3->SetParameter(7, b7-200);
    f3->SetParameter(8, b8);

    f3->SetParameter(9, 51);
    f3->SetParameter(10, 3875);
    f3->SetParameter(11, 142);

    f3->SetParName(6, "A_1"); // Gaussian amplitude
    f3->SetParName(7, "#mu_1");
    f3->SetParName(8, "#sigma_1");
    f3->SetParName(9, "A_2"); // Gaussian amplitude
    f3->SetParName(10, "#mu_2");
    f3->SetParName(11, "#sigma_2");

    hCo->Fit("f3","L","",2800, 4600);   
    gStyle->SetOptFit(1111);
    return f3;
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

void compare_fits_AIC() {
    std::string filename = "Dati/Acquisizione_notte_1203_47cm_histo.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co = "12.03.2025 18:20";
    std::string duration_Co = "Durata: 93748315 ms #approx 26.0h";

    // Vettori per memorizzare le ampiezze di impulso registrate nei canali
    std::vector<double> data_Co;
    readData(filename, data_Co);

    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 22#circ. Distanza 47 cm", kBlue+2);

    // Fit principale esponenziale
    TF1* fitFunctionExp = fit_exp(hCo);
    TF1* fitFunctionPol5 = fit_pol5(hCo);
    TF1* fitFunctionPol4 = fit_pol4(hCo);

    // Calcolo dell'AIC per ciascun fit
    double AIC_exp = calculateAIC(fitFunctionExp);
    double AIC_pol5 = calculateAIC(fitFunctionPol5);
    double AIC_pol4 = calculateAIC(fitFunctionPol4);

    // Stampa i valori dell'AIC
    std::cout << "AIC modello esponenziale: " << AIC_exp << std::endl;
    std::cout << "AIC modello polinomiale di 5° grado: " << AIC_pol5 << std::endl;
    std::cout << "AIC modello polinomiale di 4° grado: " << AIC_pol4 << std::endl;
}

int miglior_fit(){
    std::string filename = "Dati/Acquisizione_notte_1203_47cm_histo.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co = "12.03.2025 18:20";
    std::string duration_Co = "Durata: 93748315  ms #approx 26.0h";

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co;
    readData(filename, data_Co);

    TH1D* hCo_nonrebin = histogram_nonrebin(data_Co, "Spettro ^{60}Co. Angolo 22#circ. Distanza 47 cm", kBlue+2);
    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 22#circ. Distanza 47 cm", kBlue+2);

    // Fit principale esponenziale
    TF1* fitFunction = fit_exp(hCo);
    TF1* fitFunctionpol5 = fit_pol5(hCo);
    TF1* fitFunctionpol4 = fit_pol4(hCo);

    TCanvas* cCo = new TCanvas("cCo","Istogrammi Occorrenze Canali Co");
    cCo->cd();
    hCo->Draw();
    addTimestamp(cCo, timestamp_Co, duration_Co);

    // Disegna il fit esponenziale sovrapposto sul grafico
    fitFunctionpol4->SetLineColor(kRed);
    fitFunctionpol4->Draw("same");

    // Calcola l'errore statistico e sistematico per ogni gaussiana
    double mean1_exp = fitFunction->GetParameter(3);   
    double mean2_exp = fitFunction->GetParameter(6);

    double mean1_pol5 = fitFunctionpol5->GetParameter(7);
    double mean2_pol5 = fitFunctionpol5->GetParameter(10);
    
    double mean1_pol4 = fitFunctionpol4->GetParameter(6); 
    double err_stat_mean1 = fitFunctionpol4->GetParError(6);
    double mean2_pol4 = fitFunctionpol4->GetParameter(9);
    double err_stat_mean2 = fitFunctionpol4->GetParError(9);

    // Calcola i valori assoluti degli scarti utilizzando diversi 
    double diff1_1 = fabs(mean1_pol4 - mean1_pol5);
    double diff1_2 = fabs(mean1_exp - mean1_pol5);
    double diff1_3 = fabs(mean1_exp - mean1_pol4);
    double max_diff1 = std::max({diff1_1, diff1_2, diff1_3});
    double min_diff1 = std::min({diff1_1, diff1_2, diff1_3});

    // Calcola i valori assoluti delle differenze
    double diff2_1 = fabs(mean2_pol4 - mean2_pol5);
    double diff2_2 = fabs(mean2_exp - mean2_pol5);
    double diff2_3 = fabs(mean2_exp - mean2_pol4);
    double max_diff2 = std::max({diff2_1, diff2_2, diff2_3});
    double min_diff2 = std::min({diff2_1, diff2_2, diff2_3});

    double err_sist_mean1 = (max_diff1-min_diff1) / 2.0;
    double err_sist_mean2 = (max_diff2-min_diff2) / 2.0;

    std::cout << "Gaussiana 1: Media = " << mean1_pol4 << " +/- " << err_stat_mean1 << " (stat) +/- " << err_sist_mean1 << " (sist)" << std::endl;
    std::cout << "Gaussiana 2: Media = " << mean2_pol4 << " +/- " << err_stat_mean2 << " (stat) +/- " << err_sist_mean2 << " (sist)" << std::endl;

    // Calcola l'integrale dell'istogramma hCo nella regione di interesse    
    double integral = integrateHistogram(hCo);
    std::cout << "Integrale dell'istogramma = " << integral <<" conteggi "<< std::endl;
    addIntegral(cCo, integral);

    return 0;
}