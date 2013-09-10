#!/usr/bin/env bash

WHERE=`pwd`
rm -r ~/.local/dynamite
#EXTRAS="-DPROFILE" INSTALLDIR="~/.local" DEBUG="" make clean install
INSTALLDIR="~/.local" DEBUG="" make clean install
#cd ~/Desktop/benchpress/
#./press.py --suite dynamite --output /tmp/ --runs 2 ../bohrium/
cd $WHERE

python ../../test/numpy/numpytest.py
#python ../../test/numpy/numpytest.py -f test_array_create.py
#python ../../test/numpy/numpytest.py -f test_benchmarks.py
#python ../../test/numpy/numpytest.py -f test_matmul.py
#python ../../test/numpy/numpytest.py -f test_primitives.py
#python ../../test/numpy/numpytest.py -f test_specials.py
#python ../../test/numpy/numpytest.py -f test_types.py
#python ../../test/numpy/numpytest.py -f test_views.py
