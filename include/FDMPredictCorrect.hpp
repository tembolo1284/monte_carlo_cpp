#ifndef FDMPredictCorrect_HPP
#define FDMPredictCorrect_HPP

#include <functional>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "SDEGeneral.hpp"
#include "FDMType.hpp"

class FDMPredictCorrect : public FDMType {
private:
    void validateConstruction(int numTimeSteps, double alpha, double beta) {
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

public:
    double dtSqrt;
    double A; // alpha
    double B; // beta

    FDMPredictCorrect(std::shared_ptr<SDEGeneral>& stochEqn, int numTimeSteps, double alpha = 0.5, double beta = 0.5) {
        std::cout << "FDMPredictCorrect constructor start" << std::endl;
        
        // Initialize base class members
        this->sde = stochEqn;
        this->NT = numTimeSteps;
        A = alpha;
        B = beta;
        
        validateConstruction(numTimeSteps, alpha, beta);
        
        this->m = sde->data->T / static_cast<double>(numTimeSteps);
        std::cout << "Time step m calculated: " << this->m << std::endl;
        
        dtSqrt = std::sqrt(this->m);
        this->x.resize(numTimeSteps + 1);
        
        std::cout << "Initializing time points array of size: " << this->x.size() << std::endl;
        this->x[0] = 0.0;
        for (int i = 1; i < this->x.size(); i++) {
            this->x[i] = this->x[i - 1] + this->m;
        }
        
        // Validate initialization
        std::cout << "Time points initialized. First few points: ";
        for (int i = 0; i < std::min(5, static_cast<int>(this->x.size())); ++i) {
            std::cout << this->x[i] << " ";
        }
        std::cout << std::endl;
        
        std::cout << "FDMPredictCorrect constructor complete" << std::endl;
    }

    double next_n(double x_n, double t_n, double dt, double normVar, double normVar2) override {
        if (!sde) {
            throw std::runtime_error("SDE is null in next_n");
        }
        
        double adjustedDriftTerm = 0.0;
        double diffusionTerm = 0.0;
        
        // Calculate intermediate Euler step
        double eulerUsual = x_n + (sde->drift(t_n, x_n) * dt) + 
                           (sde->diffusion(t_n, x_n) * normVar * std::sqrt(dt));
        
        // Calculate corrections
        adjustedDriftTerm = (A * sde->driftCorrected(t_n + dt, eulerUsual, B) + 
                           (1.0 - A) * sde->driftCorrected(t_n, x_n, B)) * dt;
        
        diffusionTerm = (B * sde->diffusion(t_n + dt, eulerUsual) + 
                       (1.0 - B) * sde->diffusion(t_n, x_n)) * normVar * std::sqrt(dt);
        
        return x_n + adjustedDriftTerm + diffusionTerm;
    }
};

#endif
