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

// Function to generate all combinations
void generateCombinations(const std::vector<double>& energies, const std::vector<double>& err_sist, std::vector<std::vector<double>>& combinations) {
    int n = energies.size();
    int totalCombinations = pow(3, n);
    combinations.resize(totalCombinations, std::vector<double>(n));
    
    for (int i = 0; i < totalCombinations; ++i) {
        for (int j = 0; j < n; ++j) {
            int index = (i / (int)pow(3, j)) % 3;
            if (index == 0) combinations[i][j] = energies[j]+err_sist[j];
            else if (index == 1) combinations[i][j] = energies[j];
            else combinations[i][j] = energies[j] - err_sist[j];
        }
    }
}

double calculateMass(const std::vector<double>& teta, const std::vector<double>& energies, const std::vector<double>& err_teta, const std::string& fitFunc, const std::vector<double> & Err_stat) {
    int n = teta.size();
    TGraphErrors* graph = new TGraphErrors(n); 
    for (int i = 0; i < n; ++i) {
        graph->SetPoint(i, teta[i], energies[i]);
        graph->SetPointError(i, err_teta[i],  Err_stat[i]);
    }
    TF1* MassaFunc = new TF1("MassaFunc", fitFunc.c_str(), 0, 3);
    graph->Fit(MassaFunc,"WQN");
    graph->Fit(MassaFunc, "N");
    double mass = MassaFunc->GetParameter(0);
    delete graph;
    delete MassaFunc;
    return mass;
}

void processEnergies(const std::vector<double>& teta, const std::vector<double>& energies,const std::vector<double>& err_sist, const std::vector<double>& err_stat, const std::vector<double>& err_teta, const std::string& fitFunc, const std::string& title) {
    std::vector<std::vector<double>> combinations;
    generateCombinations(energies, err_sist, combinations);
    
    std::vector<double> masses;
    for (const auto& combo : combinations) {
        double mass = calculateMass(teta, combo, err_teta, fitFunc, err_stat);
        masses.push_back(mass);
    }
    
    double original_mass = calculateMass(teta, energies, err_teta, fitFunc,err_stat);
    double max_mass = *std::max_element(masses.begin(), masses.end());
    double min_mass = *std::min_element(masses.begin(), masses.end());
    double semi_difference = (max_mass - min_mass) / 2.0;
    
    std::cout << "Original mass of the electron (" << title << "): " << original_mass << std::endl;
    std::cout << "Maximum mass of the electron (" << title << "): " << max_mass << std::endl;
    std::cout << "Minimum mass of the electron (" << title << "): " << min_mass << std::endl;
    std::cout << "Semi-difference between maximum and minimum mass (" << title << "): " << semi_difference << std::endl;
}

int massa(){
//voglio organizzare i punti da inserire cosi: 18deg, 22deg, 24deg, 26deg, 28deg
//0.314159
std::vector<double> teta = {0.383972, 0.418879, 0.453786, 0.488692};
std::vector<double> coseno={cos(0.383672), cos(0.418879), cos(0.453786), cos(0.488692)};
std::vector<double> Energie_1 = { 1.02 , 0.949 , 0.944, 0.938}; 
std::vector<double> Energie_2 = { 1.152 , 1.063 , 1.057, 1.049}; 
std::vector<double> Err_stat = {0.0007, 0.0007, 0.0006, 0.0007};
std::vector<double> Err_sist= { 0.002, 0.01, 0.006, 0.003}; 
std::vector<double> Err_teta= {0.0175,0.0175,0.0175, 0.0175}; //1grado convertito in rad
int n = teta.size();

TGraphErrors* graph1_stat = new TGraphErrors(n); 
for (int i = 0; i < n; ++i) {
    graph1_stat->SetPoint(i, teta[i],Energie_1[i]);
    graph1_stat->SetPointError(i, Err_teta[i], Err_stat[i]);
        //sqrt(pow(Err_stat[i],2)+pow(Err_sist[i],2)));
}

graph1_stat->SetTitle("Energie 1#circ picco vs Theta ; #theta [rad]; E_{1} [MeV]");
graph1_stat->SetMarkerStyle(8);
graph1_stat->SetMarkerSize(1);
graph1_stat->SetLineColor(kBlue);
graph1_stat->SetMarkerColor(kBlue);

TGraphErrors* graph2_stat = new TGraphErrors(n);

 for (int i = 0; i < n; ++i) {
     graph2_stat->SetPoint(i, teta[i],Energie_2[i]);
     graph2_stat->SetPointError(i, Err_teta[i],  Err_stat[i]);
        //sqrt(pow(Err_stat[i],2)+pow(Err_sist[i],2)));
 }

graph2_stat->SetTitle("Energie 2#circ picco vs Theta ; #theta [rad]; E_{2} [MeV]");
graph2_stat->SetMarkerStyle(8);
graph2_stat->SetMarkerSize(1);
graph2_stat->SetLineColor(kBlue);
graph2_stat->SetMarkerColor(kBlue);

TF1* MassaFunc1 = new TF1("MassaFunc1", "1.17/(1+(1.17*(1-cos(x))/[0]))", 0, 3); 
TF1* MassaFunc2 = new TF1("MassaFunc2", "1.33/(1+(1.33*(1-cos(x))/[0]))", 0, 3);

graph1_stat->Fit(MassaFunc1,"WQN");
graph1_stat->Fit(MassaFunc1);
MassaFunc1->SetLineColor(kMagenta);

TCanvas* c8 = new TCanvas("c8", "Fit della massa", 1000, 1000);
c8->Divide(1,2);
c8->cd(1);
graph1_stat->Draw("AP");
MassaFunc1->Draw("same");
gStyle->SetOptFit(11111);
std::cout << "Massa dell'elettrone dal primo picco in MeV: " << MassaFunc1->GetParameter(0) << std::endl;
std::cout << "Differenza in termini di sigma: " << (MassaFunc1->GetParameter(0)-0.511)/MassaFunc1->GetParError(0) << std::endl;

graph2_stat->Fit(MassaFunc2,"WQN");
graph2_stat->Fit(MassaFunc2);
MassaFunc2->SetLineColor(kMagenta);

c8->cd(2);
graph2_stat->Draw("AP");
MassaFunc2->Draw("same");
gStyle->SetOptFit(11111);
std::cout << "Massa dell'elettrone dal secondo picco in MeV: " << MassaFunc2->GetParameter(0) << std::endl;
std::cout << "Differenza in termini di sigma: " << (MassaFunc2->GetParameter(0)-0.511)/MassaFunc2->GetParError(0) << std::endl;

//processEnergies(teta, Energie_1, Err_sist, Err_stat, Err_teta, "1.17/(1+(1.17*(1-cos(x))/[0]))", "Energie 1");
//processEnergies(teta, Energie_2, Err_sist, Err_stat, Err_teta, "1.33/(1+(1.33*(1-cos(x))/[0]))", "Energie 2");
    
return 0; 
}