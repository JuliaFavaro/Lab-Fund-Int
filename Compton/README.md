# Julia Favaro's Project - Fundamental Interactions Laboratory 2024-2025

## Compton Scattering lab experience

### Folder organization:
- _Calcoli veloci.ipynb_ : Code to reproduce the main beam divergence plot. Also verify with collected data the influence of accidental coincidences and PMT2 gain at our supply voltage and its associated sensitivity
- _Fondo.cpp_: Code to reconstruct the spectrum histogram from our measurements, derived from misaligning the two coincidence-triggering signals.
- _BKG_CS.cpp_ ,  _BKG_Na.cpp_ , _BKG_Co.cpp_ : Codes for preliminary background analysis of spectral acquisitions from reference calibration sources (Cesium, Sodium, Cobalt).
- _theta_18.cpp_, _theta_22.cpp_, theta_26.cpp_ , _theta_24.cpp_ : Code to fit the peaks of a Cobalt source at different angles. Each graph includes the date, angle and acquisition duration (with relative counts). We performed a rebinning of 4 channels after verifying that the loss of information on peak separation is negligible. For each graph, we performed a spectral background fit with 3 different models: exponential, 4th-degree polynomial, and 5th-degree polynomial. Finally we decided to use the 4th-degree polynomial as the primary fit and to use the the maximum dispersion obtained from these three methods as the systematic uncertainty on the Gaussian peak channels.


### Objectives of the experience
1. Observe the Compton scattering by measuring the energy of the outgoing photon at different angle of deflection and verify the theoretical prediction.
2. Report the rate as a function of the scattering angle.
3. Measure the mass of the electron.

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
