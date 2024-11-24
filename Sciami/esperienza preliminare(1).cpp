#include <math.h>
#include <stdio.h>
#include <iostream>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TMultiGraph.h>

void calibration() {
    // Length of the arrays for efficiency estimation
    static const int n1 = 8; // Adjusted size to match the length of the arrays
    static const int n2 = 10;
    static const int n3 = 6;
    static const int time = 30; // Time passed in seconds

    double voltage1[n1] = {1408, 1428, 1456, 1488, 1505, 1542, 1593, 1640};
    double voltage2[n2] = {1347, 1365, 1382, 1391, 1402, 1427, 1457, 1485, 1505, 1528};
    double voltage3[n3] = {1749, 1776, 1793, 1804, 1785, 1768};

    double counts1[n1] = {2165, 2939, 4107, 5707, 7323, 22248, 136428, 375583}; 
    double counts2[n2] = {3158, 4153, 6204, 7926, 10771, 13771, 22479, 31895, 40287, 51756};
    double counts3[n3] = {2006, 3237, 4132, 4974, 3823, 2763};

    double counts23[n1] = {19, 17, 22, 27, 28, 24, 33, 29};
    double counts13[n2] = {57, 40, 46, 37, 34, 42, 45, 56, 52, 45};
    double counts12[n3] = {92, 106, 101, 114, 87, 104};

    double counts123_e1[n1] = {16, 13, 18, 26, 24, 22, 29, 27};
    double counts123_e2[n2] = {19, 20, 27, 15, 19, 24, 30, 48, 41, 41};
    double counts123_e3[n3] = {17, 13, 24, 26, 12, 14};

    double eff1_acc[n1];
    double eff2_acc[n2];
    double eff3_acc[n3];

    // Declaring canvas
    TCanvas* c1 = new TCanvas("c1", "Single counts", 1000, 1000);
    c1->SetGrid(); 

    TCanvas* e1 = new TCanvas("e1", "Efficiency with Accidental Corrections", 1000, 1000);
    e1->SetGrid(); 

    // Graph with error bars
    TGraphErrors* Epsilon1_acc = new TGraphErrors(n1);
    TGraphErrors* Epsilon2_acc = new TGraphErrors(n2);
    TGraphErrors* Epsilon3_acc = new TGraphErrors(n3);

    TGraphErrors* CountsGraph1 = new TGraphErrors(n1); 
    TGraphErrors* CountsGraph2 = new TGraphErrors(n2);
    TGraphErrors* CountsGraph3 = new TGraphErrors(n3);

    CountsGraph1->SetTitle("Single Counts PMT1; Voltage (V); Counts"); 
    Epsilon1_acc->SetTitle("Efficiency of PMT1; Voltage (V); Efficiency");

    CountsGraph2->SetTitle("Single Counts PMT2; Voltage (V); Counts"); 
    Epsilon2_acc->SetTitle("Efficiency of PMT2; Voltage (V); Efficiency");

    CountsGraph3->SetTitle("Single Counts PMT3; Voltage (V); Counts"); 
    Epsilon3_acc->SetTitle("Efficiency of PMT3; Voltage (V); Efficiency");

    double w = 40e-9; //soglia: -30.4 mV
    double w_min = 2e-9;
    double acc23 = time * 150 * 140 * ((2 * w) - (2 * w_min));
    double acc13 = time * 103 * 140 * ((2 * w) - (2 * w_min));
    double acc12 = time * 103 * 150 * ((2 * w) - (2 * w_min));

    for (int k = 0; k < n1; k++) {
        eff1_acc[k] = counts123_e1[k] / (counts23[k] - acc23);
        Epsilon1_acc->SetPoint(k, voltage1[k], eff1_acc[k]);
        Epsilon1_acc->SetPointError(k, 0, sqrt((eff1_acc[k] * (1 - eff1_acc[k])) / counts23[k]));
        CountsGraph1->SetPoint(k, voltage1[k], counts1[k] / time);
        CountsGraph1->SetPointError(k, 0, sqrt(counts1[k]) / time); // Poissonian error 
    }

    for (int k = 0; k < n2; k++) {
        eff2_acc[k] = counts123_e2[k] / (counts13[k] - acc13);
        Epsilon2_acc->SetPoint(k, voltage2[k], eff2_acc[k]);
        Epsilon2_acc->SetPointError(k, 0, sqrt((eff2_acc[k] * (1 - eff2_acc[k])) / counts13[k]));
        CountsGraph2->SetPoint(k, voltage2[k], counts2[k] / time);
        CountsGraph2->SetPointError(k, 0, sqrt(counts2[k]) / time); // Poissonian error 
    }

    for (int k = 0; k < n3; k++) {
        eff3_acc[k] = counts123_e3[k] / (counts12[k] - acc12);
        Epsilon3_acc->SetPoint(k, voltage3[k], eff3_acc[k]);
        Epsilon3_acc->SetPointError(k, 0, sqrt((eff3_acc[k] * (1 - eff3_acc[k])) / counts12[k]));
        CountsGraph3->SetPoint(k, voltage3[k], counts3[k] / time);
        CountsGraph3->SetPointError(k, 0, sqrt(counts3[k]) / time); // Poissonian error 
    }

    // Creating multigraphs for single counts and efficiency
    TMultiGraph* mgCounts = new TMultiGraph();
    mgCounts->SetTitle("Single Counts for PMT1, PMT2, and PMT3; Voltage (V); Counts");

    TMultiGraph* mgEfficiency = new TMultiGraph();
    mgEfficiency->SetTitle("Efficiency with Accidental Corrections for PMT1, PMT2, and PMT3; Voltage (V); Efficiency");

    // Adding graphs to multigraph for single counts
    mgCounts->Add(CountsGraph1, "P");
    mgCounts->Add(CountsGraph2, "P");
    mgCounts->Add(CountsGraph3, "P");

    // Adding graphs to multigraph for efficiency
    mgEfficiency->Add(Epsilon1_acc, "P");
    mgEfficiency->Add(Epsilon2_acc, "P");
    mgEfficiency->Add(Epsilon3_acc, "P");

    // Creating legends
    TLegend* legendCounts = new TLegend(0.1,0.7,0.3,0.9);
    legendCounts->AddEntry(CountsGraph1, "PMT1", "P");
    legendCounts->AddEntry(CountsGraph2, "PMT2", "P");
    legendCounts->AddEntry(CountsGraph3, "PMT3", "P");

    TLegend* legendEfficiency = new TLegend(0.1,0.7,0.3,0.9);
    legendEfficiency->AddEntry(Epsilon1_acc, "PMT1", "P");
    legendEfficiency->AddEntry(Epsilon2_acc, "PMT2", "P");
    legendEfficiency->AddEntry(Epsilon3_acc, "PMT3", "P");

    // Plotting single counts
    c1->cd();
    mgCounts->Draw("A");
    legendCounts->Draw();
    c1->Update();

    // Plotting efficiency with accidental corrections
    e1->cd();
    mgEfficiency->Draw("A");
    legendEfficiency->Draw();
    e1->Update();
}

int main() {
    calibration();
    return 0;
}
