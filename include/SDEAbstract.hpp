#ifndef SDEAbstract_HPP
#define SDEAbstract_HPP
// Interface contract specification
#include <concepts>
#include <memory>
#include "OptionData.hpp"

class SDEAbstract { 

//private:
    
public:
    
    std::shared_ptr<OptionData> data;

    SDEAbstract(const OptionData& optionData) : data(new OptionData(optionData)) {}
    //interface functions that need to be impl in all derived classes
    virtual double diffusion(double t, double x) const = 0;
    virtual double drift(double t, double x) const = 0;
    virtual double diffusionDerivative(double t, double x) const;
    virtual double driftCorrected(double t, double x, double B) const;

    //virtual double jump(double t, double x) const = 0;
};

//#ifndef SDEAbstract_cpp // Must be the same name as in source file #define
//#include "SDEAbstract.cpp"
//#endif
#endif
