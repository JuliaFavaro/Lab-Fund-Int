#ifndef FLUX_GRAPHS10_H
#define FLUX_GRAPHS10_H

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TH1F.h>

// Funzione per fare un grafico del rate in funzione del tempo per i tre telescopi 
void Rategraph3(double& interval,int& num_intervals,std::vector<double>& t1,std::vector<double>& t2, std::vector<double> t3) {
    // Vettori per memorizzare il tempo medio di ciascun intervallo e il rate
    std::vector<double> time_intervals(num_intervals);
    std::vector<double> rates_1(num_intervals, 0);
    std::vector<double> errors_1(num_intervals, 0);
    std::vector<double> rates_2(num_intervals, 0);
    std::vector<double> errors_2(num_intervals, 0);
    std::vector<double> rates_3(num_intervals, 0);
    std::vector<double> errors_3(num_intervals, 0);

    // Scorri i dati e conta gli eventi per ciascun intervallo
    for (int i = 0; i < num_intervals; ++i) {
        double start_time = i * interval;
        double end_time = start_time + interval;
        double count_1;
        double count_2;
        double count_3;

        // Calcola il tempo medio dell'intervallo per il grafico finale
        time_intervals[i] = (start_time + end_time) / 2.0 ; 
        
        // Conta gli eventi nell'intervallo
        count_1 = std::count_if(t1.begin(), t1.end(), [start_time, end_time](double t) {
            return t >= start_time && t < end_time;
        });        
        count_2 = std::count_if(t2.begin(), t2.end(), [start_time, end_time](double t) {
            return t >= start_time && t < end_time;
        });
        count_3=std::count_if(t3.begin(),t3.end(), [start_time, end_time](double t){
            return t>=start_time && t<end_time;
        });

        // Calcola il rate come numero di eventi diviso per la larghezza dell'intervallo
        rates_1[i]= count_1/interval;
        errors_1[i] = sqrt(count_1) / interval;
        rates_2[i]= count_2/interval;
        errors_2[i] = sqrt(count_2) / interval;
        rates_3[i]= count_3/interval;
        errors_3[i] = sqrt(count_3) / interval;
    }

    // Crea il grafico del rate in funzione del tempo
    TCanvas* c1 = new TCanvas("c1", "Rate degli eventi in funzione del tempo", 800, 600);
	c1->SetGrid(); 
    c1->Divide(1, 3); //colonne, righe
	
    c1->cd(1);
    TGraphErrors* graph1 = new TGraphErrors(num_intervals, time_intervals.data(), rates_1.data(), nullptr, errors_1.data());
    graph1->SetTitle("Telescopio 06;Tempo (s);Rate (Hz)"); 
    graph1->SetMarkerStyle(8); 
    graph1->SetMarkerSize(1);
    graph1->SetMarkerColor(kRed + 2);
    graph1->SetLineColor(kRed + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph1->Draw("AP");
    
    // Calcola la media campionaria e la sua deviazione standard (dovrebbe essere la stessa cosa che fa THF1 di ROOT)
    double sum = std::accumulate(rates_1.begin(), rates_1.end(), 0.0);
    double mean_rate = sum / rates_1.size();
    double sigma_mean_rate= mean_rate/sqrt(rates_1.size());
    //Dovrei calcolare anche la deviazione standard?

    // Aggiungi media campionaria e deviazione standard al grafico
    TLatex latex;
    latex.SetTextSize(0.04);
    latex.SetNDC();
    latex.DrawLatex(0.5, 0.25, Form("Rate medio: %.0f #pm %.0f Hz", mean_rate, sigma_mean_rate ));

    c1->cd(2);
    TGraphErrors* graph2 = new TGraphErrors(num_intervals, time_intervals.data(), rates_2.data(), nullptr, errors_2.data()); //lo voglio riportare in h
    graph2->SetTitle("Telescopio 08;Tempo (s);Rate (Hz)"); 
    graph2->SetMarkerStyle(8); 
    graph2->SetMarkerSize(1);
    graph2->SetMarkerColor(kBlue + 2);
    graph2->SetLineColor(kBlue + 2);
    graph2->Draw("AP");
    
    sum = std::accumulate(rates_2.begin(), rates_2.end(), 0.0);
    mean_rate = sum / rates_2.size();
    sigma_mean_rate= mean_rate/sqrt(rates_2.size());

    // Aggiungi media campionaria e deviazione standard al grafico
    TLatex latex2;
    latex2.SetTextSize(0.04);
    latex2.SetNDC();
    latex2.DrawLatex(0.7, 0.45, Form("Rate medio: %.2f #pm %.2f Hz", mean_rate, sigma_mean_rate ));

    c1->cd(3);
    TGraphErrors* graph3 = new TGraphErrors(num_intervals, time_intervals.data(), rates_3.data(), nullptr, errors_3.data());
    graph3->SetTitle("Telescopio 04;Tempo (s);Rate (Hz)"); 
    graph3->SetMarkerStyle(8); 
    graph3->SetMarkerSize(1);
    graph3->SetMarkerColor(kGreen + 2);
    graph3->SetLineColor(kGreen + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph3->Draw("AP");
    
    // Calcola la media campionaria e la sua deviazione standard (dovrebbe essere la stessa cosa che fa THF1 di ROOT)
    sum = std::accumulate(rates_3.begin(), rates_3.end(), 0.0);
    mean_rate = sum / rates_3.size();
    sigma_mean_rate= mean_rate/sqrt(rates_3.size());
    //Dovrei calcolare anche la deviazione standard?

    // Aggiungi media campionaria e deviazione standard al grafico
    TLatex latex3;
    latex3.SetTextSize(0.04);
    latex3.SetNDC();
    latex3.DrawLatex(0.5, 0.25, Form("Rate medio: %.0f #pm %.0f Hz", mean_rate, sigma_mean_rate ));

    c1->Update();
} 

void efficiency_set06(double& interval_eff,int& num_intervals_eff,std::vector<double>& counts23, std::vector<double>& counts12, std::vector<double>& triple_06){

    std::vector<double> eff1_acc(num_intervals_eff, 0);
    std::vector<double> eff3_acc(num_intervals_eff, 0);
    std::vector<double> time_intervals(num_intervals_eff, 0);

    double w = 20e-9; // soglia: -30.0 mV
    double w_min = 2e-9;
    // Calcola le accidentali
    double acc23 = interval_eff * 118 * 370 * ((2 * w) - (2 * w_min));
    double acc12 = interval_eff* 100 * 118 * ((2 * w) - (2 * w_min));
    double acc123 = interval_eff * 103 * (acc23 / 30) * ((2 * w) - (2 * w_min));

    // Calcola l'efficienza in funzione del tempo
    for (int i = 0; i < num_intervals_eff; ++i) {
        double start_time = i * interval_eff;
        double end_time = start_time + interval_eff;

        // Calcola i conteggi per ciascun intervallo
        double count_triple = std::count_if(triple_06.begin(), triple_06.end(), [start_time, end_time](double t) {
            return t >= start_time && t < end_time;
        });
        double count_double23 = std::count_if(counts23.begin(), counts23.end(), [start_time, end_time](double t) {
            return t >= start_time && t < end_time;
        });
        double count_double12 = std::count_if(counts12.begin(), counts12.end(), [start_time, end_time](double t) {
            return t >= start_time && t < end_time;
        });

        // Calcola le efficienze
        eff1_acc[i] = count_triple / (count_double23 - acc23 - acc123);
        eff3_acc[i] = count_triple / (count_double12 - acc12 - acc123);
        time_intervals[i] = (start_time + end_time) / 2.0;
    }

    // Crea il grafico dell'efficienza in funzione del tempo
    TCanvas* c2 = new TCanvas("c2", "Efficienza PMT1 e PMT3 nel tempo", 800, 600);
    c2->SetGrid();
    c2->Divide(1, 2); // colonne, righe

    // Efficienza PMT1
    c2->cd(1);
    TGraph* graph_eff1 = new TGraph(num_intervals_eff, time_intervals.data(), eff1_acc.data());
    graph_eff1->SetTitle("Efficienza PMT1;Tempo (s);Efficienza");
    graph_eff1->SetMarkerStyle(20);
    graph_eff1->SetMarkerColor(kRed + 2);
    graph_eff1->SetLineColor(kRed + 2);
    graph_eff1->Draw("AP");

    // Efficienza PMT3
    c2->cd(2);
    TGraph* graph_eff3 = new TGraph(num_intervals_eff, time_intervals.data(), eff3_acc.data());
    graph_eff3->SetTitle("Efficienza PMT3;Tempo (s);Efficienza");
    graph_eff3->SetMarkerStyle(21);
    graph_eff3->SetMarkerColor(kBlue + 2);
    graph_eff3->SetLineColor(kBlue + 2);
    graph_eff3->Draw("AP");

    c2->Update();
}

#endif