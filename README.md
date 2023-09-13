Here is the README.md in Markdown format:

# Monte Carlo Option Pricing

This C++ project implements Monte Carlo simulation for pricing European and Asian options. It allows using either an Euler or Predictor-Corrector finite difference method.

## Files

- `main.cpp`: Main driver code
- `StopWatch.cpp/hpp`: Timer class to measure performance  
- `OptionData.hpp`: Holds option data
- `Pricer.hpp`: Abstract pricing class   
- `EuropeanOptionPricer.hpp`: Concrete European option pricer
- `AsianOptionPricer.hpp`: Concrete Asian option pricer
- `RandNumGen.hpp`: Abstract random number generator
- `MTEngRandNumGen.hpp`: Concrete Mersenne Twister Random Number Generator(RNG)
- `SDEGeneral.hpp`: Stochastic differential equation   
- `FDMType.hpp`: Abstract finite difference method
- `FDMEuler.hpp`: Concrete Euler finite difference   
- `FDMPredictCorrect.hpp`: Concrete Predictor-Corrector finite difference
- `MCCentralHub.hpp`: Monte Carlo simulator coordinator

## Usage

The `main.cpp` file demonstrates creating the required objects and running simulations to price European and Asian options. 

Key parameters:

- `NSIM`: Number of Monte Carlo simulation paths
- `NT`: Number of time steps per path

## Compile and Run

This project uses CMake. To compile:

```
cmake -B build 
cmake --build build
```

Then execute: 

```
./build/MonteCarloProject
```

The CMake files allow easily switching between `g++` and `clang++`.

# cmake -DUSE_CLANG=ON -B build.. to turn on clang
# cmake -DUSE_CLANG=OFF -B build.. to turn off clang

## TODO

Some ideas for future improvements:

- Add more finite difference schemes
- Support early exercise features  
- Add variance reduction techniques to the monte carlo
- Improve computational performance
