#include <TRandom.h>
#include <vector>
#include <math.h>
#include <iostream>
#include <TH1.h> 
#include <TCanvas.h>
#include <deque>
/*
 Usage: .L ToyMC.cc++
 to simulate the first 100 pulses of the system: ToyMC(100) 
 */
 
struct Event{  /* This structure describes the "atomic" event */
  
  //attributes of this class
  vector<unsigned int> fired_pmts; // set of indices of the fired pmts
  double t; // time of the event
  enum event_type { accidental=0, cosmic=1}; //a set of named possibility for event, mapped to integer numbers
  event_type type;
  
};

template< unsigned int N> // N number of PMTs, constant in Status
struct Status{  /* This structure describes the near future evolution of the system */
  
  double t_next_accidentals[N]; // vector of times of the next accidental of each PMT
  double t_next_cosmic;         // time of the next cosmic event
  double accidental_rates[N];   // vector of the accidental rate  for each PMT
  double cosmic_rate;           // rate of the cosmic rays
  double t_next_event;          // time of the next event

  // constructor to create an istance of this class
  Status( const vector<float> & rates, double cosmicRate){ //rates cannot be changed, so better using references &
    if( rates.size() != N) //sanity check
      throw;
    
    for(unsigned int i =0; i< N; i++){
      accidental_rates[i] = rates[i];
      t_next_accidentals[i] = -log( gRandom->Uniform()) / rates[i];
    }

    cosmic_rate = cosmicRate;
    t_next_cosmic = -log( gRandom->Uniform() ) / cosmic_rate ;
    t_next_event = -1.;
  }

  //inheritance of the Event struct
  Event next_event(){ // return the next event, and updates the near future state
    Event::event_type next_event_type = Event::event_type::accidental; //Initially assumes the next event type is accidental
    unsigned int firing_pmt = 0; 
    t_next_event = t_next_accidentals[0];

    //define next_event_type and t time
    for(unsigned int i = 1; i< N ; i++ ) 
      if( t_next_accidentals[i] < t_next_event ){ 
      /*If any PMT has an accidental event time sooner than the current t_next_event, 
      it updates t_next_event and sets firing_pmt to the index of that PMT.*/
        t_next_event = t_next_accidentals[i];
        firing_pmt =i ;
      }

    if( t_next_cosmic < t_next_event ){
        t_next_event = t_next_cosmic;
        next_event_type = Event::event_type::cosmic; //change assumption
        firing_pmt = N;//you can choose any, all of them have seen it
      }

    Event the_event;  //istance definition
    the_event.t = t_next_event;
    the_event.type = next_event_type;

    //define fired_pmts
    if ( next_event_type == Event::event_type::accidental ){
    /*Adds the firing_pmt to the list of fired_pmts.
    Updates t_next_accidentals for the PMT that fired to schedule the next accidental event.*/
      the_event.fired_pmts.push_back(firing_pmt);
      t_next_accidentals[ firing_pmt] += -log( gRandom->Uniform()) / accidental_rates[firing_pmt];
    }

    if( next_event_type == Event::event_type::cosmic ){
    /*Adds all PMTs to fired_pmts because a cosmic event affects all PMTs.
    Updates t_next_cosmic to schedule the next cosmic event.*/
      for(unsigned int i=0 ; i<N ;i++ )
        the_event.fired_pmts.push_back(i);
      t_next_cosmic += -log( gRandom->Uniform() ) / cosmic_rate ;
    }

    return the_event;
  }
    
};

template< unsigned int N>
class EfficiencyKill{ /* Filter out certain detected events based on specified efficiencies of PMTs.  */
public:
  //constructor
  EfficiencyKill( vector<double>  efficiencies){
    if( efficiencies.size() != N )
      throw;
    for (unsigned int i=0; i < N; i++)
      EfficiencyKill::efficiencies[i] = efficiencies[i]; //use namespace for unambigous initialization 
  }

  Event kill ( Event evt_in){
    if ( evt_in.type != Event::event_type::cosmic )
      return evt_in;

    Event evt_out;
    evt_out.t = evt_in.t;
    evt_out.type = evt_in.type;

    for( vector<unsigned int>::iterator pmt = evt_in.fired_pmts.begin(); pmt != evt_in.fired_pmts.end(); pmt++ ) //* pmt is a vector iterator index of a fired pmt
      /*Iterates through the fired PMTs in the input event and applies a random filter based on the efficiencies.
       If the random number (between 0 and 1) is less than the efficiency, it keeps the PMT in the output event.*/
      if( gRandom->Uniform() < efficiencies[ *pmt ])
        evt_out.fired_pmts.push_back( *pmt );

    return evt_out;
  }  
       
private: // you should not be able to use the interface to change efficiencies
  double efficiencies[N];
};

class Discriminator{/*Ensures that an output remains true for a specified time (gate_time)
 and can extend this period if another signal arrives within that time.*/
public:
  Discriminator( double gate_time):
    gate_time(gate_time){ //ensure the initial state is false
    true_since_time = -1.;
    true_till_time = -2.; }

  bool get_state(double t){ 
    return t >= true_since_time && t <= true_till_time ;}
  
  bool feed_signal( double t ){
    // this method returns true if the output flips state from false to true
    
    if ( get_state(t) == false ){ // discriminator output goes true till t + gate_time
      true_since_time = t;
      true_till_time = t + gate_time;
      return true;
    }
    else{ // discriminator is true: time extended
      true_till_time = t+gate_time;
      return false;
    }
  }
  
  double gate_time; // time span of the discriminated signal
  double true_since_time;
  double true_till_time;
};

template< int N> //N number of discriminators
class Discriminators{
public:
  Discriminators( vector<double> gate_times ){
    for( int i = 0 ; i <N ; i++)
      discriminators[i] = Discriminator( gate_times[i] ); //inheritance
  };

  bool feed_event( const Event & evt ){
    // this method returns true if **any** output flip state from false to true

    bool flip = false;
    for( auto pmt = evt.fired_pmts.begin() ; pmt < evt.fired_pmts.end() ; pmt++) //automatically deduces type of pmt iterator
      flip |= discriminators[ *pmt ].feed_signal( evt.t ); //bitwise OR assignment operator |=
    //if the left part is true then flip becomes true

    return flip;
  }

  Discriminator discriminators[N];
};


void ToyMC(unsigned int NExp) {
    std::vector<float> rate = {10, 10, 10};
    double cosmic = 1;

    TH1F* efficiency_histo = new TH1F("efficiency", "Efficiency PMT 2", 15, 0, 1);

    for (unsigned int exp = 0; exp < NExp; exp++) {
        Status<3> status(rate, cosmic);
        EfficiencyKill<3> efficiency_killer({0.7, 0.9, 0.8});

        double NeventsPMT1 = 0, NeventsPMT2 = 0, NeventsPMT3 = 0;
        double NeventsPMT13 = 0, NaccidentalPMT13 = 0, NeventsPMT123=0;
        double t = 0;

        std::deque<double> timesPMT1, timesPMT2, timesPMT3;

        while (t < 10) {  // Each experiment lasts 10 seconds
            Event evt = efficiency_killer.kill(status.next_event());
            t = evt.t;  // Update the time with the event time
            
            bool PMT1_fired = false;
            bool PMT2_fired = false;
            bool PMT3_fired = false;
            bool PMT1_accidental = false;
            bool PMT3_accidental = false;

            for (auto pmt = evt.fired_pmts.begin(); pmt != evt.fired_pmts.end(); pmt++){
                if (evt.type == Event::event_type::accidental) {
                    if (*pmt == 0) PMT1_accidental = true;
                    if (*pmt == 2) PMT3_accidental = true;
                }

                if (*pmt == 0) {
                    NeventsPMT1++;
                    PMT1_fired = true;
                    timesPMT1.push_back(t);
                }
                if (*pmt == 1) {
                    NeventsPMT2++;
                    PMT2_fired = true;
                    timesPMT2.push_back(t);
                }
                if (*pmt == 2) {
                    NeventsPMT3++;
                    PMT3_fired = true;
                    timesPMT3.push_back(t);
                }
            }

            // Remove old times beyond the 100ns window for PMT1 and PMT3
            while (!timesPMT1.empty() && t - timesPMT1.front() > 5e-9) timesPMT1.pop_front();
            while (!timesPMT2.empty() && t - timesPMT2.front() > 5e-9) timesPMT2.pop_front();
            while (!timesPMT3.empty() && t - timesPMT3.front() > 5e-9) timesPMT3.pop_front();

            // Check for coincidences between PMT1 and PMT3 within 100ns
            if (PMT1_fired && PMT3_fired) {
                for (double time1 : timesPMT1){
                    for (double time3 : timesPMT3){
                        if (fabs(time1 - time3) <= 5e-9) {
                            NeventsPMT13++;
                            if (PMT1_accidental || PMT3_accidental){
                                NaccidentalPMT13++;
                            }
                            break;
                        }
                    }
                    if (fabs(timesPMT1.back()-timesPMT3.back())<=5e-9) break;
                }
            }
            
            // Check for triple coincidences between PMT1, PMT2, and PMT3 within 100ns
            if (PMT1_fired && PMT2_fired && PMT3_fired) {
                for (double time1 : timesPMT1) {
                    for (double time2 : timesPMT2) {
                        for (double time3 : timesPMT3) {
                            if (fabs(time1 - time2) <= 5e-9 && fabs(time1 - time3) <= 5e-9) {
                                NeventsPMT123++;
                                break;
                            }
                        }
                        if (fabs(timesPMT1.back() - timesPMT2.back()) <= 5e-9) break;
                    }
                    if (fabs(timesPMT1.back() - timesPMT3.back()) <= 5e-9) break;
                }
            }
        }

        // Calculate efficiency for PMT 2 
        double efficiency_PMT2 = 0;
        if (NeventsPMT13 > NaccidentalPMT13) {
          efficiency_PMT2 = static_cast<double>(NeventsPMT123) / (NeventsPMT13 - NaccidentalPMT13); 
          efficiency_histo->Fill(efficiency_PMT2); 
        }

        // Print the results for each experiment
        std::cout << "Experiment " << exp + 1 << " results:" << std::endl;
        std::cout << "NeventsPMT123: " << NeventsPMT13 << std::endl;
        std::cout << "NeventsPMT13: " << NeventsPMT13 << std::endl;
        std::cout << "Efficiency " << efficiency_PMT2 << std::endl;
    }

    // Draw efficiency histogram
    TCanvas* c1 = new TCanvas("c1", "Efficiency Histogram", 800, 600);
    efficiency_histo->SetLineColor(kViolet+6); 
    efficiency_histo->SetLineWidth(3);
    efficiency_histo->Draw(); 
    double mean_efficiency = efficiency_histo->GetMean();
    double mean_efficiency = efficiency_histo->GetStd();
    double initial_efficiency = 0.9; // Efficienza iniziale fornita nel codice
    double percent_distance = fabs(mean_efficiency - initial_efficiency) / initial_efficiency * 100;
    // Aggiungere testo alla canvas
    c1->cd();
    TPaveText *pt = new TPaveText(0.1, 0.8, 0.3, 0.9, "NDC");
    pt->AddText(Form("Percent Distance: %.2f%%", percent_distance));
    pt->Draw();
    c1->Update();
}