#ifndef FLUX_COINCIDENCES
#define FLUX_COINCIDENCES

#include <iostream>
#include <vector>
#include <utility>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TMath.h>

#include <iostream>
#include <vector>
#include <algorithm>

std::vector<std::pair<double, double>> findCoincidences(const std::vector<double>& t1, const std::vector<double>& t2, 
    double minTime, double maxTime){
    std::vector<std::pair<double, double>> coincidences;

    if (t1[0] < t2[0]) {
        for (const double& time1 : t1) {
            //le funzioni auto restituiscono il primo elemento nel vettore che supera minTime e maxTime
            auto lower = std::lower_bound(t2.begin(), t2.end(), time1);
            auto upper = std::upper_bound(t2.begin(), t2.end(), time1 + maxTime);
            for (auto it = lower; it <upper; ++it) {
                coincidences.push_back(std::make_pair(time1, *it));
                //std::cout << "Added pair: (" << time1 << ", " << *it << ")" << std::endl;
            }
        }
    } else {
        for (const double& time2 : t2) {
            auto lower = std::lower_bound(t1.begin(), t1.end(), time2);
            auto upper = std::upper_bound(t1.begin(), t1.end(), time2 + maxTime);
            for (auto it = lower; it < upper; ++it) {
                coincidences.push_back(std::make_pair(*it, time2));
                //std::cout << "Added pair: (" << *it << ", " << time2 << ")" << std::endl;
            }
        }
    }
    std::cout<<"Numero totale di coincidenze: "<< coincidences.size()<<std::endl;

    return coincidences;
}

void histogram(double interval, int num_intervals, const std::vector<std::pair<double, double>>& coincidences, 
    const char* hist_name, const char* title, Color_t color, int correction = 0){
    std::vector<int> counts(num_intervals, 0);

    // Scorri i dati e conta gli eventi per ciascun intervallo
    for (const auto& c : coincidences) {
        int bin = static_cast<int>(c.first / interval);
        if (bin < num_intervals) {
            counts[bin]++;
        }
    }

    double accidental_rate = 0.368; // Hz
    double expected_accidentals = accidental_rate * interval;

    // Calcolo delle coincidenze accidentali
    if (correction > 0) {
        for (int i = 0; i < num_intervals; ++i) {
            if (counts[i] > expected_accidentals) {
                counts[i] -= expected_accidentals;
            } else {
                counts[i] = 0; // Imposta a zero se il conteggio è minore o uguale
            }
        }
    }

    // Crea l'istogramma delle occorrenze dei conteggi
    int max_count = *std::max_element(counts.begin(), counts.end());
    TH1F* hist = new TH1F(hist_name, title,max_count + 1, 0, max_count + 1);
    for (int count : counts) {
        hist->Fill(count);
    }

    // Calcola gli errori come la radice quadrata dei conteggi
    for (int i = 1; i <= hist->GetNbinsX(); ++i) {
        hist->SetBinError(i, std::sqrt(static_cast<double>(hist->GetBinContent(i))));
    }
    hist->SetXTitle("Conteggi in intervallo");
    hist->SetYTitle("Occorrenze");

    // Crea il canvas e disegna l'istogramma con il fit
    TCanvas* canvaspoiss = new TCanvas("canvaspoiss", "Fit Poissoniano dei Conteggi", 800, 600);
    canvaspoiss->cd(1);
    hist->Draw();

    // Mostra il canvas
    canvaspoiss->Update();
}

// Funzione che filtra i tempi di arrivo per i conteggi accidentali per le distribuzioni dei tempi
std::vector<double> calcolaDifferenzeTempi(const std::vector<double>& tempiArrivo) {
    const double accidental_rate = 0.368; // Hz
    const double accidental_time = 1.0 / accidental_rate; // Tempo caratteristico delle accidentali in secondi

    std::vector<double> eventiRimasti;
    
    // Variabile per tenere traccia dell'ultimo evento aggiunto
    double ultimoEvento = 0;

    for (double tempo : tempiArrivo) {
        if (tempo - ultimoEvento < accidental_time) {
            eventiRimasti.push_back(tempo);
        }else{
            ultimoEvento = tempo; // Aggiorna l'ultimo evento
        }
    }
    return eventiRimasti;
}

// Funzione per creare l'istogramma delle differenze dei tempi a binning fissato e fare il fit esponenziale
void histogram_exponential(std::vector<std::pair<double, double>>& coincidences, const char* hist_name, const char* title, Color_t color, int correction = 0) {
    std::vector<double> first_times;
    for (const auto& coincidence : coincidences) {
        first_times.push_back(coincidence.first);
    }

    std::vector<double> new_times;
    if (correction > 0){
        new_times = calcolaDifferenzeTempi(first_times);
    }
    else{
        new_times = first_times;
    }

    std::vector<double> time_differences;
    for (size_t i = 1; i < new_times.size(); ++i) {
        time_differences.push_back(new_times[i] - new_times[i - 1]);
    }

    double max_time_diff = *std::max_element(time_differences.begin(), time_differences.end());
    TH1F* h1 = new TH1F(hist_name, title, 100, 0, max_time_diff);

    for (double t_diff : time_differences) {
        h1->Fill(t_diff);
    }

    h1->SetXTitle("Differenza di tempo (s)");
    h1->SetYTitle("Conteggi");
    h1->SetLineColor(color);

    TCanvas* canvasexp = new TCanvas("canvasexp",  "Istogramma delle differenze tra i tempi (2 telescopi)", 800, 600);
    canvasexp->SetLogy();
    canvasexp->cd(1);
    h1->Draw();
    canvasexp->Update();
}

// Funzione per calcolare i rates e gli errori
void CalculateRates(double interval, int num_intervals, const std::vector<std::pair<double, double>>& coincidences,
    std::vector<double>& rates, std::vector<double>& errors, std::vector<double>& time_intervals) {

    for (int i = 0; i < num_intervals; ++i) {
        double start_time = i * interval;
        double end_time = start_time + interval;

        // Calcola il tempo medio dell'intervallo per il grafico finale
        time_intervals[i] = ((start_time + end_time) / 3600) / 2; // in ore

        int count = std::count_if(coincidences.begin(), coincidences.end(), [start_time, end_time](const std::pair<double, double>& c) {
            return c.first > start_time && c.first < end_time && c.second > start_time && c.second < end_time;
        });

        // Calcola il rate come numero di eventi diviso per la larghezza dell'intervallo
        rates[i] = count / interval;
        errors[i] = sqrt(count) / interval;
    }
}

// Funzione per fare un grafico del rate in funzione del tempo per i tre telescopi 
// stampa anche su schermo alcune proprietà come variazione di flusso percentile e correlazione tra rates
void Rategraph(double interval, int num_intervals, const std::vector<std::pair<double, double>>& coincidences) {
    // Vettori per memorizzare i risultati
    std::vector<double> time_intervals(num_intervals);
    std::vector<double> rates(num_intervals, 0);
    std::vector<double> errors(num_intervals, 0);

    // Calcola i rates
    CalculateRates(interval, num_intervals, coincidences, rates, errors, time_intervals);

    // Crea il grafico del rate in funzione del tempo
    TCanvas* c1 = new TCanvas("c1", "Rate delle coincidenze in funzione del tempo", 1500, 1500);
    c1->SetGrid(); 

    TGraphErrors* graph = new TGraphErrors(num_intervals, time_intervals.data(), rates.data()*1e3, nullptr, errors.data()*1e3);
    graph->SetTitle("Coincidenze;Tempo (h);Rate (mHz)"); 
    graph->SetMarkerStyle(8); 
    graph->SetMarkerSize(1);
    graph->SetMarkerColor(kBlue + 2);
    graph->SetLineColor(kBlue + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph->Draw("AP");

    // Calcola la media campionaria
    double sum = std::accumulate(rates.begin(), rates.end(), 0.0);
    double mean_rate = sum / rates.size();

    // Calcola la deviazione standard dei dati
    double sum_squares = std::accumulate(rates.begin(), rates.end(), 0.0,
        [mean_rate](double sum, double x) { return sum + pow(x - mean_rate, 2); });
    double sigma = sqrt(sum_squares / (rates.size() - 1)); // Utilizza N-1 per la stima non distorta

    // Calcola l'errore della media
    double sigma_mean_rate = sigma / sqrt(rates.size());

    // Calcola l'incertezza della deviazione standard
    double incertezza_sigma = sigma * sqrt(1.0 / (2 * (rates.size() - 1)));

    double fluct = (sigma / mean_rate) * 100;
    // Aggiungi media campionaria, deviazione standard e incertezza al grafico
    TPaveText* pave = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
    pave->SetFillColor(kWhite);
    pave->SetBorderSize(1);
    pave->SetTextAlign(12);
    pave->AddText(Form("mean: %.2f #pm %.2f Hz", mean_rate, sigma_mean_rate));
    pave->AddText(Form("std: %.2f #pm %.2f Hz", sigma, incertezza_sigma));
    pave->Draw();

    // Calcola le fluttuazioni in percentuale
    std::cout << "Fluttuazione: " << fluct << "%" << std::endl;

    c1->Update();
}

#endif