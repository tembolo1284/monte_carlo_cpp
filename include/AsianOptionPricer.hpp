#ifndef AsianOptionPricer_HPP
#define AsianOptionPricer_HPP
#include <functional>
#include <vector>
#include "Pricer.hpp"
#include <cmath>
class AsianOptionPricer : public Pricer {

private:
    int NSim;
    double px;
    double sum;
    double payoffT;
    double squaredPayoff;

public:
    //EuropeanOptionPricer() {}

    AsianOptionPricer(std::function<double(double)>& po, std::function<double()>& dis) : Pricer(po, dis) {
        NSim = 0;
        px = 0.0;
        sum = 0.0;
        payoffT = 0.0;
        squaredPayoff = 0.0;
    }

    double DiscountFactor() override {
        return m_discount();
    }

    double OptionPrice() override {
        return px;
    }
    std::tuple<double, double> StandardDeviationStats() {
        double M = static_cast<double>(NSim);
        double SD = std::sqrt((squaredPayoff / M) - sum * sum / (M * M));
        double SE = SD / std::sqrt(M);
        std::tuple<double, double> SDS = std::make_tuple(SD, SE);
        return SDS;
    }

    //method to churn out a single iteration/path
    void GeneratePath(std::vector<double>& vec) {
        //sum += m_payoffFunction(vec[vec.size() - 1]);// this is sum of values at maturity T //old euro option way
        double pxSum{ 0.0 }; //quick and dirty taking average of entire vector god bless math
        for (int i = 0; i < vec.size() - 1; i++) {
            pxSum += vec[i];
        }
        double avgPx = pxSum / vec.size() - 1;
        sum += m_payoffFunction(avgPx);// asian option we take an average of all values in the vector and use that
        payoffT = m_payoffFunction(avgPx);
        squaredPayoff += (payoffT * payoffT);
        NSim++;
    }
    void AfterPathCleanUp() {
        px = (DiscountFactor() * sum) / NSim; //price after all done
    }

};

//#ifndef AsianOptionPricer_cpp // Must be the same name as in source file #define
//#include "AsianOptionPricer.cpp"
//#endif

#endif
