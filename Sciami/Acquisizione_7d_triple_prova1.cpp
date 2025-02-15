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
}
int acquisizione7giorni() {
    std::cout << "Inizio" << std::endl;

    // Nome del file di input
    std::string filename = "FIFOread_20241213-091357_7day.txt"; // Questo funziona solo se "Dati" si trova in una sottocartella della principale

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

   // makeHistogram(delta_t, 50, "Delta t [ns]", *std::min_element(delta_t.begin(), delta_t.end()), *std::max_element(delta_t.begin(), delta_t.end()), "counts", "distribuzione_temporale_sciami");
    // makeHistogram(tempi_veri, 50, "Tempo [s]", 0, *std::max_element(tempi_veri.begin(), tempi_veri.end()), "counts", "tempi_veri_histogram");


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


   return(0);
}