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

TF1* fit_exp(){
    std::string filename = "Dati/Acquisizione_notte_1903_47cm_75deg.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co = "19.03.2025 18:30";
    std::string duration_Co = "Durata: 51993332 ms #approx 14.4h";

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co;
    readData(filename, data_Co);

    TH1D* hCo_nonrebin = histogram_nonrebin(data_Co, "Spettro ^{60}Co. Angolo 18#circ. Distanza 47 cm", kBlue+2);
    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 18#circ. Distanza 47 cm", kBlue+2);

    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)", 2850, 3800);
    f1->SetParameter(0, 40); 
    f1->SetParameter(1, 9e-7); 
    hCo->Fit("f1","RLM+N","",2850,3750);  //N stands for NOT DRAWING

    // Estrai i parametri del fondo
    double p0 = f1->GetParameter(0);
    double p1 = f1->GetParameter(1);
    
    // Fit 2: Fondo esponenziale + prima gaussiana
    TF1* fExp1Gaus = new TF1("fExp1Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", 2750, 5000);
    fExp1Gaus->SetParameters(40, 9e-7, 40, 4200, 120);

    double a0 = fExp1Gaus->GetParameter(0);
    double a1 = fExp1Gaus->GetParameter(1);
    double a2 = fExp1Gaus->GetParameter(2);
    double a3 = fExp1Gaus->GetParameter(3);
    double a4 = fExp1Gaus->GetParameter(4); 
    
    fExp1Gaus->SetParLimits(2, 40, 60); // Vincola ampiezza picco
    fExp1Gaus->SetParLimits(4, 90, 120); // Vincola larghezza picco
    
    hCo->Fit("fExp1Gaus","RLM+","", 2900, 4500);

    fExp1Gaus->SetParName(2, "A_1"); // Gaussian amplitude
    fExp1Gaus->SetParName(3, "#mu_1");
    fExp1Gaus->SetParName(4, "#sigma_1");
    gStyle->SetOptFit(1111);
    return f1;
}

TF1* fit_pol4(TH1D* hCo){
    // Fit a fifth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4", 2750, 3000);
    f1->SetParameter(0, 50);
    f1->SetParameter(1, 1e-2);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-7);
    f1->SetParameter(4, 1e-11);
    hCo->Fit("f1","RLMI+N","",2750, 3000); 

    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4); 

    // Fit 2
    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*exp(-0.5*((x-[6])/[7])**2)", 2750, 3850);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, 300);
    f2->SetParameter(6, 3300);
    f2->SetParameter(7, 199);
    hCo->Fit("f2","RLMI+N","", 2750, 3400);

    f2->SetParName(5, "A_1"); // Gaussian amplitude
    f2->SetParName(6, "#mu_1");
    f2->SetParName(7, "#sigma_1");
    gStyle->SetOptFit(1111);
    return f2;
}

TF1* fit_pol5(TH1D* hCo){
    // Fit a fifth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5", 2750, 3000);
    f1->SetParameter(0, 50);
    f1->SetParameter(1, 1e-2);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-7);
    f1->SetParameter(4, 1e-11);
    f1->SetParameter(5, 1e-14);
    hCo->Fit("f1","RLM+N","",2750, 3000); 

    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4); 
    double a5 = f1->GetParameter(5); 

    // Fit 2
    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5+ [6]*exp(-0.5*((x-[7])/[8])**2)", 2750, 3400);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, a5);
    f2->SetParameter(6, 300);
    f2->SetParameter(7, 3300);
    f2->SetParameter(8, 159);
    
    f2->SetParName(6, "A_1"); // Gaussian amplitude
    f2->SetParName(7, "#mu_1");
    f2->SetParName(8, "#sigma_1");

    hCo->Fit("f2","LN","", 2750, 3500);

    gStyle->SetOptFit(1111);
    return f2;
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

int miglior_fit(){
    std::string filename = "Dati/Acquisizione_notte_1903_47cm_75deg.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co = "18.03.2025 18:30";
    std::string duration_Co = "Durata: 51993332 ms #approx 14.4h";

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co;
    readData(filename, data_Co);

    TH1D* hCo_nonrebin = histogram_nonrebin(data_Co, "Spettro ^{60}Co. Angolo 18#circ. Distanza 47 cm", kBlue+2);
    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 18#circ. Distanza 47 cm", kBlue+2);

    // Fit principale esponenziale
    TF1* fitFunction = fit_exp();
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