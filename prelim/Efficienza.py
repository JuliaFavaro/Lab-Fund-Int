import numpy as np
import matplotlib.pyplot as plt

def accidentali(c1, c2):
    """Piccola funzione che calcola il numero di doppie accidentali"""
    dt=10 #s, impostata sul discriminatore
    w=50e-9 #finestra temporale dell' uscita del discriminatore
    wmin=2e-9 #finestra necessariamente per coincidenza
    rate1=c1/dt
    rate2=c2/dt
    return  dt*rate1*rate2*(2*w-2*wmin)

def mean_of_triplets(arr):
    """Funzione che calcola media e deviazione standard per ogni set di 3 di un array"""
    # Deve essere un multiplo di 3
    length = len(arr)
    if length % 3 != 0:
        print("Array length is not a multiple of 3.")
        return []
        
    reshaped_arr = np.reshape(arr, (-1, 3)) #Creo una matrice 2D in cui ciascuna riga contiene 3 elementi consecutivi di arr.
    mean_values = np.mean(reshaped_arr, axis=1) #mean for each row
    std_values =np.std(reshaped_arr, axis=1) #std deviation for each row
    return mean_values, std_values

### **Frequenza di trigger al variare della soglia**
#PMT05: a 1680 V di alimentazione. Scala di 20mV/div e 25ns/div su CH1 ad oscilloscopio

Vtrig=np.array([-10.4, -14.4,-18.4, -22.4, -26.4, -30.4, -34.2, -38.4, -42.4, -46.4])*1e-3 #mV d
dVtrig=0.1*np.ones(len(Vtrig))*1e-3 #assumiamo metà della risoluzione come errore
ftrig=np.array([401, 459, 442, 276, 225, 257, 236, 127, 185, 230, 235, 161, 184, 130, 203, 139, 140, 144, 124, 126, 101, 113, 106, 93, 105, 88, 63, 60, 42, 63])

ftrig, dftrig=mean_of_triplets(ftrig)
for ii in range(len(ftrig)):
    print(rf'Vtrig: {Vtrig[ii]*1e3:.2f} mV: {ftrig[ii]:.0f} \pm {dftrig[ii]:.0f}, errore percentuale {dftrig[ii]/ftrig[ii]*100:.0f} \%')

# Il segnale non è periodico.
# A basse frequenze, la misura è meno precisa.
# L'errore percentuale è minimo per -30.4 (per cui consideriamo il voltaggio di trigger ottimale)
# Se aumentiamo il voltaggio di trigger, aumentano il numero di volte in cui scatta il segnale.

plt.figure(1)
plt.title('Frequenza di trigger al variare della tensione di trigger')
plt.ylabel('$f_{trig}$ [Hz]')
plt.xlabel('$V_{trig}$ [mV]')
plt.errorbar(Vtrig*1e3, ftrig, dftrig, dVtrig*1e3,fmt='.')
plt.savefig("Frequenza_trigger.png")

### **Conteggi in singola**
Va5=np.array([1600, 1620, 1640, 1680, 1660, 1700, 1720, 1740, 1760, 1780, 1800])
cs5=np.array([632, 780, 987, 1256, 1710, 2218, 3182, 5396, 8386, 11568, 13554])
dc5=np.sqrt(cs5)

Va3=np.array([1640, 1660, 1680, 1700, 1720, 1740, 1760, 1780, 1800, 1810, 1815])
cs3=np.array([126, 200, 379, 487, 666, 939, 1212, 1673, 3355, 5925, 7945])
dc3=np.sqrt(cs3)

Va4=np.array([1600, 1620, 1640, 1660, 1680, 1700, 1710])
cs4=np.array([234, 477, 740, 1199, 1689, 4076, 12362])
dc4=np.sqrt(cs4)

plt.figure(2)
plt.title('Conteggi in singola')
plt.xlabel('$V_{alim}$ [Hz]')
plt.ylabel('Rate [cps]')
plt.errorbar(Va5, cs5/10, dc5/10, color='lightcoral', fmt='.', label='PMT05')
plt.errorbar(Va4, cs4/10, dc4/10, color='sandybrown', fmt='.', label='PMT04')
plt.errorbar(Va3, cs3/10, dc3/10, color='limegreen', fmt='.', label='PMT03')
plt.legend(loc='best')
plt.savefig("Conteggi_singola.png")

### **Efficienza del PMT04**
# Misuriamo le doppie da PMT03 e PMT05 mantenuti a 1680 V. Calcoliamo le triple da PMT03, PMT04, PMT05, mentre variamo la tensione di alimentazione di PMT04. 
triple=np.array([30,62, 67, 88, 63, 76, 73, 80, 107, 79, 84]) #errore binomiale
doppie=np.array([81,94, 90, 91, 64, 79, 77, 83, 108, 78, 86]) #non c'è errore
V=np.array([1580,1600.00, 1620.00, 1640.00, 1660.00, 1680.00, 1700, 1720, 1740, 1760, 1780])
dV=0.05*np.ones(len(V))#errore da GeCo

c5=1710 #conteggio singola del PMT05 a 1680 V
c3=1673 #conteggio singola del PMT03 a 1680 V
acc=accidentali(c5, c3)
print(acc)

eff=triple/(doppie-acc)
deff=np.sqrt(1/doppie*eff*(1-eff))

plt.figure(3)
fig, ax = plt.subplots()
ax2 = ax.twinx()

plt.title('Efficienza PMT04')
ax.set_ylabel("Efficienza PMT04")
ax2.set_ylabel("Rate [cps]")
ax.set_xlabel('$V_{alim}$ di PMT04')
ax.errorbar(V, eff, deff, dV, color='sandybrown', fmt='.')
ax2.errorbar(Va4, cs4/10, dc4/10, color='cornflowerblue', fmt='^')
plt.savefig("Efficienza_PMT04.png")

### **Efficienza PMT03.** 
# Misuriamo le doppie da PMT04 e PMT05 mantenuti a 1660 V. Calcoliamo le triple da PMT03, PMT04, PMT05, mentre variamo la tensione di alimentazione di PMT03. 
triple=np.array([23,29, 63,77, 106,115, 113, 110, 104, 124 ])
doppie=np.array([156,149, 156, 155, 170, 180, 168, 175, 157, 173])
V=np.array([1620,1640, 1660, 1680, 1700, 1720, 1740, 1760, 1780, 1800])
dV=0.05*np.ones(len(V))#errore da GeCo

c5=1256 #conteggio singola del PMT05 a 1660 V
c4=1199 #conteggio singola del PMT04 a 1660 V
acc=accidentali(c5,c4)
print(acc)

eff=triple/(doppie-acc)
deff=np.sqrt(1/doppie*eff*(1-eff))

plt.figure(4)
fig, ax = plt.subplots()
ax2 = ax.twinx()

plt.title('Efficienza PMT03')
ax.set_ylabel("Efficienza PMT03")
ax2.set_ylabel("Rate [cps]")
ax.set_xlabel('$V_{alim}$ di PMT04')
ax.errorbar(V, eff, deff, dV, color='limegreen', fmt='.')
ax2.errorbar(Va3, cs3/10, dc3/10, color='cornflowerblue', fmt='^')
plt.savefig("Efficienza_PMT03.png")

### **Efficienza PMT05.** 
# Misuriamo le doppie da PMT03 e PMT04 mantenuti a 1670 V. Calcoliamo le triple da PMT03, PMT04, PMT05, mentre variamo la tensione di alimentazione di PMT05. 
triple=np.array([38, 49, 49, 49,54,61,60,65,68, 85, 78, 67, 78, 82, 70])
doppie=np.array([119, 99, 100, 104,112,106, 103,96, 104, 115, 100, 100, 110, 116, 99])
V=np.array([1500.00, 1520.00, 1540.00,1560.00,1580.00,1600.00, 1620, 1640,1660, 1680, 1700, 1720, 1740, 1760, 1780])
dV=0.05*np.ones(len(V))#errore da GeCo

c4=1290 
c3=236
acc=accidentali(c3,c4)
print(acc)

eff=triple/(doppie-acc)
deff=np.sqrt(1/doppie*eff*(1-eff))

plt.figure(5)
fig, ax = plt.subplots()
ax2 = ax.twinx()

plt.title('Efficienza PMT05')
ax.set_ylabel("Efficienza PMT05")
ax2.set_ylabel("Rate [cps]")
ax.set_xlabel('$V_{alim}$ di PMT05')
ax.errorbar(V, eff, deff, dV, color='lightcoral', fmt='.')
ax2.errorbar(Va5, cs5/10, dc5/10, color='cornflowerblue', fmt='^')
plt.savefig("Efficienza_PMT05.png")