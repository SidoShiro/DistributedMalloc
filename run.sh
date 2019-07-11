mkdir build/
rm -rf build/*
cd build
cmake ..
make -j 6
cd ..

if [[ $# -eq 1 ]] ; then
  mpirun --hostfile hostfile -n $1 build/dmalloc
else
  mpirun --hostfile hostfile -n 20 build/dmalloc
fi
