int n_triple = 0;
int n_triple_vere = 0;
vector<double> tempi_veri;
vector<double> delta_t;
vector<double> time_123;
vector<int> ch_123;

struct DifferenzaTempo {
    double differenza;
    int canale_iniziale;
    int canale_finale;
};
struct TRIPLE {
    int canale1;
    int canale2;
    int canale3;
    double tempo1;
    double tempo2;
    double tempo3;
};


std::vector<TRIPLE> counter(const std::vector<std::vector<int>>& sequence, int j) {
    std::vector<TRIPLE> triple_vere;
    std::vector<int> list_for_one = {ch_123[j], ch_123[j + 1], ch_123[j + 2]};
    std::vector<double> tempi_trio = {time_123[j], time_123[j + 1], time_123[j + 2]};
    
    if (std::find(sequence.begin(), sequence.end(), list_for_one) != sequence.end()) {
        n_triple++;
        if (tempi_trio.back() - tempi_trio.front() < 480e-9) {
            n_triple_vere++;
            tempi_veri.push_back(tempi_trio.front());
            TRIPLE tripla = {ch_123[j], ch_123[j + 1], ch_123[j + 2], time_123[j], time_123[j+1], time_123[j+2]};
            triple_vere.push_back(tripla);
        }
    } 
    return triple_vere;
}



/*
std::vector<double> estraiTempi(const std::vector<TRIPLE>& triple_vere) {
    std::vector<double> tempi_arrivo;
    for (const auto& tripla : triple_vere) {
        tempi_arrivo.push_back(tripla.tempo1); 
    }
    for(int i=0;i<tempi_arrivo.size(); i++){
        std::cout<<"tempi_arrivo[i]= "<<tempi_arrivo[i]<<std::endl;
     }

    
    return tempi_arrivo;
} */

//calcola il rate delle triple
double calcolaRate(const std::vector<double>& tempi_veri) {
    if (tempi_veri.size() < 2) {
        // Se ci sono meno di due eventi, non si può calcolare una frequenza significativa
        std::cerr << "Errore: numero di eventi insufficiente." << std::endl;
        return 0.0;
    }


    // Calcola la durata totale del periodo di osservazione
    double durata_totale = tempi_veri.back() - tempi_veri.front();
    if (durata_totale <= 0.0) {
        // Evita divisioni per zero o valori negativi
        std::cerr << "Errore: durata totale non valida." << std::endl;
        return 0.0;
    }

    // Calcola il numero totale di eventi
    int numero_eventi = tempi_veri.size();
    std::cout<<"Numero di eventi di sciami estesi rivelati:"<< numero_eventi<<std::endl;

    // Calcola la frequenza (rate) degli eventi
    double rate = static_cast<double>(numero_eventi) / durata_totale;
    double errore_rate= sqrt(numero_eventi) / durata_totale;

    std::cout<<"Rate di triple calcolato:"<< rate <<"Errore: "<<errore_rate<<std::endl;
    std::cout<<"Tempo totale:  "<< durata_totale << std::endl;
    return rate;
}
// Funzione per calcolare i rates e gli errori
void CalculateRates(double interval, int num_intervals, const std::vector<double>& t1,
    std::vector<double>& rates_1, std::vector<double>& errors_1,
    std::vector<double>& time_intervals) {

for (int i = 0; i < num_intervals; ++i) {
double start_time = i * interval;
double end_time = start_time + interval;

// Calcola il tempo medio dell'intervallo per il grafico finale
time_intervals[i] = ((start_time + end_time) / 3600) / 2; // in ore

int count_1 = std::count_if(t1.begin(), t1.end(), [start_time, end_time](double t) {
return t > start_time && t < end_time;
});

// Calcola il rate come numero di eventi diviso per la larghezza dell'intervallo
rates_1[i] = count_1 / interval;
errors_1[i] = sqrt(count_1) / interval;
}
}



//Calcola le diff di tempi in base a quali canali provengono solo per le triple vere
std::vector<TRIPLE> all_triple_vere;
void calcolaDifferenzeTempi(const std::vector<TRIPLE>& triple_vere, std::vector<double>& differenze_28, std::vector<double>& differenze_864, std::vector<double>& differenze_264) {
    for (const auto& tripla : triple_vere) {
        if ((tripla.canale1 == 2 && tripla.canale2 == 8)||(tripla.canale2 == 2 && tripla.canale1 == 8)) {
            differenze_28.push_back(tripla.tempo2 - tripla.tempo1);
        }
        if ((tripla.canale2 == 2 && tripla.canale3 == 8)||(tripla.canale3 == 2 && tripla.canale2 == 8)) {
            differenze_28.push_back(tripla.tempo3 - tripla.tempo2);
        }
        if ((tripla.canale1 == 2 && tripla.canale3 == 8)||(tripla.canale3 == 2 && tripla.canale1 == 8)) {
            differenze_28.push_back(tripla.tempo3 - tripla.tempo1);
        } //Tra 2 e 8 in tutte le combinazioni possibili

       
        if ((tripla.canale1 == 8 && tripla.canale3 == 64)||(tripla.canale3 == 8 && tripla.canale1 == 64)) {
            differenze_864.push_back(tripla.tempo3 - tripla.tempo1);
        }
        if ((tripla.canale1 == 8 && tripla.canale2 == 64)||(tripla.canale2 == 8 && tripla.canale1 == 64)) {
            differenze_864.push_back(tripla.tempo2 - tripla.tempo1);
        }
        if ((tripla.canale2 == 8 && tripla.canale3 == 64)||(tripla.canale3 == 8 && tripla.canale2 == 64)) {
            differenze_864.push_back(tripla.tempo3 - tripla.tempo2);
        } //Tra 8 e 64 in tutte le combinazioni possibili

        if ((tripla.canale1 == 2 && tripla.canale3 == 64)||(tripla.canale3 == 2 && tripla.canale1 == 64)) {
            differenze_264.push_back(tripla.tempo3 - tripla.tempo1);
        }
        if ((tripla.canale1 == 2 && tripla.canale2 == 64)||(tripla.canale2 == 2 && tripla.canale1 == 64)) {
            differenze_264.push_back(tripla.tempo2 - tripla.tempo1);
        }
        if ((tripla.canale2 == 2 && tripla.canale3 == 64)||(tripla.canale3 == 2 && tripla.canale2 == 64)) {
            differenze_264.push_back(tripla.tempo3 - tripla.tempo2);
        } //Tra 2 e 64 in tutte le combinazioni possibili
    }
}

//Ora faccio tre istogrammi che contengano le diff_ch1ch2
 
void makeHistogram(const std::vector<double>& data, int n_bins,const char* xlabel, double t_min, double t_max, const char* ylabel, const char* name){
    TCanvas* c = new TCanvas(name,name,800,600);
    TH1F* hist = new TH1F(name, name,n_bins, t_min, t_max);
    for(const auto&value : data){
        hist-> Fill(value);
    }
    hist->GetXaxis()->SetTitle(xlabel);
    hist->GetYaxis()->SetTitle(ylabel);
    hist->Draw();
    
    c->SaveAs((std::string(name)+".png").c_str());
} 

//Provo cambiando tempi_arrivo con tempi_veri
std::vector<double> DiffArrivo(const std::vector<double>& tempi_veri) {
    std::vector<double> differenze;
     for(size_t i = 1; i < tempi_veri.size(); ++i) {
        differenze.push_back(tempi_veri[i] - tempi_veri[i - 1]);
    
    }
    //for(int m=0; m<tempi_veri.size(); m++){
    //    std::cout<<"Tempi_veri[i]  "<<tempi_veri[m]<<std::endl; 
    //}
    return differenze;
}

