#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime> // Gestire operazioni tra tempi in data calendariale
#include <chrono> // Definire una data calendariale
#include <algorithm> // Per std::find

#include "flux_graphs_10.h"

// Struttura per rappresentare i dati atmosferici come una specie di dizionario
struct AtmData {
    std::string date;
    std::string time;
    double temperature;
    double humidity;
    double pressure;

    // Operatore di confronto per evitare duplicati
    bool operator==(const AtmData& other) const {
        return (date == other.date && time == other.time);
    }
};

//Purtroppo va ridefinito nel caso in cui abbia un binning
struct BinnedData {
    double startTime; // Inizio del bin
    double endTime;   // Fine del bin
    double avgTemperature; // Media della temperatura nel bin
    double avgHumidity; // Media dell'umidità nel bin
    double avgPressure; // Media della pressione nel bin
};

//con timestamp si intende una stringa di secondi, un'epoch, in modo tale che sia più facile operazione di  binnaggio
double convertToTimestamp(const std::string& date, const std::string& time) {
    std::tm tm = {};
    std::istringstream ss(date + " " + time);
    ss >> std::get_time(&tm, "%d/%m/%y %H:%M");

    if (ss.fail()) {
        std::cerr << "Errore nella conversione della data/ora: " << date << " " << time << std::endl;
        return -1; // Gestisci l'errore come preferisci
    }

    return mktime(&tm); // Converti a time_t
}

// Funzione per convertire timestamp in stringa di data/ora, per facilitare la lettura
std::string timestampToString(double timestamp) {
    time_t time = static_cast<time_t>(timestamp); // Converti da double a time_t
    std::tm* tm = std::localtime(&time);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", tm);
    return std::string(buffer);
}

//funzione che a partire da 2 file, crea una struct-database di tutte le condizioni atmosferiche nell'intervallo considerato
void read_atmData(std::vector<AtmData>& atmDataList, double& total_time) {
    std::ifstream file("Dati/2024-12-15_downld02.txt");
    if (!file.is_open()) {
        std::cerr << "Errore nell'aprire il file " << "2024-12-15_downld02.txt" << std::endl;
        return;
    }

    // Salta le righe dell'header
    std::string line;
    for (int i = 0; i < 3; ++i) {
        std::getline(file, line);
    }

    // Definisci l'intervallo temporale di interesse
    std::tm dataInizio = {};
    dataInizio.tm_year = 2024 - 1900; // Anno 2024
    dataInizio.tm_mon = 11; // Dicembre (0-based)
    dataInizio.tm_mday = 13; 
    dataInizio.tm_hour = 10; 
    dataInizio.tm_min = 14;

    time_t tempoInizio = mktime(&dataInizio);
    time_t tempoFine = tempoInizio + static_cast<time_t>(total_time);

    std::cout << "Tempo Inizio: " << std::ctime(&tempoInizio);
    std::cout << "Tempo Fine: " << std::ctime(&tempoFine);

    while (std::getline(file, line)) { // Leggo una riga per volta
        std::istringstream iss(line); // Flusso di parole nella riga
        AtmData atmData; // Crea un'istanza della struttura

        std::string campo;
        int indiceCampo = 0;

        while (iss >> campo) {
            switch (indiceCampo) { 
                case 0: // Date
                    atmData.date = campo;
                    break;
                case 1: // Time
                    atmData.time = campo;
                    break;
                case 2: // Temp Out
                    atmData.temperature = std::stod(campo); 
                    break;
                case 5: // Out Hum
                    atmData.humidity = std::stod(campo);
                    break;
                case 16: // Bar
                    atmData.pressure = std::stod(campo);
                    break;
            }
            indiceCampo++;
        }

        // Converti la data e l'ora in time_t
        std::tm dataCorrente = {};
        strptime((atmData.date + " " + atmData.time).c_str(), "%d/%m/%y %H:%M", &dataCorrente);
        time_t tempoCorrente = mktime(&dataCorrente);

        // Verifica se la data e l'ora rientrano nell'intervallo temporale
        if (tempoCorrente >= tempoInizio && tempoCorrente <= tempoFine) {
            // Controlla se la combinazione date+ora è già presente nel vettore
            if (std::find(atmDataList.begin(), atmDataList.end(), atmData) == atmDataList.end()) {
                atmDataList.push_back(atmData); // Aggiungi i dati al vettore
            } 
        }
    }
    file.close();

    // Variabili per il secondo file
    std::ifstream fileSecondo("Dati/2024-12-22_downld08.txt");
    if (!fileSecondo.is_open()) {
        std::cerr << "Errore nell'aprire il file " << "2024-12-22_downld08.txt" << std::endl;
        return;
    }

    // Salta le righe dell'header
    std::string line2;
    for (int i = 0; i < 3; ++i) {
        std::getline(fileSecondo, line2);
    }

    while (std::getline(fileSecondo, line2)) {
        std::istringstream iss(line2);
        AtmData atmData; // Crea un'istanza della struttura

        std::string campo;
        int indiceCampo = 0;

        while (iss >> campo) {
            switch (indiceCampo) {
                case 0: // Date
                    atmData.date = campo;
                    break;
                case 1: // Time
                    atmData.time = campo;
                    break;
                case 2: // Temp Out
                    atmData.temperature = std::stod(campo);
                    break;
                case 5: // Out Hum
                    atmData.humidity = std::stod(campo);
                    break;
                case 16: // Bar
                    atmData.pressure = std::stod(campo);
                    break;
            }
            indiceCampo++;
        }

        // Converti la data e l'ora in time_t
        std::tm dataCorrente = {};
        strptime((atmData.date + " " + atmData.time).c_str(), "%d/%m/%y %H:%M", &dataCorrente);
        time_t tempoCorrente = mktime(&dataCorrente);

        // Verifica se la data e l'ora rientrano nell'intervallo temporale e non sono duplicati
        if (tempoCorrente >= tempoInizio && tempoCorrente <= tempoFine) {
            if (std::find(atmDataList.begin(), atmDataList.end(), atmData) == atmDataList.end()) {
                atmDataList.push_back(atmData); // Aggiungi i dati al vettore
            }
        }
    }
    
    fileSecondo.close();
}

// Funzione che a partire dal database creato lo binna secondo un intervallo di tempo prefissato
std::vector<BinnedData> interpolateAndBin(const std::vector<AtmData>& atmDataList, double& interval) {
    std::vector<BinnedData> binnedResults;

    if (atmDataList.empty()) return binnedResults;

    // Calcola il primo timestamp dal primo elemento della lista
    double currentBinStart = convertToTimestamp(atmDataList[0].date, atmDataList[0].time);
    double currentBinEnd = currentBinStart + interval; // Imposta currentBinEnd

    while (true) {
        BinnedData currentBin;
        currentBin.startTime = currentBinStart;
        currentBin.endTime = currentBinEnd;

        std::vector<double> temperatures;
        std::vector<double> humidities;
        std::vector<double> pressures;

        // Raccogli i dati per il bin corrente
        for (const auto& data : atmDataList) {
            double timestamp = convertToTimestamp(data.date, data.time);
            if (timestamp >= currentBinStart && timestamp < currentBinEnd) {
                temperatures.push_back(data.temperature);
                humidities.push_back(data.humidity);
                pressures.push_back(data.pressure);
            }
        }

        // Calcola le medie se ci sono dati nel bin
        if (!temperatures.empty()) {
            currentBin.avgTemperature = std::accumulate(temperatures.begin(), temperatures.end(), 0.0) / temperatures.size();
            currentBin.avgHumidity = std::accumulate(humidities.begin(), humidities.end(), 0.0) / humidities.size();
            currentBin.avgPressure = std::accumulate(pressures.begin(), pressures.end(), 0.0) / pressures.size();
        } else {
            currentBin.avgTemperature = 0.0; // O gestisci diversamente
            currentBin.avgHumidity = 0.0;
            currentBin.avgPressure = 0.0;
        }

        binnedResults.push_back(currentBin);

        // Aggiorna l'intervallo per il prossimo bin
        currentBinStart += interval;
        currentBinEnd += interval;

        // Termina il ciclo se l'inizio del prossimo bin supera l'ultimo timestamp disponibile
        if (currentBinStart >= convertToTimestamp(atmDataList.back().date, atmDataList.back().time)) {
            break;
        }
    }

    return binnedResults;
}

// Funzione per stampare i dati estratti
void print_atmData(const std::vector<AtmData>& atmDataList) {
    for (const auto& data : atmDataList) {
        std::cout << "Data: " << data.date 
                  << ", Ora: " << data.time 
                  << ", Temp: " << data.temperature 
                  << ", Umidità: " << data.humidity 
                  << ", Pressione: " << data.pressure 
                  << std::endl;
    }
}

// Funzione per stampare i dati estratti binnati
void print_binnedData(const std::vector<BinnedData>& atmDataBins) {
    for (const auto& data : atmDataBins) {
        std::cout << "Inizio Bin: " << timestampToString(data.startTime)
                  << ", Fine Bin: " << timestampToString(data.endTime)
                  << ", Temp Media: " << data.avgTemperature 
                  << ", Umidità Media: " << data.avgHumidity 
                  << ", Pressione Media: " << data.avgPressure 
                  << std::endl;
    }
}

// Funzione per tracciare il grafico di correlazione
void plotCorrelation(const std::vector<BinnedData>& atmDataBins, double& interval, int& num_intervals, const std::vector<double>& t1,
                 const std::vector<double>& t2, const std::vector<double>& t3) {

    // Vettori per memorizzare i risultati
    std::vector<double> time_intervals(num_intervals);
    std::vector<double> rates_1(num_intervals, 0);
    std::vector<double> errors_1(num_intervals, 0);
    std::vector<double> rates_2(num_intervals, 0);
    std::vector<double> errors_2(num_intervals, 0);
    std::vector<double> rates_3(num_intervals, 0);
    std::vector<double> errors_3(num_intervals, 0);

    // Calcola i rates
    CalculateRates(interval, num_intervals, t1, t2, t3,
                   rates_1, errors_1, rates_2, errors_2,
                   rates_3, errors_3, time_intervals);

    TGraphErrors* graph1 = new TGraphErrors(atmDataBins.size());
    TGraphErrors* graph2 = new TGraphErrors(atmDataBins.size());
    TGraphErrors* graph3 = new TGraphErrors(atmDataBins.size());

    for (size_t i = 0; i < atmDataBins.size(); ++i) {
        graph1->SetPoint(i, atmDataBins[i].avgTemperature, rates_1[i]);
        graph1->SetPointError(i, 0, errors_1[i]);

        graph2->SetPoint(i, atmDataBins[i].avgTemperature, rates_2[i]);
        graph2->SetPointError(i, 0, errors_2[i]);
        
        graph3->SetPoint(i, atmDataBins[i].avgTemperature, rates_3[i]);
        graph3->SetPointError(i, 0, errors_3[i]);
    }

    TCanvas* canvas = new TCanvas("canvas", "Correlazione tra Temperatura Atmosferica e Rates", 800, 600);
	canvas->SetGrid(); 
    canvas->Divide(1, 3); //colonne, righe

    canvas->cd(1);
    graph1->SetTitle("Telescopio 06;Temperatura (°C);Rate (Hz)"); 
    graph1->Draw("AP"); // Disegna il primo grafico
    graph1->SetMarkerStyle(8); 
    graph1->SetMarkerSize(1);
    graph1->SetMarkerColor(kRed + 2);
    graph1->SetLineColor(kRed + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph1->Draw("AP");
    
    canvas->cd(2);
    graph2->SetTitle("Telescopio 08;Temperatura (°C);Rate (Hz)"); 
    graph2->SetMarkerStyle(8); 
    graph2->SetMarkerSize(1);
    graph2->SetMarkerColor(kBlue + 2);
    graph2->SetLineColor(kBlue + 2);
    graph2->Draw("AP");

    canvas->cd(3);
    graph3->SetTitle("Telescopio 04;Temperatura (°C);Rate (Hz)"); 
    graph3->SetMarkerStyle(8); 
    graph3->SetMarkerSize(1);
    graph3->SetMarkerColor(kGreen + 2);
    graph3->SetLineColor(kGreen + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph3->Draw("AP");

    canvas->Update();
}

#endif 
