#ifndef FDMType_HPP
#define FDMType_HPP

#include <vector>
#include <memory>
#include "SDEGeneral.hpp"

class FDMType {
protected:
    std::shared_ptr<SDEGeneral> sde;
    int NT;
    std::vector<double> x;
    double m;

public:
    FDMType() = default;
    virtual ~FDMType() = default;
    
    virtual double next_n(double x_n, double t_n, double dt, double normVar, double normVar2) = 0;
    
    // Getters for accessing protected members
    const std::vector<double>& getTimePoints() const { return x; }
    double getTimeStep() const { return m; }
    int getNumTimeSteps() const { return NT; }
    double getTimePoint(size_t index) const { return x[index]; }
};

#endif
