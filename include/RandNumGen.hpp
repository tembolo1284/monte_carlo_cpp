#ifndef RandNumGen_HPP
#define RandNumGen_HPP
class RandNumGen {

public:
    //abstract class for rand num generators
    virtual double GenerateRandNum() = 0;
};

//#ifndef RandNumGen_cpp // Must be the same name as in source file #define
//#include "RandNumGen.cpp"
//#endif

#endif
