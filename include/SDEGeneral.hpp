#ifndef SDEGeneral_HPP
#define SDEGeneral_HPP
#include <concepts>
#include <memory>
#include <functional>
#include "OptionData.hpp"
//#include "SDEAbstract.hpp"

using InputFunction = std::function<double(const double input1, const double input2)>;

class SDEGeneral {

public:
    InputFunction m_drift;
    InputFunction m_diffusion;
    InputFunction m_driftCorrected;
    InputFunction m_diffusionDerivative;

    std::shared_ptr<OptionData> data;
    /*
    SDEGeneral(const InputFunction& drift, const InputFunction& diffusion, const InputFunction& driftCorrected, 
        const InputFunction& diffusionDerivative, const OptionData& optionData) : 
        m_drift(drift), m_diffusion(diffusion), m_driftCorrected(driftCorrected), m_diffusionDerivative(diffusionDerivative),
        data(new OptionData(optionData)) {} // this is a horrendously wordy constructor woof
    */

    SDEGeneral(const std::tuple<InputFunction, InputFunction, InputFunction, InputFunction> sdePieces, const OptionData& optionData) :
        m_drift(std::get<0>(sdePieces)), m_diffusion(std::get<1>(sdePieces)), m_driftCorrected(std::get<2>(sdePieces)), 
        m_diffusionDerivative(std::get<3>(sdePieces)), data(new OptionData(optionData)) {}


    double diffusion(double t, double x) const {
        return m_diffusion(t, x);  //diffusion(t, x);
    }
    double drift(double t, double x) const {
        return m_drift(t, x);// *x;  //drift(t, x);
    }
    double diffusionDerivative(double t, double x) {
        return m_diffusionDerivative(t, x);
    }
    double driftCorrected(double t, double x, double B) {
        //std::cout << "drift: " << drift(t, x) << std::endl;
        //std::cout << "diffusion: " << diffusion(t, x) << std::endl;
        //std::cout << "diffusionDerivative: " << diffusionDerivative(t, x) << std::endl;
        return drift(t, x) - B * diffusion(t, x) * diffusionDerivative(t, x);
    }
};

//#ifndef SDEGeneral_cpp // Must be the same name as in source file #define
//#include "SDEGeneral.cpp"
//#endif

#endif
