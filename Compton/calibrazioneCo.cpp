#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>
//#include<compton.h>


int calibrazioneCo() {
    std::ifstream file("calibrazioneCo_T24_2702.dat"); //organizzo il file in ingresso in un vettore di data
    std::vector<double> data;
    double value;

    // Legge i dati dal file
    while (file >> value) {
        data.push_back(value);
    }

    // Creare un istogramma
    int nbins = data.size(); // Numero di bin
   /* double min = *std::min_element(data.begin(), data.end());
      double max = *std::max_element(data.begin(), data.end()); */
    TH1D *hist = new TH1D("hist", "Istogramma_calibrazione", nbins, 0, nbins);

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < data.size(); ++i) {
        hist->SetBinContent(i+1, data[i]);
    }

   //inizio a fare fit
   double xmin = 3250;
   double xmax = 3900;
/*
  //Provo a fare fit esponenziale 
  TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)", xmin, xmax);
  f1->SetParameter(0, 29631.1);
  f1->SetParameter(1, 9e-4);

  hist->Fit("f1", "", "", 3250, 3900);
  
  //Definisco la funzione di fit gaussiana sommata all'esponenziale
  TF1* f2 = new TF1("f2", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])^2)",3900, 4450);

  f2->SetParameter(0, 2073.2); //ci metto quelli trovati dal primo fit esponenziale
  f2->SetParameter(1, 8e-4);
  f2->SetParameter(2, 500);
  f2->SetParameter(3, 4150);
  f2->SetParameter(4, 70);


  f2->SetParName(2, "A"); //ampiezza gaussiana
  f2->SetParName(3, "media"); 
  f2->SetParName(4, "sigma");

   hist->Fit("f2","","",3900, 4450); 
  
  */

/*
//Definisco la funzione di fit gaussiana del secondo picco sommata all'esponenziale
  TF1* f3 = new TF1("f3", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])^2)",4450, 4950);

  f3->SetParameter(0, 2073.2); //ci metto quelli trovati dal primo fit esponenziale
  f3->SetParameter(1, 8e-4);
  f3->SetParameter(2, 500);
  f3->SetParameter(3, 4720); //media che mi aspetto vista dal grafico precedente
  f3->SetParameter(4, 70);


  f3->SetParName(2, "A"); //ampiezza gaussiana
  f3->SetParName(3, "media"); 
  f3->SetParName(4, "sigma");
  // Esegui il fit sul range specificato

  hist->Fit("f3","","",4450, 4950); 
  */


  
   
//Definisco la funzione di fit complessiva
  TF1* f4 = new TF1("f4", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])^2)+ [5]*exp(-0.5*((x-[6])/[7])^2)",3250, 4950);

  f4->SetParameter(0, 2073.2); //ci metto quelli trovati dal primo fit esponenziale
  f4->SetParameter(1, 8e-4);
  f4->SetParameter(2, 224);
  f4->SetParameter(3, 4168); //media ricavata dal fit singolo
  f4->SetParameter(4, 96);
  f4->SetParameter(5, 196);
  f4->SetParameter(6, 4715); //media ricavata dal fit singolo
  f4->SetParameter(7, 108);
 


  f4->SetParName(2, "A1"); //ampiezza gaussiana
  f4->SetParName(3, "media1"); 
  f4->SetParName(4, "sigma1");
  f4->SetParName(5, "A2"); //ampiezza gaussiana
  f4->SetParName(6, "media2"); 
  f4->SetParName(7, "sigma2");
  // Esegui il fit sul range specificato

  hist->Fit("f4","","",3250, 4950); 
  


   gStyle->SetOptFit(15);




    TCanvas *c1 = new TCanvas("c1", "Istogramma_calibrazioneCo", 800, 600);
    hist->Draw();

return 0; 
}