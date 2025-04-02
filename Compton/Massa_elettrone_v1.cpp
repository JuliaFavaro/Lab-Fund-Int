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


int massa_elettrone(){
//voglio organizzare i punti da inserire cosi: 18deg, 22deg, 24deg, 26deg
/*  
std::vector<double> teta = {0.314159, 0.383972, 0.418879, 0.453786};


std::vector<double> coseno={ cos(0.314159), cos(0.383672), cos(0.418879), cos(0.453786)};

std::vector<double> Energie_1 = { 0.938, 1.02 , 0.949 , 0.944 };
std::vector<double> Energie_2 = { 1.049, 1.152 , 1.063 , 1.057 }; 

std::vector<double> Err_stat = {0.0007, 0.0007, 0.0007, 0.0006};

std::vector<double> Err_sist= { 0.003,  0.002, 0.01, 0.006}; 
std::vector<double> Err_teta= {0.0175,0.0175,0.0175,0.0175};//1grado convertito in rad

std::vector<double> DeltaE(teta.size());
*/
double angolo_gradi_18 = 18;
double angolo_radianti_18 = angolo_gradi_18 * TMath::Pi() / 180.0; // Conversione da gradi a radianti

double angolo_gradi_22 = 22;
double angolo_radianti_22 = angolo_gradi_22 * TMath::Pi() / 180.0; // Conversione da gradi a radianti

double angolo_gradi_24 = 24;
double angolo_radianti_24 = angolo_gradi_24 * TMath::Pi() / 180.0; // Conversione da gradi a radianti

double angolo_gradi_26 = 26;
double angolo_radianti_26 = angolo_gradi_26 * TMath::Pi() / 180.0; // Conversione da gradi a radianti

//std::vector<double> teta_v1 = {0.383972, 0.418879, 0.453786};
std::vector<double> teta_v1= { angolo_radianti_22, angolo_radianti_24, angolo_radianti_26};
std::vector<double> cos_v1={cos(angolo_radianti_22), cos(angolo_radianti_24), cos(angolo_radianti_26)};
std::vector<double> Energie_1_v1 = { 1.02 , 0.949 , 0.944 };
std::vector<double> Energie_2_v1 = { 1.152 , 1.063 , 1.057 }; 
std::vector<double> Err_stat_v1= { 0.0007, 0.0007, 0.0007}; 
std::vector<double> Err_sist_v1= { 0.01, 0.002, 0.004}; 
std::vector<double> Err_teta_v1= {0.0175,0.0175,0.0175};//1grado convertito in rad
std::vector<double> DeltaE_v1(3);

for (int i = 0; i <3; ++i) {
  DeltaE_v1[i] = Energie_2_v1[i] - Energie_1_v1[i];
  std::cout<<"DeltaE_v1[i]=  "<< DeltaE_v1[i]<<std::endl;
}

//int n = teta.size();
int m= teta_v1.size();
/*  
TGraphErrors* graph_stat = new TGraphErrors(m);

 for (int i = 0; i < 3; ++i) {
     graph_stat->SetPoint(i, teta_v1[i],DeltaE_v1[i]);
     graph_stat->SetPointError(i, Err_teta_v1[i], Err_stat_v1[i]);
 }


graph_stat->SetTitle("Delta Energie senza 18deg vs Theta ; Theta(rad); Delta_E(MeV)");
graph_stat->SetMarkerStyle(8);
graph_stat->SetMarkerSize(1);
graph_stat->SetLineColor(kBlue);
graph_stat->SetMarkerColor(kBlue);


TF1* MassaFunc = new TF1("MassaFunc", "1.33/(1+(1.33*(1-cos(x))/[0]))-(1.17/(1+(1.17*(1-cos(x))/[0])))", 0, 3); 

graph_stat->Fit(MassaFunc);
MassaFunc->SetLineColor(kMagenta);

TCanvas* c8 = new TCanvas("c8", "Fit della massa", 800, 600);
graph_stat->Draw("AP");
gStyle->SetOptFit(11111);

*/
TCanvas* c1 = new TCanvas("c1", "Fit della massa da E1", 800, 600);


TGraphErrors* graph1_stat_v1 = new TGraphErrors(m);

 for (int i = 0; i < 3; ++i) {
     graph1_stat_v1->SetPoint(i, teta_v1[i],Energie_1_v1[i]);
     graph1_stat_v1->SetPointError(i, Err_teta_v1[i], Err_sist_v1[i]);
 }


graph1_stat_v1->SetTitle("E_{1} senza 18deg  vs Theta ; Theta(rad); E_{1}(MeV)");
graph1_stat_v1->SetMarkerStyle(8);
graph1_stat_v1->SetMarkerSize(1);
graph1_stat_v1->SetLineColor(kBlue);
graph1_stat_v1->SetMarkerColor(kBlue);
TF1* MassaFunc_1 = new TF1("MassaFunc_1", "1.17/(1+(1.17*(1-cos(x))/[0]))", 0, 3);

graph1_stat_v1->Fit(MassaFunc_1);
MassaFunc_1->SetLineColor(kMagenta);
graph1_stat_v1->Draw("AP");
gStyle->SetOptFit(11111);


TCanvas* c2 = new TCanvas("c2", "Fit della massa da E2", 800, 600);


TGraphErrors* graph2_stat_v1 = new TGraphErrors(m);

 for (int i = 0; i < 3; ++i) {
     graph2_stat_v1->SetPoint(i, teta_v1[i],Energie_2_v1[i]);
     graph2_stat_v1->SetPointError(i, Err_teta_v1[i], Err_sist_v1[i]);
 }


graph2_stat_v1->SetTitle("E_{2} senza 18deg  vs Theta ; Theta(rad); E_{2}(MeV)");
graph2_stat_v1->SetMarkerStyle(8);
graph2_stat_v1->SetMarkerSize(1);
graph2_stat_v1->SetLineColor(kBlue);
graph2_stat_v1->SetMarkerColor(kBlue);

TF1* MassaFunc_2 = new TF1("MassaFunc_2", "1.33/(1+(1.33*(1-cos(x))/[0]))", 0, 3); 

graph2_stat_v1->Fit(MassaFunc_2);
MassaFunc_2->SetLineColor(kMagenta);
graph2_stat_v1->Draw("AP");
gStyle->SetOptFit(11111);

std::cout << "Massa dell'elettrone in MeV: " << MassaFunc_2->GetParameter(0) << std::endl;

return 0; 
}