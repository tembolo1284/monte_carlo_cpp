// EuropeanOptionPricer.hpp
#ifndef EuropeanOptionPricer_HPP
#define EuropeanOptionPricer_HPP

#include <functional>
#include <vector>
#include "Pricer.hpp"

class EuropeanOptionPricer : public Pricer {
public:
    EuropeanOptionPricer(std::function<double(double)>& po, std::function<double()>& dis)
        : Pricer(po, dis)
    {}

    void GeneratePath(const std::vector<double>& vec) override {
        const double payoff = m_payoffFunction(vec.back());
        updateStats(payoff);
    }

    void AfterPathCleanUp() override {}
};

#endif
