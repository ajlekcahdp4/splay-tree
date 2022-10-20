# Splay tree implementation on C++
My goal is to create a splay tree that would be more efficient in the tasks of finding the number of elements in the given range## How to compile
```sh
cmake -S . -B build -DNOGTEST=FALSE -DCOMPARE=FALSE
make -C build -j12 install
```
There I set NOGTEST to FALSE to enable unit-tests and COMPARE to FALSE to disable comparation with std::set. 
 
## How to run
```sh
# to run all the tests:
cd build
ctest
# to run end2end test:
cd test/queries
bin/queries < resources/test0.dat
# to run only comparison:
cd test/benchmark
bin/benchmark < resources/test0.dat
# to compare on all the tests:
cd test/benchmark
./test.sh
```
## Results
If you want to see the results of comparison with std::set in the tasks of finding kth smallest elemet in a set and finding the number of elements smaller then the given one you can check this [file](results/compared.dat) or enable comparison and test it again..

