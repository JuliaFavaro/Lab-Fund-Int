#include <TRandom3.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>

/* Goal: simulare l'esperienza priliminare:
   Tre rivelatori con efficienze date sovrapposti.
   Riempire l'istogramma delle coincidene doppie, triple.
   Costruire un istogramma dell'efficienza stimata dal rapporto triple/doppie */

void toyMonteCarlo(double T, double rate, int NExp, double efficienza1 = 0.9, double efficienza2 = 0.8, double efficienza3 = 0.7) {
    double t = 0.; // Time scale
    int Nevt = 0; // Event number
    int VistoDa1 = 0, VistoDa2 = 0, VistoDa3 = 0; // Counts for each PMT

    gROOT->SetStyle( "Plain" ); //nicer graphs apparently

    // Histograms for single counts, double and triple coincidences
    TH1F hSingleCounts1("SingleCounts1", "Counts PMT 1",  (int) (T*rate*3), 0, T * rate * 3);
    TH1F hSingleCounts2("SingleCounts2", "Counts PMT 2",  (int) (T*rate*3), 0, T * rate * 3);
    TH1F hSingleCounts3("SingleCounts3", "Counts PMT 3",  (int) (T*rate*3), 0, T * rate * 3);
    
    TH1F hDoubleCoincidence("DoubleCoincidence", "Double Coincidence", 100, 0, NExp);
    TH1F hTripleCoincidence("TripleCoincidence", "Triple Coincidence", 100, 0, NExp);

    // Initialize random number generator
    TRandom3 rnd;

    // Simulate experiments
    for (int experiment = 0; experiment < NExp; experiment++) {
        t = 0; // Reset clock for each experiment
        Nevt = 0;
        VistoDa1 = VistoDa2 = VistoDa3 = 0; // Reset counts

        while (t < T) {
            t += -log(rnd.Uniform()) / rate;
            Nevt++;

            if (rnd.Uniform() < efficienza1) VistoDa1++;
            if (rnd.Uniform() < efficienza2) VistoDa2++;
            if (rnd.Uniform() < efficienza3) VistoDa3++;
        }
        // Debugging output to verify counts
        //std::cout << "Experiment " << experiment << ": PMT1=" << VistoDa1 << ", PMT2=" << VistoDa2 << ", PMT3=" << VistoDa3 << std::endl;

        // Fill single count histograms
        hSingleCounts1.Fill(VistoDa1);
        hSingleCounts2.Fill(VistoDa2);
        hSingleCounts3.Fill(VistoDa3);
    }

    // Create canvas to draw single counts histograms
    TCanvas* c1 = new TCanvas("c1", "Single Counts", 1200, 900);
    c1->Divide(1, 3);

    // Draw single counts histograms
    c1->cd(1);
    hSingleCounts1.SetLineColor(kRed);
    hSingleCounts1.SetLineWidth(3);
    hSingleCounts1.DrawClone();
    c1->cd(2);
    hSingleCounts2.SetLineColor(kBlue);
    hSingleCounts2.SetLineWidth(3);
    hSingleCounts2.DrawClone();
    c1->cd(3);
    hSingleCounts3.SetLineColor(kGreen);
    hSingleCounts3.SetLineWidth(3);
    hSingleCounts3.DrawClone();
    // Update canvas
    c1->Update();


    std::cout << "PMT 1 Mean: " << hSingleCounts1.GetMean() << ", Std Dev: " << hSingleCounts1.GetStdDev() << std::endl;
    std::cout << "PMT 2 Mean: " << hSingleCounts2.GetMean() << ", Std Dev: " << hSingleCounts2.GetStdDev() << std::endl;
    std::cout << "PMT 3 Mean: " << hSingleCounts3.GetMean() << ", Std Dev: " << hSingleCounts3.GetStdDev() << std::endl;
    std::cout << "Simulation complete!" << std::endl;
}
