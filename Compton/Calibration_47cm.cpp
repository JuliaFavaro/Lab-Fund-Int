#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TStyle.h>
#include <TMath.h>
#include <cmath>

// Define a structure to hold the evaluated energy and errors
struct EnergyResult {
    double energy;
    double stat_error;
    double syst_error;
};

// Function to read mu values from a file
void readMuValues(const std::string& filename, std::vector<double>& mu_values, std::vector<double>& mu_errors, std::vector<double>& mu_values2, std::vector<double>& mu_errors2) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    double mu, mu_err, mu2, mu_err2;
    while (file >> mu >> mu_err >> mu2 >> mu_err2) {
        mu_values.push_back(mu);
        mu_errors.push_back(mu_err);
        mu_values2.push_back(mu2);
        mu_errors2.push_back(mu_err2);
    }
    file.close();
}

// Function to calculate mean values and errors
void calculateMeanValues(const std::vector<double>& mu1, const std::vector<double>& mu2, const std::vector<double>& err1, const std::vector<double>& err2, std::vector<double>& mean_mu, std::vector<double>& stat_err, std::vector<double>& syst_err) {
    size_t size = mu1.size();
    if (mu2.size() < size) {
        size = mu2.size();
    }

    for (size_t i = 0; i < size; ++i) {
        double mean = (mu1[i] + mu2[i]) / 2.0;
        double variance = (pow(mu1[i] - mean, 2) + pow(mu2[i] - mean, 2)) / 2.0;
        double syst_error = sqrt(variance);
        double stat_error = sqrt((pow(err1[i], 2) + pow(err2[i], 2)) / 2.0);
        mean_mu.push_back(mean);
        stat_err.push_back(stat_error);
        syst_err.push_back(syst_error);
    }
}

// Function to perform linear fit
TF1* performLinearFit(const std::vector<double>& mu, const std::vector<double>& stat_err, const std::vector<double>& syst_err, const std::vector<double>& energy) {
    TCanvas* cFit = new TCanvas("cFit", "Mean Calibration Function");
    TGraphErrors* mg_stat_err = new TGraphErrors(mu.size());
    TGraphErrors* mg_syst_err = new TGraphErrors(mu.size());
    mg_stat_err->SetMarkerStyle(21);
    mg_syst_err->SetMarkerStyle(21);

    for (size_t i = 0; i < mu.size(); ++i) {
        mg_stat_err->SetPoint(i, mu[i], energy[i] / 1e3);
        mg_stat_err->SetPointError(i, stat_err[i], 0.);
        mg_syst_err->SetPoint(i, mu[i], energy[i] / 1e3);
        mg_syst_err->SetPointError(i, syst_err[i], 0.);
    }

    mg_stat_err->SetMarkerStyle(8);
    mg_stat_err->SetMarkerSize(1);
    mg_stat_err->SetMarkerColor(kAzure - 9);
    mg_stat_err->SetLineColor(kAzure - 9);
    mg_stat_err->Draw("AP");

    mg_syst_err->SetMarkerStyle(8);
    mg_syst_err->SetMarkerSize(1);
    mg_syst_err->SetMarkerColor(kRed - 9);
    mg_syst_err->SetLineColor(kRed - 9);
    mg_syst_err->Draw("P same");

    double min_mu = *std::min_element(mu.begin(), mu.end());
    double max_mu = *std::max_element(mu.begin(), mu.end());

    TF1* fit = new TF1("fit", "pol1", min_mu, max_mu);
    fit->SetParameters(0, 1 / 3);
    mg_stat_err->Fit(fit, "WQN");
    mg_stat_err->Fit(fit);

    fit->SetLineColor(kRed);
    fit->SetLineStyle(1);
    fit->Draw("same");

    mg_stat_err->SetTitle("Mean Calibration Function");
    mg_stat_err->GetXaxis()->SetTitle("Mean Peak Channels");
    mg_stat_err->GetYaxis()->SetTitle("Energies [keV]");

    cFit->Update();

    return fit;
}

// Function to evaluate energies and propagate uncertainties
std::vector<EnergyResult> evaluateEnergies(const TF1* fit, const std::vector<double>& energies_final, const std::vector<double>& energies_stat, const std::string& label) {
    std::vector<EnergyResult> results;

    for (size_t i = 0; i < energies_final.size(); ++i) {
        double energy = fit->Eval(energies_final[i]);
        double a = fit->GetParameter(0);
        double b = fit->GetParameter(1);
        double err_a = fit->GetParError(0);
        double err_b = fit->GetParError(1);
        
        double stat_error = energies_stat[i];
        
        // Propagate the uncertainties
        double error = TMath::Sqrt(TMath::Power(err_a, 2) + TMath::Power(err_b * energies_final[i], 2) + TMath::Power(stat_error * b, 2));

        std::cout << "Energy (final - " << label << "): " << energies_final[i] << " -> Evaluated Energy: " << energy << " +/- " << error << " keV" << std::endl;
        
        results.push_back({energy, error, 0.0}); // Placeholder for syst_error, will be updated later
    }

    return results;
}

// Function to calculate Compton scattering and propagate errors
double compton_scattering(double E, double E1, double E1_stat, double E1_sist, double& stat_error) {
    double theta = 22.0 * M_PI / 180.0; // Convert theta from degrees to radians
    double err_theta = 1.3 * M_PI / 180.0; // Convert err_theta from degrees to radians
    double me = E * E1 / (E - E1) * (1 - cos(theta));
    
    // Propagate the statistical error of E1_stat
    double dE1 = pow(E, 2)/pow(E-E1, 2);
    stat_error = dE1*E1_stat;
    return me;
}

// Function to calculate systematic error on me
double calculateSystematicError(double me_nominal, double me_minus_sist, double me_plus_sist) {
    double mean_me = (me_nominal + me_minus_sist + me_plus_sist) / 3.0;
    double variance = (pow(me_nominal - mean_me, 2) + pow(me_minus_sist - mean_me, 2) + pow(me_plus_sist - mean_me, 2)) / 2.0;
    return sqrt(variance);
}

int main() {
    std::string filename = "calibration_47.txt"; // Replace with the actual filename

    std::vector<double> mu1, mu2, err1, err2;    
    readMuValues(filename, mu1, err1, mu2, err2);

    std::vector<double> mean_mu, stat_err, syst_err;
    calculateMeanValues(mu1, mu2, err1, err2, mean_mu, stat_err, syst_err);
    mean_mu.push_back(2559.79);
    stat_err.push_back(0.44943);
    syst_err.push_back(0);
    mean_mu.push_back(228.56);
    stat_err.push_back(0.0623565);
    syst_err.push_back(0);

    std::vector<double> energy = {1.17e6, 1.33e6, 551e3, 1.274e6, 662e3, 60e3}; // Energy values for Cs, Co1, Co2, Na1, Na2, Am
    TF1* fit = performLinearFit(mean_mu, stat_err, syst_err, energy);

    std::vector<double> energies_final = {3675, 4101}; // Channels
    std::vector<double> energies_stat = {6, 4};
    std::vector<double> energies_sist = {35, 16};

    std::vector<EnergyResult> eval_nominal = evaluateEnergies(fit, energies_final, energies_stat, "nominal");

    std::vector<double> energies_final_minus_sist(energies_final.size());
    std::vector<double> energies_final_plus_sist(energies_final.size());
    for (size_t i = 0; i < energies_final.size(); ++i) {
        energies_final_minus_sist[i] = energies_final[i] - energies_sist[i];
        energies_final_plus_sist[i] = energies_final[i] + energies_sist[i];
    }

    std::vector<EnergyResult> eval_minus_sist = evaluateEnergies(fit, energies_final_minus_sist, energies_stat, "minus systematic");
    std::vector<EnergyResult> eval_plus_sist = evaluateEnergies(fit, energies_final_plus_sist, energies_stat, "plus systematic");

    // Calculate the systematic error as the standard deviation of the three evaluations
    for (size_t i = 0; i < eval_nominal.size(); ++i) {
        double mean_energy = (eval_nominal[i].energy + eval_minus_sist[i].energy + eval_plus_sist[i].energy) / 3.0;
        double variance = (pow(eval_nominal[i].energy - mean_energy, 2) + pow(eval_minus_sist[i].energy - mean_energy, 2) + pow(eval_plus_sist[i].energy - mean_energy, 2)) / 2.0;
        double syst_error = sqrt(variance);

        eval_nominal[i].syst_error = syst_error;

        std::cout << "Energy (final - nominal): " << energies_final[i] << " -> Evaluated Energy: " << eval_nominal[i].energy << " +/- " << eval_nominal[i].stat_error << " (stat error) +/- " << eval_nominal[i].syst_error << " (syst error) keV" << std::endl;
    }

    // Calculate Compton scattering
    std::vector<double> E ={1.17e3, 1.33e3}; // Placeholder for incoming photon energy
    for (size_t i = 0; i < eval_nominal.size(); ++i) {
        double stat_error;
        double me_nominal = compton_scattering(E[i], eval_nominal[i].energy, eval_nominal[i].stat_error, eval_nominal[i].syst_error, stat_error);
        double me_minus_sist = compton_scattering(E[i], eval_minus_sist[i].energy, eval_minus_sist[i].stat_error, eval_minus_sist[i].syst_error, stat_error);
        double me_plus_sist = compton_scattering(E[i], eval_plus_sist[i].energy, eval_plus_sist[i].stat_error, eval_plus_sist[i].syst_error, stat_error);

        double syst_error_me = calculateSystematicError(me_nominal, me_minus_sist, me_plus_sist);

        std::cout << "Compton scattering result (me): " << me_nominal << " +/- " << stat_error << " (stat error) +/- " << syst_error_me << " (syst error)" << std::endl;
    }

    return 0;
}