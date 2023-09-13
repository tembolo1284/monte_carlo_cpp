#ifndef FDMPredictCorrect_HPP
#define FDMPredictCorrect_HPP
#include <functional>
#include <vector>
//#include "SDEAbstract.hpp"
#include "SDEGeneral.hpp"
#include "FDMType.hpp"

//template <SDEAbstract>
class FDMPredictCorrect : public FDMType {

public:

    //std::shared_ptr<SDEGeneral> sde;
    //int NT;
    //std::vector<double> x; //this will be the array for my space variable, my mesh
    //double m; //size of my space array

    double dtSqrt;
    double A; //alpha
    double B; //beta

    //FDMEuler() {}
    FDMPredictCorrect(std::shared_ptr<SDEGeneral>& stochEqn, int numTimeSteps, double alpha = 0.5, double beta = 0.5) {
        sde = stochEqn;
        NT = numTimeSteps;
        A = alpha;
        B = beta;
        m = sde->data->T / static_cast<double>(NT);
        dtSqrt = sqrt(m);
        x = std::vector<double>(NT + 1);
        x[0] = 0.0;

        for (int i = 1; i < x.size(); i++) {
            x[i] = x[i - 1] + m;
        }
    }

    double next_n(double x_n, double t_n, double dt, double normVar, double normVar2) override {
        //debug stuff
        //std::cout << "x_n: " << x_n;
        //std::cout << "drift: " << sde->drift(t_n, x_n) << std::endl;
        //std::cout << "diffusion: " << sde->diffusion(t_n, x_n) << std::endl;
        double adjustedDriftTerm{ 0.0 };
        double diffusionTerm{ 0.0 };
        //the usual euler part with standard drift and diffusion terms
        double eulerUsual = x_n + (sde->drift(t_n, x_n) * dt) + (sde->diffusion(t_n, x_n) * normVar * sqrt(dt));
        //return (x_n + (sde->drift(t_n, x_n) * dt) + (sde->diffusion(t_n, x_n) * normVar * sqrt(dt))); //old value for SDEGBM
        
        //drift and diffusion correction pieces
        adjustedDriftTerm = (A * sde->driftCorrected(t_n + dt, eulerUsual, B) + (1.0 - A) * sde->driftCorrected(t_n, x_n, B))* dt;
        //std::cout << "adjDrift: " << sde->driftCorrected(t_n, x_n, B) << std::endl;
        diffusionTerm = (B * sde->diffusion(t_n + dt, eulerUsual) + (1.0 - B) * sde->diffusion(t_n, x_n)) * normVar * sqrt(dt);
        //std::cout << "adjDiffusion: " << sde->diffusion(t_n, x_n) << std::endl;
        return x_n + adjustedDriftTerm + diffusionTerm;
    }

};

//#ifndef FDMPredictCorrect_cpp // Must be the same name as in source file #define
//#include "FDMPredictCorrect.cpp"
//#endif

#endif
