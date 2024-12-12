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
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>

#include "read_data0.h"
#include "flux_graphs.h"

int main(){
    // Nome del file .dat
    std::string filename = "Dati/Set0408_1.dat";

    // Vettori per memorizzare i numeri dei canali e i tempi
    std::vector<int> canali;
    std::vector<double> tempi;

    // Leggi i dati dal file
    readData(filename, canali, tempi);

    // Grafica i dati originali
    TCanvas *c1 = new TCanvas("c1", "Tempi Non Corretti", 800, 600);
    TGraph *graph_tempi = new TGraph(tempi.size());
    for (size_t i = 0; i < tempi.size(); ++i) {
        graph_tempi->SetPoint(i, i, tempi[i]);
    }
    graph_tempi->SetTitle("Tempi Non Corretti;Indice;Tempo");
    graph_tempi->SetMarkerStyle(20);
    graph_tempi->Draw("AP");

    // Correggi per i random resets
    std::vector<double> new_time;

    correctTimes(tempi, new_time,0.01);
    // Trova il tempo minimo per far partire l'asse dei tempi da 0 double 
    double time_start = *std::min_element(new_time.begin(), new_time.end());
    for (size_t i = 0; i < new_time.size(); ++i){ 
        new_time[i] -= time_start; 
    }

    // Dividere i nuovi tempi in base ai canali 
    std::vector<double> t1, t2;
    for (size_t i = 0; i < new_time.size(); ++i){
        if (canali[i] == 1) {
            t1.push_back(new_time[i]);
        } else if (canali[i] == 2) {
            t2.push_back(new_time[i]);
        }
    }

    TCanvas *c2 = new TCanvas("c2", "Tempi Corretti per Canale", 800, 600);
    TMultiGraph *mg = new TMultiGraph();
    TGraph *graph_t1 = new TGraph(t1.size());
    TGraph *graph_t2 = new TGraph(t2.size());

    for (size_t i = 0; i < t1.size(); ++i) {
        graph_t1->SetPoint(i, i, t1[i]);
    }
    for (size_t i = 0; i < t2.size(); ++i) {
        graph_t2->SetPoint(i, i, t2[i]);
    }

    graph_t1->SetMarkerStyle(20);
    graph_t1->SetMarkerColor(kRed + 2);
    graph_t2->SetMarkerStyle(21);
    graph_t2->SetMarkerColor(kBlue + 2);

    mg->Add(graph_t1, "P");
    mg->Add(graph_t2, "P");
    mg->SetTitle("Tempi Corretti;Indice;Tempo");
    mg->Draw("A");

    // Aggiungi una legenda
    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg->AddEntry(graph_t1, "Canale 1", "P");
    leg->AddEntry(graph_t2, "Canale 2", "P");
    leg->Draw();

    // Calcola il tempo totale di acquisizione
    double time_start_total = *std::min_element(new_time.begin(), new_time.end());
    double time_end_total = *std::max_element(new_time.begin(), new_time.end());
    double total_time = time_end_total - time_start_total;
    std::cout<<"Tempo totale in minuti "<<total_time/60<<std::endl;

    // Intervallo di 10s
    double interval = 10;

    // Numero di intervalli necessari
    int num_intervals = static_cast<int>(total_time / interval);

    //grafico del rate in funzione del tempo
    Rategraph(interval, num_intervals, t1, t2);

    /*
    // Vettore per memorizzare il conteggio degli eventi per ciascun intervallo
    std::vector<int> counts(num_intervals, 0);

    // Scorri i dati e conta gli eventi per ciascun intervallo
    for (double t : t1) {
        int bin = static_cast<int>(t / interval);
        if (bin < num_intervals) {
            counts[bin]++;
        }
    }

    // Crea l'istogramma delle occorrenze dei conteggi
    int max_count = *std::max_element(counts.begin(), counts.end());
    TH1F* hist = new TH1F("hist", "Occorrenze dei Conteggi in Intervalli di 10s", max_count + 1, 0, max_count + 1);

    for (int count : counts) {
        hist->Fill(count);
    }

    // Calcola gli errori come la radice quadrata dei conteggi
    for (int i = 1; i <= hist->GetNbinsX(); ++i) {
        hist->SetBinError(i, std::sqrt(static_cast<double>(hist->GetBinContent(i))));
    }

    // Definisci la funzione di fit poissoniana con due parametri liberi: N e μ
    TF1* poissonFit = new TF1("poissonFit", "[0]*TMath::Poisson(x, [1])", 0, max_count);
    poissonFit->SetParameters(1, hist->GetMean()); // Stima iniziale dei parametri

    // Fai il fit di likelihood
    hist->Fit(poissonFit, "L");

    // Calcola il chi quadro e il p-value
    double chi2 = poissonFit->GetChisquare();
    double ndf = poissonFit->GetNDF();
    double p_value = 1 - TMath::Prob(chi2, ndf);

    std::cout << "Chi quadro: " << chi2 << std::endl;
    std::cout << "Gradi di libertà (ndf): " << ndf << std::endl;
    std::cout << "P-value: " << p_value << std::endl;

    // Crea il canvas e disegna l'istogramma con il fit
    TCanvas* c4 = new TCanvas("c4", "Fit Poissoniano dei Conteggi", 800, 600);
    gStyle->SetOptFit(1111); // Mostra i parametri di fit e il chi quadro
    hist->Draw();
    poissonFit->Draw("same");

    // Mostra il canvas
    c4->Update();*/

    return 0;
}
