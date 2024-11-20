#include <math.h>
#include <stdio.h>
#include <iostream>

void efficenza() {
	//length of the arrays for single counts
	static const int n1 = ;
	static const int n2 = ;
	static const int n3 = ;

	//lenght of the arrays for efficiency estimation
	static const int ne1 = ;
	static const int ne2 = ;
	static const int ne3 = ;

	double voltage1[] = {1426, 1452, 1473,};
	double voltage2[] = { };
	double voltage3[] = { };
	double counts1[n1] = { };
	double counts2[n2] = { };
	double counts3[n3] = { };

	double counts12[ne3] = {28,22, };
	double counts13[ne2] = {};
	double counts23[ne1] = {};

	double voltage12[ne3] = {21, 18, 27};
	double voltage13[ne2] = {};
	double voltage23[ne1] = {};

	double counts12_acc[ne3];
	double counts13_acc[ne2];
	double counts23_acc[ne1]; 

	double counts123_e1[ne1] = {};
	double counts123_e2[ne2] = {};
	double counts123_e3[ne3] = {};

	double eff1_acc[ne1];
	double eff2_acc[ne2]; 
	double eff3_acc[ne3];

	double eff1[ne1];
	double eff2[ne2];
	double eff3[ne3];

	TCanvas* c1 = new TCanvas("c1", "c1", 1000, 1000);
	TCanvas* c2 = new TCanvas("c2", "c2", 1000, 1000);
	TCanvas* c3 = new TCanvas("c3", "c3", 1000, 1000);

	TCanvas* e1 = new TCanvas("e1", "e1", 1000, 1000);
	TCanvas* e2 = new TCanvas("e2", "e2", 1000, 1000);
	TCanvas* e3 = new TCanvas("e3", "e3", 1000, 1000);

	TH2D* conteggi1 = new TH2D("conteggi1", "Rate PMT1; V; cps", 100, voltage1[0] - 10, voltage1[n1-1] + 10, 100, (counts1[0]/10)-10, (counts1[n1-1]/10)+100);
	TH2D* conteggi2 = new TH2D("conteggi2", "Rate PMT2; V; cps", 100, voltage2[0] - 10, voltage2[n2 - 1] + 10, 100, (counts2[0]/10) - 10, (counts2[n2 - 1]/10) + 100);
	TH2D* conteggi1 = new TH2D("conteggi1", "Rate PMT3; V; cps", 100, voltage3[0] - 10, voltage3[n3 - 1] + 10, 100, (counts3[0]/10) - 10, (counts3[n3 - 1]/10) + 100);

	//TH2D* epsilon1 = new TH2D("epsilon1", "efficienza PMT1; V; ", 100, voltage23[0] - 10, voltage23[10] + 10, 100, 350, 21000);
	//TH2D* epsilon2 = new TH2D("epsilon2", "efficienza PMT2; V; ", 100, voltage13[0] - 10, voltage13[10] + 10, 100, 350, 21000);
	//TH2D* Epsilon1 = new TH2D("Epsilon1", "efficienza PMT3; V; ", 100, voltage12[0] - 10, voltage12[10] + 10, 100, 350, 21000);

	TGraphErrors* Conteggi1 = new TGraphErrors(n1);
	TGraphErrors* Conteggi2 = new TGraphErrors(n2);
	TGraphErrors* conteggi1 = new TGraphErrors(n3);

	TGraphErrors* Epsilon1 = new TGraphErrors(ne1);
	TGraphErrors* Epsilon2 = new TGraphErrors(ne2);
	TGraphErrors* Epsilon1 = new TGraphErrors(ne3);

	TGraphErrors* Epsilon1_acc = new TGraphErrors(ne1);
	TGraphErrors* Epsilon2_acc = new TGraphErrors(ne2);
	TGraphErrors* Epsilon1_acc = new TGraphErrors(ne3);
	
	for (int i = 0; i < n1; i++) {
		Conteggi1->SetPoint(i, voltage1[i], counts1[i]/10);
		Conteggi1->SetPointError(i, 0, (sqrt(counts1[i])/10));
	}
	for (int i = 0; i < n2; i++) {
		Conteggi2->SetPoint(i, voltage2[i], counts2[i]/10);
		Conteggi2->SetPointError(i, 0, (sqrt(counts2[i])/10));
	}
	for (int i = 0; i < n3; i++){
		conteggi1->SetPoint(i, voltage3[i], counts3[i]/10);
		conteggi1->SetPointError(i, 0, (sqrt(counts3[i])/10));
	}

	//conteggi in singola inizio PMT1
	c1->cd();
	//conteggi PMT3
	conteggi1->SetStats(0);
	conteggi1->Draw();
	conteggi1->SetMarkerStyle(8);
	conteggi1->SetMarkerSize(0.5);
	conteggi1->SetMarkerColor(9);//kGreen
	conteggi1->SetLineColor(9);//kGreen
	conteggi1->Draw("P");

	//conteggi PMT2
	c2->cd();
	conteggi2->SetStats(0);
	conteggi2->Draw();
	conteggi2->SetMarkerStyle(8);
	conteggi2->SetMarkerSize(0.5);
	conteggi2->SetMarkerColor(kRed);
	conteggi2->SetLineColor(kRed);
	conteggi2->Draw("P");

	//conteggi PMT3
	c3->cd();
	conteggi3->SetStats(0);
	conteggi3->Draw();
	conteggi3->SetMarkerStyle(8);
	conteggi3->SetMarkerSize(0.5);
	conteggi3->SetMarkerColor(kBlue);
	conteggi3->SetLineColor(kBlue);
	conteggi3->Draw("P");

	//legend->AddEntry(conteggi1, "PMT3");
	//legend->AddEntry(conteggi2, "PMT4");
	//legend->AddEntry(conteggi3, "PMT5");
	//legend->Draw();
	//conteggi in singola fine

	//efficenze inizio

	double w = (50. / 1000000000);
	double w_min = (2. / 1000000000);
	double t = 10;
	double acc23 = t * (10649/ t) * (10404/ t) * ((2 * w) - (2 * w_min));
	cout << "acc23 = " << acc23 << endl;
	for (int k = 0; k < ne1; k++) {
		//counts23_acc[k] = t * (counts2[k] / t) * (counts3[k] / t) * (2 * w - 2 * w_min);
		eff1_acc[k] = (counts123_e1[k]) / (counts23[k] - acc23);
		eff1[k] = (counts123_e1[k]) / (counts23[k]);
		//cout << "Volt = " << voltage23[k] << "  eff1 = " << eff1[k] << " +- " << sqrt((eff1[k] * (1 - eff1[k])) / counts23[k]) << "  eff1 acc = " << eff1_acc[k] << endl;
		Epsilon1->SetPoint(k, voltage23[k], eff1[k]);
		Epsilon1->SetPointError(k, 0, sqrt((eff1[k]*(1 - eff1[k])) / counts23[k]));
		Epsilon1_acc->SetPoint(k, voltage23[k], eff1_acc[k]);
	}
	cout << "-----------------------------------------------------------------------------------------------------" << endl;
	double acc13 = t * (9177/ t) * (10404/ t) * ((2 * w) - (2 * w_min));
	cout << "acc13 = " << acc13 << endl;
	for (int p = 0; p < ne2; p++) {
		//counts13_acc[p] = t * (counts1[p] / t) * (counts3[p] / t) * (2 * w - 2 * w_min);
		eff2_acc[p] = counts123_e2[p] / (counts13[p] - acc13);
		eff2[p] = counts123_e2[p] / (counts13[p]);
		cout << "Volt = " << voltage13[p] << "  eff2 = " << eff2[p] << " +- " << sqrt((eff2[p] * (1 - eff2[p])) / counts13[p]) << "  eff2 acc = " << eff2_acc[p] << endl;
		Epsilon2->SetPoint(p, voltage13[p], eff2[p]);
		Epsilon2->SetPointError(p, 0, sqrt((eff2[p]*(1 - eff2[p])) / counts13[p]));
		Epsilon2_acc->SetPoint(p, voltage13[p], eff2_acc[p]);
	}
	cout << "-----------------------------------------------------------------------------------------------------" << endl;
	double acc12 = t * (9177/ t) * (10649/ t) * ((2 * w) - (2 * w_min));
	cout << "acc12 = " << acc12 << endl;
	for (int l = 0; l < ne3; l++) {
		//counts12_acc[l] = t * (counts1[l] / t) * (counts2[l] / t) * (2 * w - 2 * w_min); //una unica
		eff3_acc[l] = counts123_e3[l] / (counts12[l] - acc12);
		eff3[l] = counts123_e3[l] / (counts12[l]);
		//cout << "Volt = " << voltage12[l] << "  eff3 = " << eff3[l] << " +- " << sqrt((eff3[l] * (1 - eff3[l])) / counts12[l]) << "  eff3 acc =" << eff3_acc[l] << endl;
		Epsilon3->SetPoint(l, voltage12[l], eff3[l]);
		Epsilon3->SetPointError(l, 0, sqrt((eff3[l]*(1-eff3[l]))/counts12[l]));
		Epsilon3_acc->SetPoint(l, voltage12[l], eff3_acc[l]);
	}

	TH2D* Epsilon1 = new TH2D("Epsilon1", "efficienza PMT1; V; ", 100, voltage23[0] - 10, voltage23[ne1 - 1] + 10, 100, 0, 1);
	TH2D* Epsilon2 = new TH2D("Epsilon2", "efficienza PMT2; V; ", 100, voltage13[0] - 10, voltage13[ne2 - 1] + 10, 100, 0, 1);
	TH2D* Epsilon3 = new TH2D("Epsilon3", "efficienza PMT3; V; ", 100, voltage12[0] - 10, voltage12[ne3 - 1] + 10, 100, 0, 1);

	e3->cd();
	Epsilon1->SetStats(0);
	Epsilon1->Draw();
	//disegno senza acc
	Epsilon1->SetMarkerStyle(8);
	Epsilon1->SetMarkerSize(0.8);//0.5
	Epsilon1->SetMarkerColor(9);//kGreen
	Epsilon1->SetLineColor(9);//kGreen
	Epsilon1->Draw("P");

	//disegno con acc
	Epsilon1_acc->SetMarkerStyle(4);
	Epsilon1_acc->SetMarkerSize(0.8);
	Epsilon1_acc->SetMarkerColor(kPink);
	Epsilon1_acc->SetLineColor(kPink);
	Epsilon1_acc->Draw("P");

	e4->cd();
	Epsilon2->SetStats(0);
	Epsilon2->Draw();
	//disegno senza acc
	Epsilon2->SetMarkerStyle(8);
	Epsilon2->SetMarkerSize(0.8);
	Epsilon2->SetMarkerColor(kRed);
	Epsilon2->SetLineColor(kRed);
	Epsilon2->Draw("P");

	//disegno con acc
	Epsilon2_acc->SetMarkerStyle(4);
	Epsilon2_acc->SetMarkerSize(0.8);
	Epsilon2_acc->SetMarkerColor(kBlack);
	Epsilon2_acc->SetLineColor(kBlack);
	//Epsilon2_acc->Draw("P");
	Epsilon2_acc->Draw("P");

	e5->cd();
	Epsilon3->SetStats(0);
	Epsilon3->Draw();
	//disegno senza acc
	Epsilon3->SetMarkerStyle(8);
	Epsilon3->SetMarkerSize(0.8);
	Epsilon3->SetMarkerColor(kBlue);
	Epsilon3->SetLineColor(kBlue);
	Epsilon3->Draw("P");

	//diegno con acc
	Epsilon3_acc->SetMarkerStyle(4);
	Epsilon3_acc->SetMarkerSize(0.8);
	Epsilon3_acc->SetMarkerColor(kPink);
	Epsilon3_acc->SetLineColor(kPink);
	Epsilon3_acc->Draw("P");
	//Epsilon3_acc->Draw("P");

}