// AsianOptionPricer.hpp
#ifndef AsianOptionPricer_HPP
#define AsianOptionPricer_HPP

#include <functional>
#include <vector>
#include <numeric>
#include "Pricer.hpp"

class AsianOptionPricer : public Pricer {
public:
    AsianOptionPricer(std::function<double(double)>& po, std::function<double()>& dis)
        : Pricer(po, dis)
    {}

    void GeneratePath(const std::vector<double>& vec) override {
        const double sum = std::accumulate(vec.begin(), vec.end() - 1, 0.0);
        const double avg = sum / (vec.size() - 1);
        const double payoff = m_payoffFunction(avg);
        updateStats(payoff);
    }

    void AfterPathCleanUp() override {}
};

#endif
