# Julia Favaro's Project - Fundamental Interactions Laboratory 2024-2025

## Extensive cosmic ray showers study

### Folder organization:
- _Efficiency_Setup08.cpp_, _Efficiency_Setup06.cpp_, Efficiency_Stanza2004.cpp_ : C++ code to calculate the efficiency and single counts for each PMT in each Setup for different Alimentation voltage.
- _Flux_calculations.py_ : Python code to estimate the cosmic flux in 10s for each setup and to estimate the geometrical acceptance of each setup.
- _De0Nano.cpp_ : Code to read the data coming from the De0Nano FPGA acqusition of data.  

### Objectives of the experience
1. Verify the existence of possible temporal correlations between the arrivals of cosmic rays at successive times.
2. Verify the existence of correlated arrivals of cosmic rays in more than one telescope simultaneously (extended air showers). Characterize their frequency and any other information useful to investigate their physical origin.
3. Measure the flux of cosmic rays, both localized or extended, over long periods (days) and verify the existence of possible variations, daily or aperiodic, in the flux.

#### Installation and usage
Some of the code was developed using the C++ language and the ROOT library. My partner Arianna and I obtained the datasets during our lab lectures. Documentation is embedded in the code. 

Some useful information on how to install ROOT:[ROOT on WSL](https://root-forum.cern.ch/t/complete-root-installation-instructions-for-wsl-ubuntu-18-04-on-windows-10/35148/3), [ROOT official installation page](https://root.cern/install/)

### Authors and acknowledgment
Julia Favaro's bio: I'm a master's student from the University of Pisa (Italy). I am currently pursuing a career in high energy astrophysics and data analysis.

Lab partner: Arianna Fornaciari, master's student from the University of Pisa.

Supervisors and professors: Prof. Emanuele Paoloni, Prof. Giovanni Punzi, and Prof. Francesco Forti.

### Support
For more explanation or any suggestions, write me at j.favaro@studenti.unipi.it .

### License
GNU General Public License v3.0 or later

### Project status
In progress
