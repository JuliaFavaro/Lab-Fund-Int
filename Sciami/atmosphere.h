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

// Funzione per rimuovere duplicati 
const double EPSILON = 1e-9; // floating-point comparison

bool areAlmostEqual(double a, double b) {
    return std::fabs(a - b) < EPSILON;
}

void removeDuplicates(const std::vector<BinnedData>& atmDataBins, std::vector<double>& uniqueTemperatures) {
    for (const auto& bin : atmDataBins) {
        double temperature = bin.avgTemperature;

        // Check if the temperature is already present within the tolerance
        bool found = false;
        for (const auto& uniqueTemp : uniqueTemperatures) {
            if (areAlmostEqual(uniqueTemp, temperature)) {
                found = true;
                break;
            }
        }

        // If not found, add the temperature
        if (!found) {
            uniqueTemperatures.push_back(temperature);
        }
        
    }
}


// Funzione per tracciare il grafico di correlazione (temperatura)
void plotCorrelationTemp(const std::vector<BinnedData>& atmDataBins, double& interval, int& num_intervals, const std::vector<double>& t1,
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

    // Verifica che le dimensioni dei vettori siano corrette
    //std::cout<<"Atmo "<<atmDataBins.size()<<" rates "<<rates_1.size()<<std::endl;
    std::vector<double> uniqueTemperatures;
    // Creazione del grafico con errori
    TGraphErrors* graph3 = new TGraphErrors(rates_3.size());
    removeDuplicates(atmDataBins, uniqueTemperatures);
    std::cout<<"Atmo "<<uniqueTemperatures.size()<<" rates "<<rates_1.size()<<std::endl;
    for (size_t i = 0; i < uniqueTemperatures.size(); ++i) {
        if (rates_3[i]!=0){
            graph3->SetPoint(i, uniqueTemperatures[i], rates_3[i]); // Imposta il punto (temperatura, rate)
            graph3->SetPointError(i, 0, errors_3[i]); // Imposta gli errori
        }
    }

    TCanvas* canvastemp = new TCanvas("canvastemp", "Correlazione tra Temperatura Atmosferica e Rates", 800, 600);
	canvastemp->SetGrid(); 

    graph3->SetTitle("Setup04 e Temperatura;Temperatura (#circC);Rate (Hz)"); 
    graph3->SetMarkerStyle(8); 
    graph3->SetMarkerSize(1);
    graph3->SetMarkerColor(kGreen + 2);
    graph3->SetLineColor(kGreen + 2); 
    graph3->Draw("AP");

    canvastemp->Update();
}

// Funzione per tracciare il grafico di correlazione (pressione)
void plotCorrelationPress(const std::vector<BinnedData>& atmDataBins, double& interval, int& num_intervals, const std::vector<double>& t1,
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

    TGraphErrors* graph3 = new TGraphErrors(num_intervals);

    for (size_t i = 0; i < num_intervals; ++i) {
        graph3->SetPoint(i, atmDataBins[i].avgPressure, rates_3[i]);
        graph3->SetPointError(i, 0, errors_3[i]);
    }

    TCanvas* canvaspress = new TCanvas("canvaspress", "Correlazione tra Pressione Atmosferica e Rates", 800, 600);
	canvaspress->SetGrid(); 

    graph3->SetTitle("Setup04 e Pressione;Pressione (mbar);Rate (Hz)"); 
    graph3->SetMarkerStyle(8); 
    graph3->SetMarkerSize(1);
    graph3->SetMarkerColor(kGreen + 2);
    graph3->SetLineColor(kGreen + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph3->Draw("AP");

    // Esegui il fit lineare
    TF1* fitFunc = new TF1("fitFunc", "pol1", 1020, 1040); // Sostituisci con l'intervallo appropriato
    
    // Impostazione dei parametri iniziali
    fitFunc->SetParameter(0, 11); // Intercetta iniziale (a)
    fitFunc->SetParameter(1, 0); // Coefficiente angolare iniziale (b)
    graph3->Fit(fitFunc);

    // Stampa i risultati del fit con incertezze
    double slope = fitFunc->GetParameter(1);   // Coefficiente angolare
    double intercept = fitFunc->GetParameter(0); // Intercetta
    double slopeError = fitFunc->GetParError(1);   // Incertezza sul coefficiente angolare
    double interceptError = fitFunc->GetParError(0); // Incertezza sull'intercetta

    // Imposta la precisione a due cifre significative
    std::cout << std::fixed << std::setprecision(2);
    
    // Aggiungi media campionaria, deviazione standard e incertezza al grafico
    TPaveText* pave2 = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
    pave2->SetFillColor(kWhite);
    pave2->SetBorderSize(1);
    pave2->SetTextAlign(14);
    pave2->AddText(Form("Coefficiente angolare: %.3f #pm %.3f Hz/#circC", slope, slopeError));
    pave2->AddText(Form("Intercetta = %.2f #pm %.2f Hz", intercept, interceptError));
    pave2->Draw();

    canvaspress->Update();
}

// Funzione per tracciare il grafico di correlazione (umiditàr)
void plotCorrelationHum(const std::vector<BinnedData>& atmDataBins, double& interval, int& num_intervals, const std::vector<double>& t1,
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

    TGraphErrors* graph3 = new TGraphErrors(num_intervals);

    for (size_t i = 0; i < num_intervals; ++i) {
        graph3->SetPoint(i, atmDataBins[i].avgHumidity, rates_3[i]);
        graph3->SetPointError(i, 0, errors_3[i]);
    }

    TCanvas* canvashum = new TCanvas("canvashum", "Correlazione tra Umidità e Rates", 800, 600);
	canvashum->SetGrid(); 

    graph3->SetTitle("Setup04 e Umidita;Umidita (%);Rate (Hz)"); 
    graph3->SetMarkerStyle(8); 
    graph3->SetMarkerSize(1);
    graph3->SetMarkerColor(kGreen + 2);
    graph3->SetLineColor(kGreen + 2); //serve per avere lo stesso colore anche nelle barre di errore
    graph3->Draw("AP");

    // Esegui il fit lineare
    TF1* fitFunc = new TF1("fitFunc", "pol1", 0, 100); // Sostituisci con l'intervallo appropriato
    
    // Impostazione dei parametri iniziali
    fitFunc->SetParameter(0, 11); // Intercetta iniziale (a)
    fitFunc->SetParameter(1, 0); // Coefficiente angolare iniziale (b)
    graph3->Fit(fitFunc);

    // Stampa i risultati del fit con incertezze
    double slope = fitFunc->GetParameter(1);   // Coefficiente angolare
    double intercept = fitFunc->GetParameter(0); // Intercetta
    double slopeError = fitFunc->GetParError(1);   // Incertezza sul coefficiente angolare
    double interceptError = fitFunc->GetParError(0); // Incertezza sull'intercetta

    // Imposta la precisione a due cifre significative
    std::cout << std::fixed << std::setprecision(2);
    
    // Aggiungi media campionaria, deviazione standard e incertezza al grafico
    TPaveText* pave3 = new TPaveText(0.65, 0.85, 0.85, 0.95, "NDC");
    pave3->SetFillColor(kWhite);
    pave3->SetBorderSize(1);
    pave3->SetTextAlign(14);
    pave3->AddText(Form("Coefficiente angolare: %.3f #pm %.3f Hz/#circC", slope, slopeError));
    pave3->AddText(Form("Intercetta = %.2f #pm %.2f Hz", intercept, interceptError));
    pave3->Draw();

    canvashum->Update();
}


#endif 