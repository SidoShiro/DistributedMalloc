if [[ $# -eq 1 ]] ; then
  mpirun --hostfile hostfile -n $1 build/dmalloc
else
  mpirun --hostfile hostfile -n 20 build/dmalloc
fi
