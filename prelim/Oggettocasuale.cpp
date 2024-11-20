#include <TRandom.h>
#include <vector>
#include <math.h>
#include <iostream>
#include <deque>
#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>

// Funzione binomiale
Double_t binomial(Double_t *x, Double_t *par) {
    Double_t k = x[0];    // Numero di successi
    Double_t n = par[0];  // Numero di prove
    Double_t p = par[1];  // Probabilità di successo
    Double_t binomial_coefficient = TMath::Binomial(n, k);
    Double_t probability = binomial_coefficient * pow(p, k) * pow(1 - p, n - k);
    return probability;
}

void ToyMC(unsigned int NExp) {
    std::vector<float> rate = {10, 10, 10};
    double cosmic = 1;

    TH1F* efficiency_histo = new TH1F("efficiency", "Efficiency PMT 2", 12, 0, 1);

    for (unsigned int exp = 0; exp < NExp; exp++) {
        Status<3> status(rate, cosmic);
        EfficiencyKill<3> efficiency_killer({0.7, 0.9, 0.8});

        double NeventsPMT1 = 0, NeventsPMT2 = 0, NeventsPMT3 = 0;
        double NeventsPMT13 = 0, NaccidentalPMT13 = 0, NeventsPMT123=0;
        double t = 0;

        std::deque<double> timesPMT1, timesPMT2, timesPMT3;

        while (t < 10) {  // Each experiment lasts 10 seconds
            Event evt = efficiency_killer.kill(status.next_event());
            t = evt.t;  // Update the time with the event time
            
            bool PMT1_fired = false;
            bool PMT2_fired = false;
            bool PMT3_fired = false;
            bool PMT1_accidental = false;
            bool PMT3_accidental = false;

            for (auto pmt = evt.fired_pmts.begin(); pmt != evt.fired_pmts.end(); pmt++){
                if (evt.type == Event::event_type::accidental) {
                    if (*pmt == 0) PMT1_accidental = true;
                    if (*pmt == 2) PMT3_accidental = true;
                }

                if (*pmt == 0) {
                    NeventsPMT1++;
                    PMT1_fired = true;
                    timesPMT1.push_back(t);
                }
                if (*pmt == 1) {
                    NeventsPMT2++;
                    PMT2_fired = true;
                    timesPMT2.push_back(t);
                }
                if (*pmt == 2) {
                    NeventsPMT3++;
                    PMT3_fired = true;
                    timesPMT3.push_back(t);
                }
            }

            // Remove old times beyond the 100ns window for PMT1 and PMT3
            while (!timesPMT1.empty() && t - timesPMT1.front() > 5e-9) timesPMT1.pop_front();
            while (!timesPMT2.empty() && t - timesPMT2.front() > 5e-9) timesPMT2.pop_front();
            while (!timesPMT3.empty() && t - timesPMT3.front() > 5e-9) timesPMT3.pop_front();

            // Check for coincidences between PMT1 and PMT3 within 100ns
            if (PMT1_fired && PMT3_fired) {
                for (double time1 : timesPMT1){
                    for (double time3 : timesPMT3){
                        if (fabs(time1 - time3) <= 5e-9) {
                            NeventsPMT13++;
                            if (PMT1_accidental || PMT3_accidental){
                                NaccidentalPMT13++;
                            }
                            break;
                        }
                    }
                    if (fabs(timesPMT1.back()-timesPMT3.back())<=5e-9) break;
                }
            }
            
            // Check for triple coincidences between PMT1, PMT2, and PMT3 within 100ns
            if (PMT1_fired && PMT2_fired && PMT3_fired) {
                for (double time1 : timesPMT1) {
                    for (double time2 : timesPMT2) {
                        for (double time3 : timesPMT3) {
                            if (fabs(time1 - time2) <= 5e-9 && fabs(time1 - time3) <= 5e-9) {
                                NeventsPMT123++;
                                break;
                            }
                        }
                        if (fabs(timesPMT1.back() - timesPMT2.back()) <= 5e-9) break;
                    }
                    if (fabs(timesPMT1.back() - timesPMT3.back()) <= 5e-9) break;
                }
            }
        }

        // Calculate efficiency for PMT 2 
        double efficiency_PMT2 = 0;
        if (NeventsPMT13 > NaccidentalPMT13) {
          efficiency_PMT2 = static_cast<double>(NeventsPMT123) / (NeventsPMT13 - NaccidentalPMT13); 
          efficiency_histo->Fill(efficiency_PMT2); 
        }

        // Print the results for each experiment
        std::cout << "Experiment " << exp + 1 << " results:" << std::endl;
        std::cout << "NeventsPMT123: " << NeventsPMT13 << std::endl;
        std::cout << "NeventsPMT13: " << NeventsPMT13 << std::endl;
        std::cout << "Efficiency " << efficiency_PMT2 << std::endl;
    }

    // Draw efficiency histogram
    TCanvas* c1 = new TCanvas("c1", "Efficiency Histogram", 800, 600);
    efficiency_histo->SetLineColor(kViolet+6); 
    efficiency_histo->SetLineWidth(3);
    efficiency_histo->Draw(); 
    double mean_efficiency = efficiency_histo->GetMean();
    double initial_efficiency = 0.9; // Efficienza iniziale fornita nel codice
    double percent_distance = fabs(mean_efficiency - initial_efficiency) / initial_efficiency * 100;
    std::cout << "Distanza percentuale dell'efficienza " << percent_distance << "%" << std::endl;

    // Definizione della funzione binomiale
    TF1 *binomFit = new TF1("binomFit", binomial, 0, 1, 2);
    binomFit->SetParameters(1000, 0.9);  // Inizializzazione dei parametri: n = 1000, p = 0.9
    efficiency_histo->Fit("binomFit");

    // Aggiungere testo alla canvas
    c1->cd();
    binomFit->Draw("same");
    TPaveText *pt = new TPaveText(0.1, 0.7, 0.3, 0.9, "NDC");
    pt->AddText(Form("Mean Efficiency: %.3f", mean_efficiency));
    pt->AddText(Form("Percent Distance: %.2f%%", percent_distance));
    pt->Draw();
    c1->Update();
}
