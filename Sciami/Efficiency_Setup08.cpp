#include <math.h>
#include <stdio.h>
#include <iostream>
#include <TCanvas.h>
#include <TH2D.h>
#include <TGraphErrors.h>
#include <TMath.h>

void calibration0() {
    // Length of the arrays for efficiency estimation
    static const int n1 = 8;
	static const int n2=10;
	static const int n3= 6;
    static const int time = 30; // Time passed in seconds

    double voltage1[n1] = {1408, 1428, 1456, 1488, 1505, 1542, 1593, 1640}; //Assuming an uncertainty of 1-2 Volts
	double voltage2[n2]={1347, 1365, 1382, 1391, 1402, 1427, 1457, 1485, 1505, 1528};
	double voltage3[n3]={1749, 1776, 1793, 1804, 1785, 1768};

    double counts1[n1] = {2165, 2939, 4107, 5707, 7323, 22248, 136428, 375583};
	double counts2[n2]={3158, 4153, 6204, 7926, 10771, 13771, 22479, 31895, 40287, 51756};
	double counts3[n3]={2006, 3237, 4132, 4974, 3823, 2763};

    double counts23[n1] = {19, 17, 22, 27, 28, 24, 33, 29};
	double counts13[n2]={57, 40, 46, 37, 34, 42, 45, 56, 52, 45};
	double counts12[n3]={92, 106, 101, 114, 87, 104};

    double counts123_e1[n1] = {16, 13, 18, 26, 24, 22, 29, 27};
	double counts123_e2[n2]={19, 20, 27, 15, 19, 24, 30, 48, 41, 41};
	double counts123_e3[n3]={17, 13, 24, 26, 12, 14};

    double eff1_acc[n1];
	double eff2_acc[n2];
	double eff3_acc[n3];

    // Declaring canvas
    TCanvas* c1 = new TCanvas("c1", "Setup08", 1500, 1500);
    c1->SetTitle("Setup08");
	c1->SetGrid(); 
    c1->Divide(2, 3); //colonne, righe
	
    // Graph with error bars
    TGraphErrors* Epsilon1_acc = new TGraphErrors(n1);
	TGraphErrors* Epsilon2_acc = new TGraphErrors(n2);
	TGraphErrors* Epsilon3_acc = new TGraphErrors(n3);

	TGraphErrors* Counts2Graph1 = new TGraphErrors(n1); 
	TGraphErrors* Counts2Graph2 = new TGraphErrors(n2);
	TGraphErrors* Counts2Graph3 = new TGraphErrors(n3);

	Counts2Graph1->SetTitle("Single Counts PMT1; Voltage (V); Rate"); 
	Epsilon1_acc->SetTitle("Efficiency of PMT1 ; Voltage (V); Efficiency");

	Counts2Graph2->SetTitle("Single Counts PMT2; Voltage (V); Rate"); 
	Epsilon2_acc->SetTitle("Efficiency of PMT2 ; Voltage (V); Efficiency");

	Counts2Graph3->SetTitle("Single Counts PMT3; Voltage (V); Rate"); 
	Epsilon3_acc->SetTitle("Efficiency of PMT3; Voltage (V); Efficiency");

    double w = 40e-9; //soglia: -30.4 mV
    double w_min = 2e-9;
    double acc23 = time * (150) * (140) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 23 " << acc23 << std::endl;
    double acc13 = time * (103) * (140) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 13 " << acc13 << std::endl;
    double acc12 = time * (103) * (150) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 12 " << acc12 << std::endl;
	double acc123= time * (103) * (acc23/30) *((2*w)-(2*w_min));
    std::cout << "Accidentali 123 " << acc123 << std::endl;

    for (int k = 0; k < n1; k++) {
        eff1_acc[k] = counts123_e1[k] / (counts23[k] - acc23-acc123);
        Epsilon1_acc->SetPoint(k, voltage1[k], eff1_acc[k]);
        Epsilon1_acc->SetPointError(k, 0, sqrt((eff1_acc[k] * (1 - eff1_acc[k])) / counts23[k]));//binomial error if we overlook the accidental counts
		Counts2Graph1->SetPoint(k, voltage1[k], counts1[k] / time);
	  	Counts2Graph1->SetPointError(k, 0, sqrt(counts1[k]) / time); // Poissonian error 
    }
std::cout << voltage1[6] << " Singola " << counts1[6] / time << " " << sqrt(counts1[6]) / time << " Efficienza " << eff1_acc[6] << " " << sqrt((eff1_acc[6] * (1 - eff1_acc[6])) / counts23[6]) << std::endl;

    for (int k = 0; k < n2; k++) {
        eff2_acc[k] = counts123_e2[k] / (counts13[k] - acc13-acc123);
        Epsilon2_acc->SetPoint(k, voltage2[k], eff2_acc[k]);
        Epsilon2_acc->SetPointError(k, 0, sqrt((eff2_acc[k] * (1 - eff2_acc[k])) / counts13[k]));//binomial error if we overlook the accidental counts
		Counts2Graph2->SetPoint(k, voltage2[k], counts2[k] / time);
	  	Counts2Graph2->SetPointError(k, 0, sqrt(counts2[k]) / time); // Poissonian error 
    }
std::cout << voltage2[7] << " Singola " << counts2[7] / time << " " << sqrt(counts2[7]) / time << " Efficienza " << eff2_acc[7] << " " << sqrt((eff2_acc[7] * (1 - eff2_acc[7])) / counts13[7]) << std::endl;

	for (int k = 0; k < n3; k++) {
        eff3_acc[k] = counts123_e3[k] / (counts12[k] - acc12-acc123);
        Epsilon3_acc->SetPoint(k, voltage3[k], eff3_acc[k]);
        Epsilon3_acc->SetPointError(k, 0, sqrt((eff3_acc[k] * (1 - eff3_acc[k])) / counts12[k]));//binomial error if we overlook the accidental counts
		Counts2Graph3->SetPoint(k, voltage3[k], counts3[k] / time);
	  	Counts2Graph3->SetPointError(k, 0, sqrt(counts3[k]) / time); // Poissonian error 
    }

std::cout << voltage3[5] << " Singola " << counts3[5] / time << " " << sqrt(counts3[5]) / time << " Efficienza " << eff3_acc[5] << " " << sqrt((eff3_acc[5] * (1 - eff3_acc[5])) / counts12[5]) << std::endl;
	 // Plotting single counts
    c1->cd(1);
    Counts2Graph1->SetMarkerStyle(8);
    Counts2Graph1->SetMarkerSize(1);
    Counts2Graph1->SetMarkerColor(kAzure -9);
    Counts2Graph1->SetLineColor(kAzure -9);
    Counts2Graph1->Draw("AP");

    c1->cd(3);
    Counts2Graph2->SetMarkerStyle(8);
    Counts2Graph2->SetMarkerSize(1);
    Counts2Graph2->SetMarkerColor(kPink +2);
    Counts2Graph2->SetLineColor(kPink +2);
    Counts2Graph2->Draw("AP");

	c1->cd(5);
    Counts2Graph3->SetMarkerStyle(8);
    Counts2Graph3->SetMarkerSize(1);
    Counts2Graph3->SetMarkerColor(kSpring +3);
    Counts2Graph3->SetLineColor(kSpring +3);
    Counts2Graph3->Draw("AP");

    // Plotting efficiency with accidental corrections
    c1->cd(2);
    Epsilon1_acc->SetMarkerStyle(8);
    Epsilon1_acc->SetMarkerSize(1);
    Epsilon1_acc->SetMarkerColor(kAzure -9);
    Epsilon1_acc->SetLineColor(kAzure -9);
    Epsilon1_acc->Draw("AP");

	c1->cd(4);
    Epsilon2_acc->SetMarkerStyle(8);
    Epsilon2_acc->SetMarkerSize(1);
    Epsilon2_acc->SetMarkerColor(kPink+2);
    Epsilon2_acc->SetLineColor(kPink+2);
    Epsilon2_acc->Draw("AP");

	c1->cd(6);
    Epsilon3_acc->SetMarkerStyle(8);
    Epsilon3_acc->SetMarkerSize(1);
    Epsilon3_acc->SetMarkerColor(kSpring +3);
    Epsilon3_acc->SetLineColor(kSpring +3);
    Epsilon3_acc->Draw("AP");
}