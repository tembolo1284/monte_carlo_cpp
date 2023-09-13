#ifndef Pricer_HPP
#define Pricer_HPP
#include <functional>
#include <vector>
#include <tuple>
class Pricer {

public:
    //abstract class. Most likely will just focus on euro style option derived class

    //method to churn out a single iteration/path
    virtual void GeneratePath(std::vector<double>& vec) = 0;
    virtual void AfterPathCleanUp() = 0;

    virtual double DiscountFactor() = 0;
    std::function<double()> m_discount;

    virtual double OptionPrice() = 0;
    virtual std::tuple<double, double> StandardDeviationStats() = 0;
    std::function<double(double)> m_payoffFunction;

    Pricer() {}
    Pricer(std::function<double(double)>& po, std::function<double()>& dis) {
        m_payoffFunction = po;
        m_discount = dis;
    }
    ~Pricer() {}
    
};

//#ifndef Pricer_cpp // Must be the same name as in source file #define
//#include "Pricer.cpp"
//#endif

#endif
