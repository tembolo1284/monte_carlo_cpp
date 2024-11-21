#ifndef Pricer_HPP
#define Pricer_HPP

#include <functional>
#include <vector>
#include <tuple>
#include <mutex>

class Pricer {
protected:
    std::mutex mtx;
    double sum{0.0};
    double squaredSum{0.0};
    int count{0};
    std::function<double(double)> m_payoffFunction;
    std::function<double()> m_discount;

public:
    Pricer() = default;
    
    Pricer(std::function<double(double)>& po, std::function<double()>& dis)
        : m_payoffFunction(po)
        , m_discount(dis)
    {}

    virtual ~Pricer() = default;

    virtual void GeneratePath(const std::vector<double>& vec) = 0;
    virtual void AfterPathCleanUp() = 0;
    
    double DiscountFactor() {
        return m_discount();
    }

    double OptionPrice() {
        if (count == 0) return 0.0;
        return (DiscountFactor() * sum) / count;
    }

    std::tuple<double, double> StandardDeviationStats() {
        if (count < 2) return {0.0, 0.0};

        const double M = static_cast<double>(count);
        const double SD = std::sqrt((squaredSum / M) - (sum * sum) / (M * M));
        const double SE = SD / std::sqrt(M);
        
        return {SD, SE};
    }

protected:
    void updateStats(double payoff) {
        std::lock_guard<std::mutex> lock(mtx);
        sum += payoff;
        squaredSum += payoff * payoff;
        count++;
    }
};

#endif
