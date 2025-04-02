# Julia Favaro's Project - Fundamental Interactions Laboratory 2024-2025

## Compton Scattering lab experience

### Folder organization:
- _Calcoli veloci.ipynb_ : Code to reproduce the main beam divergence plot. Also verify with collected data the influence of accidental coincidences and PMT2 gain at our supply voltage and its associated sensitivity
- _Fondo.cpp_: Code to reconstruct the spectrum histogram from our measurements, derived from misaligning the two coincidence-triggering signals.
- _preliminare.cpp_: Code to draw the graphs of PMT1 signal as a function of its alimentation voltage. 
- _BKG_CS.cpp_ ,  _BKG_Na.cpp_ , _BKG_Co.cpp_ : Codes for preliminary background analysis of spectral acquisitions from reference calibration sources (Cesium, Sodium, Cobalt).
- _theta_28.cpp_, _theta_22.cpp_, theta_26.cpp_ , _theta_24.cpp_ : Code to fit the peaks of a Cobalt source at different angles. Each graph includes the date, angle and acquisition duration (with relative counts). We performed a rebinning of 4 channels after verifying that the loss of information on peak separation is negligible. For each graph, we performed a spectral background fit with 3 different models: exponential, 4th-degree polynomial, and 5th-degree polynomial. Finally we decided to use the 4th-degree polynomial as the primary fit and to use the the maximum dispersion obtained from these three methods and variations over the range bin as the systematic uncertainty on the Gaussian peak channels.
For theta_26_terzopicco.cpp you can also find a gaussian estimation of the third visible peak in the spectrum.
Fro theta_18_4binning.cpp you can find a different approach at binning for shorter spectrum acquisition.
- _Compton_v3.h_, _Compton_2003.cpp_ : Code example of use of the library to create the calibration functions fit over the mean values of the wi
- _Information_fisher.cpp_ : Code that calculates the Divergence of Kullback-Leibner between the original and the binned histogram. 
- _Distanza_51cm.cpp_ : This code fits the peak of a cobalt source detected at 22° degrees for the main axis of the beam and at 51.5 cm from the source. By using this spectrum and the one that comes from theta_22.cpp you can find the difference in gaussian width between the two configurations. 
- _Calibration_47cm.cpp_ _Calibration_51cm.cpp_: used for the calibration function of the two aforementioned acquisitions so that we can quantify the difference in uncertainty on the single mass of the electron. 
- _Electron_mass.cpp_ : Codice che calcola la massa dell'elettrone in base a un fit lineare energia vs theta. Calcola l'errore sistematico tramite un approccio combinatorio su tutte le possibili configurazioni dell'energia in base ai loro errori sistematici.

### Objectives of the experience
1. Observe the Compton scattering by measuring the energy of the outgoing photon at different angle of deflection and verify the theoretical prediction.
2. Measure the mass of the electron.

#### Installation and usage
Most of the code was developed using the C++ language and the ROOT library. My partner Arianna and I obtained the datasets during our lab lectures. This is available on a private Google Drive because of GitHub space limitation. Documentation is embedded in the code. 

Some useful information on how to install ROOT:[ROOT on WSL](https://root-forum.cern.ch/t/complete-root-installation-instructions-for-wsl-ubuntu-18-04-on-windows-10/35148/3), [ROOT official installation page](https://root.cern/install/)

Disclaimer: I'm bilingual and in many cases you will find comments in both languages, as this was written on the spot. This way of thinking helped me develop my project faster.

### Authors and acknowledgment
Julia Favaro's bio: I'm a master's student from the University of Pisa (Italy). 
Supervisors for this lab experience: Prof. Giovanni Punzi, Edoardo Bossini and Jacopo Pinzino.

### Support
For more explanation or any suggestions, write me at j.favaro@studenti.unipi.it .

### License
GNU General Public License v3.0 or later

### Project status
February- March 2025
