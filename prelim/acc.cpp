#include <math.h>
#include <TRandom.h>
#include <TH1.h>
#include <iostream>

void Singole(doubl T, double rate_acc, double rate_cosmic, vector <double> *ptr_singles1,vector <double> *ptr_singles2, vector <double> *ptr_singles3){
    double t_acc1=0.; 
    double t_acc2=0.; 
    double t_acc3=0.; 

    double t_cosmic=0.;
    do{
        t_acc1+=-log((*gRandom).Uniform())/rate_acc;
        t_acc2+=-log((*gRandom).Uniform())/rate_acc;
        t_acc3+=-log((*gRandom).Uniform())/rate_acc;
        
        if(t_acc1>t_cosmic){
            if(t_cosmics>0.) (*ptr_singles1).push_back(t_cosmic);
            t_cosmic+=-log((*gRandom).Uniform())/rate_cosmic;
        }

        if(t_acc<t_cosmic && t_acc<T) (*ptr_singles1).push_back(t_acc);

        if (t_acc>T && t_cosmic>T) break;
    } while(True);
}

void analyze(vector <double> *ptr_singles){
    TH1F deltaT("deltaT", "delta T", 1000, 0, 5);
    for (unsigned long i=1; i<ptr_singles->size(); i++){
        deltaT.Fill((*ptr_singles)[i]-(*ptr_singles)[i-1]);
    }
    deltaT.DrawCopy()
}