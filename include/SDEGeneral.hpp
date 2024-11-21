#ifndef SDEGeneral_HPP
#define SDEGeneral_HPP

#include <concepts>
#include <memory>
#include <functional>
#include "OptionData.hpp"

using InputFunction = std::function<double(const double, const double)>;

class SDEGeneral {
public:
    alignas(64) InputFunction m_drift;
    alignas(64) InputFunction m_diffusion;
    alignas(64) InputFunction m_driftCorrected;
    alignas(64) InputFunction m_diffusionDerivative;
    std::shared_ptr<OptionData> data;

    SDEGeneral(const std::tuple<InputFunction, InputFunction, InputFunction, InputFunction>& sdePieces, 
               const OptionData& optionData)
        : m_drift(std::get<0>(sdePieces))
        , m_diffusion(std::get<1>(sdePieces))
        , m_driftCorrected(std::get<2>(sdePieces))
        , m_diffusionDerivative(std::get<3>(sdePieces))
        , data(std::make_shared<OptionData>(optionData))
    {}

    __attribute__((always_inline)) inline
    double diffusion(double t, double x) const noexcept {
        return m_diffusion(t, x);
    }

    __attribute__((always_inline)) inline
    double drift(double t, double x) const noexcept {
        return m_drift(t, x);
    }

    __attribute__((always_inline)) inline
    double diffusionDerivative(double t, double x) const noexcept {
        return m_diffusionDerivative(t, x);
    }

    __attribute__((always_inline)) inline
    double driftCorrected(double t, double x, double B) const noexcept {
        return drift(t, x) - B * diffusion(t, x) * diffusionDerivative(t, x);
    }
};

#endif
