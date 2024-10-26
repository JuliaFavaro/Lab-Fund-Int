#include <TRandom3.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <iostream>

void toyMonteCarlo(double T, double rate, int NExp, double efficienza1 = 0.9, double efficienza2 = 0.8, double efficienza3 = 0.7) {
    double t = 0.; // Time scale
    int Nevt = 0; // Event number
    int VistoDa1 = 0, VistoDa2 = 0, VistoDa3 = 0; // Counts for each PMT

    // Histograms for single counts, double and triple coincidences
    TH1F hSingleCounts1("hSingleCounts1", "Counts PMT 1", 100, 0, NExp);
    TH1F hSingleCounts2("hSingleCounts2", "Counts PMT 2", 100, 0, NExp);
    TH1F hSingleCounts3("hSingleCounts3", "Counts PMT 3", 100, 0, NExp);
    TH1F hDoubleCoincidence("hDoubleCoincidence", "Double Coincidence", 100, 0, NExp);
    TH1F hTripleCoincidence("hTripleCoincidence", "Triple Coincidence", 100, 0, NExp);

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

        // Fill single count histograms
        hSingleCounts1.Fill(VistoDa1);
        hSingleCounts2.Fill(VistoDa2);
        hSingleCounts3.Fill(VistoDa3);

        // Double coincidence
        if ((VistoDa1 && VistoDa2) || (VistoDa1 && VistoDa3) || (VistoDa2 && VistoDa3)) hDoubleCoincidence.Fill(1);

        // Triple coincidence
        if (VistoDa1 && VistoDa2 && VistoDa3) hTripleCoincidence.Fill(1);
    }

    // Create canvas to draw single counts histograms
    TCanvas* c1 = new TCanvas("c1", "Single Counts", 1200, 900);
    c1->Divide(1, 3);

    // Draw single counts histograms
    c1->cd(1);
    hSingleCounts1.SetLineColor(kRed);
    hSingleCounts1.Draw();
    c1->cd(2);
    hSingleCounts2.SetLineColor(kBlue);
    hSingleCounts2.Draw();
    c1->cd(3);
    hSingleCounts3.SetLineColor(kGreen);
    hSingleCounts3.Draw();

    // Add a legend for single counts
    TLegend *legend1 = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend1->AddEntry(&hSingleCounts1, "PMT 1", "l");
    legend1->AddEntry(&hSingleCounts2, "PMT 2", "l");
    legend1->AddEntry(&hSingleCounts3, "PMT 3", "l");
    legend1->Draw();

    // Create canvas to draw coincidence histograms
    TCanvas* c2 = new TCanvas("c2", "Coincidences", 1200, 900);
    c2->Divide(1, 2);

    // Draw double coincidence histogram
    c2->cd(1);
    hDoubleCoincidence.Draw();

    // Draw triple coincidence histogram
    c2->cd(2);
    hTripleCoincidence.Draw();

    // Update canvases
    c1->Update();
    c2->Update();

    // Print mean and std for each PMT
    std::cout << "PMT 1 Mean: " << hSingleCounts1.GetMean() << ", Std Dev: " << hSingleCounts1.GetStdDev() << std::endl;
    std::cout << "PMT 2 Mean: " << hSingleCounts2.GetMean() << ", Std Dev: " << hSingleCounts2.GetStdDev() << std::endl;
    std::cout << "PMT 3 Mean: " << hSingleCounts3.GetMean() << ", Std Dev: " << hSingleCounts3.GetStdDev() << std::endl;
    std::cout << "Simulation complete!" << std::endl;
}
