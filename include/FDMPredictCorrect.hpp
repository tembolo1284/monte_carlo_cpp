#ifndef FDMPredictCorrect_HPP
#define FDMPredictCorrect_HPP

#include "SDEGeneral.hpp"
#include "FDMType.hpp"

class FDMPredictCorrect : public FDMType {
public:
    double dtSqrt;
    double A; // alpha
    double B; // beta

    // Add [[maybe_unused]] to silence warnings for unused parameters
    void validateConstruction(int numTimeSteps, 
                            [[maybe_unused]] double alpha, 
                            [[maybe_unused]] double beta) {
        if (numTimeSteps <= 0) {
            throw std::runtime_error("Number of time steps must be positive");
        }
        if (!sde) {
            throw std::runtime_error("SDE pointer is null");
        }
        if (!sde->data) {
            throw std::runtime_error("SDE data pointer is null");
        }
        if (sde->data->T <= 0) {
            throw std::runtime_error("Time period T must be positive");
        }
    }

    FDMPredictCorrect(std::shared_ptr<SDEGeneral>& stochEqn, int numTimeSteps, 
                      double alpha = 0.5, double beta = 0.5) {
        sde = stochEqn;
        NT = numTimeSteps;
        A = alpha;
        B = beta;
        
        validateConstruction(numTimeSteps, alpha, beta);
        
        m = sde->data->T / static_cast<double>(NT);
        dtSqrt = std::sqrt(m);
        x = std::vector<double>(static_cast<size_t>(NT) + 1);
        x[0] = 0.0;
        
        // Fixed signed/unsigned comparison
        for (size_t i = 1; i < x.size(); ++i) {
            x[i] = x[i - 1] + m;
        }
    }

    double next_n(double x_n, double t_n, double dt, double normVar, 
                 [[maybe_unused]] double normVar2) override {
        double eulerUsual = x_n + (sde->drift(t_n, x_n) * dt) + 
                           (sde->diffusion(t_n, x_n) * normVar * std::sqrt(dt));
        
        double adjustedDriftTerm = (A * sde->driftCorrected(t_n + dt, eulerUsual, B) + 
                                  (1.0 - A) * sde->driftCorrected(t_n, x_n, B)) * dt;
        
        double diffusionTerm = (B * sde->diffusion(t_n + dt, eulerUsual) + 
                               (1.0 - B) * sde->diffusion(t_n, x_n)) * 
                               normVar * std::sqrt(dt);
        
        return x_n + adjustedDriftTerm + diffusionTerm;
    }
};

#endif
