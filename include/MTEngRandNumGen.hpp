#ifndef MTEngRandNumGen_HPP
#define MTEngRandNumGen_HPP
#include <random>

class MTEngRandNumGen {

public:
        std::mt19937 dre;
        std::normal_distribution<double> norm; //took this from previous exercises prof duffy
        MTEngRandNumGen() : dre(std::mt19937()), norm(std::normal_distribution<double>(0.0, 1.0)) {}

        double GenerateRandNum() {
            return norm(dre);
        }
};

//#ifndef MTEngRandNumGen_cpp // Must be the same name as in source file #define
//#include "MTEngRandNumGen.cpp"
//#endif

#endif
