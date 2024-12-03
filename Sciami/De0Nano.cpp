#include "read_data.h"
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TH1F.h>
#include <TLine.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>


int main() {
    // Nome del file .dat
    std::string filename = "231123test2.dat";

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

    // Dividere i nuovi tempi in base ai canali 
    std::vector<double> t1, t2;
    for (size_t i = 0; i < new_time.size(); ++i) {
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

    // Numero di bin
    int num_bins = static_cast<int>(total_time/3600);
    std::cout<<"Tempo totale in minuti "<<total_time/3600<<std::endl;

    // Creare il grafico del rate in funzione del tempo
    TH1F *rateHist1 = new TH1F("rateHist1", "Rate degli Eventi per 10 secondi (Canale 1)", num_bins, time_start_total, time_end_total);
    TH1F *rateHist2 = new TH1F("rateHist2", "Rate degli Eventi per 10 secondi (Canale 2)", num_bins, time_start_total, time_end_total);

    // Riempire gli istogrammi
    for (const auto& t : t1) {
        rateHist1->Fill(t);
    }
    for (const auto& t : t2) {
        rateHist2->Fill(t);
    }

    // Calcolare il rate come numero di eventi per bin diviso per la larghezza del bin (10 secondi)
    for (int i = 1; i <= rateHist1->GetNbinsX(); ++i) {
        rateHist1->SetBinContent(i, rateHist1->GetBinContent(i) / 10.0);
    }
    for (int i = 1; i <= rateHist2->GetNbinsX(); ++i) {
        rateHist2->SetBinContent(i, rateHist2->GetBinContent(i) / 10.0);
    }

    gStyle->SetOptStat(0000); // Mostra solo la media e la deviazione standard 
    rateHist1->SetStats(true); // Abilita la statistica 
    rateHist2->SetStats(true); 
    // Crea canvas per gli istogrammi del rate
    TCanvas *c3 = new TCanvas("c3", "Istogrammi del Rate degli Eventi", 1500, 1500);
    c3->Divide(1, 2);

    // Disegna gli istogrammi del rate
    c3->cd(1);
    rateHist1->SetFillColor(kRed + 2);
    rateHist1->SetXTitle("Tempo (s)");
    rateHist1->SetYTitle("Rate (cps)");
    rateHist1->Draw("E");

    c3->cd(2);
    rateHist2->SetFillColor(kBlue + 2);
    rateHist2->SetXTitle("Tempo (s)");
    rateHist2->SetYTitle("Rate (cps)");
    rateHist2->Draw("E");
    return 0;
}