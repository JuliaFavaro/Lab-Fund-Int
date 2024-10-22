#include <math.h>
#include <iostream> //nota bene non come in C!!

/*
Goal: simulare l'esperienza priliminare:
 Tre rivelatori con efficienze date sovrapposti.
 Riempire l'istogramma delle coincidene doppie, triple.
 Costruire un istogramma dell'efficienza stimata dal rapporto triple/doppie
 */

void toyMonteCarlo(double T, double rate, int NExp, douuble efficienza 1){ // T durate temporale + rate + numero massimo di esperimenti da fare 
    double t=0.; //scala dei tempi
    int Nevt=0; //numero evento
    int VistoDa1; //conteggi del primo PMT

    TH1F histogram("Counts", "h", (int) (T*rate*3), 0, T * rate * 3); //nota il casting

    for(int experiment=0; experiment<NExp; experiment++){ //devo ripetere questo numerose volte

        t=0; //resetto il clock ad ogni esperimento
        Nevt=0.; 

        while (t<T){
            t+=-log(gRandom->Uniform()) / rate;
            Nevt++;

            if (gRandom->Unifor()<efficienza1)
            VistoDa1++;
            //l'ho visto, altrimenti non l'ho visto

            //cout<< "The event # "<< Nevt<< "occured at t "<< t<< endl; //stampa a schermo: cout ; string formatting; andare a capo:endl
        }

        histogram.Fill(Nevt-1);
    }

    histogram.DrawCopy(); //altrimenti perderei l'informazione sull'istogramma ad ogni giro
}