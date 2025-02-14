# Julia Favaro's Project - Fundamental Interactions Laboratory 2024-2025

## Extensive cosmic ray showers study

### Folder organization:
- _Efficiency_Setup08.cpp_, _Efficiency_Setup06.cpp_, _Efficiency_Stanza2004.cpp_ : C++ code to calculate the efficiency and detection rate for each PMT in each Setup for different Alimentation voltage. It considers also the correction due to accidental counts. By using the _Efficiency_Setup06.cpp_ file you can also create a graph of the efficiency of one of the PMTs as a function of time and evaluate it's uniformity with a linear fit. 
- _ToyMonteCarlo.py_ : Python code to estimate the cosmic flux  detection rate for each setup, by adjusting for the geometrical acceptance of each setup.
- _read_data10.h_: Library in C++ that contains the definition of the reading functions required to read the data coming from the De10Nano DAQ system. 
- _flux_graphs_10.h_: Library in C++ that contains the function to calculate the rates of each of our telescopes. Creates the graphs of the rate as a function of time and the exponential and poissonian distributions of cosmic rays detected for each telescope.
- _atmosphere.h_: Library in C++ that contains the definition of the reading functions required to read and bin the data coming from the UniPi meteorologic station. Creates the dispersion graphs of the rate of each each telescope as a function of these atmospheric parameters. 
- _Acquisition_3d.cpp_ , _Acquisition_7d.cpp_ : C++ codes to calculate the cosmic ray flux registered over time by each setup in 3days and 7 days. For each setup also creates a poissonian fit over the histogram and an exponential fit over the distribution of time difference between events. Finally it shows the correlation between flux and the atmospheric conditions.
- _flux_coincidences.h_ : Library in C++ that contains the function to calculate the coincidence events between our telescopes, that are likely due to an extensive cosmic ray shower. Creates the graphs of the rate as a function of time and the exponential and poissonian distributions of cosmic rays.
- _Coincidence.cpp_:  C++ code to determine the presence of extensive cosmic ray showers, in both our datasets of 3days and 7 days. Creates a poissonian fit over the histogram and an exponential fit over the distribution of time difference between events. 

- _read_data0.h_ : File C++ that contains the definition of the reading functions required to read the data coming from the De0Nano DAQ system. It also reconverts the time in linear manner.
- _De0Nano.cpp_ : Code to read the data coming from the De0Nano FPGA acqusition system. Creates a graph of the rate of each telescope as a function of time. Creates an histogram of the counts distribution for each telescope. 

### Objectives of the experience
1. Verify the existence of possible temporal correlations between the arrivals of cosmic rays at successive times.
2. Verify the existence of correlated arrivals of cosmic rays in more than one telescope simultaneously (extended air showers). Characterize their frequency and any other information useful to investigate their physical origin.
3. Measure the flux of cosmic rays, both localized or extended, over long periods (days) and verify the existence of possible variations, daily or aperiodic, in the flux.

#### Installation and usage
Most of the code was developed using the C++ language and the ROOT library. My partner Arianna and I obtained the datasets during our lab lectures. This is available on a private Google Drive because of GitHub space limitation. Documentation is embedded in the code. 

Some useful information on how to install ROOT:[ROOT on WSL](https://root-forum.cern.ch/t/complete-root-installation-instructions-for-wsl-ubuntu-18-04-on-windows-10/35148/3), [ROOT official installation page](https://root.cern/install/)

The only Python file was originally developed as Jupyter Notebook on Anaconda.

Disclaimer: I'm bilingual and in many cases you will find comments in both languages, as this was written on the spot. This way of thinking helped me develop my project faster.

### Authors and acknowledgment
Julia Favaro's bio: I'm a master's student from the University of Pisa (Italy). 
Supervisors for this lab experience: Prof. Eugenio Paoloni and Prof. Francesco Forti.

### Support
For more explanation or any suggestions, write me at j.favaro@studenti.unipi.it .

### License
GNU General Public License v3.0 or later

### Project status
November 2024- January 2025
