#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <fstream>
#include <ctime>
//#include<root-framework/RDataFrame.hxx>

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>

using namespace std;

#include "read_data10.h"
//#include "flux_graphs_10.h"
//#include "atmosphere.h"


#include <vector>
#include <iostream>
#include <tuple>
#include <cmath>
#include <algorithm>

#include <TCanvas.h>
#include <TH1F.h>
#include <TAxis.h>
#include <TLegend.h>

int n_triple = 0;
int n_triple_vere = 0;
vector<double> tempi_veri;
vector<double> delta_t;
vector<double> time_123;
vector<int> ch_123;

struct DifferenzaTempo {
    double differenza;
    int canale_iniziale;
    int canale_finale;
};
struct TRIPLE {
    int canale1;
    int canale2;
    int canale3;
    double tempo1;
    double tempo2;
    double tempo3;
};


std::vector<TRIPLE> counter(const std::vector<std::vector<int>>& sequence, int j) {
    std::vector<TRIPLE> triple_vere;
    std::vector<int> list_for_one = {ch_123[j], ch_123[j + 1], ch_123[j + 2]};
    std::vector<double> tempi = {time_123[j], time_123[j + 1], time_123[j + 2]};
    
    if (std::find(sequence.begin(), sequence.end(), list_for_one) != sequence.end()) {
        n_triple++;
        if (tempi.back() - tempi.front() < 480e-9) {
            n_triple_vere++;
            tempi_veri.push_back(tempi.front());
            TRIPLE tripla = {ch_123[j], ch_123[j + 1], ch_123[j + 2], tempi[0], tempi[1], tempi[2]};
            triple_vere.push_back(tripla);
        }
    } 
    return triple_vere;
}
std::vector<double> estraiTempi(const std::vector<TRIPLE>& triple_vere) {
    std::vector<double> tempi_arrivo;
    for (const auto& tripla : triple_vere) {
        tempi_arrivo.push_back(tripla.tempo1);
    
    }
    return tempi_arrivo;
}

// Funzione per calcolare i rates e gli errori
void CalculateRates(double interval, int num_intervals, const std::vector<double>& tempi_arrivo,
    std::vector<double>& rates_sciami, std::vector<double>& errore,
    std::vector<double>& time_intervals) {

for (int i = 0; i < num_intervals; ++i) {
double start_time = i * interval;
double end_time = start_time + interval;

// Calcola il tempo medio dell'intervallo per il grafico finale
time_intervals[i] = ((start_time + end_time) / 3600) / 2; // in ore

int count = std::count_if(tempi_arrivo.begin(), tempi_arrivo.end(), [start_time, end_time](double t) {
return t > start_time && t < end_time;
});


// Calcola il rate come numero di eventi diviso per la larghezza dell'intervallo
rates_sciami[i] = count / interval;
errore[i] = sqrt(count) / interval;

}
}


// Funzione per fare un grafico del rate in funzione del tempo per i tre telescopi 

void RategraphSCIAMI(double& interval, int& num_intervals, const std::vector<double>& tempi_arrivo) {
// Vettori per memorizzare i risultati
std::vector<double> time_intervals(num_intervals);
std::vector<double> rates_sciami(num_intervals, 0);
std::vector<double> errore(num_intervals, 0);


// Calcola i rates
CalculateRates(interval, num_intervals, tempi_arrivo,
      rates_sciami, errore, time_intervals);

// Crea il grafico del rate in funzione del tempo
TCanvas* c1 = new TCanvas("c1", "Rate degli sciami in funzione del tempo", 800, 800);
c1->cd(1);
TGraphErrors* graph1 = new TGraphErrors(num_intervals, time_intervals.data(), rates_sciami.data(), nullptr, errore.data());
graph1->SetTitle("Sciami estesi;Tempo (h);Rate (Hz)"); 
graph1->SetMarkerStyle(8); 
graph1->SetMarkerSize(1);
graph1->Draw("AP");

// Calcola la media campionaria
double sum = std::accumulate(rates_sciami.begin(), rates_sciami.end(), 0.0);
double mean_rate_sciami= sum / rates_sciami.size();

// Calcola la deviazione standard dei dati
double sum_squares = std::accumulate(rates_sciami.begin(), rates_sciami.end(), 0.0,
    [mean_rate_sciami](double sum, double x) { return sum + pow(x - mean_rate_sciami, 2); });
double sigma_sciami = sqrt(sum_squares / (rates_sciami.size() - 1)); // Utilizza N-1 per la stima non distorta

// Calcola l'errore della media
double sigma_mean_rate_sciami = sigma_sciami / sqrt(rates_sciami.size());

// Calcola l'incertezza della deviazione standard
double incertezza_sigma_sciami = sigma_sciami * sqrt(1.0 / (2 * (rates_sciami.size() - 1)));

double fluctSciami = (sigma_sciami / mean_rate_sciami)* 100;
// Aggiungi media campionaria, deviazione standard e incertezza al grafico
TPaveText* pave = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
pave->SetFillColor(kWhite);
pave->SetBorderSize(1);
pave->SetTextAlign(12);
pave->AddText(Form("mean: %.2f #pm %.2f Hz", mean_rate_sciami, sigma_mean_rate_sciami));
pave->AddText(Form("std: %.2f #pm %.2f Hz", sigma_sciami, incertezza_sigma_sciami));
pave->Draw();

}
/*
//Calcola le diff di tempi in base a quali canali provengono solo per le triple vere
std::vector<TRIPLE> all_triple_vere;
void calcolaDifferenzeTempi(const std::vector<TRIPLE>& triple_vere, std::vector<double>& differenze_28, std::vector<double>& differenze_864, std::vector<double>& differenze_264) {
    for (const auto& tripla : triple_vere) {
        if ((tripla.canale1 == 2 && tripla.canale2 == 8)||(tripla.canale2 == 2 && tripla.canale1 == 8)) {
            differenze_28.push_back(tripla.tempo2 - tripla.tempo1);
        }
        if ((tripla.canale2 == 2 && tripla.canale2 == 8)||(tripla.canale3 == 2 && tripla.canale2 == 8)) {
            differenze_28.push_back(tripla.tempo3 - tripla.tempo2);
        }
        if ((tripla.canale1 == 2 && tripla.canale3 == 8)||(tripla.canale3 == 2 && tripla.canale1 == 8)) {
            differenze_28.push_back(tripla.tempo3 - tripla.tempo1);
        } //Tra 2 e 8 in tutte le combinazioni possibili

       
        if ((tripla.canale1 == 8 && tripla.canale3 == 64)||(tripla.canale3 == 8 && tripla.canale1 == 64)) {
            differenze_864.push_back(tripla.tempo3 - tripla.tempo1);
        }
        if ((tripla.canale1 == 8 && tripla.canale2 == 64)||(tripla.canale2 == 8 && tripla.canale1 == 64)) {
            differenze_864.push_back(tripla.tempo2 - tripla.tempo1);
        }
        if ((tripla.canale2 == 8 && tripla.canale3 == 64)||(tripla.canale3 == 8 && tripla.canale2 == 64)) {
            differenze_864.push_back(tripla.tempo3 - tripla.tempo2);
        } //Tra 8 e 64 in tutte le combinazioni possibili

        if ((tripla.canale1 == 2 && tripla.canale3 == 64)||(tripla.canale3 == 2 && tripla.canale1 == 64)) {
            differenze_264.push_back(tripla.tempo3 - tripla.tempo1);
        }
        if ((tripla.canale1 == 2 && tripla.canale2 == 64)||(tripla.canale2 == 2 && tripla.canale1 == 64)) {
            differenze_264.push_back(tripla.tempo2 - tripla.tempo1);
        }
        if ((tripla.canale2 == 2 && tripla.canale3 == 64)||(tripla.canale3 == 2 && tripla.canale2 == 64)) {
            differenze_264.push_back(tripla.tempo3 - tripla.tempo2);
        } //Tra 2 e 64 in tutte le combinazioni possibili
    }
}

//Ora faccio tre istogrammi che contengano le diff_ch2ch2
 
void makeHistogram(const std::vector<double>& data, int n_bins,const char* xlabel, double t_min, double t_max, const char* ylabel, const char* name){
    TCanvas* c = new TCanvas(name,name,800,600);
    TH1F* hist = new TH1F(name, name,n_bins, t_min, t_max);
    for(const auto&value : data){
        hist-> Fill(value);
    }
    hist->GetXaxis()->SetTitle(xlabel);
    hist->GetYaxis()->SetTitle(ylabel);
    hist->Draw();
    
    c->SaveAs((std::string(name)+".png").c_str());
} */
int acquisizione3giorni() {
    std::cout << "Inizio" << std::endl;

    // Nome del file di input
    std::string filename = "FIFOread_20241213-091357.txt"; // Questo funziona solo se "Dati" si trova in una sottocartella della principale

    // Vettori per memorizzare i numeri dei canali e i tempi del file
    std::vector<int> decimalNumbers;
    std::vector<double> counts;
    readData(filename, decimalNumbers, counts);

    // Sistema i tempi con i reset
    std::vector<int> Numbers;
    std::vector<double> tempi;
    reset_clock(decimalNumbers, counts, Numbers, tempi);



    std::vector<int> Canali_selezionati;
    std::vector<double> Tempi_selezionati;
    for (size_t k = 0; k < Numbers.size(); k++) {
        if (Numbers[k] == 2 || Numbers[k] == 8 || Numbers[k] == 64) {
            Canali_selezionati.push_back(Numbers[k]);
            Tempi_selezionati.push_back(tempi[k]);
        }
    }

    for (size_t m = 0; m < 10; m++) {
        std::cout << "canale selezionato: " << Canali_selezionati[m] << " tempo selezionato: " << Tempi_selezionati[m] << std::endl;
    }
    // Ho creato vettore con solo i canali 2, 8 e 64
    // Ora voglio cercare una sequenza di accensione dei canali per cercare le coincidenze triple

    // Trasferisci i dati nei vettori ch_123 e time_123
    for (size_t i = 0; i < Canali_selezionati.size(); ++i) {
        if (Canali_selezionati[i] == 2 || Canali_selezionati[i] == 8 || Canali_selezionati[i] == 64) {
            ch_123.push_back(Canali_selezionati[i]);
            time_123.push_back(Tempi_selezionati[i]);
        }
    }

    std::vector<std::vector<int>> sequences_2 = { {2, 8, 64}, {2, 64, 8} };
    std::vector<std::vector<int>> sequences_8 = { {8, 2, 64}, {8, 64, 2} };
    std::vector<std::vector<int>> sequences_64 = { {64, 2, 8}, {64, 8, 2} };


    std::vector<TRIPLE> all_triple_vere; 
    for (size_t j = 0; j < time_123.size() - 3; ++j) {
        if (ch_123[j] == 2) {
        auto triple = counter(sequences_2, j);
        all_triple_vere.insert(all_triple_vere.end(), triple.begin(),triple.end());}
            
            //counter(sequences_2, j);
        
        else if (ch_123[j] == 8) {
            auto triple = counter(sequences_8, j);
        all_triple_vere.insert(all_triple_vere.end(), triple.begin(),triple.end());
            
            //counter(sequences_8, j);
        }
        else if (ch_123[j] == 64) {
            auto triple = counter(sequences_64, j);
        all_triple_vere.insert(all_triple_vere.end(), triple.begin(),triple.end());
    }
            
            //counter(sequences_64, j);
        }
    

    std::cout << "Il numero di triple accettabili è: " << n_triple << std::endl;
    std::cout << "Il numero di triple vero è: " << n_triple_vere << std::endl;

    for (size_t i = 1; i < tempi_veri.size(); ++i) {
        delta_t.push_back(tempi_veri[i] - tempi_veri[i - 1]);
    }

    std::vector<double> tempi_arrivo = estraiTempi(all_triple_vere);
    

    // Calcola il tempo totale di acquisizione
    double time_start_total = *std::min_element(tempi.begin(), tempi.end());
    double time_end_total = *std::max_element(tempi.begin(), tempi.end());
    double total_time = time_end_total - time_start_total; //secondi
    std::cout << "Tempo totale in ore: " << total_time / (3600) << std::endl;
    double interval = 3600; //1 ora
    int num_intervals=static_cast<int>(total_time / interval);


    RategraphSCIAMI(interval, num_intervals,tempi_arrivo); 
   

   // makeHistogram(delta_t, 50, "Delta t [ns]", *std::min_element(delta_t.begin(), delta_t.end()), *std::max_element(delta_t.begin(), delta_t.end()), "counts", "distribuzione_temporale_sciami");
    // makeHistogram(tempi_veri, 50, "Tempo [s]", 0, *std::max_element(tempi_veri.begin(), tempi_veri.end()), "counts", "tempi_veri_histogram");

 /*
 // Calcola le differenze di tempo solo per le triple vere
 std::vector<double> differenze_28;
 std::vector<double> differenze_864;
 std::vector<double> differenze_264;
 calcolaDifferenzeTempi(all_triple_vere, differenze_28, differenze_864, differenze_264);
  
   

    // Stampa le differenze di tempo con i canali
    std::cout << "Differenze di tempo tra gli eventi di ciascuna sequenza:" << std::endl;
    for (const auto& diff : differenze_28) {
        std::cout << "Differenza: " << diff << " [um], Canale iniziale: 2,  Canale finale: 8"<< std::endl;
    }
    for(const auto& diff: differenze_864){
        std::cout<<"Differenza : "<<diff<<"[um], tra canale 8 e 64"<<std::endl;
    }
    for(const auto& diff: differenze_264){
        std::cout<<"Differenza : "<<diff<<"[um], tra canale 2 e 64"<<std::endl;
    }


   // Crea gli istogrammi delle differenze di tempo
   makeHistogram(differenze_28, 50, "Differenza di tempo [um]", 0, 200, "Conti", "istogramma_differenze_28");
   makeHistogram(differenze_864, 50, "Differenza di tempo [um]", 0, 200, "Conti", "istogramma_differenze_864");
   makeHistogram(differenze_264, 50, "Differenza di tempo [um]", 0, 200, "Conti", "istogramma_differenze_264");

*/
   return(0);
}


/*Devo creare un vettore che contenga il numero di triple e il tempo a cui sono scattate, poi calcolare il rate di loro*/