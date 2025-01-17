#include <math.h>
#include <stdio.h>
#include <iostream>
#include <TCanvas.h>
#include <TH2D.h>
#include <TGraphErrors.h>
#include <TMath.h>

double sigma_eff(double nt, double nd, double ndacc, double n1, double n2, double time, double ntacc){
    double epsilon=nt/(nd-ndacc-ntacc); //binomial normal efficiency 
    double sigma_nt=sqrt(nd*epsilon*(1-epsilon));
    double sigma_ndacc=sqrt(pow(sqrt(n1)*n2,2)+pow(n1*sqrt(n2),2))*2*(40e-9-2e-9)/time;
    //std::cout<<sqrt(pow(sigma_ndacc*103*30,2)+pow(ndacc*sqrt(103*30),2))*2*(40e-9-2e-9)/time*1e9<<std::endl;
    
    double term1=sigma_nt/(nd-ndacc-ntacc);
    double term2=nt*sigma_ndacc/pow(nd-ndacc-ntacc,2); //ignoring the third component of ntacc

    double sigma_eff=sqrt(pow(term1,2)+pow(term2,2));
    return sigma_eff;
}

void calibration04() {
    // Length of the arrays for efficiency estimation
    static const int n1 =12; //V2=1780, V3=1750
	static const int n2=12; //V1=1800, V3=1750
	static const int n3= 13; //V1=1800, V2=1780
    static const int time = 30;// Time passed in seconds
    static const int time2=10; 

    double voltage1[n1] = {1730, 1750, 1770, 1780, 1790, 1800, 1830, 1850, 1880, 1900, 1930, 1970};
	double voltage2[n2]={1700, 1730, 1750, 1780, 1790, 1800, 1830, 1870, 1900, 1930, 1980, 2000};
	double voltage3[n3]={1700, 1730, 1750, 1770, 1800, 1830, 1850, 1870, 1900, 1930, 1950, 1970, 2000};

    double counts1[n1] = {1687, 2194, 3010, 3463, 3991, 4617, 9590, 14213, 20311, 22637, 25229, 27779};
	double counts2[n2]={736, 1202, 1612, 1958, 2318, 2642, 6327, 20561, 31550, 38323, 45400, 46389};
	double counts3[n3]={723, 1140, 1466, 2922, 8182, 13497, 17276, 19703, 22478, 24964, 27007, 27971, 28760};

    double counts23[n1] = {541, 521, 484,498,  543, 517, 470, 502, 480, 525, 517, 661};
	double counts13[n2]={111, 96, 113, 102, 107, 99, 115, 118, 116, 121, 102, 109};
	double counts12[n3]={200, 148, 171, 159, 175, 180, 198, 151, 183, 173, 170, 200, 182};

    double counts123_e1[n1] = {225, 250, 252, 264, 300, 289, 275, 318, 287, 313, 312, 395};
	double counts123_e2[n2]={62, 71, 93, 94, 99, 91, 109, 113, 113, 118, 99, 104};
	double counts123_e3[n3]={102, 86, 92, 95, 98, 96, 120, 77, 108, 105, 100, 113, 104};

    double eff1_acc[n1];
	double eff2_acc[n2];
	double eff3_acc[n3];
   
    // Declaring canvas
    TCanvas* c1 = new TCanvas("c1", "Stanza2004", 1500, 1500);
	c1->SetGrid(); 
    c1->Divide(2, 3); //colonne, righe
	
    // Graph with error bars
    TGraphErrors* Epsilon1_acc = new TGraphErrors(n1);
	TGraphErrors* Epsilon2_acc = new TGraphErrors(n2);
	TGraphErrors* Epsilon3_acc = new TGraphErrors(n3);

	TGraphErrors* Counts2Graph1 = new TGraphErrors(n1); 
	TGraphErrors* Counts2Graph2 = new TGraphErrors(n2);
	TGraphErrors* Counts2Graph3 = new TGraphErrors(n3);

	Counts2Graph1->SetTitle("Single Counts PMT1; Voltage (V); Counts per second [cps]"); 
	Epsilon1_acc->SetTitle("Efficiency of PMT1 ; Voltage (V); Efficiency");

	Counts2Graph2->SetTitle("Single Counts PMT2; Voltage (V); Counts per second [cps]"); 
	Epsilon2_acc->SetTitle("Efficiency of PMT2 ; Voltage (V); Efficiency");

	Counts2Graph3->SetTitle("Single Counts PMT3; Voltage (V); Counts per second [cps]"); 
	Epsilon3_acc->SetTitle("Efficiency of PMT3; Voltage (V); Efficiency");

    double w = 40e-9; //soglia: -30.0 mV
    double w_min = 2e-9;
    double acc23 = time * (195) * (145) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 23 " << acc23 << std::endl;
    double acc13 = time * (460) * (145) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 13 " << acc13 << std::endl;
    double acc12 = time2 * (460) * (195) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 12 " << acc12 << std::endl;
	double acc123= time2 * (460) * (acc23/30) *((2*w)-(2*w_min));
    std::cout << "Accidentali 123 " << acc123 << std::endl;

    for (int k = 0; k < n1; k++) {
        eff1_acc[k] = counts123_e1[k] / (counts23[k] - acc23-acc123);
        Epsilon1_acc->SetPoint(k, voltage1[k], eff1_acc[k]);
        Epsilon1_acc->SetPointError(k, 0, sigma_eff(counts123_e1[k],counts23[k], acc23, 195*30, 145*30, 30, acc123));  //binomial error if we overlook the accidental counts
		Counts2Graph1->SetPoint(k, voltage1[k], counts1[k] / time);
	  	Counts2Graph1->SetPointError(k, 0, sqrt(counts1[k]) / time); // Poissonian error 
    }

    std::cout << std::fixed << std::setprecision(2); // Imposta la precisione a 2 cifre decimali

    std::cout << voltage1[7] << " Singola " 
            << counts1[7] / time << " " 
            << sqrt(counts1[7]) / time 
            << " Efficienza " 
            << eff1_acc[7] << " " 
            << sigma_eff(counts123_e1[7], counts23[7],  acc23, 195*30, 145*30, 30, acc123)
            << std::endl;

    for (int k = 0; k < n2; k++) {
        eff2_acc[k] = counts123_e2[k] / (counts13[k] - acc13-acc123);
        Epsilon2_acc->SetPoint(k, voltage2[k], eff2_acc[k]);
        Epsilon2_acc->SetPointError(k, 0, sigma_eff(counts123_e2[k],counts13[k], acc13, 460*10, 145*10, 10, acc123));//binomial error if we overlook the accidental counts
		Counts2Graph2->SetPoint(k, voltage2[k], counts2[k] / time2);
	  	Counts2Graph2->SetPointError(k, 0, sqrt(counts2[k]) / time2); // Poissonian error 
    }
    
    std::cout << voltage2[7] << " Singola " 
        << counts2[7] / time2<< " " 
        << sqrt(counts2[7]) / time2 
        << " Efficienza "
        << eff2_acc[7] << " "
        << sigma_eff(counts123_e2[7],counts13[7], acc13, 460*10, 145*10, 10, acc123)<< std::endl;

	for (int k = 0; k < n3; k++) {
        eff3_acc[k] = counts123_e3[k] / (counts12[k] - acc12-acc123);
        Epsilon3_acc->SetPoint(k, voltage3[k], eff3_acc[k]);
        Epsilon3_acc->SetPointError(k, 0, sigma_eff(counts123_e3[k],counts12[k], acc12, 460*10, 195*10, 10, acc123));//binomial error if we overlook the accidental counts
		Counts2Graph3->SetPoint(k, voltage3[k], counts3[k] / time2);
	  	Counts2Graph3->SetPointError(k, 0, sqrt(counts3[k]) / time2); // Poissonian error 
    }
    
    std::cout << voltage3[5] << " Singola " 
            << counts3[5] / time << " " 
            << sqrt(counts3[5]) / time 
            << " Efficienza " 
            << eff3_acc[5] << " " 
            << sigma_eff(counts123_e3[5],counts12[5], acc12, 460*10, 195*10, 10, acc123)<< std::endl;

	 // Plotting single counts
    c1->cd(1);
    Counts2Graph1->SetMarkerStyle(8);
    Counts2Graph1->SetMarkerSize(1);
    Counts2Graph1->SetMarkerColor(kAzure -9);
    Counts2Graph1->SetLineColor(kAzure -9);
    Counts2Graph1->Draw("AP");
    c1->cd(1)->SetLogy();

    c1->cd(3);
    Counts2Graph2->SetMarkerStyle(8);
    Counts2Graph2->SetMarkerSize(1);
    Counts2Graph2->SetMarkerColor(kPink +2);
    Counts2Graph2->SetLineColor(kPink +2);
    Counts2Graph2->Draw("AP");
    c1->cd(3)->SetLogy();

	c1->cd(5);
    Counts2Graph3->SetMarkerStyle(8);
    Counts2Graph3->SetMarkerSize(1);
    Counts2Graph3->SetMarkerColor(kSpring +3);
    Counts2Graph3->SetLineColor(kSpring +3);
    Counts2Graph3->Draw("AP");
    c1->cd(5)->SetLogy();

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

    c1->SaveAs("Risultati/Puntilavoro_04.png");

}

