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
#include "SCIAMI.h"


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
    std::cout << "Il numero di triple vero (entro 480ns) è: " << n_triple_vere << std::endl;


//    std::vector<double> tempi_arrivo = estraiTempi(all_triple_vere);
    

    // Calcola il tempo totale di acquisizione
    double time_start_total = *std::min_element(tempi.begin(), tempi.end());
    double time_end_total = *std::max_element(tempi.begin(), tempi.end());
    double total_time = time_end_total - time_start_total; //secondi
    std::cout << "Tempo totale in ore: " << total_time / (3600) << std::endl;
    double interval = 3600; //1 ora
    int num_intervals=static_cast<int>(total_time / interval);

    /*
    std::vector<double> time_intervals(num_intervals);
    std::vector<double> rates_sciami(num_intervals, 0);
    std::vector<double> errore(num_intervals, 0);
    
     RategraphSCIAMI(interval, num_intervals,tempi_arrivo); 
   for(int a=0; a<rates_sciami.size(); a++){
    std::cout<<"Rate sciami: "<< rates_sciami[a]<< std::endl;
   }
 */
 
 // Calcola le differenze di tempo solo per le triple vere
 std::vector<double> differenze_28;
 std::vector<double> differenze_864;
 std::vector<double> differenze_264;
 calcolaDifferenzeTempi(all_triple_vere, differenze_28, differenze_864, differenze_264);
  
   
/*
    // Stampa le differenze di tempo con i canali
    std::cout << "Differenze di tempo tra gli eventi di ciascuna sequenza:" << std::endl;
    for (const auto& diff : differenze_28) {
        std::cout << "Differenza: " << diff << " [s], Canale iniziale: 2,  Canale finale: 8"<< std::endl;
    }
    for(const auto& diff: differenze_864){
        std::cout<<"Differenza : "<<diff<<"[s], tra canale 8 e 64"<<std::endl;
    }
    for(const auto& diff: differenze_264){
        std::cout<<"Differenza : "<<diff<<"[s], tra canale 2 e 64"<<std::endl;
    }

*/
   // Crea gli istogrammi delle differenze di tempo
   makeHistogram(differenze_28, 120, "#Deltat [s]", 0, 5e-8, "Conteggi", "#Deltat tra Setup06 e Setup08, 71 ore");
   makeHistogram(differenze_864, 120, "#Deltat [s]", 0, 1e-7, "Conteggi", "#Deltat tra Setup08 e Setup04, 71 ore");
   makeHistogram(differenze_264, 120, "#Deltat [s]", 0, 1e-7, "Conteggi", "#Deltat tra Setup06 e Setup04, 71 ore");


double rate = calcolaRate(tempi_veri);
    if (rate > 0.0) {
        std::cout << "Il rate degli eventi è: " << rate << " eventi per unità di tempo." << std::endl;
        std::cout<< "Fine calcolo rate" << std::endl;
    }

    for (size_t i = 1; i < tempi_veri.size(); ++i) {
       // std::cout<<"Tempi_veri[i] :    "<<tempi_veri[i]<<std::endl;
        delta_t.push_back(tempi_veri[i] - tempi_veri[i - 1]);
    }
 std::vector<double> tempi_triple_da_TRIPLE;
 for(const auto&tripla : all_triple_vere){
    tempi_triple_da_TRIPLE.push_back(tripla.tempo1);
    for(int j=0; j<tempi_triple_da_TRIPLE.size(); j++){
   // std::cout<<"tempi_triple_da_TRIPLE[j]"   <<tempi_triple_da_TRIPLE[j]<<std::endl;
 } 
}
/*
    for(int h=0; h<tempi_veri.size(); h++){
        std::cout<<"Differenze tra arrivi di triple successive"<<delta_t[h]<<std::endl;
    } 
   */
    
        int num_bins = 100; // Numero di bin nell'istogramma
        double min_value = *min_element(delta_t.begin(), delta_t.end());
        double max_value = *max_element(delta_t.begin(), delta_t.end());
        TH1F* histo = new TH1F("histo", "Istogramma dei delta_t tra triple;Valore;Conteggio", num_bins, min_value, max_value);
    
        // Riempie l'istogramma con i dati dal vettore
        for (double valore : delta_t) {
            histo->Fill(valore);
        }
    
        // Crea un canvas e disegna l'istogramma
        TCanvas* c1 = new TCanvas("c1", "Istogramma dei Dati", 800, 600);
        histo->Draw();
        c1->Update();

 return 0;
 }

/*Devo creare un vettore che contenga il numero di triple e il tempo a cui sono scattate, poi calcolare il rate di loro*/