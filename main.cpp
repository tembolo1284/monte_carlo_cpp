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
    //K, T, r, sig, dividend, type, S_0 (IC)
    // uncomment the input data myOption you want to use below
    OptionData myOption{ 65.0, 0.25, 0.08, 0.3, 0.0, 60 };
    //OptionData myOption{ 100.0, 1.0, 0.0, 0.2, 0.0, 60 };
  
    //set simulation params early.
    int NT = 1000;
    int NSIM = 50000;

    //functions needed for SDEGeneral terms
    auto drift = [=](double t, double S) {return ((myOption.r-myOption.D) * S);};
    auto diffusion = [=](double t, double S) {return myOption.sig * S; };
    auto diffusionDerivative = [=](double t, double S) {return myOption.sig; };
    auto driftCorrected = [=](double t, double S) {return drift(t, S) - 0.5 * diffusion(t, S) * diffusionDerivative(t, S); };

    //stripped out payoff and discount formulas and put it in pricer class as a std::function type
    std::function<double(double)> payoffCall = [=](double s) { //max s-k for a call
        return std::max<double>(0.0, s - myOption.K);
    };

    std::function<double(double)> payoffPut = [=](double s) { //max k-s for a put
        return std::max<double>(0.0, myOption.K - s);
    };
   
    std::function<double()> discount = [=]() { //e^-rT discounting
        return std::exp(-myOption.r * myOption.T);
    };

    //create european and asian call and put options
    auto pricerEuroCall = std::shared_ptr<EuropeanOptionPricer>(new EuropeanOptionPricer(payoffCall, discount));
    auto pricerEuroPut = std::shared_ptr<EuropeanOptionPricer>(new EuropeanOptionPricer(payoffPut, discount));

    auto pricerAsianCall = std::shared_ptr<AsianOptionPricer>(new AsianOptionPricer(payoffCall, discount));
    auto pricerAsianPut = std::shared_ptr<AsianOptionPricer>(new AsianOptionPricer(payoffPut, discount));

    //create sde. Generalized compared to having SDEGBM before, and now brand new with tuple constructor.
    std::tuple<InputFunction, InputFunction, InputFunction, InputFunction> sdeParams = std::make_tuple(drift, diffusion, 
        driftCorrected, diffusionDerivative);

    auto sde = std::shared_ptr<SDEGeneral>(new SDEGeneral(sdeParams, myOption));
    
    //uncomment fdm scheme to be used (just Euler and predict-corrector schemes for now)
    // fully explicit: alpha = beta = 0, fully implicit: alpha = beta = 1, symmetrical(default): alpha = beta = 1/2
    // Currently if you remove alpha and beta from the constructor it defaults to 1/2 for both.
    double alpha = 0.5;
    double beta = 0.5;
    auto fdm = std::shared_ptr<FDMPredictCorrect>(new FDMPredictCorrect(sde, NT,alpha,beta));  
    //auto fdm = std::shared_ptr<FDMEuler>(new FDMEuler(sde, NT));

    //rand number generation time (Just MarsenneTwister for now)
    auto randMersenneTwister = std::shared_ptr<MTEngRandNumGen>(new MTEngRandNumGen());

    std::cout << "NT = "<< NT << ", " << "NSIM = " << NSIM << ": " << std::endl;
    //stopwatch creation and start timer
    StopWatch sw;
    sw.StartStopWatch();

    //create main central hub air traffic controller and let 'er rip!
    //European Option section
    std::tuple<std::shared_ptr<SDEGeneral>, std::shared_ptr<Pricer>,
        std::shared_ptr<FDMType>, std::shared_ptr<MTEngRandNumGen>> euroPut = std::make_tuple(sde, pricerEuroPut, fdm, randMersenneTwister);

    //old style constructor
    //MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubEuroPut(sde, pricerEuroPut, fdm, randMersenneTwister, NSIM, NT);
    MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubEuroPut(euroPut, NSIM, NT);
    centralHubEuroPut.BeginSimulation();
    std::cout << "European Put price using Mersenne Twister: " << pricerEuroPut->OptionPrice() << std::endl;
    std::cout << "Std Deviation: " << std::get<0>(pricerEuroPut->StandardDeviationStats()) << std::endl;
    std::cout << "Std Error: " << std::get<1>(pricerEuroPut->StandardDeviationStats()) << "\n\n";

    sw.StopStopWatch();
    std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';

    sw.Reset();
    sw.StartStopWatch();

    std::tuple<std::shared_ptr<SDEGeneral>, std::shared_ptr<Pricer>,
        std::shared_ptr<FDMType>, std::shared_ptr<MTEngRandNumGen>> euroCall = std::make_tuple(sde, pricerEuroCall, fdm, randMersenneTwister);
    
    MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubEuroCall(euroCall, NSIM, NT);
    centralHubEuroCall.BeginSimulation();
    std::cout << "European Call price using Mersenne Twister: " << pricerEuroCall->OptionPrice() << std::endl;
    std::cout << "Std Deviation: " << std::get<0>(pricerEuroCall->StandardDeviationStats()) << std::endl;
    std::cout << "Std Error: " << std::get<1>(pricerEuroCall->StandardDeviationStats()) << "\n\n";

    sw.StopStopWatch();
    std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';

    sw.Reset();
    sw.StartStopWatch();

    ///// Asian option section
    std::tuple<std::shared_ptr<SDEGeneral>, std::shared_ptr<Pricer>,
        std::shared_ptr<FDMType>, std::shared_ptr<MTEngRandNumGen>> asianPut = std::make_tuple(sde, pricerAsianPut, fdm, randMersenneTwister);
    
    MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubAsianPut(asianPut, NSIM, NT);
    centralHubAsianPut.BeginSimulation();
    std::cout << "Asian Put price using Mersenne Twister: " << pricerAsianPut->OptionPrice() << std::endl;
    std::cout << "Std Deviation: " << std::get<0>(pricerAsianPut->StandardDeviationStats()) << std::endl;
    std::cout << "Std Error: " << std::get<1>(pricerAsianPut->StandardDeviationStats()) << "\n\n";

    sw.StopStopWatch();
    std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';

    sw.Reset();
    sw.StartStopWatch();
    
    std::tuple<std::shared_ptr<SDEGeneral>, std::shared_ptr<Pricer>,
        std::shared_ptr<FDMType>, std::shared_ptr<MTEngRandNumGen>> asianCall = std::make_tuple(sde, pricerAsianCall, fdm, randMersenneTwister);

    MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> centralHubAsianCall(asianCall, NSIM, NT);
    centralHubAsianCall.BeginSimulation();
    std::cout << "Asian Call price using Mersenne Twister: " << pricerAsianCall->OptionPrice() << std::endl;
    std::cout << "Std Deviation: " << std::get<0>(pricerAsianCall->StandardDeviationStats()) << std::endl;
    std::cout << "Std Error: " << std::get<1>(pricerAsianCall->StandardDeviationStats()) << "\n\n";

    sw.StopStopWatch();
    std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';
   
    return 0;

}