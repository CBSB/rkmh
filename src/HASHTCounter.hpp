
#ifndef HTC_HPP
#define HTC_HPP
#include <cstdint>
#include <iostream>
#include <omp.h>
#include <cstdio>
#include <sstream>


namespace HTC{
    typedef uint64_t htc_type;
class HASHTCounter{

    public:
        HASHTCounter();
        HASHTCounter(int sz);
        ~HASHTCounter();
        int& operator[](htc_type key);

        void increment(htc_type key);
        int& get(htc_type key);

        void get(htc_type key, int& ret);

        int size(void);
        void size(int sz);
        void resize(int sz);
        inline void set(int pos, int val){
            *(counts + pos) = val;
        };

        int* begin(void);

        std::string to_string();
        
    private:
        int my_size;
        int* counts;
        std::string my_big_string;
        
};
}
#endif
