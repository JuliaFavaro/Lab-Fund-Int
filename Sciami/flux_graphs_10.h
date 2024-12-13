#ifndef FLUX_GRAPHS10_H
#define FLUX_GRAPHS10_H

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TH1F.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

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

        // Calcola il tempo medio dell'intervallo per il grafico finale (in ore così è più facile da vederci qualcosa)
        time_intervals[i] = (start_time + end_time) / 2.0 /3600; 
        
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
    graph1->SetTitle("Telescopio 06;Tempo (h);Rate (Hz)"); 
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
    graph2->SetTitle("Telescopio 08;Tempo (h);Rate (Hz)"); 
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
    graph3->SetTitle("Telescopio 04;Tempo (h);Rate (Hz)"); 
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

// Monitoraggio in funzione del tempo dell'efficienza dei PMT del Telescopio06 con fit lineare
void efficiency_set06(double& interval_eff,int& num_intervals_eff,std::vector<double>& counts23, std::vector<double>& counts12, std::vector<double>& triple_06){
    // Vettori per memorizzare le efficienze e gli errori
    std::vector<double> eff1_acc(num_intervals_eff, 0);
    std::vector<double> eff3_acc(num_intervals_eff, 0);
    std::vector<double> eff1_err(num_intervals_eff, 0);
    std::vector<double> eff3_err(num_intervals_eff, 0);
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
        eff1_err[i] = std::sqrt((eff1_acc[i] * (1 - eff1_acc[i])) / count_double23);
        eff3_acc[i] = count_triple / (count_double12 - acc12 - acc123);
        eff3_err[i] = std::sqrt((eff3_acc[i] * (1 - eff3_acc[i])) / count_double12);
        time_intervals[i] = (start_time + end_time) / 2.0/3600;
    }

    // Crea il grafico dell'efficienza in funzione del tempo
    TCanvas* c2 = new TCanvas("c2", "Efficienza PMT1 e PMT3 nel tempo", 800, 600);
    c2->SetGrid();
    c2->Divide(1, 2); // colonne, righe

    // Efficienza PMT1
    c2->cd(1);
    TGraphErrors* graph_eff1 = new TGraphErrors(num_intervals_eff, time_intervals.data(), eff1_acc.data(), nullptr, eff1_err.data());
    graph_eff1->SetTitle("Efficienza PMT1;Tempo (h);Efficienza");
    graph_eff1->SetMarkerStyle(20);
    graph_eff1->SetMarkerColor(kRed + 2);
    graph_eff1->SetLineColor(kRed + 2);
    graph_eff1->Draw("AP");
    
    // Fit lineare per Efficienza PMT1
    TF1* lin_fit1 = new TF1("lin_fit1", "[0] + [1]*x", 0, *std::max_element(time_intervals.begin(), time_intervals.end()));
    lin_fit1->SetParName(0, "cost");  
    lin_fit1->SetParName(1, "coeff. ang.");  
    graph_eff1->Fit("lin_fit1", "R");

    // Efficienza PMT3
    c2->cd(2);
    TGraphErrors* graph_eff3 = new TGraphErrors(num_intervals_eff, time_intervals.data(), eff3_acc.data(), nullptr, eff3_err.data());
    graph_eff3->SetTitle("Efficienza PMT3;Tempo (h);Efficienza");
    graph_eff3->SetMarkerStyle(21);
    graph_eff3->SetMarkerColor(kRed + 2);
    graph_eff3->SetLineColor(kRed + 2);
    graph_eff3->Draw("AP");
    
    // Fit lineare per Efficienza PMT3
    TF1* lin_fit3 = new TF1("lin_fit3", "[0] + [1]*x", 0, *std::max_element(time_intervals.begin(), time_intervals.end()));
    lin_fit3->SetParName(0, "cost");  
    lin_fit3->SetParName(1, "coeff. ang.");  
    graph_eff3->Fit("lin_fit3", "R");
    c2->Update();
}

// Funzione per creare l'istogramma dei conteggi in un intervallo temporale fissato e fare il fit poissoniano
double histogram_fitpoiss(double& interval,int& num_intervals,std::vector <double>& times){
    // Vettore per memorizzare il conteggio degli eventi per ciascun intervallo
    std::vector<int> counts(num_intervals, 0);

    // Scorri i dati e conta gli eventi per ciascun intervallo
    for (double t : times) {
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
    hist->SetXTitle("Conteggi in 10s");
    hist->SetYTitle("Occorrenze");

    // Definisci la funzione di fit poissoniana con due parametri liberi: N e μ
    TF1* poissonFit = new TF1("poissonFit", "[0]*TMath::Poisson(x, [1])", 0, max_count);
    poissonFit->SetParameters(1, hist->GetMean()); // Stima iniziale dei parametri
    poissonFit->SetParName(0, "N");  // Rinomina il parametro 0 a "N"
    poissonFit->SetParName(1, "#mu");

    // Fai il fit di likelihood
    hist->Fit(poissonFit, "L");
    // Configura il box delle statistiche per mostrare ulteriori informazioni
    gStyle->SetOptStat(1111); // Mostra Entries, Mean, Std Dev
    gStyle->SetOptFit(1111);  // Mostra i risultati del fit inclusi Chi^2/NDF, Prob, N e λ

    // Calcola il chi quadro e il p-value
    double chi2 = poissonFit->GetChisquare();
    double ndf = poissonFit->GetNDF();
    double p_value = 1 - TMath::Prob(chi2, ndf);
    std::cout << "P-value: " << p_value << std::endl;

    // Crea il canvas e disegna l'istogramma con il fit
    TCanvas* c3 = new TCanvas("c3", "Fit Poissoniano dei Conteggi", 800, 600);
    c3->cd(1);
    hist->Draw();
    poissonFit->Draw("same");

    // Mostra il canvas
    c3->Update();
    
    // Estrazione del valore di best fit per mu
    double best_fit_rate = poissonFit->GetParameter("#mu")/static_cast<double>(10);
    std::cout << "Valore di best fit per mu: " << best_fit_rate << std::endl;
    return best_fit_rate;
}

// Funzione per creare l'istogramma degli intervalli di tempo e fare il fit esponenziale
void histogram_fitexponential(std::vector<double>& times, const double& guess_rate) {
    // Calcolo delle differenze tra i tempi
    std::vector<double> time_differences;
    for (size_t i = 1; i < times.size(); ++i) {
        time_differences.push_back(times[i] - times[i - 1]);
    }

    // Creazione dell'istogramma con 100 bins equispaziati
    TH1F* h1 = new TH1F("h1", "Differenze tra i tempi del telescopio 04;Differenza di Tempo (s);Conteggi", 100, 0, *std::max_element(time_differences.begin(), time_differences.end()));

    // Riempimento dell'istogramma con le differenze tra i tempi
    for (double t_diff : time_differences) {
        h1->Fill(t_diff);
    }

    // Creazione della funzione esponenziale per il fit
    TF1* expFit = new TF1("expFit", "[0]*exp(-x*[1])", 0, *std::max_element(time_differences.begin(), time_differences.end()));
    expFit->SetParName(0, "N");  // Rinomina il parametro 0 a "N"
    expFit->SetParName(1, "#lambda");  // Rinomina il parametro 1 a "Lambda"
    expFit->SetParameters(270e3, guess_rate); // Imposta valori iniziali per i parametri del fit: N e lambda


    // Esecuzione del fit esponenziale con il metodo di Likelihood
    h1->Fit("expFit", "L");

    // Configura il box delle statistiche per mostrare ulteriori informazioni
    gStyle->SetOptStat(1111); // Mostra Entries, Mean, Std Dev
    gStyle->SetOptFit(1111);  // Mostra i risultati del fit inclusi Chi^2/NDF, Prob, N e λ

    // Creazione del canvas e disegno dell'istogramma con il fit
    TCanvas* c4 = new TCanvas("c4", "Istogramma delle differenze tra i tempi (singolo telescopio)", 800, 600);
    c4->cd(1);
    h1->Draw();
    expFit->Draw("same");
    c4->Update();
}


#endif