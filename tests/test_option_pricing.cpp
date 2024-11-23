#include <gtest/gtest.h>
#include <memory>
#include <cmath>    
#include <numeric>
#include "EuropeanOptionPricer.hpp"
#include "AsianOptionPricer.hpp"
#include "OptionData.hpp"

class OptionPricingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize all fields of OptionData
        optionData = OptionData{
            .K = 100.0,        // Strike
            .T = 1.0,          // Time to maturity
            .r = 0.05,         // Risk-free rate
            .sig = 0.2,        // Volatility
            .D = 0.0,          // Dividend
            .S_0 = 100.0,      // Initial price
            .type = 1,         // Call option
            .H = 0.0,          // No barrier
            .betaCEV = 1.0,    // Standard CEV parameter
            .scale = 1.0       // Standard scale
        };
        
        payoffCall = std::function<double(double)>([](double s) { 
            return std::max<double>(0.0, s - 100.0); 
        });
        
        payoffPut = std::function<double(double)>([](double s) { 
            return std::max<double>(0.0, 100.0 - s); 
        });
        
        discount = std::function<double()>([]() { 
            return std::exp(-0.05 * 1.0); 
        });
    }

    OptionData optionData;
    std::function<double(double)> payoffCall;
    std::function<double(double)> payoffPut;
    std::function<double()> discount;
    const double tolerance = 1e-10;
};

TEST_F(OptionPricingTest, EuropeanCallPayoff) {
    auto pricer = std::make_shared<EuropeanOptionPricer>(payoffCall, discount);
    std::vector<double> path = {100.0, 105.0, 110.0, 115.0, 120.0};
    pricer->GeneratePath(path);
    pricer->AfterPathCleanUp();
    EXPECT_NEAR(pricer->OptionPrice(), 20.0 * std::exp(-0.05), tolerance);
}

TEST_F(OptionPricingTest, EuropeanPutPayoff) {
    auto pricer = std::make_shared<EuropeanOptionPricer>(payoffPut, discount);
    std::vector<double> path = {100.0, 95.0, 90.0, 85.0, 80.0};
    pricer->GeneratePath(path);
    pricer->AfterPathCleanUp();
    EXPECT_NEAR(pricer->OptionPrice(), 20.0 * std::exp(-0.05), tolerance);
}

TEST_F(OptionPricingTest, AsianCallAverage) {
    auto pricer = std::make_shared<AsianOptionPricer>(payoffCall, discount);
    // Modified test path
    std::vector<double> path = {100.0, 105.0, 110.0, 115.0, 120.0};
    
    // Calculate the arithmetic average (excluding the last point for Asian option)
    double sum = std::accumulate(path.begin(), path.end() - 1, 0.0);
    double avgPrice = sum / (path.size() - 1);
    
    pricer->GeneratePath(path);
    pricer->AfterPathCleanUp();
    
    double expected_payoff = std::max(0.0, avgPrice - optionData.K);
    EXPECT_NEAR(pricer->OptionPrice(), expected_payoff * std::exp(-0.05), tolerance);
}

TEST_F(OptionPricingTest, ZeroPathTest) {
    auto pricer = std::make_shared<AsianOptionPricer>(payoffCall, discount);
    std::vector<double> path(5, 100.0);  // All values are 100.0
    pricer->GeneratePath(path);
    pricer->AfterPathCleanUp();
    EXPECT_NEAR(pricer->OptionPrice(), 0.0, tolerance);
}

TEST_F(OptionPricingTest, SinglePathAsianTest) {
    auto pricer = std::make_shared<AsianOptionPricer>(payoffCall, discount);
    std::vector<double> path = {100.0, 100.0, 100.0, 100.0, 100.0};
    pricer->GeneratePath(path);
    pricer->AfterPathCleanUp();
    EXPECT_NEAR(pricer->OptionPrice(), 0.0, tolerance);
}

TEST_F(OptionPricingTest, InTheMoneyAsianCallTest) {
    auto pricer = std::make_shared<AsianOptionPricer>(payoffCall, discount);
    std::vector<double> path = {120.0, 120.0, 120.0, 120.0, 120.0};
    pricer->GeneratePath(path);
    pricer->AfterPathCleanUp();
    double expected_payoff = 20.0;  // Average is 120, K is 100
    EXPECT_NEAR(pricer->OptionPrice(), expected_payoff * std::exp(-0.05), tolerance);
}

TEST_F(OptionPricingTest, OutOfTheMoneyAsianCallTest) {
    auto pricer = std::make_shared<AsianOptionPricer>(payoffCall, discount);
    std::vector<double> path = {80.0, 80.0, 80.0, 80.0, 80.0};
    pricer->GeneratePath(path);
    pricer->AfterPathCleanUp();
    EXPECT_NEAR(pricer->OptionPrice(), 0.0, tolerance);
}

TEST_F(OptionPricingTest, AsianPutTest) {
    auto pricer = std::make_shared<AsianOptionPricer>(payoffPut, discount);
    std::vector<double> path = {90.0, 95.0, 100.0, 105.0, 110.0};
    
    // Calculate average excluding last point
    double sum = std::accumulate(path.begin(), path.end() - 1, 0.0);
    double avgPrice = sum / (path.size() - 1);
    
    pricer->GeneratePath(path);
    pricer->AfterPathCleanUp();
    
    double expected_payoff = std::max(0.0, optionData.K - avgPrice);
    EXPECT_NEAR(pricer->OptionPrice(), expected_payoff * std::exp(-0.05), tolerance);
}

TEST_F(OptionPricingTest, DiscountFactorTest) {
    auto pricer = std::make_shared<EuropeanOptionPricer>(payoffCall, discount);
    EXPECT_NEAR(pricer->DiscountFactor(), std::exp(-0.05), tolerance);
}
