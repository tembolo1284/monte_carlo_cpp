#ifndef FDMEuler_HPP
#define FDMEuler_HPP

#include "SDEGeneral.hpp"
#include "FDMType.hpp"

class FDMEuler: public FDMType {
public:
    double dtSqrt;

    FDMEuler(std::shared_ptr<SDEGeneral>& stochEqn, int numTimeSteps) {
        sde = stochEqn;
        NT = numTimeSteps;
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
        return (x_n + (sde->drift(t_n, x_n) * dt) + 
                (sde->diffusion(t_n, x_n) * normVar * std::sqrt(dt)));
    }
};

#endif
