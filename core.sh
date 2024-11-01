#!/bin/bash

# Configurazione per i7-12900H usando tutti i thread dei P-core
# P-cores fisici: 0-5
# P-cores thread virtuali: 6-11
# E-cores: 12-19 (non utilizzati in questa configurazione)

# Imposta le variabili d'ambiente per OpenMP
export OMP_PLACES="{0:1},{1:1},{2:1},{3:1},{4:1},{5:1},{6:1},{7:1},{8:1},{9:1},{10:1},{11:1}"
export OMP_PROC_BIND=true
export OMP_NUM_THREADS=12

# Funzione per verificare la configurazione
check_config() {
    echo "Configurazione OpenMP corrente:"
    echo "OMP_PLACES=$OMP_PLACES"
    echo "OMP_PROC_BIND=$OMP_PROC_BIND"
    echo "OMP_NUM_THREADS=$OMP_NUM_THREADS"

    echo -e "\nQuesta configurazione utilizza:"
    echo "- P-core fisici (0-5)"
    echo "- Thread virtuali dei P-core (6-11)"
    echo "- Totale thread: 12"
}

# Esegui la verifica
check_config
