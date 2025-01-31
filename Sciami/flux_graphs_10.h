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

// Funzione per calcolare i rates e gli errori
void CalculateRates(double interval, int num_intervals, const std::vector<double>& t1,
                    const std::vector<double>& t2, const std::vector<double>& t3,
                    std::vector<double>& rates_1, std::vector<double>& errors_1,
                    std::vector<double>& rates_2, std::vector<double>& errors_2,
                    std::vector<double>& rates_3, std::vector<double>& errors_3,
                    std::vector<double>& time_intervals) {
    
    for (int i = 0; i < num_intervals; ++i) {
        double start_time = i * interval;
        double end_time = start_time + interval;

        // Calcola il tempo medio dell'intervallo per il grafico finale
        time_intervals[i] = ((start_time + end_time) / 3600) / 2; // in ore

        int count_1 = std::count_if(t1.begin(), t1.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });

        int count_2 = std::count_if(t2.begin(), t2.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });

        int count_3 = std::count_if(t3.begin(), t3.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });

        // Calcola il rate come numero di eventi diviso per la larghezza dell'intervallo
        rates_1[i] = count_1 / interval;
        errors_1[i] = sqrt(count_1) / interval;
        rates_2[i] = count_2 / interval - 367e-3; // Correggere per il rumore di fondo (367 mHz)
        errors_2[i] = sqrt(count_2) / interval + sqrt(pow(2e-3, 2));
        rates_3[i] = count_3 / interval;
        errors_3[i] = sqrt(count_3) / interval;
    }
}

// Funzione per calcolare il coefficiente di correlazione di Pearson
double pearsonCorrelation(const std::vector<double>& v1, const std::vector<double>& v2) {
    double mean1 = std::accumulate(v1.begin(), v1.end(), 0.0) / v1.size();
    double mean2 = std::accumulate(v2.begin(), v2.end(), 0.0) / v2.size();

    double cov = std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0,
        [](double sum, double x) { return sum + x; },
        [mean1, mean2](double x, double y) { return (x - mean1) * (y - mean2); }) / v1.size();

    double std1 = std::sqrt(std::accumulate(v1.begin(), v1.end(), 0.0,
        [mean1](double sum, double x) { return sum + std::pow(x - mean1, 2); }) / v1.size());
    double std2 = std::sqrt(std::accumulate(v2.begin(), v2.end(), 0.0,
        [mean2](double sum, double x) { return sum + std::pow(x - mean2, 2); }) / v2.size());

    return cov / (std1 * std2);
}

// Funzione per fare un grafico del rate in funzione del tempo per i tre telescopi 
// stampa anche su schermo alcune proprietà come variazione di flusso percentile e correlazione tra rates
void Rategraph3(double& interval, int& num_intervals, const std::vector<double>& t1,
                 const std::vector<double>& t2, const std::vector<double>& t3) {
    // Vettori per memorizzare i risultati
    std::vector<double> time_intervals(num_intervals);
    std::vector<double> rates_1(num_intervals, 0);
    std::vector<double> errors_1(num_intervals, 0);
    std::vector<double> rates_2(num_intervals, 0);
    std::vector<double> errors_2(num_intervals, 0);
    std::vector<double> rates_3(num_intervals, 0);
    std::vector<double> errors_3(num_intervals, 0);

    // Calcola i rates
    CalculateRates(interval, num_intervals, t1, t2, t3,
                   rates_1, errors_1, rates_2, errors_2,
                   rates_3, errors_3, time_intervals);

    // Crea il grafico del rate in funzione del tempo
    TCanvas* c1 = new TCanvas("c1", "Rate degli eventi in funzione del tempo", 1500, 1500);
	c1->SetGrid(); 
    c1->Divide(1, 3); //colonne, righe
	
    c1->cd(1);
    TGraphErrors* graph1 = new TGraphErrors(num_intervals, time_intervals.data(), rates_1.data(), nullptr, errors_1.data());
    graph1->SetTitle("Telescopio 06;Tempo (h);Rate (Hz)"); 
    graph1->SetMarkerStyle(8); 
    graph1->SetMarkerSize(1);
    graph1->SetMarkerColor(kBlue + 2);
    graph1->SetLineColor(kBlue + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph1->Draw("AP");
    
    // Calcola la media campionaria
    double sum = std::accumulate(rates_1.begin(), rates_1.end(), 0.0);
    double mean_rate_06= sum / rates_1.size();

    // Calcola la deviazione standard dei dati
    double sum_squares = std::accumulate(rates_1.begin(), rates_1.end(), 0.0,
        [mean_rate_06](double sum, double x) { return sum + pow(x - mean_rate_06, 2); });
    double sigma_06 = sqrt(sum_squares / (rates_1.size() - 1)); // Utilizza N-1 per la stima non distorta

    // Calcola l'errore della media
    double sigma_mean_rate_06 = sigma_06 / sqrt(rates_1.size());

    // Calcola l'incertezza della deviazione standard
    double incertezza_sigma_06 = sigma_06 * sqrt(1.0 / (2 * (rates_1.size() - 1)));

    double fluctSetup06 = (sigma_06 / mean_rate_06)* 100;
    // Aggiungi media campionaria, deviazione standard e incertezza al grafico
    TPaveText* pave = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
    pave->SetFillColor(kWhite);
    pave->SetBorderSize(1);
    pave->SetTextAlign(12);
    pave->AddText(Form("mean: %.2f #pm %.2f Hz", mean_rate_06, sigma_mean_rate_06));
    pave->AddText(Form("std: %.2f #pm %.2f Hz", sigma_06, incertezza_sigma_06));
    pave->Draw();
    /*----------------------------------------------------------------------------------------------*/

    c1->cd(2);
    TGraphErrors* graph2 = new TGraphErrors(num_intervals, time_intervals.data(), rates_2.data(), nullptr, errors_2.data()); //lo voglio riportare in h
    graph2->SetTitle("Telescopio 08;Tempo (h);Rate (Hz)"); 
    graph2->SetMarkerStyle(8); 
    graph2->SetMarkerSize(1);
    graph2->SetMarkerColor(kRed + 2);
    graph2->SetLineColor(kRed + 2);
    graph2->Draw("AP");

    // Calcola la media campionaria
    sum = std::accumulate(rates_2.begin(), rates_2.end(), 0.0);
    double mean_rate_08 = sum / rates_2.size();

    // Calcola la deviazione standard dei dati
    sum_squares = std::accumulate(rates_2.begin(), rates_2.end(), 0.0,
        [mean_rate_08](double sum, double x) { return sum + pow(x - mean_rate_08, 2); });
    double sigma_08 = sqrt(sum_squares / (rates_2.size() - 1)); // Utilizza N-1 per la stima non distorta

    // Calcola l'errore della media
    double sigma_mean_rate_08 = sigma_08 / sqrt(rates_2.size());

    // Calcola l'incertezza della deviazione standard
    double incertezza_sigma_08 = sigma_08 * sqrt(1.0 / (2 * (rates_2.size() - 1)));
    
    double fluctSetup08 = (sigma_08 / mean_rate_08)* 100;

    // Aggiungi media campionaria, deviazione standard e incertezza al grafico
    TPaveText* pave2 = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
    pave2->SetFillColor(kWhite);
    pave2->SetBorderSize(1);
    pave2->SetTextAlign(12);
    pave2->AddText(Form("mean: %.2f #pm %.2f Hz", mean_rate_08, sigma_mean_rate_08));
    pave2->AddText(Form("std: %.2f #pm %.2f Hz", sigma_08, incertezza_sigma_08));
    pave2->Draw();
    /*----------------------------------------------------------------------------------------------*/

    c1->cd(3);
    TGraphErrors* graph3 = new TGraphErrors(num_intervals, time_intervals.data(), rates_3.data(), nullptr, errors_3.data());
    graph3->SetTitle("Telescopio 04;Tempo (h);Rate (Hz)"); 
    graph3->SetMarkerStyle(8); 
    graph3->SetMarkerSize(1);
    graph3->SetMarkerColor(kGreen + 2);
    graph3->SetLineColor(kGreen + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph3->Draw("AP");
    
    // Calcola la media campionaria
    sum = std::accumulate(rates_3.begin(), rates_3.end(), 0.0);
    double mean_rate_04 = sum / rates_3.size();

    // Calcola la deviazione standard dei dati
    sum_squares = std::accumulate(rates_3.begin(), rates_3.end(), 0.0,
        [mean_rate_04](double sum, double x) { return sum + pow(x - mean_rate_04, 2); });
    double sigma_04 = sqrt(sum_squares / (rates_3.size() - 1)); // Utilizza N-1 per la stima non distorta

    // Calcola l'errore della media
    double sigma_mean_rate_04 = sigma_04 / sqrt(rates_3.size());

    // Calcola l'incertezza della deviazione standard
    double incertezza_sigma_04 = sigma_04 * sqrt(1.0 / (2 * (rates_3.size() - 1)));

    double fluctSetup04 = (sigma_04 / mean_rate_04)* 100;

    // Aggiungi media campionaria, deviazione standard e incertezza al grafico
    TPaveText* pave3 = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
    pave3->SetFillColor(kWhite);
    pave3->SetBorderSize(1);
    pave3->SetTextAlign(12);
    pave3->AddText(Form("mean: %.2f #pm %.2f Hz", mean_rate_04, sigma_mean_rate_04 ));
    pave3->AddText(Form("std: %.2f #pm %.2f Hz", sigma_04, incertezza_sigma_04));
    pave3->Draw();
    /*----------------------------------------------------------------------------------------------*/
    // Calcola le fluttuazioni in percentuale
    std::cout << "Fluttuazione Setup06: " << fluctSetup06 << "%" << std::endl;
    std::cout << "Fluttuazione Setup08: " << fluctSetup08 << "%" << std::endl;
    std::cout << "Fluttuazione Setup04: " << fluctSetup04 << "%" << std::endl;

    // Calcola le correlazioni
    double corrSetup06Setup08 = pearsonCorrelation(rates_1, rates_2);
    double corrSetup06Stanza2004 = pearsonCorrelation(rates_1, rates_3);
    double corrSetup08Stanza2004 = pearsonCorrelation(rates_2, rates_3);

    std::cout << "Correlazione Setup06-Setup08: " << corrSetup06Setup08 << std::endl;
    std::cout << "Correlazione Setup06-Stanza2004: " << corrSetup06Stanza2004 << std::endl;
    std::cout << "Correlazione Setup08-Stanza2004: " << corrSetup08Stanza2004 << std::endl;

    c1->Update();
    
    c1->SaveAs("Risultati/Rateneltempo_7d.pdf");
} 

// Funzione per creare l'istogramma dei conteggi in un intervallo temporale fissato e fare il fit poissoniano
void histogram_fitpoiss(double interval,int num_intervals,std::vector <double>& times,const char* hist_name, const char* title, Color_t color, int correction=0){
    std::vector<int> counts(num_intervals, 0);

    // Scorri i dati e conta gli eventi per ciascun intervallo
    for (double t : times) {
        int bin = static_cast<int>(t / interval);
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
                counts[i] -=expected_accidentals;
            } else {
                counts[i] = 0; // Imposta a zero se il conteggio è minore o uguale
            }
        }
    }

    // Crea l'istogramma delle occorrenze dei conteggi
    int max_count = *std::max_element(counts.begin(), counts.end());
    TH1F* hist = new TH1F(hist_name, title, max_count + 1, 0, max_count + 1);

    for (int count : counts) {
        hist->Fill(count);
    }

    // Calcola gli errori come la radice quadrata dei conteggi
    for (int i = 1; i <= hist->GetNbinsX(); ++i) {
        hist->SetBinError(i, std::sqrt(static_cast<double>(hist->GetBinContent(i))));
    }
    hist->SetXTitle("Conteggi in 10 s");
    hist->SetYTitle("Occorrenze");

    // Definisci la funzione di fit poissoniana con due parametri liberi: N e μ
    TF1* poissonFit = new TF1("poissonFit", "[0]*TMath::Poisson(x, [1])", 0, max_count);
    poissonFit->SetParameters(1, hist->GetMean()); // Stima iniziale dei parametri
    poissonFit->SetParName(0, "N");  // Rinomina il parametro 0 a "N"
    poissonFit->SetParName(1, "#mu");
    poissonFit->SetLineColor(color); // Imposta il colore della linea di fit

    // Fai il fit di likelihood
    hist->Fit(poissonFit, "L");
    // Configura il box delle statistiche per mostrare ulteriori informazioni
    gStyle->SetOptStat(1111); // Mostra Entries, Mean, Std Dev
    gStyle->SetOptFit(1111);  // Mostra i risultati del fit inclusi Chi^2/NDF, Prob, N e λ

    double mu = poissonFit->GetParameter(1);
    double muError = poissonFit->GetParError(1);
    // Stampa mu con 5 cifre significative
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Valore di mu: " << mu << " ± " << muError << std::endl;

    // Crea il canvas e disegna l'istogramma con il fit
    TCanvas* canvaspoiss = new TCanvas("canvaspoiss", "Fit Poissoniano dei Conteggi", 800, 600);
    canvaspoiss->cd(1);
    hist->Draw();
    poissonFit->Draw("same");

    // Mostra il canvas
    canvaspoiss->Update();
    
    // Estrazione del valore di best fit per mu
    double best_fit_rate = poissonFit->GetParameter("#mu")/static_cast<double>(10);
    double sigma_best_fit_rate=poissonFit->GetParError(1)/static_cast<double>(10);
    std::cout << "Valore di best fit per il rate: " << best_fit_rate << " +- "<< sigma_best_fit_rate<<" Hz"<< std::endl;
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
void histogram_fitexponential(std::vector<double>& times, const char* hist_name, const char* title, Color_t color,int correction=0) {
    std::vector<double> new_times;
    if (correction>0){
        new_times=calcolaDifferenzeTempi(times);
    }else{
        new_times=times;
    }

    // Calcolo delle differenze tra i tempi
    std::vector<double> time_differences;
    for (size_t i = 1; i < new_times.size(); ++i) {
        time_differences.push_back(new_times[i] - new_times[i - 1]);
    }

    // Creazione dell'istogramma con 100 bins equispaziati
    double max_time_diff = *std::max_element(time_differences.begin(), time_differences.end());
    TH1F* h1 = new TH1F(hist_name, title, 100, 0, max_time_diff);

    // Riempimento dell'istogramma con le differenze tra i tempis
    for (double t_diff : time_differences) {
        h1->Fill(t_diff);
    }

    h1->SetXTitle("Differenza di tempo (s)");
    h1->SetYTitle("Conteggi");

    /*
    // Creazione della funzione esponenziale per il fit
    TF1* expFit = new TF1("expFit", "[0]*exp(-x*[1])", 0, max_time_diff);
    expFit->SetParName(0, "N");
    expFit->SetParName(1, "#lambda");

    //Impostazione dei valori iniziali per i parametri del fit
    double max_occurrences = h1->GetMaximum(); // p0: massimo delle occorrenze
    expFit->SetParameter(0, max_occurrences);   // Imposta p0

    // Calcolo della media delle differenze di tempo per stimare p1
    double mean_time_diff = h1->GetMean();
    expFit->SetParameter(1, 1.0 / mean_time_diff); // Imposta p1

    // Esecuzione del fit esponenziale con il metodo di Likelihood
    h1->Fit("expFit", "L");

    // Configura il box delle statistiche
    gStyle->SetOptStat(1111);
    gStyle->SetOptFit(1011);*/

    
    // Creazione del canvas e disegno dell'istogramma con il fit
    TCanvas* canvasexp = new TCanvas("canvasexp", "Istogramma delle differenze tra i tempi (singolo telescopio)", 800, 600);
    
    canvasexp->SetLogy();
    canvasexp->cd(1);
    h1->Draw();
    //expFit->Draw("same");
    //expFit->SetLineColor(color);
    canvasexp->Update();
    
    /*
    double best_fit_rate = expFit->GetParameter("#lambda");
    std::cout<< best_fit_rate<<std::endl;
    double sigma_best_fit_rate=expFit->GetParError(1);
    std::cout << "Valore di best fit per la media: " << 1/best_fit_rate << " +- "<< sigma_best_fit_rate/pow(best_fit_rate,2)<< std::endl;*/
}

void histogram_fitlinear(std::vector<double>& times, const char* hist_name, const char* title, Color_t color, int correction = 0) {
    // Calcolo delle differenze tra i tempi
    std::vector<double> new_times = (correction > 0) ? calcolaDifferenzeTempi(times) : times;

    std::vector<double> time_differences;
    for (size_t i = 1; i < new_times.size(); ++i) {
        time_differences.push_back(new_times[i] - new_times[i - 1]);
    }

    // Creazione dell'istogramma con 100 bins equispaziati
    double max_time_diff = *std::max_element(time_differences.begin(), time_differences.end());
    TH1F* h1 = new TH1F(hist_name, title, 100, 0, max_time_diff);

    // Riempimento dell'istogramma con le differenze di tempo
    for (double t_diff : time_differences) {
        h1->Fill(t_diff);
    }

    h1->SetXTitle("Differenza di tempo (s)");
    h1->SetYTitle("Conteggi");

    // Creazione del canvas e disegno dell'istogramma
    TCanvas* canvaslin = new TCanvas("canvaslin", "Istogramma delle differenze tra i tempi (singolo telescopio)", 1000, 800);
    
    canvaslin->SetLogy(); // Imposta l'asse y in scala logaritmica
    canvaslin->cd(1);
    
    h1->Draw();

    // Creazione dei punti per il fit lineare
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> y_errors; // Errori per il grafico

    for (int i = 1; i <= h1->GetNbinsX(); ++i) {
        double binContent = h1->GetBinContent(i);
        if (binContent > 0) { // Considera solo i bin con conteggi maggiori di zero
            x.push_back(h1->GetBinCenter(i));
            y.push_back(log(binContent)); // Logaritmo delle occorrenze
            y_errors.push_back(sqrt(binContent)/binContent); // Imposta un errore costante, ad esempio 1
        }
    }

    // Creazione del grafico TGraphErrors per il fit
    TGraphErrors *graphErrors = new TGraphErrors(x.size(), x.data(), y.data(), nullptr, y_errors.data());

    // Creazione della funzione lineare per il fit
    TF1* linearFit = new TF1("linearFit", "[0] + [1]*x", 0, max_time_diff);
    linearFit->SetParName(0, "cost");  
    linearFit->SetParName(1, "coeff. ang.");  

    // Esecuzione del fit lineare
    graphErrors->Fit(linearFit, "L");

    // Disegno della funzione di fit
    linearFit->SetLineColor(color);
    
    // Creazione di un nuovo canvas per il grafico delle occorrenze
    TCanvas* canvasOccorrenze = new TCanvas("canvasOccorrenze", "Grafico semilog delle differenze tra i tempi (singolo telescopio)", 1000, 800);

    graphErrors->Draw("AP"); // Disegna il grafico con punti e errori
    graphErrors->GetXaxis()->SetTitle("Differenza di tempo (s)");
    graphErrors->GetYaxis()->SetTitle("ln(conteggi)");
    graphErrors->SetTitle(hist_name);

    // Sovrapponi la linea di fit al grafico delle occorrenze
    linearFit->Draw("same");

    // Estrai parametri dal fit
    double intercept = linearFit->GetParameter(0);
    double intercept_err = linearFit->GetParError(0);
    double slope = linearFit->GetParameter(1);
    double slope_err = linearFit->GetParError(1);

    // Calcola chi-quadro ridotto
    double chi_square = linearFit->GetChisquare();
    int ndf = x.size() - linearFit->GetNpar(); // numero di gradi di libertà
    double reduced_chi_square = chi_square / ndf;

    // Calcolo del p-value
    double p_value = TMath::Prob(chi_square, ndf);

    TPaveText* pave = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
    pave->SetFillColor(kWhite);
    pave->SetBorderSize(1);
    
    pave->AddText(Form("#lambda: %.3f #pm %.3f Hz ", slope, slope_err));
    pave->AddText(Form("#chi^{2}/ndf: %.2f", reduced_chi_square));
    
   // Aggiungi il p-value al box di statistiche
   pave->AddText(Form("p-value: %.3f", p_value));

   pave->Draw();
    canvasOccorrenze->Update();
}

#endif