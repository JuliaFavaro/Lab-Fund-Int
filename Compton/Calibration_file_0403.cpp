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
    std::string filename_Cs = "Dati/Giorno_0403/calibrazioneCs_T2501_0403_histo.dat";
    std::string filename_Co = "Dati/Giorno_0403/calibrazioneCo_T25_0403_histo.dat";
    std::string filename_Na = "Dati/Giorno_0403/calibrazioneNa_T2501_0403_histo.dat";
    
    // Vettori per memorizzare le ampiezze di impulo registrate nei canali
    std::vector<double> data_Cs;
    std::vector<double> data_Co;
    std::vector<double> data_Na;
    readData(filename_Cs, data_Cs);
    readData(filename_Co, data_Co);
    readData(filename_Na, data_Na);

    // Ottenere il timestamp di creazione dei file
    std::string timestamp_Cs = "04.03.2025 14:30";
    std::string timestamp_Co = "04.03.2025 14:35";
    std::string timestamp_Na = "04.03.2025 14:40";

    //vedere i nostri grafici uno dopo l'altro
    TH1D* hCs = histogram(data_Cs, "hCs", "Calibrazione 137Cs", kGreen+2);
    TH1D* hCo = histogram(data_Co, "hCo", "Calibrazione 60Co", kBlue+2);
    TH1D* hNa = histogram(data_Na, "hNa", "Calibrazione 22Na", kRed+2);

    // Visualizzazione separata per ogni istogramma
    TCanvas* cCs = new TCanvas("cCs","Istogrammi Occorrenze Canali Cs");
    cCs->cd();
    hCs->Draw();
    addTimestamp(cCs, timestamp_Cs);
    
    // Fit 2
    TF1* f2c = new TF1("f2c", "[0] * exp([1] * x)+[2]*exp(-0.5*((x-[3])/[4])**2)", 1650, 2800);
    f2c->SetParameter(0, 1.91e5);
    f2c->SetParameter(1, -0.003);
    //f2c->SetParameter(2, a2);
    //f2c->SetParameter(3, a3);
    //f2c->SetParameter(4, a4);
    f2c->SetParameter(2, 9648);
    f2c->SetParameter(3, 2420);
    f2c->SetParameter(4, 80);
    f2c->SetParName(2, "A_1"); //ampiezza gaussiana
    f2c->SetParName(3, "#mu_1"); 
    f2c->SetParName(4, "#sigma_1");
    hCs->Fit("f2c","RLI+N","", 1650, 2800);
    gStyle->SetOptFit(1111);

    // Aggiungi il valore di mu alla lista
    double mu_Cs = f2c->GetParameter(3);
    double mu_err_Cs = f2c->GetParError(3);
    double energy_Cs = 662e3;
    
    cCs->SaveAs("Risultati/CalCs_0403.pdf");

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
   
    f2->SetParameter(6, 400);
    f2->SetParameter(7, 4150);
    f2->SetParameter(8, 70);
    f2->SetParameter(9, 296);
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

    cCo->SaveAs("Risultati/CalCo_0403.pdf");
    /*-------------------------------------------------------------------------*/

    TCanvas* cNa = new TCanvas("cNa","Istogrammi Occorrenze Canali Na");
    cNa->cd();
    hNa->Draw();
    addTimestamp(cNa, timestamp_Na);

    //Provo a fare fit esponenziale solo nella parte di fondo
    TF1 *fa = new TF1("fa", "[0]*exp(-[1]*x)", 1000, 1600);
    fa->SetParameters(10, 0.001);
    hNa->Fit("fa","L","", 2100, 3700);
    
    // Estrai i parametri del fondo
    double p0 = fa->GetParameter(0);
    double p1 = fa->GetParameter(1);

    // Fit 2: Fondo esponenziale + prima gaussiana
    TF1* fExp1Gaus = new TF1("fExp1Gaus", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2)", 1000, 2000);
    fExp1Gaus->SetParameters(p0, p1, 60, 1850, 50);
    hNa->Fit("fExp1Gaus","L","", 1000, 2000);

    double b0 = fExp1Gaus->GetParameter(0);
    double b1 = fExp1Gaus->GetParameter(1);
    double b2 = fExp1Gaus->GetParameter(2);
    double b3 = fExp1Gaus->GetParameter(3);
    double b4 = fExp1Gaus->GetParameter(4); 

    TF1* fFinal = new TF1("fFinal", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])**2) + [5]*exp(-0.5*((x-[6])/[7])**2)", 1000, 5000);
    fFinal->SetParameters(b0, b1, b2, b3, b4, 20, 4500, 75);
    fFinal->SetParLimits(6, 4200, 4800); // Vincola la media del secondo picco
    fFinal->SetParLimits(7, 50, 80); // Vincola la media del secondo picco
    hNa->Fit("fFinal","L","", 1000, 5000);
    gStyle->SetOptFit(1111);

    fFinal->SetParName(2, "A1");
    fFinal->SetParName(3, "#mu_1");
    fFinal->SetParName(4, "#sigma_1");
    fFinal->SetParName(5, "A2");
    fFinal->SetParName(6, "#mu_2");
    fFinal->SetParName(7, "#sigma_2");

    // Aggiungi i valori di mu alla lista
    double mu_Na1 = fFinal->GetParameter(3);
    double mu_err_Na1 = fFinal->GetParError(3);
    double energy_Na1 = 551e3; //keV
    double mu_Na2 = fFinal->GetParameter(6);
    double mu_err_Na2 = fFinal->GetParError(6);
    double energy_Na2 = 1102e3;

    cNa->SaveAs("Risultati/CalNa_0403.pdf");
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

    cFit->SaveAs("Risultati/Calibration_0403.pdf");
    return 0;
}