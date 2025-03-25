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

std::string convertResolution(double radian_resolution) {
    double degree_resolution = radian_resolution * (180.0 / TMath::Pi());
    return "Risoluzione angolare: " + std::to_string(degree_resolution).substr(0, 3) + "#circ";
}

void addResolution(TCanvas* canvas, const std::string& resolution) {
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.03);
    latex->DrawLatex(0.1, 0.78, resolution.c_str()); // Posizionamento in alto a sinistra del grafico
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

int distanza_sorgente() {
    std::string filename = "Dati/Compton_T25_115deg_1303_47cm.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co = "12.03.2025 18:20";
    std::string duration_Co = "Durata: 93748315 ms #approx 26.8h";

    double rad_res = 1.0 / 47.0; // risoluzione angolare in radianti
    std::string resolution = convertResolution(rad_res);

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co_47cm;
    readData(filename, data_Co_47cm);

    TH1D* hCo = histogram(data_Co_47cm, "hCo", "Spettro ^{60}Co. Angolo 25#circ. Distanza 47 cm", kBlue+2);

    // Creare una canvas per le acquisizioni 
    TCanvas* c1h47 = new TCanvas("c1h47", "Istogrammi Occorrenze Canali Co - Durata 26h");
    hCo->Draw();
    addTimestamp(c1h47, timestamp_Co,duration_Co);
    addResolution(c1h47, resolution);

    /*--------------------------------*/
    std::string filename2 = "Dati/Acquisizione_notte_0503_51cm_histo.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co2 = "04.03.2025 18:20";
    std::string duration_Co2 = "Durata: 96651319 ms #approx 26.8h";

    double rad_res2 = 1.0 / 51.5; // risoluzione angolare in radianti
    std::string resolution2 = convertResolution(rad_res2);

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co_51cm;
    readData(filename2, data_Co_51cm);

    TH1D* hCo2 = histogram(data_Co_51cm, "hCo2", "Spettro ^{60}Co. Angolo 25#circ. Distanza 51.5 cm", kBlue+2);
    TF1* fitFunction = fit_exp(hCo2);

    TCanvas* c1h51 = new TCanvas("c1h51", "Istogrammi Occorrenze Canali Co - Durata 27h");
    hCo2->Draw();
    addTimestamp(c1h51, timestamp_Co2,duration_Co2);
    addResolution(c1h51, resolution2);
    
    fitFunction->SetLineColor(kRed);
    fitFunction->Draw("same"); // Disegna la funzione di fit sovrapposta sul grafico

    
    /*--------------------------------*/
    std::string filename3 = "Dati/Giorno_0403/Compton_T25_115deg_0403_40cm_v2.dat";

    // Timestamp di creazione del file
    std::string timestamp_Co3 = "04.03.2025 17:20";
    std::string duration_Co3 = "Durata #approx 1h";

    double rad_res3 = 1.0 / 40; // risoluzione angolare in radianti
    std::string resolution3 = convertResolution(rad_res2);

    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Co_40cm;
    readData(filename3, data_Co_40cm);
    
    TH1D* hCo3 = histogram(data_Co_40cm, "hCo3", "Spettro ^{60}Co. Angolo 25#circ. Distanza 40 cm", kBlue+2);

    TCanvas* c1h512 = new TCanvas("c1h51", "Istogrammi Occorrenze Canali Co - Durata 27h");
    hCo3->Draw();
    addTimestamp(c1h512, timestamp_Co3,duration_Co3);
    addResolution(c1h512, resolution3);
    return 0;
}
