# Julia Favaro's Project - Fundamental Interactions Laboratory 2024-2025

## Preliminary lab experience: cosmic ray detection through Scintillators and PMT detectors.

### Folder organization:
- _Efficienza.py_: Python code used to create the graphs in the _Pictures_ folder: Trigger frequency dependency on the voltage and Efficiency of each of the PMTs on the alimentation voltage.
- _notesROOT.txt_: a couple of notes taken during Paoloni's lecture on ROOT
- _EfficiencyEstimation.cpp_: ToyMonteCarlo that calculates the PMT efficiency as simply as the rate of triple coincidences over the double coincidences. Does not consider accidental counts.
- _ToyMC.cpp_: ToyMontecarlo that replicates the sequence of accidental and cosmic events. Defines also the function of discriminators in our simulated experimental setup. This code was implemented by Prof. Paoloni.
- _NewTry.cpp_: Final version of ToyMontecarlo that implements the accidental over cosmic events simulation and coincidence time on discrminated signals.

### Objectives of the experience
- 

#### Installation and usage
Some of the code was developed using the C++ language and the ROOT library. My partner Arianna and I obtained the datasets during our lab lectures. Documentation is embedded in the code. 

Some useful information on how to install ROOT:[ROOT on WSL](https://root-forum.cern.ch/t/complete-root-installation-instructions-for-wsl-ubuntu-18-04-on-windows-10/35148/3), [ROOT official installation page](https://root.cern/install/)

### Authors and acknowledgment
Julia Favaro's bio: I'm a master's student from the University of Pisa (Italy). I am currently pursuing a career in high energy astrophysics and data analysis.

Lab partner: Samuele Chiarugi, master's student from the University of Pisa.

Supervisors and professors: Prof. Emanuele Paoloni, Prof. Giovanni Punzi, and Prof. Francesco Forti.

### Support
For more explanation or any suggestions, write me at j.favaro@studenti.unipi.it .

### License
GNU General Public License v3.0 or later

### Project status
Completed. Submitted on 20.11.2024.
