#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <zlib.h>
#include <omp.h>
#include <getopt.h>
#include <map>
#include <unordered_map>
#include "mkmh.hpp"
#include "kseq.hpp"
#include "equiv.hpp"

KSEQ_INIT(gzFile, gzread)

    using namespace std;
    using namespace mkmh;
    void print_help(char** argv){
        cerr << "Usage: " << argv[0] << " [options]" << endl
            << "Options:" << endl
            << "--reads/-r   <READFILE>" << endl
            << "--fasta/-f   <FASTAFILE>" << endl
            << "--kmer/-k    <KMERSIZE>" << endl
            << "--minhash/-h <HASHSIZE>" << endl
            << "--threads/-t <THREADS>" << endl
            << "--min-kmer-occurence/-M <MINOCCURENCE>" << endl;
    }


int main(int argc, char** argv){
    char* ref_file;
    char* read_file;
    vector<char*> ref_files;
    vector<char*> read_files;
    vector<int> kmer;
    int sketch_size = -1;
    int threads = 1;
    int min_kmer_occ = 0;

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
            {"threads", required_argument, 0, 't'},
            {"min-kmer-occurence", required_argument, 0, 'M'},
            {0,0,0,0}
        };

        int option_index = 0;
        c = getopt_long(argc, argv, "hm:k:r:f:t:M:", long_options, &option_index);
        if (c == -1){
            break;
        }

        switch (c){
            case 't':
                threads = atoi(optarg);
                break;
            case 'f':
                //ref_file = optarg;
                ref_files.push_back(optarg);
                break;
            case 'r':
                //read_file = optarg;
                read_files.push_back(optarg);
                break;
            case 'k':
                kmer.push_back(atoi(optarg));
                break;
            case '?':
            case 'h':
                print_help(argv);
                exit(1);
                break;
            case 'm':
                sketch_size = atoi(optarg);
                break;
            case 'M':
                min_kmer_occ = atoi(optarg);
                break;
            default:
                print_help(argv);
                abort();

        }
    }


    map<string, string> ref_to_seq;
    map<string, vector<int64_t> > ref_to_hashes;
    map<string, vector<string> > ref_to_kmers;

    map<string, string> read_to_seq;
    map<string, vector<int64_t> > read_to_hashes;
    map<string, vector<string> > read_to_kmers;

    unordered_map<string, int> kmer_to_depth;
    unordered_map<int64_t, int> hash_to_depth;


    omp_set_num_threads(threads);
    // Read in fastas
    gzFile fp;
    kseq_t *seq;
    int l;

    if (ref_files.size() > 0){
        for (auto f : ref_files){
            fp = gzopen(f, "r");
            seq = kseq_init(fp);
            // Read in reads, cluster, spit it back out
            while ((l = kseq_read(seq)) >= 0) {
                ref_to_seq[seq->name.s] = to_upper(seq->seq.s);
                //ref_seq.push_back(std::make_pair(seq->name.s, to_upper(seq->seq.s)));
            } 
        }

        cerr << "Loaded " << ref_to_seq.size() << " reference sequences." << endl;
    }
    else{
        cerr << "Please provide a fasta file containing references." << endl;
        exit(1);
    }


    if (read_files.size() > 0){
        for (auto f : read_files){
            fp = gzopen(f, "r");
            seq = kseq_init(fp);
            while ((l = kseq_read(seq)) >= 0) {
                read_to_seq[seq->name.s] = to_upper(seq->seq.s);
                //read_seq.push_back(make_pair(seq->name.s, seq->seq.s));
            }
        }

        cerr << "Loaded " << read_to_seq.size() << " reads." << endl;
    }
    else{
        cerr << "Please provide a read file containing query sequences." << endl;
        exit(1);
    }

    vector<pair<string, string > > ref_seq(ref_to_seq.begin(), ref_to_seq.end());
    vector<pair<string, string> > read_seq(read_to_seq.begin(), read_to_seq.end());
    

    if (min_kmer_occ > 0){
        // fill in kmer_to_depth;
        #pragma omp parallel for
        for (int i = 0; i < read_seq.size(); i++){
            vector<int64_t> rhash = allhash_unsorted_64(read_seq[i].second, kmer);
            read_to_hashes[read_seq[i].first] = rhash;
            for (auto j : rhash){
                hash_to_depth[j] += 1;
            }
        }
    }


    if (sketch_size > 0){
        cerr << "Making reference sketches..." << endl;

        #pragma omp parallel for
        for (int i = 0; i < ref_seq.size(); i++){
            ref_to_hashes[ref_seq[i].first] = minhash_64(ref_seq[i].second, kmer, sketch_size, true);
        }
        cerr << "Processed " << ref_to_hashes.size() << " references to MinHashes" << endl;

        #pragma omp parallel for
        for (int i = 0; i < read_seq.size(); i++){
            stringstream outre;
            vector<int64_t> hashes;
            if (min_kmer_occ > 0){
                hashes = minhash_64_depth_filter(read_to_hashes[read_seq[i].first], sketch_size, true, min_kmer_occ, hash_to_depth);

                cerr << hashes.size() << endl;
            }
            else{
                hashes = minhash_64(read_seq[i].second, kmer, sketch_size, true);
            }
            tuple<string, int, int> result;
            result = classify_and_count(hashes, ref_to_hashes);
            //#pragma omp critical
            outre  << "Sample: " << read_seq[i].first << "\t" << "Result: " << 
                std::get<0>(result) << "\t" << std::get<1>(result) << "\t" << std::get<2>(result) << endl;   
            cout << outre.str();
        }
    }

    else{
        cerr << "Performing direct kmer-based comparison." << endl;
        map<string, string>::iterator itersk;
        for (itersk = ref_to_seq.begin(); itersk != ref_to_seq.end(); itersk++){
            ref_to_kmers[itersk->first] = multi_kmerize(itersk->second, kmer);

            std::sort(ref_to_kmers[itersk->first].begin(), ref_to_kmers[itersk->first].end());
        }

        cerr << "Processed " << ref_to_kmers.size() << " references to kmers." << endl;

        for (itersk = read_to_seq.begin(); itersk != read_to_seq.end(); itersk++){
            read_to_kmers[itersk->first] = multi_kmerize(itersk->second, kmer);
            std::sort(read_to_kmers[itersk->first].begin(), read_to_kmers[itersk->first].end());

            tuple<string, int, int> result = kmer_classify(read_to_kmers[itersk->first], ref_to_kmers);
            cout  << "Sample: " << itersk->first << "\t"
                << "Result: " << std::get<0>(result) << "\t" << std::get<1>(result) << "\t" << std::get<2>(result) << endl;


        }

        //cerr << "Processed " << read_to_kmers.size() << " reads to kmers." << endl;


    }








    return 1;
}