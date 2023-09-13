#ifndef FDMType_HPP
#define FDMType_HPP
#include <functional>
#include <vector>
#include <memory>
//#include "SDEAbstract.hpp"
//#include "SDEGBM.hpp"
#include "SDEGeneral.hpp"
//template <SDEAbstract>
class FDMType {

public:
    //abstract class.
    std::shared_ptr<SDEGeneral> sde;
    int NT;
    std::vector<double> x; //this will be the array for my space variable, my mesh
    double m; //size of my space array

    virtual double next_n(double x_n, double t_n, double dt, double normVar, double normVar2) = 0;
};

//#ifndef FDMType_cpp // Must be the same name as in source file #define
//#include "FDMType.cpp"
//#endif

#endif
