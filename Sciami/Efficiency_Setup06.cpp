#include <math.h>
#include <stdio.h>
#include <iostream>
#include <TCanvas.h>
#include <TH2D.h>
#include <TGraphErrors.h>
#include <TMath.h>

#include "read_data10.h"

double sigma_eff(double nt, double nd, double ndacc, double n1, double n2, double time, double ntacc){
    double epsilon=nt/(nd-ndacc-ntacc); //binomial normal efficiency 
    double sigma_nt=sqrt(nd*epsilon*(1-epsilon));
    double sigma_ndacc=sqrt(pow(sqrt(n1)*n2,2)+pow(n1*sqrt(n2),2))*2*(40e-9-2e-9)/time;
    //std::cout<<sqrt(pow(sigma_ndacc*103*30,2)+pow(ndacc*sqrt(103*30),2))*2*(40e-9-2e-9)/time<<std::endl;
    
    double term1=sigma_nt/(nd-ndacc-ntacc);
    double term2=nt*sigma_ndacc/pow(nd-ndacc-ntacc,2); //ignoring the third component of ntacc

    double sigma_eff=sqrt(pow(term1,2)+pow(term2,2));
    return sigma_eff;
}


void calibration06() {
    // Length of the arrays for efficiency estimation

    static const int n1 =11; //V2=1700 V, V3=1700 V
	static const int n2=8; //v1=1620 V, V3=1700 V
	static const int n3=11; //V1=1620 V, V2=1700 V
    static const int time = 10;// Time passed in seconds 
    //double voltage1[n1] = {1500, 1520, 1540, 1560, 1580, 1600, 1620, 1640, 1660, 1680, 1700, 1730};
    double voltage1[11]={1500, 1525, 1550, 1575, 1600, 1625, 1650, 1675, 1700, 1725, 1750};
	double voltage2[n2]={1640,1660, 1680, 1700, 1720, 1740, 1770, 1790, };
	double voltage3[n3]={1500, 1525, 1550, 1575, 1600, 1625, 1650, 1675, 1700, 1725, 1750};

    //double counts1[n1] ={511, 655,1014, 1604, 2116,2503, 2631, 2975, 2969, 4180, 6876, 18389};
    double counts1[n1]={308, 460, 546, 657, 811, 1017, 1276, 1860, 3465, 7626, 19101};
	double counts2[n2]={365, 653,1005, 1181, 1825, 2850, 5017, 8096 };
    double counts3[n3]={463, 561, 656,767, 1092, 1324,2036, 3030,3737,5990,7124,};
	//double counts3[n3]={516, 521, 634, 685,917, 1170, 1645,1704,2761,3116,3666, 5731, 4697, 8118,  };

    //double counts23[n1] = {35, 34,30, 35, 38, 32,22, 24, 33,23,24, 33};
    double counts23[n1]={24, 27, 23,  23, 17, 21, 27, 12, 21, 22, 18};
	double counts13[n2]={83, 85, 84,77, 72, 89,94, 69};
	double counts12[n3]={14, 22, 27,31, 21, 22,16, 25,22,28,17};
	//double counts12[n3]={15,28,25, 22, 23, 18, 23, 23, 20,32,22,20,21, 24};

    //double counts123_e1[n1] = {24,23, 21, 26, 28,24, 16,18, 21,12, 17,24};
    double counts123_e1[n1]={16, 20, 19, 17, 12, 15, 19, 9, 15, 14, 15};
	double counts123_e2[n2]={12,13, 15, 11, 16, 31, 35,25  };
	double counts123_e3[n3]={8,12,  20,22,11,12,10, 18,19,20,12};
	//double counts123_e3[n3]={13,23, 18, 16, 15,14, 16, 15, 6, 23,15,12,15, 17};

    double eff1_acc[n1];
	double eff2_acc[n2];
	double eff3_acc[n3];
   
    // Declaring canvas
    TCanvas* c1 = new TCanvas("c1", "Setup06", 1500, 1500);
	c1->SetGrid(); 
    c1->Divide(2, 3); //colonne, righe
	
    // Graph with error bars
    TGraphErrors* Epsilon1_acc = new TGraphErrors(n1);
	TGraphErrors* Epsilon2_acc = new TGraphErrors(n2);
	TGraphErrors* Epsilon3_acc = new TGraphErrors(n3);

	TGraphErrors* Counts2Graph1 = new TGraphErrors(n1); 
	TGraphErrors* Counts2Graph2 = new TGraphErrors(n2);
	TGraphErrors* Counts2Graph3 = new TGraphErrors(n3);

	Counts2Graph1->SetTitle("Conteggi in singola PMT1; Voltaggio (V); Conteggi al secondo [cps]"); 
	Epsilon1_acc->SetTitle("Efficienza PMT1 ; Voltaggio (V); Efficienza");

	Counts2Graph2->SetTitle("Conteggi in singola PMT2; Voltaggio (V); Conteggi al secondo [cps]"); 
	Epsilon2_acc->SetTitle("Efficienza PMT2 ; Voltaggio (V); Efficienza");

	Counts2Graph3->SetTitle("Conteggi in singola PMT3; Voltaggio (V); Conteggi al secondo [cps]"); 
	Epsilon3_acc->SetTitle("Efficienza PMT3; Voltaggio (V); Efficienza");

    double w = 20e-9; //soglia: -30.0 mV
    double w_min = 2e-9;
    double acc23 = time * (118) * (370) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 23 " << acc23 << std::endl;
    double acc13 = time * (100) * (370) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 13 " << acc13 << std::endl;
    double acc12 = time * (100) * (118) * ((2 * w) - (2 * w_min));
    std::cout << "Accidentali 12 " << acc12 << std::endl;
	double acc123= time * (103) * (acc23/30) *((2*w)-(2*w_min));
    std::cout << "Accidentali 123 " << acc123 << std::endl;

    for (int k = 0; k < n1; k++) {
        eff1_acc[k] = counts123_e1[k] / (counts23[k] - acc23-acc123);
        Epsilon1_acc->SetPoint(k, voltage1[k], eff1_acc[k]);
        Epsilon1_acc->SetPointError(k, 0, sigma_eff(counts123_e1[k],counts23[k], acc23,118*10, 370*10, 10, acc123));  //binomial error if we overlook the accidental counts
		Counts2Graph1->SetPoint(k, voltage1[k], counts1[k] / time);
	  	Counts2Graph1->SetPointError(k, 0, sqrt(counts1[k]) / time); // Poissonian error 
    }

    std::cout << std::fixed << std::setprecision(2); // Imposta la precisione a 2 cifre decimali

    std::cout << voltage1[0] << " Singola " 
            << counts1[0] / time << " " 
            << sqrt(counts1[0]) / time 
            << " Efficienza " 
            << eff1_acc[0] << " " 
            << sigma_eff(counts123_e1[0], counts23[0],  acc23,118*10, 370*10, 10, acc123) 
            << std::endl;

    for (int k = 0; k < n2; k++) {
        eff2_acc[k] = counts123_e2[k] / (counts13[k] - acc13-acc123);
        Epsilon2_acc->SetPoint(k, voltage2[k], eff2_acc[k]);
        Epsilon2_acc->SetPointError(k, 0, sigma_eff(counts123_e2[k],counts13[k], acc13,100*10, 370*10, 10, acc123));//binomial error if we overlook the accidental counts
		Counts2Graph2->SetPoint(k, voltage2[k], counts2[k] / time);
	  	Counts2Graph2->SetPointError(k, 0, sqrt(counts2[k]) / time); // Poissonian error 
    }

    std::cout << voltage2[6] << " Singola " 
        << counts2[6] / time << " " 
        << sqrt(counts2[6]) / time 
        << " Efficienza "
        << eff2_acc[6] << " "
        << sigma_eff(counts123_e2[6],counts13[6], acc13,100*10, 370*10, 10, acc123)<< std::endl;

	for (int k = 0; k < n3; k++) {
        eff3_acc[k] = counts123_e3[k] / (counts12[k] - acc12-acc123);
        Epsilon3_acc->SetPoint(k, voltage3[k], eff3_acc[k]);
        Epsilon3_acc->SetPointError(k, 0, sigma_eff(counts123_e3[k],counts12[k], acc12,100*10, 118*10, 10, acc123));//binomial error if we overlook the accidental counts
		Counts2Graph3->SetPoint(k, voltage3[k], counts3[k] / time);
	  	Counts2Graph3->SetPointError(k, 0, sqrt(counts3[k]) / time); // Poissonian error 
        }

    
    std::cout << voltage3[0] << " Singola " 
            << counts3[0] / time << " " 
            << sqrt(counts3[0]) / time 
            << " Efficienza " 
            << eff3_acc[0] << " " 
            << sigma_eff(counts123_e3[0],counts12[0], acc12,100*10, 118*10, 10, acc123)<< std::endl;

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
    c1->SaveAs("Risultati/Puntilavoro_06.png");

}

// Funzione per calcolare i rates e gli errori
void CalculateRates(double interval, int num_intervals, const std::vector<double>& t1,
                    const std::vector<double>& t2, const std::vector<double>& t3,
                    std::vector<double>& rates_1, std::vector<double>& errors_1,
                    std::vector<double>& rates_2, std::vector<double>& errors_2,
                    std::vector<double>& rates_3, std::vector<double>& errors_3,
                    std::vector<double>& time_intervals) {
    
    for (int i = 0; i < num_intervals; ++i) {
        double start_time = i * interval;
        double end_time = start_time + interval;

        // Calcola il tempo medio dell'intervallo per il grafico finale
        time_intervals[i] = ((start_time + end_time) / 3600) / 2; // in ore

        int count_1 = std::count_if(t1.begin(), t1.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });

        int count_2 = std::count_if(t2.begin(), t2.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });

        int count_3 = std::count_if(t3.begin(), t3.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });

        // Calcola il rate come numero di eventi diviso per la larghezza dell'intervallo
        rates_1[i] = count_1 / interval;
        errors_1[i] = sqrt(count_1) / interval;
        rates_2[i] = count_2 / interval - 367e-3; // Correggere per il rumore di fondo (367 mHz)
        errors_2[i] = sqrt(count_2) / interval + sqrt(pow(2e-3, 2));
        rates_3[i] = count_3 / interval;
        errors_3[i] = sqrt(count_3) / interval;
    }
}


// Monitoraggio in funzione del tempo dell'efficienza dei PMT del Telescopio06 con fit lineare
void efficiency_set06(double& interval_eff,int& num_intervals_eff,std::vector<double>& counts23, std::vector<double>& counts12, 
                    std::vector<double>& triple_06){
    // Vettori per memorizzare le efficienze e gli errori
    std::vector<double> eff1_acc(num_intervals_eff, 0);
    std::vector<double> eff3_acc(num_intervals_eff, 0);
    std::vector<double> eff1_err(num_intervals_eff, 0);
    std::vector<double> eff3_err(num_intervals_eff, 0);
    std::vector<double> time_intervals(num_intervals_eff, 0);

    // Calcola l'efficienza in funzione del tempo
    for (int i = 0; i < num_intervals_eff; ++i) {
        double start_time = i * interval_eff;
        double end_time = start_time + interval_eff;

        // Calcola i conteggi per ciascun intervallo
        double count_triple = std::count_if(triple_06.begin(), triple_06.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });
        double count_double23 = std::count_if(counts23.begin(), counts23.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });
        double count_double12 = std::count_if(counts12.begin(), counts12.end(), [start_time, end_time](double t) {
            return t > start_time && t < end_time;
        });

        // Calcola le efficienze
        eff1_acc[i] = count_triple / (count_double23);
        eff1_err[i] = std::sqrt((eff1_acc[i] * (1 - eff1_acc[i])) / count_double23);
        eff3_acc[i] = count_triple / (count_double12);
        eff3_err[i] = std::sqrt((eff3_acc[i] * (1 - eff3_acc[i])) / count_double12);
        time_intervals[i] = (start_time + end_time) / 2.0/3600;
    }

    // Crea il grafico dell'efficienza in funzione del tempo
    TCanvas* ceff = new TCanvas("ceff", "Efficienza PMT1", 800, 600);
    ceff->SetGrid();

    TGraphErrors* graph_eff1 = new TGraphErrors(num_intervals_eff, time_intervals.data(), eff1_acc.data(), nullptr, eff1_err.data());
    graph_eff1->SetTitle("Efficienza PMT1;Tempo (h);Efficienza");
    graph_eff1->SetMarkerStyle(20);
    graph_eff1->SetMarkerColor(kBlue + 2);
    graph_eff1->SetLineColor(kBlue + 2);
    graph_eff1->Draw("AP");
    
    // Fit lineare per Efficienza PMT1
    TF1* lin_fit1 = new TF1("lin_fit1", "[0] + [1]*x", 0, *std::max_element(time_intervals.begin(), time_intervals.end()));
    lin_fit1->SetParName(0, "cost");  
    lin_fit1->SetParName(1, "coeff. ang.");  
    
    graph_eff1->Fit("lin_fit1", "L");
    lin_fit1->SetLineColor(kBlue + 5);

    // Estrai parametri dal fit
    double intercept = lin_fit1->GetParameter(0);
    double intercept_err = lin_fit1->GetParError(0);
    double slope = lin_fit1->GetParameter(1);
    double slope_err = lin_fit1->GetParError(1);
    
    // Calcola chi-quadro ridotto
    double chi_square = lin_fit1->GetChisquare();
    int ndf = num_intervals_eff - lin_fit1->GetNpar(); // numero di gradi di libertà
    double reduced_chi_square = chi_square / ndf;

    // Calcola la fluttuazione percentuale delle efficienze
    double mean_efficiency = std::accumulate(eff1_acc.begin(), eff1_acc.end(), 0.0) / num_intervals_eff;
    std::cout<<mean_efficiency<<std::endl;
    
    double sum_squared_diff = std::accumulate(eff1_acc.begin(), eff1_acc.end(), 0.0,
        [mean_efficiency](double sum, double value) {
            return sum + (value - mean_efficiency) * (value - mean_efficiency);
        });
    
    double std_dev = std::sqrt(sum_squared_diff / num_intervals_eff);
    
    // Fluttuazione percentuale
    double percent_fluctuation = (std_dev / mean_efficiency) * 100;
    std::cout << "Fluttuazione percentuale delle efficienze: " << percent_fluctuation << "%" << std::endl;

    // Aggiungi informazioni al riquadro
    TPaveText* pave = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
    pave->SetFillColor(kWhite);
    pave->SetBorderSize(1);
    
    pave->AddText(Form("Intercept: %.3f #pm %.3f Hz", intercept, intercept_err));
    pave->AddText(Form("Slope: (%.f #pm %.f) * 10^{-5} Hz", slope*1e5, slope_err*1e5));
    pave->AddText(Form("#chi^{2}/ndf: %.f", reduced_chi_square));

    pave->Draw();
    ceff->Update();
}

int acquisizione7giorni() {
    std::cout<<"Inizio"<<std::endl;

    // Nome del file di input
    std::string filename = "Dati/FIFOread_20241213-091357_7day.txt"; // Questo funziona solo se "Dati" si trova in una sottocartella della principale

    // Vettori per memorizzare i numeri dei canali e i tempi del file
    std::vector<int> decimalNumbers;
    std::vector<double> counts;
    readData(filename, decimalNumbers, counts);

    // Sistema i tempi con i reset
    std::vector<int> Numbers;
    std::vector<double> tempi;
    reset_clock(decimalNumbers, counts, Numbers, tempi);

    // Vettore di vettori per memorizzare i tempi di arrivo per ciascun canale
    std::vector<std::vector<double>> channelTimes(7);

    // Processa ciascun numero decimale per determinare i canali attivati
    for (size_t i = 0; i < decimalNumbers.size(); ++i) {
        int Number = Numbers[i];
        std::vector<int> activeChannels = getActiveChannels(Number);

        // Aggiungi il tempo corrispondente ai canali attivati
        for (int channel : activeChannels) {
            channelTimes[channel].push_back(tempi[i]);
        }
    }

    // Calcola il tempo totale di acquisizione
    double time_start_total = *std::min_element(tempi.begin(), tempi.end());
    double time_end_total = *std::max_element(tempi.begin(), tempi.end());
    double total_time = time_end_total - time_start_total; //secondi
    std::cout << "Tempo totale in ore: " << total_time / (3600) << std::endl;

    double interval = 3600; 
    int num_intervals=static_cast<int>(total_time / interval);

    efficiency_set06(interval, num_intervals,channelTimes[0], channelTimes[1], channelTimes[2]);

    return 0;
}
