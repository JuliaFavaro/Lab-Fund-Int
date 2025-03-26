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
#include "compton_v3.h"


int Compton2003_v3(){ 
  std::ifstream CalibCoIn("../Compton/DATI/Calibrazioni/Calib_1803_fine/Calib_Co_1803_finepome_histo_T251.dat");
  Cobalto Cobalto_calib_In = CalibrazioneCoInizio(CalibCoIn);
  std::cout << "Media Picco1Co Calib Inizio: " << Cobalto_calib_In.MediaPicco1Co << std::endl;
  std::cout << "Media Picco2Co Calib Inizio: " << Cobalto_calib_In.MediaPicco2Co<< std::endl;
  std::ifstream CalibCoFin("../Compton/DATI/Calibrazioni/Calib_1803_fine/Calib_Co_1803_finepome_histo_T251.dat");
  Cobalto Cobalto_calib_Fin = CalibrazioneCoFine(CalibCoFin);
  std::cout << "Media Picco1Co Calib Fine: " << Cobalto_calib_Fin.MediaPicco1Co << std::endl;
  std::cout << "Media Picco2Co Calib Fine: " << Cobalto_calib_Fin.MediaPicco2Co<< std::endl;
  double MediaCobalto1_Calib; 
  MediaCobalto1_Calib= (Cobalto_calib_In.MediaPicco1Co + Cobalto_calib_Fin.MediaPicco1Co)/2.0;
  double MediaCobalto2_Calib; 
  MediaCobalto2_Calib= (Cobalto_calib_In.MediaPicco2Co + Cobalto_calib_Fin.MediaPicco2Co)/2.0;
  std::cout << "Media Picco1Co Calib Totale: " << MediaCobalto1_Calib << std::endl;
  std::cout << "Media Picco2Co Calib Totale: " << MediaCobalto2_Calib<< std::endl;
  std::cout<< "Sono riuscito a fare la media"<<std::endl; 
  double Err_Co1;
  Err_Co1 = TMath::Abs((Cobalto_calib_In.MediaPicco1Co - Cobalto_calib_Fin.MediaPicco1Co)/2.0);
  std::cout << "Errore Picco1 Co Picco2Co Calib Totale: " << Err_Co1 << std::endl;
  double Err_Co2;
  Err_Co2= TMath::Abs((Cobalto_calib_In.MediaPicco2Co - Cobalto_calib_Fin.MediaPicco2Co)/2.0);
  std::cout << "Errore Picco2 Co Picco2Co Calib Totale: " << Err_Co2 << std::endl;
 
  
  std::ifstream CalibAmIn("../Compton/DATI/Calibrazioni/Calib_1903_fine/Calib_Am_1903_T279_histo_v2.dat");
  Americio Americio_calib_In = CalibrazioneAmIn(CalibAmIn);
  std::cout << "MediaPiccoAm_iniziale: " << Americio_calib_In.MediaPiccoAm<< std::endl;
  std::ifstream CalibAmFin("../Compton/DATI/Calibrazioni/Calib_1903_fine/Calib_Am_1903_T279_histo_v2.dat");
  Americio Americio_calib_Fin = CalibrazioneAmFin(CalibAmFin);
  std::cout << "MediaPiccoAm_finale: " << Americio_calib_Fin.MediaPiccoAm<< std::endl;
  double MediaAm; 
  MediaAm= (Americio_calib_In.MediaPiccoAm + Americio_calib_Fin.MediaPiccoAm)/2.0;
  std::cout << "Media Picco Am Calib Totale: " << MediaAm << std::endl;
  std::cout<< "Sono riuscito a fare la media"<<std::endl; 
  double Err_Am;
  Err_Am = TMath::Abs((Americio_calib_In.MediaPiccoAm - Americio_calib_Fin.MediaPiccoAm)/2.0);
  std::cout << "Errore Am Calib Totale: " << Err_Am << std::endl;


  std::ifstream CalibCsIn("../Compton/DATI/Calibrazioni/Calib_1903_fine/Calib_Cs_1903_T275_histo_v2.dat");
  Cesio Cesio_calibIn = CalibrazioneCsIn(CalibCsIn);
  std::cout << "MediaPiccoCs_INIZIO: " << Cesio_calibIn.MediaPiccoCs << std::endl;
  std::ifstream CalibCsFin("../Compton/DATI/Calibrazioni/Calib_1903_fine/Calib_Cs_1903_T275_histo_v2.dat");
  Cesio Cesio_calibFin = CalibrazioneCsFin(CalibCsFin);
  std::cout << "MediaPiccoCs_FINE: " << Cesio_calibFin.MediaPiccoCs << std::endl;
  double MediaCs; 
  MediaCs= (Cesio_calibIn.MediaPiccoCs + Cesio_calibFin.MediaPiccoCs)/2.0;
  std::cout << "Media Picco Cs Calib Totale: " << MediaCs << std::endl;
  std::cout<< "Sono riuscito a fare la media"<<std::endl; 
  double Err_Cs;
  Err_Cs = TMath::Abs((Cesio_calibIn.MediaPiccoCs - Cesio_calibFin.MediaPiccoCs)/2.0);
  std::cout << "Errore Cs Calib Totale: " << Err_Cs << std::endl;


  std::ifstream CalibNaIn("../Compton/DATI/Calibrazioni/Calib_1903_fine/Calib_Na_1903_T283_histo_v2.dat");
  Sodio Sodio_calibIn = CalibrazioneNaIn(CalibNaIn); 
  std::cout << "MediaPiccoNa iniziale: " << Sodio_calibIn.MediaPiccoNa << std::endl;
  std::ifstream CalibNaFin("../Compton/DATI/Calibrazioni/Calib_1903_fine/Calib_Na_1903_T283_histo_v2.dat");
  Sodio Sodio_calibFin = CalibrazioneNaFin(CalibNaFin); 
  std::cout << "MediaPiccoNa finale: " << Sodio_calibFin.MediaPiccoNa << std::endl;
  double MediaNa; 
  MediaNa= (Sodio_calibIn.MediaPiccoNa + Sodio_calibFin.MediaPiccoNa)/2.0;
  std::cout << "Media Picco Na Calib Totale: " << MediaNa << std::endl;
  std::cout<< "Sono riuscito a fare la media"<<std::endl; 
  double Err_Na;
  Err_Na = TMath::Abs((Sodio_calibIn.MediaPiccoNa - Sodio_calibFin.MediaPiccoNa)/2.0);
  std::cout << "Errore Sodio Calib Totale: " << Err_Na << std::endl;

  

  std::ifstream Acquisizione("../Compton/DATI/Acquisizioni/Acquisizione_notte_2003_47cm_70deg.dat");
  Compton Compton_acq = SpettroCo(Acquisizione); 
  std::cout << "MediaPicco1_Co: " << Compton_acq.Picco1Co << std::endl;
  std::cout << "MediaPicco2_Co: " << Compton_acq.Picco2Co << std::endl;


  std::vector<double> Medie_Canali;
  Medie_Canali.push_back(Cobalto_calib_In.MediaPicco1Co);
  Medie_Canali.push_back(Cobalto_calib_In.MediaPicco2Co);
  Medie_Canali.push_back(226); //ci metto il valore ottenuto dal fitpanel
  Medie_Canali.push_back(2422);
  Medie_Canali.push_back(1847);

     
  std::vector<double> Errori_Medie_Canali;
  
  Errori_Medie_Canali.push_back(Err_Co1);
  Errori_Medie_Canali.push_back(Err_Co2);
  Errori_Medie_Canali.push_back(Err_Am);
  Errori_Medie_Canali.push_back(Err_Cs);
  Errori_Medie_Canali.push_back(Err_Na);

  
//std::vector<double> Errori_Medie_Canali = {0.00000001, 0.00000001, 0.00000001, 0.00000001,0.00000001};

  
  std::vector<double> Errori_Energie_vere = {0.00000001, 0.00000001, 0.00000001, 0.00000001,0.00000001};

  std::vector<double> Energie_vere;
  Energie_vere.push_back(1.17);
  Energie_vere.push_back(1.33);
  Energie_vere.push_back(0.06);
  Energie_vere.push_back(0.662);
  Energie_vere.push_back(0.511);

  if (Medie_Canali.size() != Energie_vere.size()) {
    std::cerr << "Errore: i vettori hanno dimensioni diverse!" << std::endl;
    return 1;
   }


  //Ora faccio grafico con barre di errore
  TGraphErrors* calibrazioneErr = new TGraphErrors(
  Medie_Canali.size(),
  Medie_Canali.data(),
  Energie_vere.data(),
  Errori_Medie_Canali.data(),
  Errori_Energie_vere.data()
    );

 calibrazioneErr->SetTitle("Calibrazione con errori;Medie Canali;Energie Vere (MeV)");
 calibrazioneErr->SetMarkerStyle(21); // Stile dei punti
 calibrazioneErr->SetMarkerColor(kBlue); // Colore dei punti
 calibrazioneErr->SetLineColor(kRed); // Colore della linea


 TCanvas* c9 = new TCanvas("c9", "Grafico con errori", 800, 600);
 calibrazioneErr->Draw("AP"); 


 TGraph* calibrazione = new TGraph(Medie_Canali.size(), Medie_Canali.data(), Energie_vere.data());
 calibrazione->SetTitle("Grafico Medie Canali vs Energie Vere;Medie Canali;Energie Vere (MeV)");
 calibrazione->SetMarkerStyle(21);  // Stile del marker
 calibrazione->SetMarkerColor(kBlue);  // Colore del marker
 calibrazione->SetLineColor(kRed);  // Colore della linea


 TCanvas* c1 = new TCanvas("c1", "Grafico", 800, 600);
 calibrazione->Draw("AP"); 

 TF1* fitFunc = new TF1("fitFunc", "pol1", 0, 5000);
 calibrazione->Fit(fitFunc); 
 fitFunc->SetLineColor(kGreen);    

 double MediaCo1 = Compton_acq.Picco1Co ;
 double E1_mis = fitFunc->Eval(MediaCo1); 

 double MediaCo2 = Compton_acq.Picco2Co;
 double E2_mis = fitFunc->Eval(MediaCo2); 

 std::cout << "Valore di x fornito: " << MediaCo1 << std::endl;
 std::cout << "Valore di y corrispondente: " << E1_mis <<  std::endl;
 std::cout << "Valore di x fornito: " << MediaCo2 << std::endl;
 std::cout << "Valore di y corrispondente: " << E2_mis << std::endl;
  
 
 double E[] ={1.17, 1.33};
 double E_mis[] =  {E1_mis, E2_mis};


 TGraph* Massa = new TGraph(2, E, E_mis);
 Massa->SetTitle("Fit massa dalle energie; E ;E_mis"); 
 Massa->SetMarkerStyle(21); 
 Massa->SetMarkerColor(kBlue);

 TF1* MassaFunc = new TF1("MassaFunc", "x/(1+x*(1-cos(0.4014))/[0])", 0, 3); 

 Massa->Fit(MassaFunc);

 TCanvas* c8 = new TCanvas("c8", "Fit della massa", 800, 600);
 Massa->Draw("AP");

 std::cout << "Massa dell'elettrone in MeV: " << MassaFunc->GetParameter(0) << std::endl;
 

  return 0; 
}