#ifndef FDMEuler_HPP
#define FDMEuler_HPP
#include <functional>
#include <vector>
//#include "SDEAbstract.hpp"
//#include "SDEGBM.hpp"
#include "SDEGeneral.hpp"
#include "FDMType.hpp"

//template <SDEAbstract>
class FDMEuler: public FDMType {

public:
    
    //std::shared_ptr<SDEGeneral> sde;
    //int NT;
    //std::vector<double> x; //this will be the array for my space variable, my mesh
    //double m; //size of my space array

    double dtSqrt;

    //FDMEuler() {}
    FDMEuler(std::shared_ptr<SDEGeneral>& stochEqn, int numTimeSteps) {
        sde = stochEqn;
        NT = numTimeSteps;
        m = sde->data->T/ static_cast<double>(NT);
        dtSqrt = sqrt(m);
        x = std::vector<double>(NT + 1);
        x[0] = 0.0;

        for (int i = 1; i < x.size(); i++) {
            x[i] = x[i - 1] + m;
        }
    }

    double next_n(double x_n, double t_n, double dt, double normVar, double normVar2) override{
        //debug stuff
        //std::cout << "x_n: " << x_n;
        //std::cout << "drift: " << sde->drift(t_n, x_n) << std::endl;
        //std::cout << "diffusion: " << sde->diffusion(t_n, x_n) << std::endl;
        return (x_n + (sde->drift(t_n, x_n) * dt) + (sde->diffusion(t_n, x_n) * normVar * sqrt(dt)));
    }

};

//#ifndef FDMEuler_cpp // Must be the same name as in source file #define
//#include "FDMEuler.cpp"
//#endif

#endif
