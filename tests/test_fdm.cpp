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
        // Set up common test data
        optionData = OptionData{100.0, 1.0, 0.05, 0.2, 0.0, 100.0};
        
        // Define SDE functions for Black-Scholes model
        drift = [](double t, double S) { return 0.05 * S; };  // rS
        diffusion = [](double t, double S) { return 0.2 * S; };  // σS
        diffusionDerivative = [](double t, double S) { return 0.2; };  // σ
        driftCorrected = [](double t, double S) { 
            return 0.05 * S - 0.5 * 0.2 * S * 0.2; 
        };  // rS - 0.5σ²S
        
        // Create SDE
        auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
        sde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    }

    // Helper function to check if numerical solution follows expected properties
    void ValidateNumericalSolution(const std::vector<double>& path, 
                                 double S0, double dt, double tolerance) {
        ASSERT_GT(path.size(), 0) << "Path should not be empty";
        EXPECT_NEAR(path[0], S0, tolerance) << "Initial value should be S0";
        
        // Check that values remain positive (property of geometric Brownian motion)
        for (const auto& value : path) {
            EXPECT_GT(value, 0.0) << "Stock price should remain positive";
        }

        // Check for reasonable growth bounds
        // Using rough estimates based on drift and volatility
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
    std::shared_ptr<SDEGeneral> sde;
    InputFunction drift;
    InputFunction diffusion;
    InputFunction diffusionDerivative;
    InputFunction driftCorrected;
    const double tolerance = 1e-10;
};

TEST_F(FDMTest, EulerConstructor) {
    int NT = 100;
    auto fdm = std::make_shared<FDMEuler>(sde, NT);
    
    EXPECT_EQ(fdm->getTimePoints().size(), NT + 1);
    EXPECT_NEAR(fdm->getTimeStep(), optionData.T / NT, tolerance);
    EXPECT_NEAR(fdm->getTimePoints()[0], 0.0, tolerance);
    EXPECT_NEAR(fdm->getTimePoints().back(), optionData.T, tolerance);
}

TEST_F(FDMTest, PredictCorrectConstructor) {
    int NT = 100;
    double alpha = 0.5;
    double beta = 0.5;
    auto fdm = std::make_shared<FDMPredictCorrect>(sde, NT, alpha, beta);
    
    EXPECT_EQ(fdm->getTimePoints().size(), NT + 1);
    EXPECT_NEAR(fdm->getTimeStep(), optionData.T / NT, tolerance);
    EXPECT_NEAR(fdm->getTimePoints()[0], 0.0, tolerance);
    EXPECT_NEAR(fdm->getTimePoints().back(), optionData.T, tolerance);
}

TEST_F(FDMTest, EulerPathGeneration) {
    int NT = 1000;
    auto fdm = std::make_shared<FDMEuler>(sde, NT);
    std::vector<double> path(NT + 1);
    path[0] = optionData.S_0;
    
    // Generate a path using normal random variables
    double dt = fdm->getTimeStep();
    double VOld = optionData.S_0;
    
    // Use fixed random numbers for reproducibility
    for (int j = 1; j < NT + 1; j++) {
        double t = fdm->getTimePoints()[j - 1];
        // Using fixed standard normal values for testing
        double normVar = 0.1;  // Fixed value for reproducibility
        double normVar2 = 0.1;
        
        double VNew = fdm->next_n(VOld, t, dt, normVar, normVar2);
        path[j] = VNew;
        VOld = VNew;
    }
    
    ValidateNumericalSolution(path, optionData.S_0, dt, tolerance);
}

TEST_F(FDMTest, PredictCorrectPathGeneration) {
    int NT = 1000;
    auto fdm = std::make_shared<FDMPredictCorrect>(sde, NT);
    std::vector<double> path(NT + 1);
    path[0] = optionData.S_0;
    
    double dt = fdm->getTimeStep();
    double VOld = optionData.S_0;
    
    // Generate a path using fixed random variables
    for (int j = 1; j < NT + 1; j++) {
        double t = fdm->getTimePoints()[j - 1];
        // Using fixed standard normal values for testing
        double normVar = 0.1;  // Fixed value for reproducibility
        double normVar2 = 0.1;
        
        double VNew = fdm->next_n(VOld, t, dt, normVar, normVar2);
        path[j] = VNew;
        VOld = VNew;
    }
    
    ValidateNumericalSolution(path, optionData.S_0, dt, tolerance);
}

TEST_F(FDMTest, CompareMethods) {
    int NT = 1000;
    auto eulerFdm = std::make_shared<FDMEuler>(sde, NT);
    auto pcFdm = std::make_shared<FDMPredictCorrect>(sde, NT);
    
    double dt = eulerFdm->getTimeStep();
    double S0 = optionData.S_0;
    double t = 0.0;
    
    // Compare single step with same random variables
    double normVar = 0.1;
    double normVar2 = 0.1;
    
    double eulerStep = eulerFdm->next_n(S0, t, dt, normVar, normVar2);
    double pcStep = pcFdm->next_n(S0, t, dt, normVar, normVar2);
    
    // Predictor-Corrector should be more conservative than Euler
    // due to its improved stability properties
    EXPECT_NE(eulerStep, pcStep) 
        << "Methods should give different results for non-zero volatility";
}

TEST_F(FDMTest, TimeStepValidity) {
    std::vector<int> timeSteps = {10, 100, 1000};
    
    for (int NT : timeSteps) {
        auto fdm = std::make_shared<FDMEuler>(sde, NT);
        double dt = fdm->getTimeStep();
        
        EXPECT_GT(dt, 0.0) << "Time step should be positive";
        EXPECT_LT(dt, optionData.T) << "Time step should be less than total time";
        
        const auto& timePoints = fdm->getTimePoints();
        for (size_t i = 1; i < timePoints.size(); ++i) {
            EXPECT_NEAR(timePoints[i] - timePoints[i-1], dt, tolerance)
                << "Time steps should be uniform";
        }
    }
}

TEST_F(FDMTest, ZeroVolatilityCase) {
    // Create a special case with zero volatility
    optionData.sig = 0.0;
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto zeroVolSde = std::make_shared<SDEGeneral>(sdeParams, optionData);
    
    int NT = 100;
    auto fdm = std::make_shared<FDMEuler>(zeroVolSde, NT);
    
    double dt = fdm->getTimeStep();
    double S0 = optionData.S_0;
    double t = 0.0;
    
    // With zero volatility, the solution should be deterministic
    double expected = S0 * std::exp(optionData.r * dt);
    double actual = fdm->next_n(S0, t, dt, 0.0, 0.0);
    
    EXPECT_NEAR(actual, expected, tolerance)
        << "Zero volatility case should match deterministic solution";
}

TEST_F(FDMTest, StabilityTest) {
    int NT = 1000;
    auto fdm = std::make_shared<FDMPredictCorrect>(sde, NT);
    std::vector<double> path(NT + 1);
    path[0] = optionData.S_0;
    
    double dt = fdm->getTimeStep();
    double VOld = optionData.S_0;
    
    // Test stability with large random numbers
    for (int j = 1; j < NT + 1; j++) {
        double t = fdm->getTimePoints()[j - 1];
        // Using large values to test stability
        double normVar = 3.0;  // 3 standard deviations
        double normVar2 = 3.0;
        
        double VNew = fdm->next_n(VOld, t, dt, normVar, normVar2);
        path[j] = VNew;
        VOld = VNew;
        
        // Check for stability (value shouldn't explode)
        EXPECT_LT(std::abs(VNew), optionData.S_0 * 100)
            << "Solution should remain bounded for stability";
    }
}
