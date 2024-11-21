#include <gtest/gtest.h>
#include "SDEGeneral.hpp"
#include "OptionData.hpp"

class SDETest : public ::testing::Test {
protected:
    void SetUp() override {
        optionData = OptionData{100.0, 1.0, 0.05, 0.2, 0.0, 100.0};
        
        drift = [](double t, double S) { return 0.05 * S; };
        diffusion = [](double t, double S) { return 0.2 * S; };
        diffusionDerivative = [](double t, double S) { return 0.2; };
        driftCorrected = [](double t, double S) { 
            return 0.05 * S - 0.5 * 0.2 * S * 0.2; 
        };
    }

    OptionData optionData;
    InputFunction drift;
    InputFunction diffusion;
    InputFunction diffusionDerivative;
    InputFunction driftCorrected;
};

TEST_F(SDETest, DriftCalculation) {
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    
    double S = 100.0;
    double t = 0.5;
    EXPECT_NEAR(sde->drift(t, S), 5.0, 1e-10);
}

TEST_F(SDETest, DiffusionCalculation) {
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    
    double S = 100.0;
    double t = 0.5;
    EXPECT_NEAR(sde->diffusion(t, S), 20.0, 1e-10);
}
