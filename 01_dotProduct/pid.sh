#!/bin/bash

# Avvia il programma `./omp-dot` con affinità sui core 0-11
sleep 5 && taskset -c 0-11 ./omp-dot &
pid=$!

# Attendi un attimo e verifica l'affinità del processo appena lanciato
sleep 1
taskset -cp $pid
