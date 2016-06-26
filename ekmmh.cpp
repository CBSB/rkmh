#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <zlib.h>
#include "kseq.hpp"
#include <map>
#include "mkmh.hpp"
#include "equiv.hpp"
#include <getopt.h>

KSEQ_INIT(gzFile, gzread)

    using namespace std;
    using namespace mkmh;
    void print_help(char** argv){
        cerr << "Usage: " << argv[0] << " [options]" << endl
            << "Options:" << endl
            << "--reads/-r   <READFILE>" << endl
            << "--fasta/-f   <FASTAFILE>" << endl
            << "--kmer/-k    <KMERSIZE>" << endl
            << "--minhash/-h <HASHSIZE>" << endl;
    }


int main(int argc, char** argv){
    char* ref_file;
    string read_file = "";
    int kmer = 0;
    int sketch_size = 0;

    int c;
    if (argc < 2){
        print_help(argv);
        exit(1);
    }

    while (true){
        static struct option long_options[] =
        {
            {"help", no_argument, 0, 'h'},
            {"kmer", no_argument, 0, 'k'},
            {"reads", required_argument, 0, 'r'},
            {"fasta", required_argument, 0, 'f'},
            {"minhash", required_argument, 0, 'm'},
            {0,0,0,0}
        };

        int option_index = 0;
        c = getopt_long(argc, argv, "hm:k:r:f:", long_options, &option_index);
        if (c == -1){
            break;
        }

        switch (c){
            case 'f':
                ref_file = optarg;
                break;
            case 'r':
                read_file = optarg;
                break;
            case 'k':
                kmer = atoi(optarg);
                break;
            case '?':
            case 'h':
                print_help(argv);
                exit(1);
                break;
            case 'm':
                sketch_size = atoi(optarg);
                break;
            default:
                print_help(argv);
                abort();

        }
    }
    map<string, vector<string> > kmer_to_samples;
    map<string, vector<string> > sample_to_kmers;
    map<int64_t, vector<string> > hash_to_samples;
    map<string, vector<int64_t> > sample_to_hashes;
    map<string, string> name_to_seq;
    // Read in fastas
    gzFile fp;
    kseq_t *seq;
    int l;
    fp = gzopen(ref_file, "r");
    seq = kseq_init(fp);
    // Read in reads, cluster, spit it back out
    while ((l = kseq_read(seq)) >= 0) {
        name_to_seq[seq->name.s] = seq->seq.s;
        //printf("name: %s\n", seq->name.s);
        //if (seq->comment.l) printf("comment: %s\n", seq->comment.s);
        //printf("seq: %s\n", seq->seq.s);
        //if (seq->qual.l) printf("qual: %s\n", seq->qual.s);
    } 

    cerr << "Loaded " << name_to_seq.size() << " sequences." << endl;

    sample_to_kmers = make_sample_to_kmers(name_to_seq, 12);

    map<string, string>::iterator itersk;
    for (itersk =
    /*for (iter = sample_to_kmers.begin(); iter != sample_to_kmers.end(); iter++){
        cout << iter->first << endl;
        cout << (iter->second).size() << endl;
    }*/
    //kmer_to_samples = make_kmer_to_samples(sample_to_kmers);







    // Kmerize sample to make kmer to samples and sample to kmers

    // For read in readset:
    // kmerize read
    // look up kmers in kmer_to_sample
    // if a sufficient number match and a dominant equiv class forms then
    // return the sample name.
    return 1;
}
