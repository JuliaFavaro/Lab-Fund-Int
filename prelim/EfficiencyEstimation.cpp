#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <vector> //variable length array

void toyMonteCarlo(double T, double rate, int NExp, double efficienza1 = 0.9, double efficienza2 = 0.8, double efficienza3 = 0.7){
    
    /* Goal: simulare l'esperienza priliminare:
    Tre rivelatori con efficienze date sovrapposti: istogrammi dei conteggi singoli.
    Riempire l'istogramma delle coincidenze doppie e triple. (no accidentali per ora)
    Costruire un istogramma dell'efficienza stimata dal rapporto triple/doppie */

    double t = 0.; // Time scale
    int Nevt = 0; // Event number
    int VistoDa1 = 0, VistoDa2 = 0, VistoDa3 = 0; // Counts for each PMT
    double detected12=0, detected13=0, detected23=0; //DoubleCoincidence, we need to have doubles to do the division without getting zero
    double detected=0; //TripleCoincidence

    // Histograms for single counts, double and triple coincidences
    TH1F* hSingleCounts1=new TH1F("SingleCounts1", "Counts PMT 1",  (int) (T*rate*3), 0, T * rate * 3);
    TH1F* hSingleCounts2=new TH1F("SingleCounts2", "Counts PMT 2",  (int) (T*rate*3), 0, T * rate * 3);
    TH1F* hSingleCounts3=new TH1F("SingleCounts3", "Counts PMT 3",  (int) (T*rate*3), 0, T * rate * 3);
    
    TH1F* hDoubleCoincidence12=new TH1F("DoubleCoincidence", "Double Coincidence12", (int) (T*rate*3), 0, T * rate * 3);
    TH1F* hDoubleCoincidence23=new TH1F("DoubleCoincidence", "Double Coincidence23", (int) (T*rate*3), 0, T * rate * 3);
    TH1F* hDoubleCoincidence13=new TH1F("DoubleCoincidence", "Double Coincidence13", (int) (T*rate*3), 0, T * rate * 3);
    TH1F* hTripleCoincidence=new TH1F("TripleCoincidence", "Triple Coincidence", (int) (T*rate*3), 0, T * rate * 3);

    TH1F* eff1 = new TH1F("eff1", "Efficienza 1", 10, 0, 1);
    TH1F* eff2 = new TH1F("eff2", "Efficienza 2", 10, 0, 1);
    TH1F* eff3 = new TH1F("eff3", "Efficienza 3", 10, 0, 1);
    
    //Fit functions
    TF1* gauss=new TF1("gauss", "gaus", 0, T * rate * 3); 
    TF1* g = new TF1("g", "gaus", 0, 1);


    // Simulate experiments
    for (int experiment = 0; experiment < NExp; experiment++){
        t = 0; // Reset clock for each experiment
        Nevt = 0;
        VistoDa1 = VistoDa2 = VistoDa3 = 0; // Reset counts
        detected12=detected13=detected23=0;
        detected=0; 

        vector <double> t1; //arrival times
        vector <double> t2;
        vector <double> t3;

        while (t < T) {
            t += -log(gRandom->Uniform()) / rate;
            Nevt++;

            /*bool detected1 = gRandom->Uniform() < efficienza1;
            bool detected2 =gRandom->Uniform()< efficienza2;
            bool detected3 =gRandom->Uniform() < efficienza3;*/

            if (gRandom->Uniform() < efficienza1){
                VistoDa1++;
                //cout << "visto da 1" << endl;
                t1.push_back(t); //sending this value back to the vector
            }
            else t1.push_back(-1);
            if (gRandom->Uniform() < efficienza2){
                VistoDa2++;
                //cout << "visto da 2" << endl;
                t2.push_back(t);
            }
            else t2.push_back(-1);
            if (gRandom->Uniform() < efficienza3){
                VistoDa3++;
                //cout << "visto da 3" << endl;
                t3.push_back(t);
            }
            else t3.push_back(-1);
        }
        
        //cout << "Nevt = " << Nevt << "  t1 size = " << t1.size() << "  t2 size = " << t2.size() << "  t3 size = " << t3.size() << endl;
        
        //Check for coincidence
        for (int j = 0; j < Nevt + 1; j++){
            if (t1[j] == t2[j] && t1[j] != -1)detected12++;
            if (t1[j] == t3[j] && t1[j] != -1)detected13++;
            if (t2[j] == t3[j] && t2[j] != -1)detected23++;
            if (t1[j] == t2[j] && t2[j] == t3[j] && t1[j] != -1)detected++;
        }

        // Fill count histograms
        hSingleCounts1->Fill(VistoDa1);
        hSingleCounts2->Fill(VistoDa2);
        hSingleCounts3->Fill(VistoDa3);

        hDoubleCoincidence12->Fill(detected12);
        hDoubleCoincidence23->Fill(detected23);
        hDoubleCoincidence13->Fill(detected13);

        hTripleCoincidence->Fill(detected);
        eff1->Fill((detected/ detected23));
        eff2->Fill((detected/ detected13));
        eff3->Fill((detected/ detected12));
    }

    gStyle->SetOptStat("e"); //only put entries in the histogram info boxes
    gStyle->SetOptFit(1111); //print out fit values in the histogram info boxes

    // Create canvas to draw single counts histograms
    TCanvas* c1 = new TCanvas("c1", "Single Counts", 1500, 500);
    c1->Divide(1, 3);

    // Draw single counts histograms
    c1->cd(1);
    hSingleCounts1->SetLineColor(kMagenta);
    hSingleCounts1->SetLineWidth(3);
    hSingleCounts1->Fit("gauss");
    //hSingleCounts1.DrawClone(); //you need to use this only when not working with pointers
    c1->cd(2);
    hSingleCounts2->SetLineColor(kCyan);
    hSingleCounts2->SetLineWidth(3);
    hSingleCounts2->Fit("gauss");
    c1->cd(3);
    hSingleCounts3->SetLineColor(kYellow);
    hSingleCounts3->SetLineWidth(3);
    hSingleCounts3->Fit("gauss");

    TCanvas* c2 = new TCanvas("c2", "Coincidence Counts", 1500, 500);
    c2->Divide(1, 3);

    // Draw double coincidence histograms
    c2->cd(1);
    hDoubleCoincidence12->SetLineColor(kViolet -3);
    hDoubleCoincidence12->SetLineWidth(3);
    hDoubleCoincidence12->Fit("gauss");
    c2->cd(2);
    hDoubleCoincidence23->SetLineColor(kTeal -3);
    hDoubleCoincidence23->SetLineWidth(3);
    hDoubleCoincidence23->Fit("gauss");
    c2->cd(3);
    hDoubleCoincidence13->SetLineColor(kPink -3);
    hDoubleCoincidence13->SetLineWidth(3);
    hDoubleCoincidence13->Fit("gauss");

    TCanvas* c3 = new TCanvas("c3", "Effiency", 1500, 500);
    c3->Divide(1, 3);

    //Draw efficiency histograms
    c3->cd(1);
    eff1->SetLineColor(kMagenta);
    eff1->SetLineWidth(3);
    eff1->Draw();
    eff1->Fit("g");
    c3->cd(2);
    eff2->SetLineColor(kCyan);
    eff2->SetLineWidth(3);
    eff2->Draw();
    eff2->Fit("g");
    c3->cd(3);
    eff3->SetLineColor(kYellow);
    eff3->SetLineWidth(3);
    eff3->Draw();
    eff3->Fit("g");

    // Get the mean values from the Gaussian fits
    double meanEff1 = eff1->GetFunction("g")->GetParameter(1);
    double meanEff2 = eff2->GetFunction("g")->GetParameter(1);
    double meanEff3 = eff3->GetFunction("g")->GetParameter(1);

    // Calculate the differences
    double diffEff1 = std::abs(meanEff1 - efficienza1)/efficienza1;
    double diffEff2 = std::abs(meanEff2 - efficienza2)/efficienza2;
    double diffEff3 = std::abs(meanEff3 - efficienza3)/efficienza3;
    
    // Print the differences
    std::cout << "Difference in efficiency 1: " << diffEff1*100 << std::endl;
    std::cout << "Difference in efficiency 2: " << diffEff2*100 << std::endl;
    std::cout << "Difference in efficiency 3: " << diffEff3*100 << std::endl;

    std::cout << "Simulation complete!" << std::endl;
}