#include <gtest/gtest.h>
#include <memory>
#include <cmath>
#include "FDMEuler.hpp"
#include "FDMPredictCorrect.hpp"
#include "SDEGeneral.hpp"
#include "OptionData.hpp"

class FDMTest : public ::testing::Test {
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
        
        drift = []([[maybe_unused]] double t, double S) { 
            return 0.05 * S; 
        };
        
        diffusion = []([[maybe_unused]] double t, double S) { 
            return 0.2 * S; 
        };
        
        diffusionDerivative = []([[maybe_unused]] double t, [[maybe_unused]] double S) { 
            return 0.2; 
        };
        
        driftCorrected = []([[maybe_unused]] double t, double S) { 
            return 0.05 * S - 0.5 * 0.2 * S * 0.2;  // rS - 0.5σ²S
        };

        auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
        sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    }

    void ValidateNumericalSolution(const std::vector<double>& path, 
                                 double S0, 
                                 [[maybe_unused]] double dt, 
                                 double tol) {  // Changed from 'tolerance' to avoid shadowing
        ASSERT_GT(path.size(), 0) << "Path should not be empty";
        EXPECT_NEAR(path[0], S0, tol) << "Initial value should be S0";
        
        // Check that values remain positive (property of geometric Brownian motion)
        for (const auto& value : path) {
            EXPECT_GT(value, 0.0) << "Stock price should remain positive";
        }

        // Check for reasonable growth bounds
        double maxGrowthRate = std::exp((optionData.r + std::pow(optionData.sig, 2)) * optionData.T);
        double minGrowthRate = std::exp((optionData.r - std::pow(optionData.sig, 2)) * optionData.T);
        
        for (const auto& value : path) {
            EXPECT_LT(value, S0 * maxGrowthRate * 2) 
                << "Stock price grew unreasonably large";
            EXPECT_GT(value, S0 * minGrowthRate / 2) 
                << "Stock price decreased unreasonably";
        }
    }

    OptionData optionData;
    InputFunction drift;
    InputFunction diffusion;
    InputFunction diffusionDerivative;
    InputFunction driftCorrected;
    std::shared_ptr<SDEGeneral> sde;
    const double tolerance = 1e-10;
};

TEST_F(FDMTest, EulerConstructor) {
    const int NT = 100;
    auto fdm = std::make_shared<FDMEuler>(sde, NT);
    
    EXPECT_EQ(fdm->getTimePoints().size(), static_cast<size_t>(NT) + 1);
    EXPECT_NEAR(fdm->getTimeStep(), optionData.T / NT, tolerance);
    EXPECT_NEAR(fdm->getTimePoints()[0], 0.0, tolerance);
    EXPECT_NEAR(fdm->getTimePoints().back(), optionData.T, tolerance);
}

TEST_F(FDMTest, PredictCorrectConstructor) {
    const int NT = 100;
    const double alpha = 0.5;
    const double beta = 0.5;
    auto fdm = std::make_shared<FDMPredictCorrect>(sde, NT, alpha, beta);
    
    EXPECT_EQ(fdm->getTimePoints().size(), static_cast<size_t>(NT) + 1);
    EXPECT_NEAR(fdm->getTimeStep(), optionData.T / NT, tolerance);
    EXPECT_NEAR(fdm->getTimePoints()[0], 0.0, tolerance);
    EXPECT_NEAR(fdm->getTimePoints().back(), optionData.T, tolerance);
}

TEST_F(FDMTest, TimeStepConsistency) {
    const int NT = 100;
    auto fdm = std::make_shared<FDMEuler>(sde, NT);
    const auto& timePoints = fdm->getTimePoints();
    
    for (size_t i = 1; i < timePoints.size(); ++i) {
        EXPECT_NEAR(timePoints[i] - timePoints[i-1], fdm->getTimeStep(), tolerance)
            << "Time step size should be consistent";
    }
}

TEST_F(FDMTest, BoundaryConditions) {
    const int NT = 100;
    auto fdm = std::make_shared<FDMEuler>(sde, NT);
    const double S0 = 100.0;
    const double dt = fdm->getTimeStep();
    const double normVar = 0.0;  // Use zero for deterministic test
    const double normVar2 = 0.0;
    
    // With zero random input, should follow deterministic path
    const double nextValue = fdm->next_n(S0, 0.0, dt, normVar, normVar2);
    const double expected = S0 * (1 + optionData.r * dt);
    EXPECT_NEAR(nextValue, expected, tolerance);
}

TEST_F(FDMTest, ZeroVolatilityCase) {
    // Create new option data with zero volatility
    auto zeroVolOption = optionData;
    zeroVolOption.sig = 0.0;
    
    // Create functions with zero volatility
    auto zeroDiffusion = []([[maybe_unused]] double t, [[maybe_unused]] double S) { 
        return 0.0; 
    };
    auto zeroDiffDeriv = []([[maybe_unused]] double t, [[maybe_unused]] double S) { 
        return 0.0; 
    };
    
    auto sdeParams = std::make_tuple(drift, zeroDiffusion, driftCorrected, zeroDiffDeriv);
    auto zeroVolSde = std::make_shared<SDEGeneral>(sdeParams, zeroVolOption);
    
    const int NT = 100;
    auto fdm = std::make_shared<FDMEuler>(zeroVolSde, NT);
    const double S0 = 100.0;
    const double dt = fdm->getTimeStep();
    
    // With zero volatility, solution should be purely deterministic
    const double nextValue = fdm->next_n(S0, 0.0, dt, 0.0, 0.0);
    const double expected = S0 * (1 + optionData.r * dt);
    EXPECT_NEAR(nextValue, expected, tolerance);
}
