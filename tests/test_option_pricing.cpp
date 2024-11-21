#include <gtest/gtest.h>
#include <memory>
#include "EuropeanOptionPricer.hpp"
#include "AsianOptionPricer.hpp"
#include "OptionData.hpp"

class OptionPricingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common test data
        optionData = OptionData{100.0, 1.0, 0.05, 0.2, 0.0, 100.0};
        
        payoffCall = [](double s) { return std::max<double>(0.0, s - 100.0); };
        payoffPut = [](double s) { return std::max<double>(0.0, 100.0 - s); };
        discount = []() { return std::exp(-0.05 * 1.0); };
    }

    OptionData optionData;
    std::function<double(double)> payoffCall;
    std::function<double(double)> payoffPut;
    std::function<double()> discount;
};

TEST_F(OptionPricingTest, EuropeanCallPayoff) {
    auto pricer = std::make_shared<EuropeanOptionPricer>(payoffCall, discount);
    std::vector<double> path = {100.0, 105.0, 110.0, 115.0, 120.0};
    pricer->GeneratePath(path);
    EXPECT_EQ(pricer->OptionPrice(), 20.0 * std::exp(-0.05));
}

TEST_F(OptionPricingTest, EuropeanPutPayoff) {
    auto pricer = std::make_shared<EuropeanOptionPricer>(payoffPut, discount);
    std::vector<double> path = {100.0, 95.0, 90.0, 85.0, 80.0};
    pricer->GeneratePath(path);
    EXPECT_EQ(pricer->OptionPrice(), 20.0 * std::exp(-0.05));
}

TEST_F(OptionPricingTest, AsianCallAverage) {
    auto pricer = std::make_shared<AsianOptionPricer>(payoffCall, discount);
    std::vector<double> path = {100.0, 105.0, 110.0, 115.0, 120.0};
    pricer->GeneratePath(path);
    double expected_avg = 110.0;  // Average of path elements
    double expected_payoff = std::max(0.0, expected_avg - 100.0);
    EXPECT_NEAR(pricer->OptionPrice(), expected_payoff * std::exp(-0.05), 1e-10);
}
