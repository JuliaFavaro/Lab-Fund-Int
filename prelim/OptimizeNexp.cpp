#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <vector> //variable length array
#include <algorithm>

// Global histograms for efficiencies
TH1F* eff1;
TH1F* eff2;
TH1F* eff3;

void toyMonteCarlo(double T, double rate, int NExp, double efficienza1, double efficienza2, double efficienza3){
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

    eff1 = new TH1F("eff1", "Efficienza 1", 10, 0, 1);
    eff2 = new TH1F("eff2", "Efficienza 2", 10, 0, 1);
    eff3 = new TH1F("eff3", "Efficienza 3", 10, 0, 1);
    
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
    
    eff1->Fit("g");
    eff2->Fit("g");
    eff3->Fit("g");
}

void findOptimalNExp(double T, double rate, double efficienza1, double efficienza2, double efficienza3) {
    int minNExp = 50;
    int maxNExp = 10000;
    int step = 50;
    
    vector<int> NExpValues;
    vector<double> avgDiffValues;

    for (int NExp = minNExp; NExp <= maxNExp; NExp += step) {
        toyMonteCarlo(T, rate, NExp, efficienza1, efficienza2, efficienza3);

        // Calculate the absolute differences from the mean efficiencies and initial efficiencies
        double meanEff1 = eff1->GetFunction("g")->GetParameter(1);
        double meanEff2 = eff2->GetFunction("g")->GetParameter(1);
        double meanEff3 = eff3->GetFunction("g")->GetParameter(1);

        double diffEff1 = std::abs(meanEff1 - efficienza1) / efficienza1;
        double diffEff2 = std::abs(meanEff2 - efficienza2) / efficienza2;
        double diffEff3 = std::abs(meanEff3 - efficienza3) / efficienza3;

        double avgDiff = (diffEff1 + diffEff2 + diffEff3) / 3.0;

        NExpValues.push_back(NExp);
        avgDiffValues.push_back(avgDiff);

        std::cout << "NExp: " << NExp << ", Avg Relative Difference: " << avgDiff << std::endl;
    }

    // Find the NExp that minimizes the average relative difference
    auto minElement = std::min_element(avgDiffValues.begin(), avgDiffValues.end());
    int optimalNExpIndex = std::distance(avgDiffValues.begin(), minElement);
    int optimalNExp = NExpValues[optimalNExpIndex];

    std::cout << "Optimal NExp: " << optimalNExp << std::endl;
}

int main() {
    double T = 10; // Example time scale,s
    double rate = 1; // Example event rate
    double efficienza1 = 0.9;
    double efficienza2 = 0.8;
    double efficienza3 = 0.7;
    
    findOptimalNExp(T, rate, efficienza1, efficienza2, efficienza3);
    std::cout << "Simulation complete!" << std::endl;
    return 0;
}

/*The variation in results each time you run the code is due to the inherent
randomness in Monte Carlo simulations. Monte Carlo methods rely on random 
sampling to obtain numerical results, which means that each run will produce
slightly different outcomes because the random numbers generated will vary. 
Try to use a seed to fix the results*/