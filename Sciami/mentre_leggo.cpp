#include <iostream>
#include <vector>
#include <algorithm>
#include <TH1F.h>
#include <TCanvas.h>
#include <TApplication.h>  // Necessario per far funzionare ROOT senza command line
#include <cmath> //per std::abs

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <utility>

// Funzione che filtra i tempi di arrivo per i conteggi accidentali per le distribuzioni dei tempi
std::vector<double> calcolaDifferenzeTempi(const std::vector<double>& tempiArrivo) {
    const double accidental_rate = 0.368; // Hz
    const double accidental_time = 1.0 / accidental_rate; // Tempo caratteristico delle accidentali in secondi

    std::vector<double> eventiRimasti;
    
    // Variabile per tenere traccia dell'ultimo evento aggiunto
    double ultimoEvento = 0;

    for (double tempo : tempiArrivo) {
        if (tempo - ultimoEvento < accidental_time) {
            eventiRimasti.push_back(tempo);
        }else{
            ultimoEvento = tempo; // Aggiorna l'ultimo evento
        }
    }
    return eventiRimasti;
}

// Funzione per leggere e formattare i dati dal file da De0Nano
void readData(const std::string& filename, std::vector<int>& decimalNumbers, std::vector<double>& counts) {
    // Apri il file
    std::ifstream file(filename);

    // Verifica che il file sia stato aperto correttamente
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file " << filename << std::endl;
        exit(1); // Uscita dal programma in caso di errore
    }

    int number;
    double tempo;

    // Leggi i dati dal file
    while (file >> number >> tempo) {  //risoluzione temporale di 5 ns
        decimalNumbers.push_back(number);
        counts.push_back(tempo);
    }

    // Chiudi il file
    file.close();
}


void reset_clock( std::vector<int>& decimalNumbers, std::vector<double>& counts, std::vector<int>& Number, std::vector <double>& tempi_sistemati){
    int numero_reset=0;
    int inizio_file=0;
    int it=0;

    //trovo il primo reset
    while (true){
        if (decimalNumbers[it]==static_cast<int>(524288)){
            inizio_file=it;
            break;
        }
        ++it;
    }

    int i=0;
    for (i=inizio_file+1; i<decimalNumbers.size(); ++i){
        if (decimalNumbers[i]==static_cast<int>(524288)){
            ++numero_reset; //clock resettato
            ++i; //salti questo elemento
        }
        else{
            Number.push_back(decimalNumbers[i]);
            tempi_sistemati.push_back(counts[i]*5e-9+numero_reset*5.36870912); //converto i counts in secondi
        }
    }
}

// Funzione per determinare i canali attivati dato un numero decimale 
std::vector<int> getActiveChannels(int decimalNumber) {//reset counter del clock a 20^30 ns-> 5 s
    std::vector<int> activeChannels;
    std::bitset<7> bits(decimalNumber); // Assumiamo un sistema con 7 canali
    
    for (int i = 0; i < 7; ++i) {
        if (bits[i]) {
            activeChannels.push_back(i);
        }
    }
    return activeChannels;
}

std::vector<std::pair<double, double>> findCoincidences(const std::vector<double>& t1, const std::vector<double>& t2, 
    double minTime, double maxTime) {
    std::vector<std::pair<double, double>> coincidences;
    for (double time1 : t1) {
        for (double time2 : t2) {
            double delta = std::abs(time1 - time2);
            if (delta >= minTime && delta <= maxTime) {
                coincidences.emplace_back(time1, time2);
            }
        }
    }
    return coincidences;
}

void histogram(double interval, int num_intervals, const std::vector<std::pair<double, double>>& coincidences, 
    const char* hist_name, const char* title, Color_t color, int correction = 0){
    std::vector<int> counts(num_intervals, 0);

    // Scorri i dati e conta gli eventi per ciascun intervallo
    for (const auto& c : coincidences) {
        int bin = static_cast<int>(c.first / interval);
        if (bin < num_intervals) {
            counts[bin]++;
        }
    }

    double accidental_rate = 0.368; // Hz
    double expected_accidentals = accidental_rate * interval;

    // Calcolo delle coincidenze accidentali
    if (correction > 0) {
        for (int i = 0; i < num_intervals; ++i) {
            if (counts[i] > expected_accidentals) {
                counts[i] -= expected_accidentals;
            } else {
                counts[i] = 0; // Imposta a zero se il conteggio è minore o uguale
            }
        }
    }

    // Crea l'istogramma delle occorrenze dei conteggi
    int max_count = *std::max_element(counts.begin(), counts.end());
    TH1F* hist = new TH1F(hist_name, title,max_count + 1, 0, max_count + 1);
    for (int count : counts) {
        hist->Fill(count);
    }

    // Calcola gli errori come la radice quadrata dei conteggi
    for (int i = 1; i <= hist->GetNbinsX(); ++i) {
        hist->SetBinError(i, std::sqrt(static_cast<double>(hist->GetBinContent(i))));
    }
    hist->SetXTitle("Conteggi in intervallo");
    hist->SetYTitle("Occorrenze");

    // Crea il canvas e disegna l'istogramma con il fit
    TCanvas* canvaspoiss = new TCanvas("canvaspoiss", "Fit Poissoniano dei Conteggi", 800, 600);
    canvaspoiss->cd(1);
    hist->Draw();

    // Mostra il canvas
    canvaspoiss->Update();
}

// Funzione per creare l'istogramma delle differenze dei tempi a binning fissato e fare il fit esponenziale
void histogram_exponential(std::vector<std::pair<double, double>>& coincidences, const char* hist_name, const char* title, Color_t color, int correction = 0) {
    std::vector<double> first_times;
    for (const auto& coincidence : coincidences) {
        first_times.push_back(coincidence.first);
    }

    std::vector<double> new_times;
    if (correction > 0){
        new_times = calcolaDifferenzeTempi(first_times);
    }
    else{
        new_times = first_times;
    }

    std::vector<double> time_differences;
    for (size_t i = 1; i < new_times.size(); ++i) {
        time_differences.push_back(new_times[i] - new_times[i - 1]);
    }

    double max_time_diff = *std::max_element(time_differences.begin(), time_differences.end());
    TH1F* h1 = new TH1F(hist_name, title, 100, 0, max_time_diff);

    for (double t_diff : time_differences) {
        h1->Fill(t_diff);
    }

    h1->SetXTitle("Differenza di tempo (s)");
    h1->SetYTitle("Conteggi");
    h1->SetLineColor(color);

    TCanvas* canvasexp = new TCanvas("canvasexp",  "Istogramma delle differenze tra i tempi (2 telescopi)", 800, 600);
    canvasexp->SetLogy();
    canvasexp->cd(1);
    h1->Draw();
    canvasexp->Update();
}

int main() {
    std::string filename = "Dati/FIFOread_20241213-091357_7day.txt";
    std::vector<int> decimalNumbers;
    std::vector<double> counts;
    std::vector<int> Number;
    std::vector<double> tempi_sistemati;
    
    readData(filename, decimalNumbers, counts);
    reset_clock(decimalNumbers, counts, Number, tempi_sistemati);
    
    std::vector<double> channel1Times;
    std::vector<double> channel6Times;
    
    for (size_t i = 0; i < Number.size(); ++i) {
        std::vector<int> activeChannels = getActiveChannels(Number[i]);
        for (int channel : activeChannels) {
            if (channel == 1) {
                channel1Times.push_back(tempi_sistemati[i]);
            } else if (channel == 6) {
                channel6Times.push_back(tempi_sistemati[i]);
            }
        }
    }
    
    double minTime = 0.0;
    double maxTime = 230e-9;
    std::vector<std::pair<double, double>> coincidences = findCoincidences(channel1Times, channel6Times, minTime, maxTime);
    
    for (const auto& coincidence : coincidences) {
        std::cout << "Coincidenza trovata: " << coincidence.first << " s e " << coincidence.second << " s" << std::endl;
    }
    
    return 0;
}