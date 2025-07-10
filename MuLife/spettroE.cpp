#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
//Calcola la differenza tra il primo punto crescente tra gli 8 gi ogni trigger e il precedente
//Applica la correzione di discesa della curva per il punto precedente con exp
int spettroE() {
    std::ifstream file("AnalogDivisi_elettrone_unito.txt");
    if (!file) {
        std::cerr << "Errore nell'apertura del file!\n";
        return 1;
    }

    std::vector<double> differenze;
    std::vector<double> Delta1;
    std::vector<double> Delta2;
    std::vector<double> Delta;
    std::vector<int> posizioni;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<double> gruppo;
        double val;

        while (iss >> val) {
            gruppo.push_back(val);
        }

        if (!gruppo.empty()) {
            for (size_t i = 1; i < gruppo.size(); i++) {
                if (gruppo[i] > gruppo[i - 1]|| gruppo[i+1]>gruppo[i-1]) { // Trova il primo valore > precedente con anche il successivo >
                    differenze.push_back(gruppo[i] - gruppo[i - 1]);
                    Delta1.push_back(differenze.back()+gruppo[i]*(2.2/17.4));
                    Delta2.push_back(differenze.back()+gruppo[i-1]*(2.2/17.4));
                    Delta.push_back((Delta1.back()+Delta2.back())/2);
                    posizioni.push_back(i); // Salva la posizione (indice i)
                    break; // Esce appena trova il primo valore maggiore
                }
            }
        }
    }

    file.close();


    // Creazione dell'istogramma con ROOT
    TCanvas *c1 = new TCanvas("c1", "Istogramma delle differenze dei massimi", 800, 600);
    TH1F *hist = new TH1F("hist", "Spettro energia elettrone ;Differenza (MeV);Counts ", 100, 0, 80); // Adatta il range se necessario

    for (double diff : Delta) {
        hist->Fill(diff*0.035);
    }
   hist->SetLineWidth(3); // Imposta la larghezza della linea a 3

    hist->Draw();
     
    double xmin = 61; // Limite inferiore del range
    double xmax = 80; // Limite superiore del range

    int bin_min = hist->FindBin(xmin); // Trova il bin corrispondente a xmin
    int bin_max = hist->FindBin(xmax); // Trova il bin corrispondente a xmax

    double eventi = hist->Integral(bin_min, bin_max); // Somma i conteggi nei bin nel range
  
    double xmin1 = 10; // Limite inferiore del range
    double xmax1 = 20; // Limite superiore del range

    int bin_min1 = hist->FindBin(xmin1); // Trova il bin corrispondente a xmin
    int bin_max1 = hist->FindBin(xmax1); // Trova il bin corrispondente a xmax

    double eventi1 = hist->Integral(bin_min1, bin_max1); // Somma i conteggi nei bin nel range

    std::cout << "Numero di eventi nel range [" << xmin << ", " << xmax << "] : " << eventi << std::endl;

      int nBins = hist->GetNbinsX(); // Ottieni il numero di bin



   TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
   legend->SetTextSize(0.03);
  // legend->AddEntry((TObject*)0, Form( "Media tra i #Delta"),"");
   //legend->AddEntry((TObject*)0, Form( "#Delta1= #DeltaV+V_{1}(2.2#mus/#tau)"), "");
   //legend->AddEntry( (TObject*)0, Form("Eventi oltre 61 MeV = %.1f", eventi),"");
   legend->AddEntry( (TObject*)0, Form("#tau = 17.4 #mus"),"");
   legend->AddEntry((TObject*)0, Form("Numero di bin : %d  ", nBins),"");
   legend->AddEntry( (TObject*)0, Form("f = 0.035 MeV/mV"),"");
   legend->AddEntry( (TObject*)0, Form("Dati raccolti in circa 270h  dal 14/05 al 28/05 "),"");
   legend->AddEntry((TObject*)0, Form("Istogramma eseguito il 29/05/2025"),"");
   legend->AddEntry( (TObject*)0, Form("Eventi oltre 61 MeV = %.1f", eventi),"");
   legend->Draw(); 

    c1->Update();
    c1->SaveAs("SpettroE_unito.pdf");   // Salva in formato PDF


      // Salvataggio delle posizioni in un file
    std::ofstream outputFile("posizioniCrescenti_unione_v1.txt");
    if (!outputFile) {
        std::cerr << "Errore nell'apertura del file di output!\n";
        return 1;
    }

    for (int pos : posizioni) {
        outputFile << pos << "\n"; // Scrive ogni posizione in una nuova riga
    }

    outputFile.close();
    std::cout << "Posizioni salvate in posizioniCrescenti_unione_v1.txt\n";

    return 0;
}
