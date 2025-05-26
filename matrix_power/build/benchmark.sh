#!/usr/bin/env bash
set -euo pipefail

bins=(power_repeat_serial power_fast power_strassen)
mpi_bin=power_mpi
ns=(128 256 512)
ps=(10 100)
procs=(1 2 4 8)

echo "method,n,p,procs,time_s" > results.csv

# 1) Metodele seriale (uses stdin)
for method in "${bins[@]}"; do
  for n in "${ns[@]}"; do
    for p in "${ps[@]}"; do
      infile="../data/input_${n}_${p}.txt"
      echo ">>> $method | n=$n p=$p procs=1"
      start=$(date +%s.%N)
      "./$method" < "$infile" >/dev/null
      end=$(date +%s.%N)
      dt=$(awk "BEGIN {print $end - $start}")
      echo "$method,$n,$p,1,$dt" >> results.csv
    done
  done
done

# 2) Metoda MPI, fără stdin-redir; fișierul e argument
for n in "${ns[@]}"; do
  for p in "${ps[@]}"; do
    infile="../data/input_${n}_${p}.txt"
    for proc in "${procs[@]}"; do
      echo ">>> $mpi_bin | n=$n p=$p procs=$proc"
      start=$(date +%s.%N)
      mpirun -np "$proc" "./$mpi_bin" "$infile" >/dev/null
      end=$(date +%s.%N)
      dt=$(awk "BEGIN {print $end - $start}")
      echo "$mpi_bin,$n,$p,$proc,$dt" >> results.csv
    done
  done
done
