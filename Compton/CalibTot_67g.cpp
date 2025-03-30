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
#include <TGraph.h>
#include <TAxis.h>
#include <TApplication.h>


int calibTOT_67() { //tra 2603 e 2703

    std::vector<double> Medie_i= { 1810,4087, 4629, 2359, 210.1};
    std::vector<double> Medie_f= { 1823,4107, 4650, 2366, 210.1};
    std::vector<double> Medie;
    if (Medie_i.size() == Medie_f.size()) {
        for (size_t i = 0; i < Medie_i.size(); ++i) {
            Medie.push_back((Medie_i[i] + Medie_f[i]) / 2.0);
        }

        std::cout << "Canale media calibrazione : ";
        for (double val : Medie) {
            std::cout << val << " ";
        }
    } else {
        std::cerr << "I vettori hanno dimensioni diverse!" << std::endl;
    }


    
    std::vector<double> Energie = { 0.511, 1.17 , 1.33 , 0.662 , 0.06}; //Na, Co1, Co2, Cs, Am
    std::vector<double> Err_x= { 0.7, 0.8, 0.8, 0.5, 0.1};
    std::vector<double> Err_y= {0.0001,0.00001,0.00001,0.00001,0.00001};
    int n = Medie.size();
     TGraphErrors* graph = new TGraphErrors(n);
 
     for (int i = 0; i < n; ++i) {
         graph->SetPoint(i, Medie[i], Energie[i]);
         graph->SetPointError(i, Err_x[i], Err_y[i]);
     }
    

    graph->SetTitle("Calibrazione 2603-2703 ; Medie dei fit calibrazioni; Energie dei decadiemnti");
    graph->SetMarkerStyle(8);
    graph->SetMarkerSize(1);
    graph->SetLineColor(kBlue);
    graph->SetMarkerColor(kBlue);

 
 TF1* lineare = new TF1("fitFunc", "pol1", 0 , 5000);

 lineare->SetLineColor(kGreen);
  graph->Fit(lineare, "R");

 TF1* pol2 = new TF1("fitFunc", "pol2", 0 , 5000);

  pol2->SetLineColor(kMagenta);
  graph->Fit(pol2, "R+");
   

 TCanvas* c1 = new TCanvas("c1", "Canvas", 800, 600);
 graph->Draw("AP");

 /* 
    double slope = fit->GetParameter(1);
    double intercept = fit->GetParameter(0);
    printf("Slope: %f, Intercept: %f\n", slope, intercept);
*/

std::ostringstream fit1Info, fit2Info; // Per concatenare i dettagli come stringa
fit1Info <<"a = " << lineare->GetParameter(0) << ", "
         << "b = " << lineare->GetParameter(1) << ", "
         << "#chi^{2} = " << lineare->GetChisquare()
         << ", NDF = " << lineare->GetNDF();

fit2Info << "a = " << pol2->GetParameter(2) << ", "<<std::endl
         << "b = " << pol2->GetParameter(1) << ", "<<std::endl
         << "c = " << pol2->GetParameter(0) << ", "<<std::endl
         << "#chi^{2} = " << pol2->GetChisquare()
         << ", NDF = " << pol2->GetNDF();

TLegend* legend = new TLegend(0.1, 0.7, 0.5, 0.9);
legend->AddEntry(graph, "Dati con err statistico","p" );
legend->AddEntry(lineare, "Fit lineare", "l");
legend->AddEntry((TObject*)0, fit1Info.str().c_str(), ""); 
legend->AddEntry(pol2, "Fit polinomiale", "l");
legend->AddEntry((TObject*)0, fit2Info.str().c_str(), ""); 
legend->SetTextSize(0.02);
legend->Draw();

//TLegend* legend =new TLegend(0.1, 0.7, 0.5, 0.9);

legend->Draw();
  
    double Canale1 = 3333; 
    double max1=  3343;
    double min1= 3323;
    double E1 = pol2->Eval(Canale1); 
    printf("Il valore di E1 corrispondente al Canale=%.3f è E1=%.3f\n", Canale1, E1);
    double E1max = pol2->Eval(max1); 
    printf("max1=%.3f   E1max=%.3f\n", max1, E1max);
    double E1min = pol2->Eval(min1); 
    printf("min1=%.3f  E1min=%.3f\n", min1, E1min);

    double Canale2= 3719;
    double max2= 3731;
    double min2= 3707;
    double E2 = pol2->Eval(Canale2); 
    printf("Il valore di E2 corrispondente al Canale=%.3f è E2=%.3f\n", Canale2, E2);
    double E2max = pol2->Eval(max2);
    printf("max2=%.3f    E2max=%.3f\n", max2, E2max);
    double E2min = pol2->Eval(min2); 
    printf("min1=%.3f    E2min=%.3f\n", min2, E2min);


double a = pol2->GetParameter(2);
double b = pol2->GetParameter(1);
double c = pol2->GetParameter(0);
double sigma_a = pol2->GetParError(2);
double sigma_b = pol2->GetParError(1);
double sigma_c = pol2->GetParError(0);

//derivate parziali per formula errore
double partial_a1 = pow(Canale1, 2);
double partial_b1 = Canale1;
double partial_c1 = 1.0;


double Err_E1 = sqrt(pow(partial_a1 * sigma_a, 2) + pow(partial_b1 * sigma_b, 2) + pow(partial_c1 * sigma_c, 2));
std::cout << "L'errore associato a E1 è: " <<Err_E1 << std::endl;

//derivate parziali per formula errore
double partial_a2 = pow(Canale2, 2);
double partial_b2 = Canale2;
double partial_c2 = 1.0;


double Err_E2 = sqrt(pow(partial_a2 * sigma_a, 2) + pow(partial_b2 * sigma_b, 2) + pow(partial_c2 * sigma_c, 2));
std::cout << "L'errore associato a E2 è: " <<Err_E2 << std::endl;

    c1->Update();



 return 0; 
}
