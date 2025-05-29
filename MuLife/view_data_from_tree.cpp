#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <limits> // Per std::numeric_limits

void addTimestamp(TCanvas *canvas, const std::string &timestamp, const std::string &duration) {
    TLatex *latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.03);
    latex->DrawLatex(0.1, 0.86, timestamp.c_str()); // Posizionamento in alto a sinistra del grafico
    latex->DrawLatex(0.1, 0.81, duration.c_str()); // Posizionamento sotto il timestamp
    latex->DrawLatex(0.1, 0.76, "Bin: 100"); // Posizionamento sotto il timestamp
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
        return 1;
    }

    // Variabile per leggere il TTree
    double timeDifference;
    tree->SetBranchAddress("timeDifference", &timeDifference);

    // Determina il numero di entries
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Numero di entries nel TTree: " << nEntries << std::endl;

    // Calcola i limiti minimo e massimo
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        if (timeDifference < minValue) minValue = timeDifference;
        if (timeDifference > maxValue) maxValue = timeDifference;
    }

    std::cout << "Valore minimo di #Delta t: " << minValue << " ns" << std::endl;
    std::cout << "Valore massimo di #Delta t: " << maxValue << " ns" << std::endl;

    // Crea un istogramma con i limiti aggiornati
    TH1F* histogram = new TH1F("histogram", "#Delta t (Start-Stop)", 100, 0, 20*1000);

    // Riempi l'istogramma con i valori dal TTree
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        histogram->Fill(timeDifference);
    }

    // Disegna l'istogramma
    TCanvas* canvas = new TCanvas("canvas", "Istogramma Delta t", 1500, 1500);
    canvas->SetGrid();

    histogram->SetLineColor(kBlue);
    histogram->SetLineWidth(2);
    histogram->SetXTitle("#Delta t (ns)");
    histogram->SetYTitle("Conteggi");
    histogram->Draw("E");

    std::string timestamp = "06.05.2025 18:30";
    std::string duration = "Durata: 266.5 ore";

    double binWidth = histogram->GetBinWidth(1); 
    std::cout << "La larghezza di un bin è: " << binWidth << " ns" << std::endl; //non deve essere più piccola della risoluzione di 5 ns per ovvi motivi

    addTimestamp(canvas, timestamp, duration);

    // Fit esponenziale
    TF1* expFit = new TF1("expFit", "[0] * exp(-x / [1]) + [2]", 0, 20*1000);
    expFit->SetParameters(1000, 2200, 10); // Parametri iniziali: [0]=ampiezza, [1]=decadimento (2200 ns = 2.2 µs), [2]=costante
    expFit->SetParNames("Amplitude", "Decay Time (ns)", "Constant");

    histogram->Fit(expFit, "RL"); // Esegui il fit sul range specificato

    // Disegna il fit sul canvas
    expFit->SetLineColor(kRed);
    expFit->SetLineWidth(2);
    expFit->Draw("SAME");
    gStyle->SetOptFit(1111);

    // Salva il grafico su file immagine
    canvas->SaveAs("Acquisizione_Fit_0607.png");

    // Aggiorna il canvas
    canvas->Modified();
    canvas->Update();

    // Stampa i risultati del fit
    std::cout << "Risultati del fit:" << std::endl;
    std::cout << "Ampiezza: " << expFit->GetParameter(0) << std::endl;
    std::cout << "Tempo di decadimento (ns): " << expFit->GetParameter(1) << std::endl;
    std::cout << "Costante: " << expFit->GetParameter(2) << std::endl;

    // Chiudi il file ROOT
    inputFile->Close();

    return 0;
}