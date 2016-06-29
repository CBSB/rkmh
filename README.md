RKMH: Read classification by Kmers or MinHash
--------------------------------------------
Eric T Dawson  
June 2016

### What is it
rkmh performs MinHash (as implemented in [Mash](https://github.com/marbl/Mash)) on a set of reads
and reference genomes to classify each read in the set to its nearest match in the reference set.
It can also classify reads by directly comparing all of their kmers.


On a set of 1000 minION reads from a known HPV strain, rkmh is ~97% accurate (correctly placing the read in the right strain
of 182 input reference strains) and runs in <30 seconds. We're working on ways to improve sensitivity with filtering and correction.

### Dependencies and build process
The only external dependencies should be zlib and a compiler supporting OpenMP. To download and build:  

                    git clone --recursive https://github.com/edawson/rkmh.git  
                    cd rkmh  
                    make  

This should build rkmh and its library dependencies (mkmh and murmur3).

### Usage
To classify reads by kmers of length 10:  
```./rkmh -f references.fa -r reads.fq -k 10```

To do the same, but use a MinHash sketch of size 1000 instead of just comparing kmers:  
```./rkmh -f references.fa -r reads.fq -k 10 -m 1000```

### Other options
```-t / --threads   number of OpenMP threads to use (default is 1)```  

### Getting help
Please post to the [github](https://github.com/edawson/rkmh.git) for help.
