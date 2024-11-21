#ifndef OptionPricers_HPP
#define OptionPricers_HPP

#include <functional>
#include <vector>
#include <cmath>
#include <numeric>
#include "Pricer.hpp"

// Common base implementation for shared functionality
class PricerBase : public Pricer {
protected:
    alignas(64) struct Stats {  // Aligned for SIMD
        double sum{0.0};
        double squaredSum{0.0};
        int count{0};
        double price{0.0};
    };
    
    // Thread-local statistics
    std::vector<Stats> threadStats;
    
    PricerBase(std::function<double(double)>& po, std::function<double()>& dis)
        : Pricer(po, dis)
        , threadStats(omp_get_max_threads())
    {}

    void updateStats(double payoff, int threadId) {
        auto& stats = threadStats[threadId];
        stats.sum += payoff;
        stats.squaredSum += payoff * payoff;
        stats.count++;
    }

public:
    double OptionPrice() override {
        return std::accumulate(threadStats.begin(), threadStats.end(), 0.0,
            [](double sum, const Stats& stats) { return sum + stats.price; });
    }

    std::tuple<double, double> StandardDeviationStats() {
        double totalSum = 0.0;
        double totalSquaredSum = 0.0;
        int totalCount = 0;

        for (const auto& stats : threadStats) {
            totalSum += stats.sum;
            totalSquaredSum += stats.squaredSum;
            totalCount += stats.count;
        }

        const double M = static_cast<double>(totalCount);
        const double SD = std::sqrt((totalSquaredSum / M) - (totalSum * totalSum) / (M * M));
        const double SE = SD / std::sqrt(M);
        
        return {SD, SE};
    }

    void AfterPathCleanUp() {
        const double discountFactor = m_discount();
        #pragma omp parallel for
        for (size_t i = 0; i < threadStats.size(); ++i) {
            auto& stats = threadStats[i];
            if (stats.count > 0) {
                stats.price = (discountFactor * stats.sum) / stats.count;
            }
        }
    }
};

class EuropeanOptionPricer : public PricerBase {
public:
    EuropeanOptionPricer(std::function<double(double)>& po, std::function<double()>& dis)
        : PricerBase(po, dis) {}

    void GeneratePath(std::vector<double>& vec) {
        const int threadId = omp_get_thread_num();
        const double payoff = m_payoffFunction(vec.back());
        updateStats(payoff, threadId);
    }
};

class AsianOptionPricer : public PricerBase {
private:
    static constexpr int SIMD_WIDTH = 8;  // AVX2 double width

public:
    AsianOptionPricer(std::function<double(double)>& po, std::function<double()>& dis)
        : PricerBase(po, dis) {}

    void GeneratePath(std::vector<double>& vec) {
        const int threadId = omp_get_thread_num();
        const size_t n = vec.size() - 1;
        
        // Vectorized sum calculation
        double sum = 0.0;
        size_t i = 0;
        
        // SIMD sum of vector elements
        alignas(64) double partial_sums[SIMD_WIDTH] = {0};
        
        #pragma omp simd reduction(+:sum)
        for (; i + SIMD_WIDTH <= n; i += SIMD_WIDTH) {
            for (int j = 0; j < SIMD_WIDTH; ++j) {
                sum += vec[i + j];
            }
        }
        
        // Handle remaining elements
        for (; i < n; ++i) {
            sum += vec[i];
        }

        const double avgPrice = sum / n;
        const double payoff = m_payoffFunction(avgPrice);
        updateStats(payoff, threadId);
    }
};

#endif
