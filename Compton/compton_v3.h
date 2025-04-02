#ifndef COMPTON_H
#define COMPTON_H


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


//libreria dove definisco le funzioni da usare

//voglio leggere i dati che mi arrivano dalla CaenN957

//Calibrazione Cobalto
struct Cobalto{
    double MediaPicco1Co;
    double MediaPicco2Co;
    double Err_Co1;
    double Err_Co2;
};

Cobalto CalibrazioneCoInizio(std::ifstream &fileCo) {
    std::string linea;
    Cobalto risultatoCoIn{0.0, 0.0, 0.0, 0.0}; 

    if (!fileCo.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoCoIn;
    }

    std::vector<double> data;
    double value;

    // Legge i dati dal file
    while (fileCo>> value) {
        data.push_back(value);
    }
    // Creare un istogramma
    int nbins = data.size(); // Numero di bin
    TH1D *histCo1 = new TH1D("histCo1", "Calibrazione_Co_Inizio", nbins, 0, nbins);

    // Riempire l'istogramma con i dati
    for (size_t i = 0; i < data.size(); ++i) {
        histCo1->SetBinContent(i+1, data[i]);
    }
   
   TCanvas *calibCoIn = new TCanvas("calibCoIn", "CalibCoInizio", 800, 600);
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
    histCo1->Fit("f4","","",3250, 4950); 
    gStyle->SetOptFit(15);
    calibCoIn->cd();
    histCo1->Draw();
    calibCoIn->Update();
   
 risultatoCoIn.MediaPicco1Co = f4->GetParameter(3); //media picco2
 risultatoCoIn.MediaPicco2Co = f4->GetParameter(6); //media picco2
 return risultatoCoIn; 
}

Cobalto CalibrazioneCoFine(std::ifstream &fileCo) {
    std::string linea;
    Cobalto risultatoCoFin{0.0, 0.0, 0.0, 0.0}; 

    if (!fileCo.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoCoFin;
    }

    std::vector<double> data;
    double value;

    while (fileCo>> value) {
        data.push_back(value);
    }
    
    int nbins = data.size(); // Numero di bin
    TH1D *histCo2 = new TH1D("histCo2", "Calibrazione_Co_Fine", nbins, 0, nbins);

    for (size_t i = 0; i < data.size(); ++i) {
        histCo2->SetBinContent(i+1, data[i]);
    }
   
    TCanvas *CalibFine = new TCanvas("calibFin", "CalibCo_Fine", 800, 600);
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
    histCo2->Fit("f4","","",3250, 4950); 
    gStyle->SetOptFit(15);
    CalibFine->cd();
    histCo2->Draw();
    CalibFine->Update();

 risultatoCoFin.MediaPicco1Co = f4->GetParameter(3); //media picco2
 risultatoCoFin.MediaPicco2Co = f4->GetParameter(6); //media picco2
 return risultatoCoFin; 
}

struct Americio{
    double MediaPiccoAm; 
    double Errore_Am;
};  

Americio CalibrazioneAmIn(std::ifstream &fileAm){
    std::string linea;
    Americio risultatoAmIn{0.0, 0.0};

    if (!fileAm.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoAmIn;
    }
    std::vector<double> data;
    double value;
    
   while (fileAm>> value) {
      data.push_back(value);
    }

  int nbins = data.size();
    
  TH1D *histAmIn = new TH1D("histAmIn", "Istogramma_calibrazione_Am_INIZIO", nbins, 0, nbins);
    
  for (size_t i = 0; i < data.size(); ++i) {
     histAmIn->SetBinContent(i+1, data[i]);
  }
  TCanvas *cAmInizio = new TCanvas("cAmINizioCalib", "Istogramma_calibrazione_Am", 800, 600);
  
  TF1* f3 = new TF1("f3", "[0]*exp(-0.5*((x-[1])/[2])^2)",100, 300);
  f3->SetParameter(0, 2073.2); //ci metto quelli trovati dal primo fit esponenziale
  f3->SetParameter(1, 8e-4);
  f3->SetParameter(2, 500);
  f3->SetParameter(3, 4720); //media che mi aspetto vista dal grafico precedente
  f3->SetParameter(4, 70);
  f3->SetParName(0, "A"); //ampiezza gaussiana
  f3->SetParName(1, "media"); 
  f3->SetParName(2, "sigma");
  gStyle->SetOptFit(15);
  cAmInizio->cd();
  histAmIn->Draw();   
  histAmIn->Fit("f3","","",100, 300); 
  cAmInizio->Update();

  risultatoAmIn.MediaPiccoAm = f3->GetParameter(1);
  std::cout << "Media del primo picco (tra i due file): " << risultatoAmIn.MediaPiccoAm<< std::endl;
  return risultatoAmIn; 
}

Americio CalibrazioneAmFin(std::ifstream &fileAm){
    std::string linea;
    Americio risultatoAmFin{0.0, 0.0};

    if (!fileAm.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoAmFin;
    }
    std::vector<double> data;
    double value;
    
   while (fileAm>> value) {
      data.push_back(value);
    }

  int nbins = data.size();
    
  TH1D *histAmFin = new TH1D("histAmFine", "Istogramma_calibrazione_Am_FINE", nbins, 0, nbins);
    
  for (size_t i = 0; i < data.size(); ++i) {
     histAmFin->SetBinContent(i+1, data[i]);
  }
  TCanvas *cAmericFine = new TCanvas("cAmFINECalib", "Istogramma_calibrazione_Am", 800, 600);
  
  TF1* f3 = new TF1("f3", "[0]*exp(-0.5*((x-[1])/[2])^2)",100, 300);
  f3->SetParameter(0, 2073.2); //ci metto quelli trovati dal primo fit esponenziale
  f3->SetParameter(1, 8e-4);
  f3->SetParameter(2, 500);
  f3->SetParameter(3, 4720); //media che mi aspetto vista dal grafico precedente
  f3->SetParameter(4, 70);
  f3->SetParName(0, "A"); //ampiezza gaussiana
  f3->SetParName(1, "media"); 
  f3->SetParName(2, "sigma");
  gStyle->SetOptFit(15);
  cAmericFine->cd();
  histAmFin->Draw();   
  histAmFin->Fit("f3","","",100, 300); 
  cAmericFine->Update();


  risultatoAmFin.MediaPiccoAm = f3->GetParameter(1);
  std::cout << "Media del picco da fine calib: " << risultatoAmFin.MediaPiccoAm<< std::endl;
  return risultatoAmFin; 
}

struct Cesio{
    double MediaPiccoCs;
    double Err_Cs;
} ; 

Cesio CalibrazioneCsIn (std::ifstream &fileCs){
    std::string linea;
    Cesio risultatoCsIn{0.0,0.0};

    if (!fileCs.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoCsIn;
    }
  std::vector<double> data;
   double value;

   while (fileCs>> value) {
      data.push_back(value);
   }

 TCanvas *cCesioIn = new TCanvas("cCesioIn", "Istogramma_calibrazione_Cs_INIZIO", 800, 600);
  cCesioIn->cd();

   int nbins = data.size();
   TH1D *histCsIn = new TH1D("histCsIn", "Istogramma_calibrazioneCs", nbins, 0, nbins);
    
  for (size_t i = 0; i < data.size(); ++i) {
   histCsIn->SetBinContent(i+1, data[i]);
 }
   histCsIn->Draw();
  TF1* f3= new TF1("f3", "[0]*exp(-0.5*((x-[1])/[2])^2)",2100, 2700);
    
    f3->SetParameter(0, 1300);
    f3->SetParLimits(0,1300,10000);
    f3->SetParameter(1, 210);
    f3->SetParameter(2,50);
    f3->SetParName(0, "A"); 
    f3->SetParName(1, "media"); 
    f3->SetParName(2, "sigma");
    
  gStyle->SetOptFit(15);
  histCsIn->Fit("f3","","",2100, 2700);
  risultatoCsIn.MediaPiccoCs =f3->GetParameter(1);
  std::cout << "Media del primo picco (tra i due file): " << risultatoCsIn.MediaPiccoCs << std::endl;
  return risultatoCsIn; 
}

Cesio CalibrazioneCsFin (std::ifstream &fileCs){
    std::string linea;
    Cesio risultatoCsFin{0.0,0.0};

    if (!fileCs.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoCsFin;
    }
  std::vector<double> data;
   double value;

   while (fileCs>> value) {
      data.push_back(value);
   }

 TCanvas *cCesioFin = new TCanvas("cCesioIn", "Istogramma_calibrazione_Cs_FINE", 800, 600);
  cCesioFin->cd();

   int nbins = data.size();
   TH1D *histCsFin = new TH1D("histCsFin", "Istogramma_calibrazioneCs_FINE", nbins, 0, nbins);
    
  for (size_t i = 0; i < data.size(); ++i) {
   histCsFin->SetBinContent(i+1, data[i]);
 }
   histCsFin->Draw();
  TF1* f3= new TF1("f3", "[0]*exp(-0.5*((x-[1])/[2])^2)",2100, 2700);
    
    f3->SetParameter(0, 1300);
    f3->SetParLimits(0,1300,10000);
    f3->SetParameter(1, 210);
    f3->SetParameter(2,50);
    f3->SetParName(0, "A"); 
    f3->SetParName(1, "media"); 
    f3->SetParName(2, "sigma");
    
  gStyle->SetOptFit(15);
  histCsFin->Fit("f3","","",2100, 2700);
  risultatoCsFin.MediaPiccoCs =f3->GetParameter(1);
  std::cout << "Media del Cesio finale " << risultatoCsFin.MediaPiccoCs << std::endl;
  return risultatoCsFin; 
}

struct Sodio{
    double MediaPiccoNa;
    double Err_Na;
};

Sodio CalibrazioneNaIn(std::ifstream &fileNa){ 
    std::string linea;
    Sodio risultatoNaIn{0.0,0.0};

    if (!fileNa.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoNaIn;
    }
   
  std::vector<double> data;
  double value;

   while (fileNa>> value) {
      data.push_back(value);
     }
    
 int nbins = data.size();
   TH1D *histNaIn = new TH1D("histNaIn", "Istogramma_calibrazioneNa", nbins, 0, nbins);
    
    for (size_t i = 0; i < data.size(); ++i) {
        histNaIn->SetBinContent(i+1, data[i]);
    }
    TCanvas *cSodioIn = new TCanvas("cSodioIn", "calibrazione_Na_INIZIO", 800, 600);
    histNaIn->Draw();

  TF1* f3 = new TF1("f3", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])^2)+ [5]*exp(-0.5*((x-[6])/[7])^2)",30, 4950);
    
  f3->SetParameter(0, 112.17); //ci metto quelli trovati dal primo fit esponenziale
  f3->SetParameter(1, 0.00174);
  f3->SetParameter(2, 45);
  f3->SetParameter(3, 1847.37);
  f3->SetParameter(4, 64.5);
  f3->SetParameter(5, 2.86);
 //f3->SetParameter(6, 5804.75); //media ricavata dal fit singolo
 //f3->SetParameter(7, 108);
     
  f3->SetParName(2, "A1"); //ampiezza gaussiana
  f3->SetParName(3, "media1"); 
  f3->SetParName(4, "sigma1");
  f3->SetParName(5, "A2"); //ampiezza gaussiana
  f3->SetParName(6, "media2"); 
  f3->SetParName(7, "sigma2");
 histNaIn->Fit("f3","","",30, 4950); 
 risultatoNaIn.MediaPiccoNa=f3->GetParameter(3);
 std::cout << "Media del Picco Na INIZIO: " << risultatoNaIn.MediaPiccoNa << std::endl;
 return risultatoNaIn; 
}

Sodio CalibrazioneNaFin(std::ifstream &fileNa){ 
    std::string linea;
    Sodio risultatoNaFin{0.0,0.0};

    if (!fileNa.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoNaFin;
    }
   
  std::vector<double> data;
  double value;

   while (fileNa>> value) {
      data.push_back(value);
     }
    
 int nbins = data.size();
   TH1D *histNaFin = new TH1D("histNaFin", "Istogramma_calibrazioneNa_Fine", nbins, 0, nbins);
    
    for (size_t i = 0; i < data.size(); ++i) {
        histNaFin->SetBinContent(i+1, data[i]);
    }
    TCanvas *cSodioFin = new TCanvas("cSodioFin", "calibrazione_Na_FINE", 800, 600);
    histNaFin->Draw();

  TF1* f3 = new TF1("f3", "[0]*exp(-[1]*x) + [2]*exp(-0.5*((x-[3])/[4])^2)+ [5]*exp(-0.5*((x-[6])/[7])^2)",30, 4950);
    
  f3->SetParameter(0, 112.17); //ci metto quelli trovati dal primo fit esponenziale
  f3->SetParameter(1, 0.00174);
  f3->SetParameter(2, 45);
  f3->SetParameter(3, 1847.37);
  f3->SetParameter(4, 64.5);
  f3->SetParameter(5, 2.86);
 //f3->SetParameter(6, 5804.75); //media ricavata dal fit singolo
 //f3->SetParameter(7, 108);
     
  f3->SetParName(2, "A1"); //ampiezza gaussiana
  f3->SetParName(3, "media1"); 
  f3->SetParName(4, "sigma1");
  f3->SetParName(5, "A2"); //ampiezza gaussiana
  f3->SetParName(6, "media2"); 
  f3->SetParName(7, "sigma2");
 histNaFin->Fit("f3","","",30, 4950); 
 risultatoNaFin.MediaPiccoNa=f3->GetParameter(3);
 std::cout << "Media del Picco Na FINALE: " << risultatoNaFin.MediaPiccoNa << std::endl;
 return risultatoNaFin; 
}

struct Compton{
    double Picco1Co;
    double Picco2Co; 
    double Err_1;
    double Err_2;
};

Compton SpettroCo(std::ifstream &file) {
    std::string linea;
    Compton risultatoCompton{ 0.0 , 0.0, 0.0, 0.0 };

    if (!file.is_open()) {
        std::cerr << "Errore: il file non è aperto!" << std::endl;
        return risultatoCompton;
    }

    std::vector<double> data;
    double value;

    while (file >> value) {
        data.push_back(value);
    }


    TCanvas *cspettro = new TCanvas("cspettro", "Effetto_Compton_47cm'", 800, 600);
    cspettro->cd();
    
    int nbins = data.size();
    TH1D *hspettro = new TH1D("spettroCo", "Effetto_Compton_47cm", nbins, 0, nbins);

    for (size_t i = 0; i < data.size(); ++i) {
        hspettro->SetBinContent(i+1, data[i]);
    }
    double eventiTot = hspettro->Integral(); 
    std::cout << "Numero totale di counts nell'istogramma: " << eventiTot << std::endl;
    hspettro->Draw();
  TF1* f5 = new TF1("f5", "pol9  + [10]*exp(-0.5*((x-[11])/[12])^2)+ [13]*exp(-0.5*((x-[14])/[15])^2)",2700, 4500);
 
   //f5->SetParLimits(0,3000, 6000);
   //f5->SetParameter(1,-4.775); 
   //f5->SetParameter(2, 0.001473);
   //f5->SetParameter(3, -1.497e-07); 
   //f5->SetParameter(4, -32.4);
   f5->SetParLimits(11, 3300, 3800);
   f5->SetParLimits(12, 50, 120);
   f5->SetParLimits(14, 4000, 4300);
   f5->SetParLimits(15, 50, 120); 
   
   f5->SetParName(10, "A1"); 
   f5->SetParName(11, "media1"); 
   f5->SetParName(12, "sigma1");
   f5->SetParName(13, "A2"); 
   f5->SetParName(14, "media2"); 
   f5->SetParName(15, "sigma2");

   hspettro->Fit("f5","R","",2700, 4700); 

   gStyle->SetOptFit(15);

  
   cspettro->Update();

   risultatoCompton.Picco1Co = f5->GetParameter(11);    
   risultatoCompton.Picco2Co = f5->GetParameter(14);  

   std::cout << "MediaPicco1_Co_misurata: " << risultatoCompton.Picco1Co << std::endl;
   std::cout << "MediaPicco2_Co_misurata: " << risultatoCompton.Picco2Co << std::endl;
  return risultatoCompton; 
}

#endif
