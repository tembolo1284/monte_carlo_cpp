#ifndef SDEGBM_HPP
#define SDEGBM_HPP
#include <concepts>
#include <memory>
#include <functional>
#include "OptionData.hpp"
#include "SDEAbstract.hpp"
 
using InputFunction = std::function<double(const double input1, const double input2)>;

class SDEGBM: public SDEAbstract {
    
public:
    InputFunction m_drift;
    InputFunction m_diffusion;
    InputFunction m_driftCorrected;
    InputFunction m_diffusionDerivative;

    std::shared_ptr<OptionData> data;
    //dS_t = (r-D) * S_t dt + sig * S_t * dZ_t
    SDEGBM(const OptionData& optionData) : data(new OptionData(optionData)), SDEAbstract(optionData) {}

   /* double diffusion(double t, double x) const {
        return (*data).sig * x;  //diffusion(t, x);
    }
    double drift(double t, double x) const {
        return ((*data).r - (*data).D) * x;  //drift(t, x);
    }
    double jump(double t, double x) const {
        return 0.0; // jump(t, x);
    }
    */
    double diffusion(double t, double x) const {
        return m_diffusion(t, x);  //diffusion(t, x);
    }
    double drift(double t, double x) const {
        return m_drift(t, x) * x;  //drift(t, x);
    }
    double diffusionDerivative(double t, double x) {
        return 0.0;
    }
    double driftCorrected(double t, double x, double B) {
        return 0.0;
    }
};

//#ifndef SDEGBM_cpp // Must be the same name as in source file #define
//#include "SDEGBM.cpp"
//#endif

#endif
