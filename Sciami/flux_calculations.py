import numpy as np
import matplotlib.pyplot as plt

cosmic=1 #al cm^2, al min 
#oppure 100 al s su metro per ster radianti

def errore_prodotto(x,y,dx,dy):
    return np.sqrt((y*dx)**2+(x*dy)**2)
def errore_prodotto3(x,dx, y,dy, z, dz):
    return np.sqrt((x*y*dz)**2+(x*z*dy)**2+(y*z)**2)

def simulazione(length1,length2, width,height):
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
        theta = np.arccos(rng.uniform(0, 1)**(1/4))  # distribuito come cos2(θ), cos in più considerando che non è sempre perpendicolare (non tutte le sorgenti
        
        # Proiezione del fascio sul secondo scintillatore
        x2 = x1 - height * np.tan(theta) * np.cos(phi)
        y2 = y1 - height * np.tan(theta) * np.sin(phi)
                 
        # Verifica se x2 e y2 sono compresi nelle dimensioni del secondo scintillatore
        if 0 <= x2 <= width and 0 <= y2 <= length2:
            successi += 1
            
        tentativi += 1
    
    # Calcola l'efficienza geometrica
    efficienza_geometrica = successi / tentativi
    # Calcola l'errore come una semplice distribuzione binomiale
    errore = np.sqrt(efficienza_geometrica * (1 - efficienza_geometrica) / tentativi)
    return efficienza_geometrica, errore

def simulazione_3(length1,length2, length3,width,height1,height2):
    """Accettanza geometrica per 3 scintillatori sovrapposti con 3 grandezze diverse."""
    n = 10000 #Numero di tentativi
    # Inizializza contatori
    successi = 0
    tentativi = 0
    
    # Generatore di numeri casuali
    rng = np.random.default_rng() #random seed
    theta_dist=[]
    for i in range(1000):
        theta = np.arccos(rng.uniform(0, 1)**(1/4))
        theta_dist=np.append(theta_dist,theta)
    plt.hist(np.cos(theta_dist))
    
    for _ in range(n):
        # Genera coordinate x1 e y1 sul primo scintillatore
        x1 = rng.uniform(0, width)
        y1 = rng.uniform(0, length1)
        
        # Genera angoli ϕ e θ
        phi = rng.uniform(0, 2 * np.pi)
        theta = np.arccos(rng.uniform(0, 1)**(1/4))  # distribuito come cos2(θ), cos in più considerando che non è sempre perpendicolare (non tutte le sorgenti
        
        # Proiezione del fascio sul secondo scintillatore
        x2 = x1 - height1 * np.tan(theta) * np.cos(phi)
        y2 = y1 - height1 * np.tan(theta) * np.sin(phi)

        x3 = x2 - height2 * np.tan(theta) * np.cos(phi)
        y3 = y2 - height2 * np.tan(theta) * np.sin(phi)
                 
        # Verifica se x2 e y2 sono compresi nelle dimensioni del secondo scintillatore
        if 0 <= x2 <= width and 0 <= y2 <= length2:
            if 0 <= x3 <= width and 0 <= y3 <= length3:
                successi += 1
            
        tentativi += 1
    
    # Calcola l'efficienza geometrica
    efficienza_geometrica = successi / tentativi
    # Calcola l'errore come una semplice distribuzione binomiale
    errore = np.sqrt(efficienza_geometrica * (1 - efficienza_geometrica) / tentativi)
    return efficienza_geometrica, errore

def simulazione_fatt_geom(length1, length2, length3, width,height1, height2):
    """Nel passare da 3 a 2 scintillatori sovrapposti, si deve tenere conto per passare dal conteggio di triple su doppie 
    alla vera efficienza va considerata la probabilità condizionata che data una doccia ci sia anche una tripla.
    In questo programma 1 e 2 rappresentano in PMT in doppia, mentre 3 è il restante PMT per le triple.
    La height è da considerarsi positiva se va dal basso verso l'alto."""
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
        theta = np.arccos(rng.uniform(0, 1)**(1/4))  # distribuito come cos2(θ)
        
        # Proiezione del fascio sul secondo scintillatore
        x2 = x1 - height1 * np.tan(theta) * np.cos(phi) #height positivo se vado in basso
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

#Setup08
length1=60.0
length2=39.0
dlength=0.1
width=20.0
dwidth=0.5

aeff=length2*width #area di sovrapposizione
daeff=errore_prodotto(length2,width,dlength,dwidth)

print(f"Inizio Setup08\n----------\nArea efficace {aeff:.0f} ± {daeff:.0f} cm^2")

triple_doppie1=0.88
errore1=0.06
triple_doppie2=0.86
errore2=0.05

fattore_geom1, errore_fattore1=simulazione_fatt_geom(length2,length2,length1, width, -2.5,4.5) #ricorda la distanza può essere definita negativa o positiva
print(f'Fattore correttivo per 1: {fattore_geom1}')

epsilon1=triple_doppie1*1/fattore_geom1
print(f"Vera efficienza PMT1 {epsilon1}")
depsilon1=errore_prodotto(triple_doppie1,fattore_geom1,errore1,errore_fattore1)

fattore_geom2, errore_fattore2=simulazione_fatt_geom(length1,length2,length2, width, -4.5,2.5)
print(f'Fattore correttivo per 2: {fattore_geom2}')

epsilon2=triple_doppie2*1/fattore_geom2
print(f"Vera efficienza PMT2 {epsilon2}")
depsilon2=errore_prodotto(triple_doppie2,fattore_geom2,errore2,errore_fattore2)

epsilon=epsilon1*epsilon2
depsilon=errore_prodotto(epsilon1,epsilon2,depsilon1,depsilon2)

print(f"Efficienza non corretta per l'accettanza: {epsilon:.2f} ± {depsilon:.2f} cm^2")

geom_eff, err_geom=simulazione(length1,length2,width,2)
print(f"Efficienza geometrica: {geom_eff:.2f}  ± {depsilon:.2f}")
epsilon_eff=epsilon*geom_eff
depsilon_eff=errore_prodotto(epsilon,geom_eff,depsilon,err_geom)
print(f"Efficienza corretta per l'accettanza: {epsilon_eff:.2f} ± {depsilon_eff:.2f} cm^2")

rate_cosmic1=cosmic*aeff*1/60
event_doppie=epsilon_eff*rate_cosmic1*10
print(f"Eventi doppie {event_doppie},  rate Setup08: {epsilon_eff*rate_cosmic1}")

# Stanza 2004
length=41.0
dlength=0.5
width=49.0
dwidth=0.5
height=20
dheight=0.1 #sono tutti esattamente uguali

aeff=length*width
daeff=errore_prodotto(length,width,dlength,dwidth)

print(f"Inizio Stanza2004\n----------\nArea efficace {aeff:.0f} ± {daeff:.0f} cm^2")

# Decidiamo che dato che si trovano in alto, mettiamo un errore di mezzo centimetro, molto più grande della risoluzione del metro a nastro

triple_doppie1=0.63
errore1=0.02
triple_doppie2=0.96
errore2=0.02
triple_doppie3=0.53
errore3=0.04

fattore_geom1,errore_fattore1=simulazione_fatt_geom(length,length,length,width,-20,40)
print(f'Fattore correttivo per 1: {fattore_geom1}')
epsilon1=triple_doppie1*1/fattore_geom1
print(f"Vera efficienza PMT1 {epsilon1}")
depsilon1=errore_prodotto(triple_doppie1,fattore_geom1,errore1,errore_fattore1)

fattore_geom2, errore_fattore2=simulazione_fatt_geom(length,length,length, width, -40,20)
print(f'Fattore correttivo per 2: {fattore_geom2}')
epsilon2=triple_doppie2*1/fattore_geom2
print(f"Vera efficienza PMT2 {epsilon2}")
depsilon2=errore_prodotto(triple_doppie2,fattore_geom2,errore2,errore_fattore2)

fattore_geom3, errore_fattore3=simulazione_fatt_geom(length,length,length, width, -20,-20)
print(f'Fattore correttivo per 3: {fattore_geom3}')
epsilon3=triple_doppie3*1/fattore_geom3
print(f"Vera efficienza PMT3 {epsilon3}")
depsilon3=errore_prodotto(triple_doppie3,fattore_geom3,errore3,errore_fattore3)

Epsilon=epsilon1*epsilon2*epsilon3
dEpsilon=errore_prodotto3(epsilon1,depsilon1,epsilon2,depsilon2,epsilon3,depsilon3)
print(f"Efficienza non corretta per l'accettanza: {Epsilon:.2f} ± {dEpsilon:.2f} cm^2")

rate_cosmic2=cosmic*aeff*1/60
print(rate_cosmic2)

geom_eff, err_geom=simulazione_3(length,length,length,width,height,height)
print(f"Efficienza geometrica: {geom_eff:.2f}  ± {dEpsilon:.2f}")
epsilon=Epsilon*geom_eff
depsilon=errore_prodotto(Epsilon,geom_eff,dEpsilon,err_geom)
print(f"Efficienza corretta per l'accettanza: {epsilon:.2f} ± {depsilon:.2f} cm^2")

event_triple=epsilon*rate_cosmic2*10
print(f"Eventi triple {event_triple},  rate Stanza2004: {epsilon*rate_cosmic2}")

#Setup06
length1=35.0
length2=41.0
dlength=0.1
width=20.0
dwidth=0.1
distance=3.5

aeff=length1*width
daeff=errore_prodotto(length1,width,dlength,dwidth)

print(f"Inizio Setup06\n----------\nArea efficace {aeff:.0f} ± {daeff:.0f} cm^2")

triple_doppie1=0.88
errore1=0.06
triple_doppie3=0.86
errore3=0.05

fattore_geom1, errore_fattore1=simulazione_fatt_geom(length1,length2,length1, width, -2,4.5) #ricorda la distanza può essere definita negativa o positiva
print(f'Fattore correttivo per 1: {fattore_geom1}')

epsilon1=triple_doppie1*1/fattore_geom1
print(f"Vera efficienza PMT1 {epsilon1}")
depsilon1=errore_prodotto(triple_doppie1,fattore_geom1,errore1,errore_fattore1)

fattore_geom3, errore_fattore3=simulazione_fatt_geom(length1,length1,length2, width, -3.5,-2)
print(f'Fattore correttivo per 3: {fattore_geom3}')
epsilon3=triple_doppie3*1/fattore_geom3
print(f"Vera efficienza PMT3 {epsilon3}")
depsilon3=errore_prodotto(triple_doppie3,fattore_geom3,errore3,errore_fattore3)

Epsilon=epsilon1*epsilon3
dEpsilon=errore_prodotto(epsilon1,depsilon1,epsilon3,depsilon3)
print(f"Efficienza non corretta per l'accettanza: {Epsilon:.2f} ± {dEpsilon:.2f} cm^2")

rate_cosmic3=cosmic*aeff*1/60
print(rate_cosmic3)

geom_eff, err_geom=simulazione(length1,length2,width,5.5)
print(f"Efficienza geometrica: {geom_eff:.2f}  ± {dEpsilon:.2f}")
epsilon=Epsilon*geom_eff
depsilon=errore_prodotto(Epsilon,geom_eff,dEpsilon,err_geom)
print(f"Efficienza corretta per l'accettanza: {epsilon:.2f} ± {depsilon:.2f} cm^2")

event_triple=epsilon*rate_cosmic3*10
print(f"Eventi doppie {event_doppie},  rate Setup06: {epsilon*rate_cosmic3}")

# Coincidenze tra questi due telescopi??
#Va valutato Dtcoinc dalla scheda De0Nano