# Julia Favaro's Project - Fundamental Interactions Laboratory 2024-2025

## Muon decay lab experience
- File _decoder.cpp_ and _decoder_: 
Decode data stored in an output file produced by FIFOread.c (FPGA De10Nano) and print a table in a file.
The output file contains (from left to right):
- timestamp: number of clock cycle since FPGA deassertion (perodical clock counter resets are resolved and removed)
- digital channels: logic value of the 12 digital channel (from ch 0 to ch 11)
- place holder: fixed to "0 0 0"
- analog flag: 1 if the analog value is valid in this entry (the reading was not performed at this timestamp)
- analog id: identifier of the analog reading
- analag value: digitalized value of the analog signal
Usage: ./decoder inputFile outputFile
- File _Punti.lavoro.angolo.ipynb_ : 
Jupyter notebook (Python) that calculates the efficiency of each PMT of our experiment, prints the graphs of efficiency over single counts for each PMT and calculates the solid angle described by our system.
- File _ToyMonteCarlo.ipynb_ :
Jupyter notebook (Python) that cointains the MC simulation of predicted START and STOP rate signals from our experiments. It also calculates the intrinsic efficiency of our PMT.
- File _Acquisizione.2404.5giorni.cpp_ :
File C++/ROOT che individua la differenza nella distribuzione degli intervalli temporali per 2 diverse logiche di STOP.
- File _Distribuzione.start.stop.cpp_ : 
File C++/ROOT che individua la differenza nella distribuzione degli intervalli temporali per 3 diverse logiche di STOP. Infine controlla la distribuzione del numero di stop che susseguono ciascuno start per queste 3 diverse logiche.
- File _Afterpulses.cpp_ :
File che evidenzia come il picco intorno a 200-250 ns sia un picco spurio più probabilmente legato a qualche tipo di afterpulse che a un problema della logica. In questa distribuzione degli intervalli temporali infatti lo start è rappresentato da un semplice muone che attraversa tutto  il sistema senza mai fermarsi. 
- File _save.data.to.tree.cpp_ : 
- File _view.data.from.tree.cpp_ :


### Folder organization:

### Objectives of the experience
1. Measure as precisely as possible the lifetime of muons that stop in the target, by creating a system to measure the time between their arrival and their subsequent decay. 
2. Determine as precisely as possible the mass of these particles, based on the distribution of energy released by the decay products.

#### Installation and usage
Most of the code was developed using the C++ language and the ROOT library. My partner Arianna and I obtained the datasets during our lab lectures. This is available on a private Google Drive because of GitHub space limitation. Documentation is embedded in the code. 

Some useful information on how to install ROOT:[ROOT on WSL](https://root-forum.cern.ch/t/complete-root-installation-instructions-for-wsl-ubuntu-18-04-on-windows-10/35148/3), [ROOT official installation page](https://root.cern/install/)

Disclaimer: I'm bilingual and in many cases you will find comments in both languages, as this was written on the spot. This way of thinking helped me develop my project faster.

### Authors and acknowledgment
Julia Favaro's bio: I'm a master's student from the University of Pisa (Italy). 
Supervisors for this lab experience: Prof. Giovanni Punzi and Prof. Francesco Forti.

### Support
For more explanation or any suggestions, write me at j.favaro@studenti.unipi.it .

### License
GNU General Public License v3.0 or later

### Project status
April 2025- May 2025
