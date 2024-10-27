#include <TRandom3.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>

void toyMonteCarlo(double T, double rate, int NExp, double efficienza1 = 0.9, double efficienza2 = 0.8, double efficienza3 = 0.7){
    
    /* Goal: simulare l'esperienza priliminare:
    Tre rivelatori con efficienze date sovrapposti: istogrammi dei conteggi singoli.
    Riempire l'istogramma delle coincidenze doppie e triple.
    Contare i conteggi accidentali.
    Costruire un istogramma dell'efficienza stimata dal rapporto triple/doppie */

    double t = 0.; // Time scale
    int Nevt = 0; // Event number
    int VistoDa1 = 0, VistoDa2 = 0, VistoDa3 = 0; // Counts for each PMT
    int detected12=0, detected13=0, detected23=0; //DoubleCoincidence
    int detected=0; //TripleCoincidence

    // Histograms for single counts, double and triple coincidences
    TH1F hSingleCounts1("SingleCounts1", "Counts PMT 1",  (int) (T*rate*3), 0, T * rate * 3);
    TH1F hSingleCounts2("SingleCounts2", "Counts PMT 2",  (int) (T*rate*3), 0, T * rate * 3);
    TH1F hSingleCounts3("SingleCounts3", "Counts PMT 3",  (int) (T*rate*3), 0, T * rate * 3);
    
    TH1F hDoubleCoincidence12("DoubleCoincidence", "Double Coincidence12", (int) (T*rate*3), 0, T * rate * 3);
    TH1F hDoubleCoincidence23("DoubleCoincidence", "Double Coincidence23", (int) (T*rate*3), 0, T * rate * 3);
    TH1F hDoubleCoincidence13("DoubleCoincidence", "Double Coincidence13", (int) (T*rate*3), 0, T * rate * 3);
    TH1F hTripleCoincidence("TripleCoincidence", "Triple Coincidence", (int) (T*rate*3), 0, T * rate * 3);

    // Simulate experiments
    for (int experiment = 0; experiment < NExp; experiment++){
        t = 0; // Reset clock for each experiment
        Nevt = 0;
        VistoDa1 = VistoDa2 = VistoDa3 = 0; // Reset counts
        detected12=detected13=detected23=0;
        detected=0; 

        while (t < T) {
            t += -log(gRandom->Uniform()) / rate;
            Nevt++;

            bool detected1 = gRandom->Uniform() < efficienza1;
            bool detected2 =gRandom->Uniform()< efficienza2;
            bool detected3 =gRandom->Uniform() < efficienza3;

            if (detected1) VistoDa1++;
            if (detected2) VistoDa2++;
            if (detected3) VistoDa3++;
 
            // Check for coincidences
            if (detected1 && detected2) detected12++;
            if (detected2 && detected3) detected23++;
            if (detected1 && detected3) detected13++;

            if (detected1 && detected2 && detected3) detected++;
        }

        // Fill count histograms
        hSingleCounts1.Fill(VistoDa1);
        hSingleCounts2.Fill(VistoDa2);
        hSingleCounts3.Fill(VistoDa3);

        hDoubleCoincidence12.Fill(detected12);
        hDoubleCoincidence23.Fill(detected23);
        hDoubleCoincidence13.Fill(detected13);

        hTripleCoincidence.Fill(detected);
    }

    // Create canvas to draw single counts histograms
    TCanvas* c1 = new TCanvas("c1", "Single Counts", 1200, 900);
    c1->Divide(1, 3);

    // Draw single counts histograms
    c1->cd(1);
    hSingleCounts1.SetLineColor(kMagenta);
    hSingleCounts1.SetLineWidth(3);
    hSingleCounts1.DrawClone();
    c1->cd(2);
    hSingleCounts2.SetLineColor(kCyan);
    hSingleCounts2.SetLineWidth(3);
    hSingleCounts2.DrawClone();
    c1->cd(3);
    hSingleCounts3.SetLineColor(kYellow);
    hSingleCounts3.SetLineWidth(3);
    hSingleCounts3.DrawClone();
    // Update canvas
    c1->Update();

    TCanvas* c2 = new TCanvas("c2", "Coincidence Counts", 1200, 900);
    c2->Divide(3, 2);
    // Draw double coincidence histograms
    c2->cd(1);
    hDoubleCoincidence12.SetLineColor(kViolet -3);
    hDoubleCoincidence12.SetLineWidth(3);
    hDoubleCoincidence12.DrawClone();
    c2->cd(2);
    hDoubleCoincidence23.SetLineColor(kTeal -3);
    hDoubleCoincidence23.SetLineWidth(3);
    hDoubleCoincidence23.DrawClone();
    c2->cd(3);
    hDoubleCoincidence13.SetLineColor(kPink -3);
    hDoubleCoincidence13.SetLineWidth(3);
    hDoubleCoincidence13.DrawClone();
    // Update canvas
    c2->Update();


    // Printing on display mean and standard deviation
    std::cout << "PMT 1 Mean: " << hSingleCounts1.GetMean() << ", Std Dev: " << hSingleCounts1.GetStdDev() << std::endl;
    std::cout << "PMT 2 Mean: " << hSingleCounts2.GetMean() << ", Std Dev: " << hSingleCounts2.GetStdDev() << std::endl;
    std::cout << "PMT 3 Mean: " << hSingleCounts3.GetMean() << ", Std Dev: " << hSingleCounts3.GetStdDev() << std::endl;

    std::cout << "PMT 12 Mean: " <<  hDoubleCoincidence12.GetMean() << ", Std Dev: " <<  hDoubleCoincidence12.GetStdDev() << std::endl;
    std::cout << "PMT 23 Mean: " <<  hDoubleCoincidence23.GetMean() << ", Std Dev: " << hDoubleCoincidence23.GetStdDev() << std::endl;
    std::cout << "PMT 13 Mean: " <<  hDoubleCoincidence13.GetMean() << ", Std Dev: " <<  hDoubleCoincidence13.GetStdDev() << std::endl;

    std::cout << "PMT 123 Mean: " << hTripleCoincidence.GetMean() << ", Std Dev: " <<  hTripleCoincidence.GetStdDev() << std::endl;
    std::cout << "Simulation complete!" << std::endl;
}
