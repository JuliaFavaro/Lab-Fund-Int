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

    hCo->Fit("f3","RLMI+N","",2750, 4520);   

    double c0 = f3->GetParameter(0);
    double c1 = f3->GetParameter(1);
    double c2 = f3->GetParameter(2);
    double c3 = f3->GetParameter(3);
    double c4 = f3->GetParameter(4); 
    double c5 = f3->GetParameter(5); 
    double c6 = f3->GetParameter(6); 
    double c7 = f3->GetParameter(7); 
    double c8 = f3->GetParameter(8); 
    double c9 = f3->GetParameter(9); 
    double c10 = f3->GetParameter(10); 

    TF1* f4 = new TF1("f4", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*TMath::Gaus(x, [6], [7]) + [8]*TMath::Gaus(x, [9], [10])+[11]*TMath::Gaus(x, [12], [13])", 2750, 5000);
    f4->SetParameter(0, c0);
    f4->SetParameter(1, c1);
    f4->SetParameter(2, c2);
    f4->SetParameter(3, c3);
    f4->SetParameter(4, c4);
    f4->SetParameter(5, c5);
    f4->SetParameter(6, c6);
    f4->SetParameter(7, c7);
    f4->SetParameter(8, c8);
    f4->SetParameter(9, c9);
    f4->SetParameter(10, c10);

    f4->SetParameter(11, 5);
    f4->SetParameter(12, 4500);
    f4->SetParameter(13, 100);

    f4->SetParName(5, "A_1"); // Gaussian amplitude
    f4->SetParName(6, "#mu_1");
    f4->SetParName(7, "#sigma_1");
    f4->SetParName(8, "A_2"); // Gaussian amplitude
    f4->SetParName(9, "#mu_2");
    f4->SetParName(10, "#sigma_2");
    f4->SetParName(11, "A_3"); // Gaussian amplitude
    f4->SetParName(12, "#mu_3");
    f4->SetParName(13, "#sigma_3");

    hCo->Fit("f4","L","",2750, 5000);
    gStyle->SetOptFit(1111);

    double d6 = f4->GetParameter(6); 
    double d7 = f4->GetParameter(7); 
    double d9 = f4->GetParameter(9); 
    double d10 = f4->GetParameter(10); 
    double d12 = f4->GetParameter(12); 
    double d13 = f4->GetParameter(13); 

    // Calculate integral of each Gaussian using histogram
    int bin1_low = hCo->FindBin(d6 - 3*d7);
    int bin1_high = hCo->FindBin(d6 + 3*d7);
    double gauss1_integral = hCo->Integral(bin1_low, bin1_high);

    int bin2_low = hCo->FindBin(d9 - 3*d10);
    int bin2_high = hCo->FindBin(d9 + 3*d10);
    double gauss2_integral = hCo->Integral(bin2_low, bin2_high);

    int bin3_low = hCo->FindBin(d12 - 3*d13);
    int bin3_high = hCo->FindBin(d12 + 3*d13);
    double gauss3_integral = hCo->Integral(bin3_low, bin3_high);
    
    std::cout << "Integrale Gaussiana 1: " << gauss1_integral << std::endl;
    std::cout << "Integrale Gaussiana 2: " << gauss2_integral << std::endl;
    std::cout << "Integrale Gaussiana 3: " << gauss3_integral << std::endl;

    // Calculate total integral of the histogram
    double total_integral = hCo->Integral();
    std::cout << "Integrale totale dell'istogramma = " << total_integral << " conteggi" << std::endl;

    // Calculate percentage for each Gaussian
    double percentage_gauss1 = (gauss1_integral / total_integral) * 100;
    double percentage_gauss2 = (gauss2_integral / total_integral) * 100;
    double percentage_gauss3 = (gauss3_integral / total_integral) * 100;

    std::cout << "Percentuale Gaussiana 1: " << percentage_gauss1 << "%" << std::endl;
    std::cout << "Percentuale Gaussiana 2: " << percentage_gauss2 << "%" << std::endl;
    std::cout << "Percentuale Gaussiana 3: " << percentage_gauss3 << "%" << std::endl;

    return f4;
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
    TF1* fitFunctionpol4 = fit_pol4(hCo);

    TCanvas* cCo = new TCanvas("cCo","Istogrammi Occorrenze Canali Co");
    cCo->cd();
    hCo->Draw("E");
    addTimestamp(cCo, timestamp_Co, duration_Co);

    // Disegna il fit principale sul grafico
    fitFunctionpol4->SetLineColor(kRed);
    fitFunctionpol4->Draw("same");

    double mean1_pol4 = fitFunctionpol4->GetParameter(6); 
    double err_stat_mean1 = fitFunctionpol4->GetParError(6);
    double mean2_pol4 = fitFunctionpol4->GetParameter(9);
    double err_stat_mean2 = fitFunctionpol4->GetParError(9);    
    double mean3_pol4 = fitFunctionpol4->GetParameter(12);
    double err_stat_mean3 = fitFunctionpol4->GetParError(12);

    std::cout << "Gaussiana 1: Media = " << mean1_pol4 << " +/- " << err_stat_mean1 << " (stat) " << std::endl;
    std::cout << "Gaussiana 2: Media = " << mean2_pol4 << " +/- " << err_stat_mean2 << " (stat) " << std::endl;
    std::cout << "Gaussiana 1: Media = " << mean3_pol4 << " +/- " << err_stat_mean3 << " (stat) " << std::endl;

    // Calcola l'integrale dell'istogramma hCo nella regione di interesse    
    double integral = hCo->Integral();
    std::cout << "Integrale dell'istogramma = " << integral <<" conteggi "<< std::endl;
    addIntegral(cCo, integral);

    return 0;
}