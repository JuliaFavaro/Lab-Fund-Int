## Librerie e funzioni
import numpy as np
import matplotlib.pyplot as plt

cosmic=1 #al cm^2, al min 
#oppure 100 al s su metro per ster radianti

def errore_prodotto(x,y,dx,dy):
    return np.sqrt((y*dx)**2+(x*dy)**2)
def errore_prodotto3(x,dx, y,dy, z, dz):
    return np.sqrt((x*y*dz)**2+(x*z*dy)**2+(dx*y*z)**2)

def accettanza_sistema2(length1,length2, width,height):
    """Calcolo della efficienza geometrica per 2 scintillatori sovrapposti con geometria diversa"""
    n = 10000 #Numero di tentativi
    # Inizializza contatori
    successi = 0
    tentativi = 0
    
    # Generatore di numeri casuali
    rng = np.random.default_rng() #random seed
    
    for _ in range(n):
        # Genera coordinate x1 e y1 sul primo scintillatore
        x1 = rng.uniform(0, width)
        y1 = rng.uniform(0, length1)
        
        # Genera angoli ϕ e θ
        phi = rng.uniform(0, 2 * np.pi)
        theta = np.arccos(rng.uniform(0, 1)**(1/4))  # distribuito come cos2(θ), cos in più considerando che non è sempre perpendicolare 
        
        # Proiezione del fascio sul secondo scintillatore
        x2 = x1 - height * np.tan(theta) * np.cos(phi)
        y2 = y1 - height * np.tan(theta) * np.sin(phi)
                 
        # Verifica se x2 e y2 sono compresi nelle dimensioni del secondo scintillatore
        if 0 <= x2 <= width and 0 <= y2 <= length2:
            successi += 1
        tentativi += 1 #se passa dall'altro scintillatore, quanto è probabile che passi anche dal secondo?
    
    # Calcola l'efficienza geometrica
    efficienza_geometrica = successi / tentativi
    # Calcola l'errore come una semplice distribuzione binomiale
    errore = np.sqrt(efficienza_geometrica * (1 - efficienza_geometrica) / tentativi)
    return efficienza_geometrica, errore

def accettanza_sistema3(length1, length2, length3, width,height1, height2):
    """Calcolo della efficienza geometrica per 3 scintillatori sovrapposti con geometria diversa.
    L'altezza è positiva se considerata dal basso verso l'alto, negativa se viceversa.
    1 e 2 rappresentano la doppia, mentre 3 rappresenta lo scintillatore di cui vogliamo calcolare l'accettanza geometrica."""
    n = 10000 #Numero di tentativi
    # Inizializza contatori
    successi = 0
    tentativi = 0
    
    # Generatore di numeri casuali
    rng = np.random.default_rng() #random seed
    
    for _ in range(n):
        # Genera coordinate x1 e y1 sul primo scintillatore
        x1 = rng.uniform(0, width)
        y1 = rng.uniform(0, length1)
        
        # Genera angoli ϕ e θ
        phi = rng.uniform(0, 2 * np.pi)
        theta = np.arccos(rng.uniform(0, 1)**(1/4))  # distribuito come cos3(θ)
        
        # Proiezione del fascio sul secondo scintillatore
        x2 = x1 - height1 * np.tan(theta) * np.cos(phi) 
        y2 = y1 - height1 * np.tan(theta) * np.sin(phi)

        # Proiezione del fascio sul terzo scintillatore
        x3 = x2 - height2 * np.tan(theta) * np.cos(phi) 
        y3 = y2 - height2 * np.tan(theta) * np.sin(phi)
        
        # Verifica se x2 e y2 sono compresi nelle dimensioni del secondo scintillatore
        if 0 <= x2 <= width and 0 <= y2 <= length2:
            if 0 <= x3 <= width and 0 <= y3 <= length3:
                successi += 1
            tentativi+=1 #probabilità che passi dal terzo PMT data la doppia
    
    efficienza_geometrica = successi / tentativi
    errore = np.sqrt(efficienza_geometrica * (1 - efficienza_geometrica) / tentativi)
    return efficienza_geometrica, errore

## Setup08
length1=60.0
length2=39.0
dlength=0.1
width=20.0
dwidth=0.5

height1=2
height2=2.5 #considerati dal basso verso l'alto

aeff=length2*width #area di sovrapposizione
daeff=errore_prodotto(length2,width,dlength,dwidth)

print(f"Inizio Setup08\n----------\nArea efficace {aeff:.0f} ± {daeff:.0f} cm^2")

triple_doppie1=0.88
errore1=0.06
triple_doppie2=0.86
errore2=0.05

acc3_1, errore_acc3_1=accettanza_sistema3(length2,length2,length1, width,-height2, height1+height2) 
print(f'Accettanza1: {acc3_1}')

epsilon_intr1=triple_doppie1/acc3_1
depsilon_intr1=errore_prodotto(triple_doppie1,acc3_1,errore1,errore_acc3_1)
print(f"Efficienza intrinseca PMT1 {epsilon_intr1:.2f} ± {depsilon_intr1:.2f}")

acc3_2, errore_acc3_2=accettanza_sistema3(length1,length2,length2, width,-(height1+height2), height2) 
print(f'Accettanza2: {acc3_2}')

epsilon_intr2=triple_doppie2/acc3_2
depsilon_intr2=errore_prodotto(triple_doppie2,acc3_2,errore2,errore_acc3_2)
print(f"Efficienza intrinseca PMT2 {epsilon_intr2:.2f} ± {depsilon_intr2:.2f}")

geom_eff1, err_geom1=accettanza_sistema2(length2,length1,width,height1)
print(f"Efficienza geometrica 1: {geom_eff1:.3f}  ± {err_geom1:.3f}")

geom_eff2, err_geom2=accettanza_sistema2(length1,length2,width,-height1)
print(f"Efficienza geometrica 2: {geom_eff2:.3f}  ± {err_geom2:.3f}")

epsilon_eff1=epsilon_intr1*geom_eff1
depsilon_eff1=errore_prodotto(epsilon_intr1,geom_eff1,depsilon_intr1,err_geom1)
print(f"Efficienza corretta per l'accettanza 1: {epsilon_eff1:.2f} ± {depsilon_eff1:.2f}")

epsilon_eff2=epsilon_intr2*geom_eff2
depsilon_eff2=errore_prodotto(epsilon_intr2,geom_eff2,depsilon_intr2,err_geom2)
print(f"Efficienza corretta per l'accettanza 2: {epsilon_eff2:.2f} ± {depsilon_eff2:.2f}")

epsilon_eff=epsilon_eff1*epsilon_eff2
depsilon_eff=errore_prodotto(epsilon_eff1,depsilon_eff1,epsilon_eff2,depsilon_eff2)
print(f"Efficienza Setup08: {epsilon_eff:.2f} ± {depsilon_eff:.2f}")

rate_cosmic1=cosmic*aeff*1/60
drate_cosmic1=cosmic*daeff*1/60
print(f"Rate Setup08: {epsilon_eff*rate_cosmic1} ± {errore_prodotto(rate_cosmic1, epsilon_eff, drate_cosmic1, depsilon_eff)}")

rate1=4548
drate1=12
rate2=1063
drate2=6
racc=rate1*rate2*2*(40e-9-2e-9)
dracc=np.sqrt((drate1*rate2)**2+(rate1*drate2)**2)*2*(40e-9-2e-9)
print(f"Rate eventi accidentali: {racc*1e3:.0f}± {dracc*1e3:.0f} mHz")

## Setup06
length1=35.0
length2=41.0
dlength=0.1
width=20.0
dwidth=0.1
distance=3.5

height1=3.5
height2=2.0

aeff=length1*width
daeff=errore_prodotto(length1,width,dlength,dwidth)

print(f"Inizio Setup06\n----------\nArea efficace {aeff:.0f} ± {daeff:.0f} cm^2")

triple_doppie1=0.7
errore1=0.1
triple_doppie3=0.6
errore3=0.1

acc3_1, errore_acc3_1=accettanza_sistema3(length2,length2,length1, width,-height2, height1+height2) 
print(f'Accettanza1: {acc3_1}')

epsilon_intr1=triple_doppie1/acc3_1
depsilon_intr1=errore_prodotto(triple_doppie1,acc3_1,errore1,errore_acc3_1)
print(f"Efficienza intrinseca PMT1 {epsilon_intr1:.2f} ± {depsilon_intr1:.2f}")

acc3_3, errore_acc3_3=accettanza_sistema3(length1,length2,length2, width,-height1,-height2) 
print(f'Accettanza2: {acc3_3}')

epsilon_intr3=triple_doppie3/acc3_3
depsilon_intr3=errore_prodotto(triple_doppie3,acc3_3,errore3,errore_acc3_3)
print(f"Efficienza intrinseca PMT3 {epsilon_intr3:.2f} ± {depsilon_intr3:.2f}")

geom_eff1, err_geom1=accettanza_sistema2(length2,length1,width,height1+height2)
print(f"Efficienza geometrica 1: {geom_eff1:.3f}  ± {err_geom1:.3f}")

geom_eff3, err_geom3=accettanza_sistema2(length1,length2,width,-height1-height2)
print(f"Efficienza geometrica 3: {geom_eff3:.3f}  ± {err_geom3:.3f}")

epsilon_eff1=epsilon_intr1*geom_eff1
depsilon_eff1=errore_prodotto(epsilon_intr1,geom_eff1,depsilon_intr1,err_geom1)
print(f"Efficienza corretta per l'accettanza 1: {epsilon_eff1:.2f} ± {depsilon_eff1:.2f}")

epsilon_eff3=epsilon_intr3*geom_eff3
depsilon_eff3=errore_prodotto(epsilon_intr3,geom_eff3,depsilon_intr3,err_geom3)
print(f"Efficienza corretta per l'accettanza 3: {epsilon_eff3:.2f} ± {depsilon_eff3:.2f}")

epsilon_eff=epsilon_eff1*epsilon_eff3
depsilon_eff=errore_prodotto(epsilon_eff1,depsilon_eff1,epsilon_eff3,depsilon_eff3)
print(f"Efficienza Setup06: {epsilon_eff:.2f} ± {depsilon_eff:.2f}")

rate_cosmic2=cosmic*aeff*1/60
drate_cosmic2=cosmic*daeff*1/60
print(f"Rate Setup06: {epsilon_eff*rate_cosmic2} ± {errore_prodotto(rate_cosmic2, epsilon_eff, drate_cosmic2, depsilon_eff)}")

rate1=31
drate1=2
rate3=46
drate3=2
racc=rate1*rate3*2*(40e-9-2e-9)
dracc=np.sqrt((drate1*rate3)**2+(rate1*drate3)**2)*2*(40e-9-2e-9)
print(f"Rate eventi accidentali:{racc*1e6:.0f}± {dracc*1e6:.0f} muHz")

## Setup04

length=41.0
dlength=0.5
width=49.0
dwidth=0.5
height=20
dheight=0.5 #sono tutti esattamente uguali

aeff=length*width
daeff=errore_prodotto(length,width,dlength,dwidth)

print(f"Inizio Stanza2004\n----------\nArea efficace {aeff:.0f} ± {daeff:.0f} cm^2")

epsilon1=0.63
depsilon1=0.02
epsilon2=0.96
depsilon2=0.02
epsilon3=0.53
depsilon3=0.04

epsilon_eff=epsilon1*epsilon2*epsilon3
depsilon_eff=errore_prodotto3(epsilon1,depsilon1,epsilon2,depsilon2,epsilon3,depsilon3)
print(f"Efficienza Setup04: {epsilon_eff:.2f} ± {depsilon_eff:.2f}")

rate_cosmic3=cosmic*aeff*1/60
drate_cosmic3=cosmic*daeff*1/60
print(f"Rate Setup04: {epsilon_eff*rate_cosmic3} ± {errore_prodotto(rate_cosmic3, epsilon_eff, drate_cosmic3, depsilon_eff)}")

rate1=474
drate1=4
rate2=756
drate2=9
rate3=1350
drate3=11
racc=rate1*rate2*rate3*4*(40e-9-2e-9)**2
dracc=np.sqrt((drate1*rate2*rate3)**2+(rate1*drate2*rate3)**2+(rate1*rate2*drate3)**2)*4*(40e-9-2e-9)**2
print(f"Rate eventi accidentali:{racc*1e6:.2f}± {dracc*1e6:.2f} mu Hz")

## Distanze tra i telescopi
mattonelle_muro=40
larghezza_matt_muro=20
err_matt=0.2 #doppio della risoluzione del metro a nastro
lunghezza_muro=mattonelle_muro*larghezza_matt_muro
dlunghezza_muro=mattonelle_muro*err_matt
print(f"Lunghezza muro {lunghezza_muro/100:.2f} +- {dlunghezza_muro/100} m")

tel_04_08=lunghezza_muro-6*larghezza_matt_muro
dtel_04_08=dlunghezza_muro-6*err_matt
#trascuriamo lo spessore del muro
print(f"Distanza tra Setup04 e Setup08 {tel_04_08/100:.2f} +- {dtel_04_08/100:.2f} m")

mattonelle_stanza=17
larghezza_matt_stanza=30
err_matt_stanza=0.2
larghezza_stanza=mattonelle_stanza*larghezza_matt_stanza
dlarghezza=mattonelle_stanza*err_matt_stanza
print(f"Larghezza della stanza {larghezza_stanza/100:.2f} +- {dlarghezza/100:.2f} m")

distanza_lungo_08_06=13*30
ddistanza_lungo_08_06=0.2*13
tel_08_06=np.sqrt(distanza_lungo_08_06**2+larghezza_stanza**2)
dtel_08_06=np.sqrt((distanza_lungo_08_06/tel_08_06*ddistanza_lungo_08_06)**2+(larghezza_stanza/tel_08_06*dlarghezza)**2)
print(f"Distanza tra Setup08 e Setup06 {tel_08_06/100:.2f} +- {dtel_08_06/100:.2f} m")

distanza_lungo_06_04=17*20 #il setup06 a metà della stanza della sua stanza
ddistanza_06_04=17*0.2
tel_04_06=np.sqrt(distanza_lungo_06_04**2+larghezza_stanza**2)
dtel_04_06=np.sqrt((distanza_lungo_06_04/tel_04_06*ddistanza_06_04)**2+(larghezza_stanza/tel_04_06*dlarghezza)**2)
print(f"Distanza tra Setup04 e Setup06 {tel_04_06/100:.2f} +- {dtel_04_06/100:.2f} m")
