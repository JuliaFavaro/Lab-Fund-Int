#include <math.h>
#include <stdio.h>
#include <iostream>

void efficenza() {
	static const int n3 = 9;
	static const int n4 = 9;
	static const int n5 = 21;//17;

	static const int ne3 = 14;
	static const int ne4 = 18;
	static const int ne5 = 20;

	double voltage[] = { 1426, 1452, 1473,};
	double voltage4[10] = { };
	double voltage5[21] = { };//17
	double counts3[n3] = { };
	double counts4[n4] = { };
	double counts5[n5] = { };

	double counts34[ne5] = {28,22, };
	double counts35[ne4] = {};
	double counts45[ne3] = {};

	double voltage34[ne5] = {21, 18, 27};
	double voltage35[ne4] = {};
	double voltage45[ne3] = {};

	double counts34_acc[ne5];// = { 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000 };
	double counts35_acc[ne4]; // = { 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000 };
	double counts45_acc[ne3]; // = { 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000 };

	double counts345_e3[ne3] = {};
	double counts345_e4[ne4] = {};
	double counts345_e5[ne5] = {};

	double eff3_acc[ne3];
	double eff4_acc[ne4]; 
	double eff5_acc[ne5];

	double eff3[ne3];
	double eff4[ne4];
	double eff5[ne5];

	TCanvas* c1 = new TCanvas("c1", "c1", 1000, 1000);
	TCanvas* c2 = new TCanvas("c2", "c2", 1000, 1000);
	TCanvas* c3 = new TCanvas("c3", "c3", 1000, 1000);

	TCanvas* e3 = new TCanvas("e3", "e3", 1000, 1000);
	TCanvas* e4 = new TCanvas("e4", "e4", 1000, 1000);
	TCanvas* e5 = new TCanvas("e5", "e5", 1000, 1000);

	TH2D* conteggi3 = new TH2D("conteggi3", "Rate PMT3; V; cps", 100, voltage[0] - 10, voltage[n3-1] + 10, 100, (counts3[0]/10)-10, (counts3[n3-1]/10)+100);
	TH2D* conteggi4 = new TH2D("conteggi4", "Rate PMT4; V; cps", 100, voltage4[0] - 10, voltage4[n4 - 1] + 10, 100, (counts4[0]/10) - 10, (counts4[n4 - 1]/10) + 100);
	TH2D* conteggi5 = new TH2D("conteggi5", "Rate PMT5; V; cps", 100, voltage5[0] - 10, voltage5[n5 - 1] + 10, 100, (counts5[0]/10) - 10, (counts5[n5 - 1]/10) + 100);

	//TH2D* epsilon3 = new TH2D("epsilon3", "efficienza PMT3; V; ", 100, voltage45[0] - 10, voltage45[10] + 10, 100, 350, 21000);
	//TH2D* epsilon4 = new TH2D("epsilon4", "efficienza PMT4; V; ", 100, voltage35[0] - 10, voltage35[10] + 10, 100, 350, 21000);
	//TH2D* epsilon5 = new TH2D("epsilon5", "efficienza PMT5; V; ", 100, voltage34[0] - 10, voltage34[10] + 10, 100, 350, 21000);

	TGraphErrors* Conteggi3 = new TGraphErrors(n3);
	TGraphErrors* Conteggi4 = new TGraphErrors(n4);
	TGraphErrors* Conteggi5 = new TGraphErrors(n5);

	TGraphErrors* Epsilon3 = new TGraphErrors(ne3);
	TGraphErrors* Epsilon4 = new TGraphErrors(ne4);
	TGraphErrors* Epsilon5 = new TGraphErrors(ne5);

	TGraphErrors* Epsilon3_acc = new TGraphErrors(ne3);
	TGraphErrors* Epsilon4_acc = new TGraphErrors(ne4);
	TGraphErrors* Epsilon5_acc = new TGraphErrors(ne5);
	//TLegend* legend = new TLegend(0.4, 0.6, 0.65, 0.9);
	//cout << "radice conteggio" << sqrt(counts3[0]) << endl;
	for (int i = 0; i < n3; i++) {
		Conteggi3->SetPoint(i, voltage[i], counts3[i]/10);
		Conteggi3->SetPointError(i, 0, (sqrt(counts3[i])/10));
	}
	for (int i = 0; i < n4; i++) {
		Conteggi4->SetPoint(i, voltage4[i], counts4[i]/10);
		Conteggi4->SetPointError(i, 0, (sqrt(counts4[i])/10));
	}
	for (int i = 0; i < n5; i++){
		Conteggi5->SetPoint(i, voltage5[i], counts5[i]/10);
		Conteggi5->SetPointError(i, 0, (sqrt(counts5[i])/10));
	}
	//conteggi in singola inizio
	c1->cd();
	//conteggi PMT3
	conteggi3->SetStats(0);
	conteggi3->Draw();
	Conteggi3->SetMarkerStyle(8);
	Conteggi3->SetMarkerSize(0.5);
	Conteggi3->SetMarkerColor(9);//kGreen
	Conteggi3->SetLineColor(9);//kGreen
	Conteggi3->Draw("P");

	//conteggi PMT4
	c2->cd();
	conteggi4->SetStats(0);
	conteggi4->Draw();
	Conteggi4->SetMarkerStyle(8);
	Conteggi4->SetMarkerSize(0.5);
	Conteggi4->SetMarkerColor(kRed);
	Conteggi4->SetLineColor(kRed);
	Conteggi4->Draw("P");

	//conteggi PMT5
	c3->cd();
	conteggi5->SetStats(0);
	conteggi5->Draw();
	Conteggi5->SetMarkerStyle(8);
	Conteggi5->SetMarkerSize(0.5);
	Conteggi5->SetMarkerColor(kBlue);
	Conteggi5->SetLineColor(kBlue);
	Conteggi5->Draw("P");

	//legend->AddEntry(Conteggi3, "PMT3");
	//legend->AddEntry(Conteggi4, "PMT4");
	//legend->AddEntry(Conteggi5, "PMT5");
	//legend->Draw();
	//conteggi in singola fine

	//efficenze inizio

	double w = (50. / 1000000000);
	double w_min = (2. / 1000000000);
	double t = 10;
	double acc45 = t * (10649/ t) * (10404/ t) * ((2 * w) - (2 * w_min));
	cout << "acc45 = " << acc45 << endl;
	for (int k = 0; k < ne3; k++) {
		//counts45_acc[k] = t * (counts4[k] / t) * (counts5[k] / t) * (2 * w - 2 * w_min);
		eff3_acc[k] = (counts345_e3[k]) / (counts45[k] - acc45);
		eff3[k] = (counts345_e3[k]) / (counts45[k]);
		//cout << "Volt = " << voltage45[k] << "  eff3 = " << eff3[k] << " +- " << sqrt((eff3[k] * (1 - eff3[k])) / counts45[k]) << "  eff3 acc = " << eff3_acc[k] << endl;
		Epsilon3->SetPoint(k, voltage45[k], eff3[k]);
		Epsilon3->SetPointError(k, 0, sqrt((eff3[k]*(1 - eff3[k])) / counts45[k]));
		Epsilon3_acc->SetPoint(k, voltage45[k], eff3_acc[k]);
	}
	cout << "-----------------------------------------------------------------------------------------------------" << endl;
	double acc35 = t * (9177/ t) * (10404/ t) * ((2 * w) - (2 * w_min));
	cout << "acc35 = " << acc35 << endl;
	for (int p = 0; p < ne4; p++) {
		//counts35_acc[p] = t * (counts3[p] / t) * (counts5[p] / t) * (2 * w - 2 * w_min);
		eff4_acc[p] = counts345_e4[p] / (counts35[p] - acc35);
		eff4[p] = counts345_e4[p] / (counts35[p]);
		cout << "Volt = " << voltage35[p] << "  eff4 = " << eff4[p] << " +- " << sqrt((eff4[p] * (1 - eff4[p])) / counts35[p]) << "  eff4 acc = " << eff4_acc[p] << endl;
		Epsilon4->SetPoint(p, voltage35[p], eff4[p]);
		Epsilon4->SetPointError(p, 0, sqrt((eff4[p]*(1 - eff4[p])) / counts35[p]));
		Epsilon4_acc->SetPoint(p, voltage35[p], eff4_acc[p]);
	}
	cout << "-----------------------------------------------------------------------------------------------------" << endl;
	double acc34 = t * (9177/ t) * (10649/ t) * ((2 * w) - (2 * w_min));
	cout << "acc34 = " << acc34 << endl;
	for (int l = 0; l < ne5; l++) {
		//counts34_acc[l] = t * (counts3[l] / t) * (counts4[l] / t) * (2 * w - 2 * w_min); //una unica
		eff5_acc[l] = counts345_e5[l] / (counts34[l] - acc34);
		eff5[l] = counts345_e5[l] / (counts34[l]);
		//cout << "Volt = " << voltage34[l] << "  eff5 = " << eff5[l] << " +- " << sqrt((eff5[l] * (1 - eff5[l])) / counts34[l]) << "  eff5 acc =" << eff5_acc[l] << endl;
		Epsilon5->SetPoint(l, voltage34[l], eff5[l]);
		Epsilon5->SetPointError(l, 0, sqrt((eff5[l]*(1-eff5[l]))/counts34[l]));
		Epsilon5_acc->SetPoint(l, voltage34[l], eff5_acc[l]);
	}

	TH2D* epsilon3 = new TH2D("epsilon3", "efficienza PMT3; V; ", 100, voltage45[0] - 10, voltage45[ne3 - 1] + 10, 100, 0, 1);
	TH2D* epsilon4 = new TH2D("epsilon4", "efficienza PMT4; V; ", 100, voltage35[0] - 10, voltage35[ne4 - 1] + 10, 100, 0, 1);
	TH2D* epsilon5 = new TH2D("epsilon5", "efficienza PMT5; V; ", 100, voltage34[0] - 10, voltage34[ne5 - 1] + 10, 100, 0, 1);

	e3->cd();
	epsilon3->SetStats(0);
	epsilon3->Draw();
	//disegno senza acc
	Epsilon3->SetMarkerStyle(8);
	Epsilon3->SetMarkerSize(0.8);//0.5
	Epsilon3->SetMarkerColor(9);//kGreen
	Epsilon3->SetLineColor(9);//kGreen
	Epsilon3->Draw("P");

	//disegno con acc
	Epsilon3_acc->SetMarkerStyle(4);
	Epsilon3_acc->SetMarkerSize(0.8);
	Epsilon3_acc->SetMarkerColor(kPink);
	Epsilon3_acc->SetLineColor(kPink);
	Epsilon3_acc->Draw("P");

	e4->cd();
	epsilon4->SetStats(0);
	epsilon4->Draw();
	//disegno senza acc
	Epsilon4->SetMarkerStyle(8);
	Epsilon4->SetMarkerSize(0.8);
	Epsilon4->SetMarkerColor(kRed);
	Epsilon4->SetLineColor(kRed);
	Epsilon4->Draw("P");

	//disegno con acc
	Epsilon4_acc->SetMarkerStyle(4);
	Epsilon4_acc->SetMarkerSize(0.8);
	Epsilon4_acc->SetMarkerColor(kBlack);
	Epsilon4_acc->SetLineColor(kBlack);
	//Epsilon4_acc->Draw("P");
	Epsilon4_acc->Draw("P");

	e5->cd();
	epsilon5->SetStats(0);
	epsilon5->Draw();
	//disegno senza acc
	Epsilon5->SetMarkerStyle(8);
	Epsilon5->SetMarkerSize(0.8);
	Epsilon5->SetMarkerColor(kBlue);
	Epsilon5->SetLineColor(kBlue);
	Epsilon5->Draw("P");

	//diegno con acc
	Epsilon5_acc->SetMarkerStyle(4);
	Epsilon5_acc->SetMarkerSize(0.8);
	Epsilon5_acc->SetMarkerColor(kPink);
	Epsilon5_acc->SetLineColor(kPink);
	Epsilon5_acc->Draw("P");
	//Epsilon5_acc->Draw("P");

}