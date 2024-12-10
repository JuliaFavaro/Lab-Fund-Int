#ifndef FLUX_GRAPHS_H
#define FLUX_GRAPHS_H

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TH1F.h>

// Funzione per fare un grafico del rate in funzione del tempo per i tre telescopi 
void Rategraph(double& interval,int& num_intervals,std::vector<double>& t1,std::vector<double>& t2, std::vector<double> t3) {
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
        double count_1,count_2, count_3;

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
    TCanvas* c3 = new TCanvas("c3", "Rate degli eventi in funzione del tempo", 800, 600);
	c3->SetGrid(); 
    c3->Divide(1, 3); //colonne, righe
	
    c3->cd(1);
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

    c3->cd(2);
    TGraphErrors* graph2 = new TGraphErrors(num_intervals, time_intervals.data(), rates_2.data(), nullptr, errors_2.data());
    graph2->SetTitle("Telescopio 08;Tempo (s);Rate (Hz)"); 
    graph2->SetMarkerStyle(8); 
    graph2->SetMarkerSize(1);
    graph2->SetMarkerColor(kBlue + 2);
    graph2->SetLineColor(kBlue + 2);
    graph2->Draw("AP");
    
    sum = std::accumulate(rates_2.begin(), rates_2.end(), 0.0);
    mean_rate = sum / rates_2.size();
    sigma_mean_rate= mean_rate/sqrt(rates_2.size());
    //Dovrei calcolare anche la deviazione standard?

    // Aggiungi media campionaria e deviazione standard al grafico
    TLatex latex2;
    latex.SetTextSize(0.04);
    latex.SetNDC();
    latex.DrawLatex(0.7, 0.45, Form("Rate medio: %.2f #pm %.2f Hz", mean_rate, sigma_mean_rate ));

    c3->cd(3);
    TGraphErrors* graph3 = new TGraphErrors(num_intervals, time_intervals.data(), rates_1.data(), nullptr, errors_1.data());
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
    latex.SetTextSize(0.04);
    latex.SetNDC();
    latex.DrawLatex(0.5, 0.25, Form("Rate medio: %.0f #pm %.0f Hz", mean_rate, sigma_mean_rate ));

    c3->Update();
}
#endif 