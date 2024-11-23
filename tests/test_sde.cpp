#include <gtest/gtest.h>
#include "SDEGeneral.hpp"
#include "OptionData.hpp"

class SDETest : public ::testing::Test {
protected:
    void SetUp() override {
        optionData = OptionData{
            .K = 100.0,        // Strike price
            .T = 1.0,          // Time to maturity
            .r = 0.05,         // Risk-free rate
            .sig = 0.2,        // Volatility
            .D = 0.0,          // Dividend rate
            .S_0 = 100.0,      // Initial stock price
            .type = 1,         // Call option
            .H = 0.0,          // No barrier
            .betaCEV = 1.0,    // Standard CEV parameter
            .scale = 1.0       // Standard scale
        };
        
        // muS
        drift = []([[maybe_unused]] double t, double S) { 
            return 0.05 * S; 
        };
        
        // sigS
        diffusion = []([[maybe_unused]] double t, double S) { 
            return 0.2 * S; 
        };
        
        // sig
        diffusionDerivative = []([[maybe_unused]] double t, [[maybe_unused]] double S) { 
            return 0.2; 
        };
        
        // muS - 0.5sig^2S
        driftCorrected = []([[maybe_unused]] double t, double S) { 
            const double mu = 0.05;
            const double sigma = 0.2;
            return S * (mu - 0.5 * sigma * sigma);
        };
    }

    OptionData optionData;
    InputFunction drift;
    InputFunction diffusion;
    InputFunction diffusionDerivative;
    InputFunction driftCorrected;
    const double tolerance = 1e-10;
};

TEST_F(SDETest, DriftCalculation) {
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    
    double S = 100.0;
    double t = 0.5;
    EXPECT_NEAR(sde->drift(t, S), 5.0, tolerance);
}

TEST_F(SDETest, DiffusionCalculation) {
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    
    double S = 100.0;
    double t = 0.5;
    EXPECT_NEAR(sde->diffusion(t, S), 20.0, tolerance);
}

TEST_F(SDETest, DiffusionDerivativeCalculation) {
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    
    double S = 100.0;
    double t = 0.5;
    EXPECT_NEAR(sde->diffusionDerivative(t, S), 0.2, tolerance);
}

TEST_F(SDETest, DriftCorrectedCalculation) {
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    
    double S = 100.0;
    double t = 0.5;
    
    // For GBM: S * (mu - 0.5sig^2)
    const double mu = 0.05;
    const double sigma = 0.2;
    const double expected = S * (mu - 0.5 * sigma * sigma);
    
    EXPECT_NEAR(sde->driftCorrected(t, S, 1.0), expected, tolerance);
}

TEST_F(SDETest, ConsistencyCheck) {
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    
    double S = 100.0;
    double t = 0.5;
    
    // For GBM: S * (mu - 0.5sig^2)
    double sigma = sde->diffusion(t, S) / S;
    double mu = sde->drift(t, S) / S;
    double manualDriftCorrected = S * (mu - 0.5 * sigma * sigma);
    
    EXPECT_NEAR(sde->driftCorrected(t, S, 1.0), manualDriftCorrected, tolerance);
}
