#ifndef MTEngRandNumGen_HPP
#define MTEngRandNumGen_HPP

#include <random>

class MTEngRandNumGen {
private:
    std::mt19937 dre;
    std::normal_distribution<double> norm;
    
public:
    MTEngRandNumGen() 
        : dre(std::random_device{}())
        , norm(0.0, 1.0) 
    {}
    
    double GenerateRandNum() {
        return norm(dre);
    }
};

#endif
