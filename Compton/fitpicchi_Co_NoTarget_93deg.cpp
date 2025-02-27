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



int acquisizioneCo93deg() {
    std::ifstream file("histo_NOtarget_93deg.dat"); //organizzo il file in ingresso in un vettore di data
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
    TH1D *hist = new TH1D("hist", "Istogramma_calibrazioneCo_93deg", nbins, 0, nbins);

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < data.size(); ++i) {
        hist->SetBinContent(i+1, data[i]);
    }

    // Definisci i limiti del range
    double xmin = 3300;
    double xmax = 4000;

    // Definisci la funzione di fit esponenziale senza impostare i parametri iniziali
    TF1 *fitFunction = new TF1("fitFunction", "[0]*TMath::Exp(-[1]*x)", xmin, xmax);
    fitFunction->SetParameter(0, 29631.1);
    fitFunction->SetParameter(1, 9e-4);

    // Esegui il fit sul range specificato
    hist->Fit("fitFunction", "", "", xmin, xmax);
    gStyle->SetOptFit(15);

    // Creare un canvas e disegnare l'istogramma
    TCanvas *c1 = new TCanvas("c1", "Istogramma_calibrazioneCo_93deg", 800, 600);
    hist->Draw();
    //fitFunction->Draw("same");
    c1->SaveAs("istogramma.png");


   

    return 0;
}
