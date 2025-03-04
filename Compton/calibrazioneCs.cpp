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


int calibrazioneCs() {
    std::ifstream file("calibrazioneCs_T24_2702.dat"); //organizzo il file in ingresso in un vettore di data
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
    TH1D *hist = new TH1D("hist", "Istogramma_calibrazioneCs", nbins, 0, nbins);

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < data.size(); ++i) {
        hist->SetBinContent(i+1, data[i]);
    }

 /*
  //Provo a fare un primo fit esponenziale della scesa

  TF1 *f1 = new TF1("f1", "[0]*exp(-[1]*x)",1530 , 2500);
 
  char lambda[] = "\u03BB";
  f1->SetParName(0, "A0"); //ampiezza exp
  f1->SetParName(1, "#lambda"); 
  hist->Fit("f1", "", "", 1530, 2500);

  */

  // Crea una funzione esponenziale
  TF1 *expFit = new TF1("expFit", "[0]*exp(-[1]*x)", 1530, 2000);
  expFit->SetParameters(0, 108586); 
  expFit->SetParameters(1, 0.00401229); 

  char lambda[] = "\u03BB";
  expFit->SetParName(0, "A0");
  expFit->SetParName(1,"lambda");

  // Esegui il fit
  hist->Fit("expFit", "","", 1530, 2000);


  TCanvas *canvas = new TCanvas("canvas", "Fit esponenziale", 800, 600);
  hist->Draw("AP");
  expFit->Draw("same");

  gStyle->SetOptFit(15);

return 0; 
}