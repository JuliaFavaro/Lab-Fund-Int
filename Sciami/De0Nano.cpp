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
    std::string filename = "Set0408_1.dat";

    // Vettori per memorizzare i numeri dei canali e i tempi
    std::vector<int> canali;
    std::vector<double> tempi;

    // Leggi i dati dal file
    readData(filename, canali, tempi);
    
    //Grafica i dati originali
    TCanvas *c1 = new TCanvas("c1", "Tempi Non Corretti", 800, 600);
    TGraph *graph_tempi = new TGraph(tempi.size());
    for (size_t i = 0; i < tempi.size(); ++i) {
        graph_tempi->SetPoint(i, i, tempi[i]);
    }
    graph_tempi->SetTitle("Tempi Non Corretti;Indice;Tempo");
    graph_tempi->SetMarkerStyle(20);
    graph_tempi->Draw("AP");

    //Correggi per i random resets

    std::vector<double> new_time;

    // Intervallo di tempo
    double dt = 0.01;
    // Contatore time_start inizialmente 0
    double time_start = 0.0;

    // Assicurati che ci siano almeno 2 elementi per poter fare il confronto
    if (tempi.size() > 1) {
        // Iniziare il ciclo da 1 perché compariamo con t[i-1]
        for (size_t i = 1; i < tempi.size(); ++i) {
            if (tempi[i-1] <= tempi[i]) {
                new_time.push_back(tempi[i-1] + time_start);
            } else {
                new_time.push_back(tempi[i-1] + time_start);
                time_start += tempi[i-1] + dt - tempi[i]; //per permettere di avere una dipendenza lineare dal tempo senza discontinuità
            }
        }
        //Torno indietro di uno per aggiungere l'ultimo elemento 
        new_time.push_back(tempi.back() + time_start);
    }

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
    int num_bins = static_cast<int>(total_time / 60.0);
    std::cout<<"Tempo totale in minuti"<<total_time/60<<std::endl;

    // Creare gli istogrammi
    TH1F *hist1 = new TH1F("hist1", "Numero di eventi per 10 secondi (Canale 1)", num_bins, time_start_total, time_end_total);
    TH1F *hist2 = new TH1F("hist2", "Numero di eventi per 10 secondi (Canale 2)", num_bins, time_start_total, time_end_total);

    // Riempire gli istogrammi
    for (const auto& t : t1) {
        hist1->Fill(t);
    }
    for (const auto& t : t2) {
        hist2->Fill(t);
    }


    // Calcola i valori medi
    double mean1 = hist1->GetMean(); //andrebbe commentato il metodo con cui le ottiene
    double mean2 = hist2->GetMean();

    // Crea canvas per gli istogrammi
    TCanvas *c3 = new TCanvas("c3", "Istogrammi degli Eventi", 800, 600);
    c3->Divide(1, 2);

    // Disegna gli istogrammi
    c3->cd(1);
    hist1->SetFillColor(kRed + 2);
    hist1->SetXTitle("Tempo (s)");
    hist1->SetYTitle("Numero di Eventi");
    hist1->Draw("E");
    
    TLine *line1 = new TLine(time_start_total, mean1, time_end_total, mean1);
    line1->SetLineColor(kRed);
    line1->SetLineStyle(2); // linea tratteggiata
    line1->Draw("same");

    c3->cd(2);
    hist2->SetFillColor(kBlue + 2);
    hist2->SetXTitle("Tempo (s)");
    hist2->SetYTitle("Numero di Eventi");
     hist2->Draw("E"); //errori poissoniani per default
     
    TLine *line2 = new TLine(time_start_total, mean2, time_end_total, mean2);
    line2->SetLineColor(kRed);
    line2->SetLineStyle(2); // linea tratteggiata
    line2->Draw("same");


    return 0;
}