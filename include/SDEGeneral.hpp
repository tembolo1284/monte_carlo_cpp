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
    double driftCorrected(double t, double x, [[maybe_unused]] double B) const noexcept {
        // For GBM: muS - 0.5sig^2S = m_drift - 0.5 * m_diffusion * m_diffusionDerivative
        double sigma = m_diffusion(t, x) / x;  // Convert to volatility
        double mu = m_drift(t, x) / x;         // Convert to drift rate
        return x * (mu - 0.5 * sigma * sigma); // Return drift-corrected term
    }
};

#endif
