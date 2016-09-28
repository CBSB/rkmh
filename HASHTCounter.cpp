#include "HASHTCounter.hpp"

namespace HTC{ 
    using namespace HTC;
    using namespace std;

    HASHTCounter::HASHTCounter(){
        my_size = 1000000;
        counts = new int [my_size];
    }

    HASHTCounter::HASHTCounter(int sz){
        my_size = sz;
        counts = new int [my_size];
    }

    HASHTCounter::~HASHTCounter(){
        delete [] counts;
        my_size = 0;
    }

    void HASHTCounter::increment(htc_type key){
        //cout << (++counts [ key % my_size ]) << endl;
        #pragma omp atomic update
        ++(counts[ key % my_size ]);
    }

    int& HASHTCounter::get(htc_type key){
        return (counts[ key % my_size ]);
    }

    int HASHTCounter::size(void){
        return my_size;
    }

    void HASHTCounter::size(int sz){
        my_size = sz;
        counts = new int [my_size];
    }

    int& HASHTCounter::operator[](htc_type key){
        //value_t& operator[](std::size_t idx)       { return mVector[idx]; }
        //const value_t& operator[](std::size_t idx) const { return mVector[idx]; }
        return (counts [ key % my_size ]);
    }

    int* HASHTCounter::begin(void){
        return counts;
    }
}
