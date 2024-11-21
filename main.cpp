#include <random>
#include <memory>
#include <cmath>
#include <iostream>
#include <tuple>
#include "EuropeanOptionPricer.hpp"
#include "AsianOptionPricer.hpp"
#include "FDMEuler.hpp"
#include "FDMPredictCorrect.hpp"
#include "FDMType.hpp"
#include "MCCentralHub.hpp"
#include "MTEngRandNumGen.hpp"
#include "OptionData.hpp"
#include "Pricer.hpp"
#include "RandNumGen.hpp"
#include "SDEGeneral.hpp"
#include "StopWatch.hpp"

int main() {
    std::cout << "1 factor MC with explicit Euler or Predictor-Corrector method\n";
    
    // Option parameters
    OptionData myOption{ 65.0, 0.25, 0.08, 0.3, 0.0, 60 };
    
    // Simulation parameters
    const int NT = 1000;
    const int NSIM = 50000;

    // SDE functions
    const auto drift = [=](double t, double S) { return (myOption.r - myOption.D) * S; };
    const auto diffusion = [=](double t, double S) { return myOption.sig * S; };
    const auto diffusionDerivative = [=](double t, double S) { return myOption.sig; };
    const auto driftCorrected = [=](double t, double S) { 
        return drift(t, S) - 0.5 * diffusion(t, S) * diffusionDerivative(t, S); 
    };

    // Payoff and discount functions
    std::function<double(double)> payoffCall = [=](double s) { 
        return std::max<double>(0.0, s - myOption.K);
    };
    
    std::function<double(double)> payoffPut = [=](double s) { 
        return std::max<double>(0.0, myOption.K - s);
    };
    
    std::function<double()> discount = [=]() { 
        return std::exp(-myOption.r * myOption.T);
    };

    // Create pricers
    auto pricerEuroCall = std::make_shared<EuropeanOptionPricer>(payoffCall, discount);
    auto pricerEuroPut = std::make_shared<EuropeanOptionPricer>(payoffPut, discount);
    auto pricerAsianCall = std::make_shared<AsianOptionPricer>(payoffCall, discount);
    auto pricerAsianPut = std::make_shared<AsianOptionPricer>(payoffPut, discount);

    // Create SDE
    auto sdeParams = std::make_tuple(drift, diffusion, driftCorrected, diffusionDerivative);
    auto sde = std::make_shared<SDEGeneral>(sdeParams, myOption);

    // FDM parameters
    const double alpha = 0.5;
    const double beta = 0.5;
    auto fdm = std::make_shared<FDMPredictCorrect>(sde, NT, alpha, beta);
    
    // Random number generator
    auto randMersenneTwister = std::make_shared<MTEngRandNumGen>();

    std::cout << "NT = " << NT << ", NSIM = " << NSIM << ":\n";
    StopWatch sw;

    // European Put
    sw.StartStopWatch();
    auto euroPut = std::make_tuple(sde, pricerEuroPut, fdm, randMersenneTwister);
    MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubEuroPut(euroPut, NSIM, NT);
    centralHubEuroPut.BeginSimulation();
    
    std::cout << "European Put price using Mersenne Twister: " << pricerEuroPut->OptionPrice() << '\n'
              << "Std Deviation: " << std::get<0>(pricerEuroPut->StandardDeviationStats()) << '\n'
              << "Std Error: " << std::get<1>(pricerEuroPut->StandardDeviationStats()) << "\n\n";
    
    sw.StopStopWatch();
    std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';

    // European Call
    sw.Reset();
    sw.StartStopWatch();
    
    auto euroCall = std::make_tuple(sde, pricerEuroCall, fdm, randMersenneTwister);
    MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubEuroCall(euroCall, NSIM, NT);
    centralHubEuroCall.BeginSimulation();
    
    std::cout << "European Call price using Mersenne Twister: " << pricerEuroCall->OptionPrice() << '\n'
              << "Std Deviation: " << std::get<0>(pricerEuroCall->StandardDeviationStats()) << '\n'
              << "Std Error: " << std::get<1>(pricerEuroCall->StandardDeviationStats()) << "\n\n";
    
    sw.StopStopWatch();
    std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';

    // Asian Put
    sw.Reset();
    sw.StartStopWatch();
    
    auto asianPut = std::make_tuple(sde, pricerAsianPut, fdm, randMersenneTwister);
    MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubAsianPut(asianPut, NSIM, NT);
    centralHubAsianPut.BeginSimulation();
    
    std::cout << "Asian Put price using Mersenne Twister: " << pricerAsianPut->OptionPrice() << '\n'
              << "Std Deviation: " << std::get<0>(pricerAsianPut->StandardDeviationStats()) << '\n'
              << "Std Error: " << std::get<1>(pricerAsianPut->StandardDeviationStats()) << "\n\n";
    
    sw.StopStopWatch();
    std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';

    // Asian Call
    sw.Reset();
    sw.StartStopWatch();
    
    auto asianCall = std::make_tuple(sde, pricerAsianCall, fdm, randMersenneTwister);
    MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubAsianCall(asianCall, NSIM, NT);
    centralHubAsianCall.BeginSimulation();
    
    std::cout << "Asian Call price using Mersenne Twister: " << pricerAsianCall->OptionPrice() << '\n'
              << "Std Deviation: " << std::get<0>(pricerAsianCall->StandardDeviationStats()) << '\n'
              << "Std Error: " << std::get<1>(pricerAsianCall->StandardDeviationStats()) << "\n\n";
    
    sw.StopStopWatch();
    std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';

    return 0;
}
