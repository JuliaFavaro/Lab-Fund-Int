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

TF1* fit_exp(TH1D* hCo){
    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)", 2750, 3000);
    f1->SetParameter(0, 180); 
    f1->SetParameter(1, 9e-4);
    hCo->Fit("f1","RLM+N","",2750,3000); 

    // Estrai i parametri del fondo
    double p0 = f1->GetParameter(0);
    double p1 = f1->GetParameter(1);
    
    // Fit 2: Fondo esponenziale + prima gaussiana
    TF1* fExp1Gaus = new TF1("fExp1Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", 2750, 3500);
    fExp1Gaus->SetParameters(p0, p1, 300, 3300, 80);
    hCo->Fit("fExp1Gaus","RLM+N","", 2750, 3400); //N stands for NOT DRAWING


    double a0 = fExp1Gaus->GetParameter(0);
    double a1 = fExp1Gaus->GetParameter(1);
    double a2 = fExp1Gaus->GetParameter(2);
    double a3 = fExp1Gaus->GetParameter(3);
    double a4 = fExp1Gaus->GetParameter(4); 

    // Fit 3: Fondo esponenziale + prima gaussiana + seconda gaussiana
    TF1* fExp2Gaus = new TF1("fExp2Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)+[5]*exp(-0.5*((x-[6])/[7])**2)", 2750, 410);
    fExp2Gaus->SetParameters(p0, p1, 300, 3300, 100, 360, 3875, 142);
    hCo->Fit("fExp2Gaus","LN","", 2750, 4250);

    fExp2Gaus->SetParName(2, "A_1"); //ampiezza gaussiana
    fExp2Gaus->SetParName(3, "#mu_1"); 
    fExp2Gaus->SetParName(4, "#sigma_1");  
    fExp2Gaus->SetParName(5, "A_2"); //ampiezza gaussiana
    fExp2Gaus->SetParName(6, "#mu_2"); 
    fExp2Gaus->SetParName(7, "#sigma_2");
    
    gStyle->SetOptFit(1111);
    return fExp2Gaus;
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

    double b0 = f2->GetParameter(0);
    double b1 = f2->GetParameter(1);
    double b2 = f2->GetParameter(2);
    double b3 = f2->GetParameter(3);
    double b4 = f2->GetParameter(4); 
    double b5 = f2->GetParameter(5); 
    double b6 = f2->GetParameter(6); 
    double b7 = f2->GetParameter(7); 

    //Fit totale
    TF1* f3 = new TF1("f3", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*TMath::Gaus(x, [6], [7]) + [8]*TMath::Gaus(x, [9], [10])", 2750, 4520);
    f3->SetParameter(0, b0);
    f3->SetParameter(1, b1);
    f3->SetParameter(2, b2);
    f3->SetParameter(3, b3);
    f3->SetParameter(4, b4);
    f3->SetParameter(5, b5);
    f3->SetParameter(6, b6);
    f3->SetParameter(7, b7);

    f3->SetParameter(8, 360);
    f3->SetParameter(9, 3875);
    f3->SetParameter(10, 142);

    f3->SetParName(5, "A_1"); // Gaussian amplitude
    f3->SetParName(6, "#mu_1");
    f3->SetParName(7, "#sigma_1");
    f3->SetParName(8, "A_2"); // Gaussian amplitude
    f3->SetParName(9, "#mu_2");
    f3->SetParName(10, "#sigma_2");

    hCo->Fit("f3","L","",2750, 4520);   
    gStyle->SetOptFit(1111);
    return f3;
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
    hCo->Fit("f2","LN","", 2750, 3500);

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
    TF1* f3 = new TF1("f3", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*x^5 + [6]*TMath::Gaus(x, [7], [8]) + [9]*TMath::Gaus(x, [10], [11])", 2750, 4200);
    f3->SetParameter(0, b0);
    f3->SetParameter(1, b1);
    f3->SetParameter(2, b2);
    f3->SetParameter(3, b3);
    f3->SetParameter(4, b4);
    f3->SetParameter(5, b5);
    f3->SetParameter(6, 300);
    f3->SetParameter(7, 3300);
    f3->SetParameter(8, 140);

    f3->SetParameter(9, 360);
    f3->SetParameter(10, 3875);
    f3->SetParameter(11, 132);

    f3->SetParLimits(6, 280, 300); // Vincola ampiezza picco
    f3->SetParLimits(8,  130, 150); // Vincola larghezza picco
    f3->SetParLimits(9,  330, 390); // Vincola ampiezza picco
    f3->SetParLimits(11,  100, 140); // Vincola larghezza picco

    f3->SetParName(6, "A_1"); // Gaussian amplitude
    f3->SetParName(7, "#mu_1");
    f3->SetParName(8, "#sigma_1");
    f3->SetParName(9, "A_2"); // Gaussian amplitude
    f3->SetParName(10, "#mu_2");
    f3->SetParName(11, "#sigma_2");

    hCo->Fit("f3","LN","",2800, 4200);   
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
    std::string filename = "Dati/Acquisizione_notte_2703_67deg.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co = "27.03.2025 18:20";
    std::string duration_Co = "Durata: 93748315 ms #approx 26.0h";

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co;
    readData(filename, data_Co);

    TH1D* hCo_nonrebin = histogram_nonrebin(data_Co, "Spettro ^{60}Co. Angolo 26#circ. Distanza 47 cm", kBlue+2);
    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 26#circ. Distanza 47 cm", kBlue+2);
    
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

std::pair<double, double> evaluate_systematic_error(TH1D* hCo, double x_min, double x_max, double delta) {
    std::vector<double> mu1_values;
    std::vector<double> mu2_values;

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

    double b0 = f2->GetParameter(0);
    double b1 = f2->GetParameter(1);
    double b2 = f2->GetParameter(2);
    double b3 = f2->GetParameter(3);
    double b4 = f2->GetParameter(4); 
    double b5 = f2->GetParameter(5); 
    double b6 = f2->GetParameter(6); 
    double b7 = f2->GetParameter(7); 
    // Variazione degli estremi del dominio
    for (double dx = -delta; dx <= delta; dx += delta) { //in pratica controlla -dx, 0, dx!
        TF1* f3 = new TF1("f3", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*TMath::Gaus(x, [6], [7]) + [8]*TMath::Gaus(x, [9], [10])", x_min + dx, x_max + dx);
        f3->SetParameter(0, b0);
        f3->SetParameter(1, b1);
        f3->SetParameter(2, b2);
        f3->SetParameter(3, b3);
        f3->SetParameter(4, b4);
        f3->SetParameter(5, b5);
        f3->SetParameter(6, b6);
        f3->SetParameter(7, b7);
    
        f3->SetParameter(8, 360);
        f3->SetParameter(9, 3875);
        f3->SetParameter(10, 142);     
        hCo->Fit("f3", "LN", "", x_min + dx, x_max + dx);
        
        double mu1 = f3->GetParameter(6);
        double mu2 = f3->GetParameter(9);

        mu1_values.push_back(mu1);
        mu2_values.push_back(mu2);

        delete f3;
    }

    // Calcolo della deviazione standard delle medie
    double mean_mu1 = std::accumulate(mu1_values.begin(), mu1_values.end(), 0.0) / mu1_values.size();
    double mean_mu2 = std::accumulate(mu2_values.begin(), mu2_values.end(), 0.0) / mu2_values.size();

    double std_dev_mu1 = std::sqrt(std::accumulate(mu1_values.begin(), mu1_values.end(), 0.0, [mean_mu1](double sum, double val) {
        return sum + (val - mean_mu1) * (val - mean_mu1);
    }) / mu1_values.size());

    double std_dev_mu2 = std::sqrt(std::accumulate(mu2_values.begin(), mu2_values.end(), 0.0, [mean_mu2](double sum, double val) {
        return sum + (val - mean_mu2) * (val - mean_mu2);
    }) / mu2_values.size());

    // Calcolo dell'errore sistematico in percentuale
    double perc_error_mu1 = fabs(std_dev_mu1 / mean_mu1) * 100;
    double perc_error_mu2 = fabs(std_dev_mu2 / mean_mu2) * 100;

    return std::make_pair(std_dev_mu1, std_dev_mu2);
}

void test_systematic_error() {
    // Esempio di utilizzo
    std::string filename = "Dati//Acquisizione_notte_2703_67deg.dat";
    std::vector<double> data_Co;
    readData(filename, data_Co);
    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 22#circ. Distanza 47 cm", kBlue+2);

    double x_min = 2750;
    double x_max = 4520;

    auto errors_5= evaluate_systematic_error(hCo, x_min, x_max, 5);    
    auto errors_10 = evaluate_systematic_error(hCo, x_min, x_max, 10);
    auto errors_50 = evaluate_systematic_error(hCo, x_min, x_max, 50);
    auto errors_100 = evaluate_systematic_error(hCo, x_min, x_max, 100);
    
    std::cout << "Errore sistematico variazione di 5 canali sulla media del primo picco perc: " << errors_5.first << std::endl;
    std::cout << "Errore sistematico variazione di 5 canali sulla media del secondo picco perc: " << errors_5.second << std::endl;
    std::cout << "Errore sistematico variazione di 10 canali sulla media del primo picco perc: " << errors_10.first << std::endl;
    std::cout << "Errore sistematico variazione di 10 canali sulla media del secondo picco perc: " << errors_10.second << std::endl;
    std::cout << "Errore sistematico variazione di 50 canali sulla media del primo picco perc: " << errors_50.first << std::endl;
    std::cout << "Errore sistematico variazione di 50 canali sulla media del secondo picco perc: " << errors_50.second << std::endl;
}

int miglior_fit(){
    std::string filename = "Dati/Acquisizione_notte_2703_67deg.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co = "26.03.2025 18:20";
    std::string duration_Co = "Durata: 93748315 ms #approx 26.0h";

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co;
    readData(filename, data_Co);

    TH1D* hCo_nonrebin = histogram_nonrebin(data_Co, "Spettro ^{60}Co. Angolo 26#circ. Distanza 47 cm", kBlue+2);
    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 26#circ. Distanza 47 cm", kBlue+2);

    // Fit principale 
    TF1* fitFunction = fit_exp(hCo);
    TF1* fitFunctionpol5 = fit_pol5(hCo);
    TF1* fitFunctionpol4 = fit_pol4(hCo);

    TCanvas* cCo = new TCanvas("cCo","Istogrammi Occorrenze Canali Co");
    cCo->cd();
    hCo->Draw("E");
    addTimestamp(cCo, timestamp_Co, duration_Co);

    // Disegna il fit principale sul grafico
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

    
    // Calcola l'errore dovuto alla scelta degli estremi (delta di 5)
    auto errors_delta5 = evaluate_systematic_error(hCo, 2750, 4520, 5);
    double err_delta5_mean1 = errors_delta5.first;
    double err_delta5_mean2 = errors_delta5.second;

    // Somma in quadratura degli errori
    double total_err_mean1 = err_sist_mean1 +err_delta5_mean1;
    double total_err_mean2 = err_sist_mean2 +err_delta5_mean2;

    
    std::cout << "Gaussiana 1: Media = " << err_sist_mean1<< std::endl;
    std::cout << "Gaussiana 2: Media = " << err_sist_mean2<< std::endl;

    std::cout << "Gaussiana 1: Media = " << err_delta5_mean1<< std::endl;
    std::cout << "Gaussiana 2: Media = " << err_delta5_mean2<< std::endl;
    std::cout << "Gaussiana 1: Media = " << mean1_pol4 << " +/- " << err_stat_mean1 << " (stat) +/- " << total_err_mean1 << " (sist)" << std::endl;
    std::cout << "Gaussiana 2: Media = " << mean2_pol4 << " +/- " << err_stat_mean2 << " (stat) +/- " << total_err_mean2 << " (sist)" << std::endl;

    // Calcola l'integrale dell'istogramma hCo nella regione di interesse    
    double integral = integrateHistogram(hCo);
    std::cout << "Integrale dell'istogramma = " << integral <<" conteggi "<< std::endl;
    addIntegral(cCo, integral);

    return 0;
}