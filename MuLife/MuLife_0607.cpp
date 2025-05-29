#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <TVirtualFFT.h>

// Funzione che esegue il fit e disegna tutto sul pad corrente
TF1* fit_exp(TH1F* histogram){
    // Fit esponenziale (creato qui dentro!)
    TF1* expFit = new TF1("expFit", "[0] * exp(-x / [1]) + [2]", 300, 20*1000);
    expFit->SetParameters(1000, 2200, 10);
    expFit->SetParNames("Amplitude", "Decay Time (ns)", "Constant");
    histogram->Fit("expFit","RL","",300, 20*1000); 
    gStyle->SetOptFit(1111);
    return expFit;
}

// Funzione che calcola tau_mu- e il suo errore
std::pair<double, double> muonNegativeTauWithError(double tau_fit, double tau_fit_err, double tau_muplus = 2.196) {
    double tau_muminus = 2.0 * tau_fit - tau_muplus;
    double err_muminus = 2.0 * tau_fit_err;
    return {tau_muminus, err_muminus};
}

// Funzione di log-verosimiglianza
double logLikelihood(const std::vector<int>& counts, const std::vector<double>& edges, double tau, int nTotal) {
    double ll = 0;
    for (size_t i = 0; i < counts.size(); ++i) {
        double p_i = std::exp(-edges[i]/tau) - std::exp(-edges[i+1]/tau);
        if (p_i <= 0) p_i = 1e-15; // evitare log(0)
        ll += counts[i] * std::log(p_i);
    }
    return ll;
}

// Calcolo informazione di Fisher numericamente
double fisherInformation(const std::vector<int>& counts, const std::vector<double>& edges, double tau, int nTotal) {
    double h = tau * 1e-5;
    double ll_plus = logLikelihood(counts, edges, tau + h, nTotal);
        double ll_minus = logLikelihood(counts, edges, tau - h, nTotal);
        double ll_0 = logLikelihood(counts, edges, tau, nTotal);
        double second_derivative = (ll_plus - 2*ll_0 + ll_minus) / (h*h);
    return -second_derivative;
}

// Funzione che effettua il plot tau vs binning
void plotTauVsBinning(const std::vector<double>& data, int rangeStart, int rangeEnd) {
    std::vector<int> binCounts = {55, 115, 175, 235, 295, 355,415};

    std::vector<double> tauValues, tauErrors;
    double minValue = 300.0;
    double maxValue = 20*1000.0;

    for (size_t idx = 0; idx < binCounts.size(); ++idx) {
        int nBins = binCounts[idx];
        TString histName = Form("hist_tau_%zu", idx);
        TString fitName = Form("fit_tau_%zu", idx);

        // Istogramma con binning corrente
        TH1D* hist = new TH1D(histName, "#Delta t (Start-Stop)", nBins, minValue, maxValue);
        for (const auto& v : data) hist->Fill(v);

        // Fit esponenziale
        TF1* fit = new TF1(fitName, "[0] * exp(-x / [1]) + [2]", rangeStart, rangeEnd);
        fit->SetParameters(1000, 2200, 10);
        fit->SetParNames("Amplitude", "Decay Time (ns)", "Constant");
        hist->Fit(fit, "RLQ0");

        double tau = fit->GetParameter(1);
        double tauErr = fit->GetParError(1);

        tauValues.push_back(tau);
        tauErrors.push_back(tauErr);
        std::cout << "nBins = " << nBins
                  << " | tau = " << tau << " +- " << tauErr << std::endl;

        delete fit;
        delete hist;
    }

    // Calcola la semidispersione dei tau ottenuti (errore sistematico da binning)
    auto [tauMin, tauMax] = std::minmax_element(tauValues.begin(), tauValues.end());
    double tauSistErr = (*tauMax - *tauMin) / 2.0;
    std::cout << "Errore sistematico da binning su tau_fit: " << tauSistErr << " ns" << std::endl;

    // Propaga su tau_mu-
    double tau_muplus_ns = 2196; // 2.196 μs in ns
    auto [tau_muminus_max, _] = muonNegativeTauWithError(*tauMax / 1000.0, 0, tau_muplus_ns / 1000.0);
    auto [tau_muminus_min, __] = muonNegativeTauWithError(*tauMin / 1000.0, 0, tau_muplus_ns / 1000.0);
    double tau_muminus_sist_err = (tau_muminus_max - tau_muminus_min) / 2.0;

    std::cout << "Errore sistematico su tau_mu- (da binning): " << tau_muminus_sist_err << " μs" << std::endl;

    // --- DISEGNO ---
    TCanvas* cTau = new TCanvas("cTau", "Tau vs Binning", 700, 600);
    TGraphErrors* grTau = new TGraphErrors(binCounts.size());
    for (size_t i = 0; i < binCounts.size(); ++i) {
        grTau->SetPoint(i, binCounts[i], tauValues[i]);
        grTau->SetPointError(i, 0.0, tauErrors[i]);
    }
    grTau->SetTitle("#tau in funzione del numero di bin");
    grTau->GetXaxis()->SetTitle("Numero di bin");
    grTau->GetYaxis()->SetTitle("#tau (ns)");
    grTau->SetMarkerStyle(21);
    grTau->SetMarkerColor(kBlue+2);
    grTau->SetLineColor(kBlue+2);
    grTau->Draw("AP");
    cTau->Update();
}

// Calcola la semidispersione di tau_fit per binning da 50 a 1000 (passo 20)
double semidispersioneTauFit(const std::vector<double>& data, int rangeStart, int rangeEnd) {
    std::vector<double> tauFits;
    double minValue = 300.0;
    double maxValue = 20*1000.0;

    for (int nBins = 50; nBins <= 1000; nBins += 20) {
        TH1D* hist = new TH1D("hist_scan", "", nBins, minValue, maxValue);
        for (const auto& v : data) hist->Fill(v);

        TF1* fit = new TF1("fit_scan", "[0]*exp(-x/[1])+[2]", rangeStart, rangeEnd);
        fit->SetParameters(1000, 2200, 10);
        fit->SetParNames("Amplitude", "Decay Time (ns)", "Constant");
        hist->Fit(fit, "RLQ0");

        tauFits.push_back(fit->GetParameter(1));
        delete fit;
        delete hist;
    }

    auto [tauMin, tauMax] = std::minmax_element(tauFits.begin(), tauFits.end());
    double semidisp = (*tauMax - *tauMin) / 2.0;
    std::cout << "Semidispersione tau_fit (tutti i bin da 50 a 1000, passo 20): " << semidisp << " ns" << std::endl;
    return semidisp;
}

int sceltabinning() {
    // Apri il file ROOT
    TFile* inputFile = TFile::Open("time_differences_0607.root", "READ");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Errore: impossibile aprire il file ROOT!" << std::endl;
        return 1;
    }

    // Recupera il TTree
    TTree* tree = (TTree*)inputFile->Get("TimeDifferences");
    if (!tree) {
        std::cerr << "Errore: impossibile trovare il TTree nel file ROOT!" << std::endl;
        inputFile->Close();
        return 1;
    }

    // Variabile per leggere il TTree
    double timeDifference;
    tree->SetBranchAddress("timeDifference", &timeDifference);

    // Numero di entries
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Numero di entries nel TTree: " << nEntries << std::endl;

    // Leggi tutti i dati in un vettore
    std::vector<double> data;
    data.reserve(nEntries);
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        if (timeDifference >= 0)
            data.push_back(timeDifference);
    }

    if (data.empty()) {
        std::cerr << "Errore: nessun dato valido trovato!" << std::endl;
        inputFile->Close();
        return 1;
    }

    // Stima iniziale di tau come media campionaria
    double tau_hat = 2200;
    std::cout << "Stima iniziale di tau: " << tau_hat << std::endl;

    // Range dati
    double minVal = *std::min_element(data.begin(), data.end());
    double maxVal = *std::max_element(data.begin(), data.end());

    // Range di numero di bin da testare
    int kMin = 50;
    int kMax = 1000;

    double maxFisher = -1;
    int bestK = kMin;

    for (int k = kMin; k <= kMax; ++k) {
        // Crea i bordi dei bin uniformi
        std::vector<double> edges(k+1);
        double binWidth = (maxVal - minVal) / k;
        for (int i = 0; i <= k; ++i) {
            edges[i] = minVal + i * binWidth;
        }

        // Conta gli eventi per bin
        std::vector<int> counts(k, 0);
        for (auto val : data) {
            int binIndex = int((val - minVal) / binWidth);
            if (binIndex < 0) binIndex = 0;
            if (binIndex >= k) binIndex = k - 1; // edge case
            counts[binIndex]++;
        }

        // Calcola l'informazione di Fisher per tau
        double fisher = fisherInformation(counts, edges, tau_hat, (int)data.size());

        if (fisher > maxFisher) {
            maxFisher = fisher;
            bestK = k;
        }
    }

    std::cout << "Numero di bin ottimale (massimizza informazione di Fisher): " << bestK << std::endl;

    //plotTauVsBinning(data, 300, 20*1000);
    double semiDisp = semidispersioneTauFit(data, 300, 20000);
    inputFile->Close();
    delete inputFile;
    return 0;
}

// Funzione per aggiungere timestamp e durata al canvas
void addTimestamp(const std::string &timestamp, const std::string &duration) {
    TLatex *latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);
    latex->DrawLatex(0.1, 0.96, timestamp.c_str());
    latex->DrawLatex(0.1, 0.91, duration.c_str());
}

// Funzione per calcolare i residui normalizzati
TGraphErrors* computeNormalizedResiduals(TH1F* histo, TF1* fit, double rangeMin, double rangeMax) {
    int nBins = histo->GetNbinsX();
    std::vector<double> x, y, ex, ey;

    for (int i = 1; i <= nBins; ++i) {
        double binCenter = histo->GetBinCenter(i);
        if (binCenter < rangeMin || binCenter > rangeMax) continue;
        double observed = histo->GetBinContent(i);
        double error = histo->GetBinError(i);
        double expected = fit->Eval(binCenter);

        // Solo se errore > 0
        if (error > 0) {
            x.push_back(binCenter);
            y.push_back((observed - expected) / error);
            ex.push_back(0.0);
            ey.push_back(0.0); // errore normalizzato
        }
    }

    auto* residuals = new TGraphErrors(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        residuals->SetPoint(i, x[i], y[i]);
        residuals->SetPointError(i, ex[i], ey[i]);
    }
    residuals->SetMarkerStyle(20);
    residuals->SetMarkerSize(0.8);
    return residuals;
}

int main() {
    // Apri il file ROOT
    TFile* inputFile = new TFile("time_differences_0607.root", "READ");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Errore: impossibile aprire il file ROOT!" << std::endl;
        return 1;
    }

    // Recupera il TTree
    TTree* tree = (TTree*)inputFile->Get("TimeDifferences");
    if (!tree) {
        std::cerr << "Errore: impossibile trovare il TTree nel file ROOT!" << std::endl;
        inputFile->Close();
        return 1;
    }

    // Variabile per leggere il TTree
    double timeDifference;
    tree->SetBranchAddress("timeDifference", &timeDifference);

    // Determina il numero di entries
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Numero di entries nel TTree: " << nEntries << std::endl;

    // Istogramma
    TH1F* histogram = new TH1F("histogram", "#Delta t (Start-StopC)", 88, 300, 20*1000);
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        histogram->Fill(timeDifference);
    }
    double binWidth = histogram->GetBinWidth(1); // Larghezza di un bin (tutti i bin hanno la stessa larghezza in un istogramma a bin uniforme)
    std::cout << "Bin width: " << binWidth << std::endl;

    TF1* expFit=fit_exp(histogram);
    // Ottieni tau_fit e errore dal fit (in ns), converti in μs
    double tau_fit = expFit->GetParameter(1) / 1000.0;    // in μs
    double tau_fit_err = expFit->GetParError(1) / 1000.0; // in μs

    // Calcola tau muone negativo e il suo errore
    auto [tau_muminus, err_muminus] = muonNegativeTauWithError(tau_fit, tau_fit_err);
    std::cout << "Tau muone negativo: " << tau_muminus << " ± " << err_muminus << " μs" << std::endl;

    
    std::string timestamp = "06.05.2025 14:45 Bin:88";
    std::string duration = "Durata: 40.5 ore";

    // Creazione del canvas con due subplot
    TCanvas* canvas1 = new TCanvas("canvas1", "Fit con Residui", 800, 600);
    canvas1->Divide(1, 2);

    // --- Primo pad: fit e istogramma ---

    canvas1->cd(1);
    histogram->SetLineColor(kGreen);
    histogram->SetYTitle("Conteggi");
    histogram->SetXTitle("#Delta t (ns)");

    histogram->Draw();
    expFit->SetLineColor(kGreen-6);
    expFit->Draw("SAME");
    addTimestamp(timestamp, duration);

    // --- Secondo pad: residui ---
    canvas1->cd(2);

    // Recupera il fit dalla lista delle funzioni dell'istogramma
    TGraphErrors* residuals = computeNormalizedResiduals(histogram, expFit, 300, 20*1000);
    residuals->SetMinimum(-5);
    residuals->SetMaximum(5);
    residuals->SetTitle("");
    residuals->Draw("AP");
    residuals->GetYaxis()->SetTitle("Residui norm.");
    residuals->GetXaxis()->SetTitle("#Delta t (ns)");
    TLine* zeroLine = new TLine(300, 0, 20*1000, 0);
    zeroLine->SetLineStyle(2);
    zeroLine->SetLineColor(kGray+2);
    zeroLine->Draw("SAME");

    canvas1->Update();
    canvas1->Draw();
    gStyle->SetOptFit(1111);
    canvas1->SaveAs("Risultati/Vita media/Fit_0607_bin100.jpg");
    canvas1->SaveAs("Risultati/Vita media/Fit_0607_bin100.pdf");

    std::cout << "Premi invio per uscire..." << std::endl;
    std::cin.get();

    return 0;
}
