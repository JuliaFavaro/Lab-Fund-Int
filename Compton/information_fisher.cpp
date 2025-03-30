#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <TCanvas.h>
#include <cmath>
#include <iomanip>
#include <sstream>

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>
#include <sys/stat.h>
#include <TMinuit.h>

void readData(const std::string& filename, std::vector<double>& channels) {
    // Open the file
    std::ifstream file(filename);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        exit(1); // Exit the program in case of error
    }

    double number;

    // Read data from the file
    while (file >> number) {
        channels.push_back(number);
    }
    // Close the file
    file.close();
}

TF1* fit_pol4_nonrebin(TH1D* hCo) {
    // Fit a fourth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4", 2800, 3300);
    f1->SetParameter(0, 50);
    f1->SetParameter(1, 1e-2);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-7);
    f1->SetParameter(4, 1e-11);
    hCo->Fit("f1", "RLMI+N", "", 2800, 3300);

    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4);
    // Fit 2
    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*exp(-0.5*((x-[6])/[7])**2)", 2800, 3850);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, 80);
    f2->SetParameter(6, 3703);
    f2->SetParameter(7, 199);
    hCo->Fit("f2", "RLMI+N", "", 2800, 3900);

    double b0 = f2->GetParameter(0);
    double b1 = f2->GetParameter(1);
    double b2 = f2->GetParameter(2);
    double b3 = f2->GetParameter(3);
    double b4 = f2->GetParameter(4);
    double b5 = f2->GetParameter(5);
    double b6 = f2->GetParameter(6);
    double b7 = f2->GetParameter(7);

    // Total fit
    TF1* f3 = new TF1("f3", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*TMath::Gaus(x, [6], [7]) + [8]*TMath::Gaus(x, [9], [10])", 2750, 4600);
    f3->SetParameter(0, a0);
    f3->SetParameter(1, a1);
    f3->SetParameter(2, a2);
    f3->SetParameter(3, a3);
    f3->SetParameter(4, a4);
    f3->SetParameter(5, b5);
    f3->SetParameter(6, b6);
    f3->SetParameter(7, b7);

    f3->SetParameter(8, 100);
    f3->SetParameter(9, 4100);
    f3->SetParameter(10, 142);

    f3->SetParName(5, "A_1"); // Gaussian amplitude
    f3->SetParName(6, "#mu_1");
    f3->SetParName(7, "#sigma_1");
    f3->SetParName(8, "A_2");
    f3->SetParName(9, "#mu_2");
    f3->SetParName(10, "#sigma_2");

    hCo->Fit("f3", "LN", "", 2750, 4600);
    gStyle->SetOptFit(1111);
    return f3;
}

TF1* fit_pol4(TH1D* hCo) {
    // Fit a fourth-degree polynomial to the background part
    TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4", 2800, 3200);
    f1->SetParameter(0, 50);
    f1->SetParameter(1, 1e-2);
    f1->SetParameter(2, 1e-6);
    f1->SetParameter(3, 1e-7);
    f1->SetParameter(4, 1e-11);
    hCo->Fit("f1", "RLMI+N", "", 2800, 3200);

    double a0 = f1->GetParameter(0);
    double a1 = f1->GetParameter(1);
    double a2 = f1->GetParameter(2);
    double a3 = f1->GetParameter(3);
    double a4 = f1->GetParameter(4);

    // Fit 2
    TF1* f2 = new TF1("f2", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*exp(-0.5*((x-[6])/[7])**2)", 2800, 3850);
    f2->SetParameter(0, a0);
    f2->SetParameter(1, a1);
    f2->SetParameter(2, a2);
    f2->SetParameter(3, a3);
    f2->SetParameter(4, a4);
    f2->SetParameter(5, 40);
    f2->SetParameter(6, 3283);
    f2->SetParameter(7, 199);
    hCo->Fit("f2", "RLMI+N", "", 2800, 3850);

    double b0 = f2->GetParameter(0);
    double b1 = f2->GetParameter(1);
    double b2 = f2->GetParameter(2);
    double b3 = f2->GetParameter(3);
    double b4 = f2->GetParameter(4);
    double b5 = f2->GetParameter(5);
    double b6 = f2->GetParameter(6);
    double b7 = f2->GetParameter(7);

    // Total fit
    TF1* f3 = new TF1("f3", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4 + [5]*TMath::Gaus(x, [6], [7]) + [8]*TMath::Gaus(x, [9], [10])", 2750, 4520);
    f3->SetParameter(0, a0);
    f3->SetParameter(1, a1);
    f3->SetParameter(2, a2);
    f3->SetParameter(3, a3);
    f3->SetParameter(4, a4);
    f3->SetParameter(5, b5);
    f3->SetParameter(6, b6);
    f3->SetParameter(7, b7);

    f3->SetParameter(8, 51);
    f3->SetParameter(9, 3875);
    f3->SetParameter(10, 142);

    f3->SetParName(5, "A_1"); // Gaussian amplitude
    f3->SetParName(6, "#mu_1");
    f3->SetParName(7, "#sigma_1");
    f3->SetParName(8, "A_2");
    f3->SetParName(9, "#mu_2");
    f3->SetParName(10, "#sigma_2");

    hCo->Fit("f3", "L", "", 2750, 4520);
    gStyle->SetOptFit(1111);
    return f3;
}

TH1D* histogram_nonrebin(const std::vector<double>& channels, const char* title, Color_t color) {
    TH1D* h1 = new TH1D("h1_nonrebin", title, channels.size(), 0, channels.size());

    // Fill the histogram with data
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i + 1, channels[i]);
    }

    h1->SetLineColor(color);
    h1->GetXaxis()->SetTitle("Canali");
    h1->GetYaxis()->SetTitle("Conteggi");
    return h1;
}

TH1D* histogram(const std::vector<double>& channels, const char* hist_name, const char* title, Color_t color) {
    TH1D* h1 = new TH1D(hist_name, title, channels.size(), 0, channels.size());

    // Fill the histogram with data
    for (size_t i = 0; i < channels.size(); ++i) {
        h1->SetBinContent(i + 1, channels[i]);
    }

    TH1D* h1Rebinned2 = dynamic_cast<TH1D*>(h1->Rebin(4, hist_name));
    h1Rebinned2->SetLineColor(color);
    h1Rebinned2->GetXaxis()->SetTitle("Canali");
    h1Rebinned2->GetYaxis()->SetTitle("Conteggi");
    return h1Rebinned2;
}

double computeFisherInformation(TH1D* bins, const std::vector<double>& xvals, double A, double mu, double sigma) {
    double fisherInfo = 0.0;
    int numBins = bins->GetNbinsX();
    for (int i = 1; i <= numBins; ++i) {
        double pi = bins->GetBinContent(i);
        if (pi > 0) {
            double x = xvals[i - 1]; // xvals should correspond to bin centers
            double dpi_dmu = (A / (sigma * sigma)) * (x - mu) * std::exp(-std::pow(x - mu, 2) / (2 * sigma * sigma));
            fisherInfo += (dpi_dmu * dpi_dmu) / pi;
        }
    }
    return fisherInfo;
}

bool isFisherLossNegligible(double fisherBefore, double fisherAfter) {
    double relativeLoss = (fisherBefore - fisherAfter) / fisherBefore;
    double varianceRatio = (1.0 / fisherAfter) / (1.0 / fisherBefore);
    double infoRatio = fisherAfter / fisherBefore;

    std::cout << "Relative Fisher Loss: " << relativeLoss * 100 << "%\n";
    std::cout << "Variance Increase Ratio: " << varianceRatio << "\n";
    std::cout << "Information Ratio: " << infoRatio << "\n";

    return relativeLoss < 0.05; // 5% is a threshold for negligible loss
}

double calculateKLDivergence(TH1D* hOriginal, TH1D* hRebinned) {
    double klDivergence = 0.0;
    int nBinsOriginal = hOriginal->GetNbinsX();
    int nBinsRebinned = hRebinned->GetNbinsX();

    if (nBinsOriginal != nBinsRebinned) {
        std::cerr << "Error: Histograms must have the same number of bins to calculate KL divergence." << std::endl;
        return klDivergence;
    }

    for (int i = 1; i <= nBinsOriginal; ++i) {
        double p = hOriginal->GetBinContent(i);
        double q = hRebinned->GetBinContent(i);

        if (p > 0 && q > 0) {
            klDivergence += p * std::log(p / q);
        }
    }

    return klDivergence;
}

int main() {
    std::string filename = "Dati/Acquisizione_notte_1203_47cm_histo.dat";

    // Vectors to store the pulse amplitudes recorded in the channels
    std::vector<double> data_Co;
    readData(filename, data_Co);

    // Initialize x coordinates vector
    std::vector<double> xvals(data_Co.size());
    for (size_t i = 0; i < data_Co.size(); ++i) {
        xvals[i] = static_cast<double>(i);
    }

    TH1D* hCo_nonrebin = histogram_nonrebin(data_Co, "Spettro ^{60}Co. Angolo 22#circ. Distanza 47 cm", kBlue + 2);

    TF1* fit_nonrebin = fit_pol4_nonrebin(hCo_nonrebin);

    double A1_nonrebin = fit_nonrebin->GetParameter(5);
    double mu1_nonrebin = fit_nonrebin->GetParameter(6);
    double sigma1_nonrebin = fit_nonrebin->GetParameter(7);
    double A2_nonrebin = fit_nonrebin->GetParameter(8);
    double mu2_nonrebin = fit_nonrebin->GetParameter(9);
    double sigma2_nonrebin = fit_nonrebin->GetParameter(10);


    TH1D* hCo = histogram(data_Co, "hCo", "Spettro ^{60}Co. Angolo 22#circ. Distanza 47 cm", kBlue + 2);

    TF1* fitFunctionpol4 = fit_pol4(hCo);

    double A1 = fitFunctionpol4->GetParameter(5);
    double mu1 = fitFunctionpol4->GetParameter(6);
    double sigma1 = fitFunctionpol4->GetParameter(7);
    double A2 = fitFunctionpol4->GetParameter(8);
    double mu2 = fitFunctionpol4->GetParameter(9);
    double sigma2 = fitFunctionpol4->GetParameter(10);

    // Compute Fisher Information
    double fisherBefore_mu1 = computeFisherInformation(hCo_nonrebin, xvals, A1_nonrebin, mu1_nonrebin, sigma1_nonrebin);
    double fisherBefore_mu2 = computeFisherInformation(hCo_nonrebin, xvals, A2_nonrebin, mu2_nonrebin, sigma2_nonrebin);

    // Initialize rebinnedXvals
    std::vector<double> rebinnedXvals;
    for (size_t i = 0; i < data_Co.size(); i += 4) {
        rebinnedXvals.push_back(xvals[i]); // Pick representative x value
    }

    // Compute Fisher Information after re-binning
    double fisherAfter_mu1 = computeFisherInformation(hCo, rebinnedXvals, A1_nonrebin, mu1_nonrebin, sigma1_nonrebin);
    double fisherAfter_mu2 = computeFisherInformation(hCo, rebinnedXvals, A2_nonrebin, mu2_nonrebin, sigma2_nonrebin);

    std::cout << "Fisher Information for mu1 (before binning): " << fisherBefore_mu1 << std::endl;
    std::cout << "Fisher Information for mu1 (after binning):  " << fisherAfter_mu1 << std::endl;
    std::cout << "Loss for mu1: " << (fisherBefore_mu1 - fisherAfter_mu1) << std::endl;
    bool relativeLossmu1 = isFisherLossNegligible(fisherBefore_mu1, fisherAfter_mu1);
    std::cout << (relativeLossmu1 ? "Negligible loss for mu1" : "Significant loss for mu1") << std::endl;

    std::cout << std::endl;
    std::cout << "Fisher Information for mu2 (before binning): " << fisherBefore_mu2 << std::endl;
    std::cout << "Fisher Information for mu2 (after binning):  " << fisherAfter_mu2 << std::endl;
    std::cout << "Loss for mu2: " << (fisherBefore_mu2 - fisherAfter_mu2) << std::endl;
    bool relativeLossmu2 = isFisherLossNegligible(fisherBefore_mu2, fisherAfter_mu2);
    std::cout << (relativeLossmu2 ? "Negligible loss for mu2" : "Significant loss for mu2") << std::endl;

    return 0;
}