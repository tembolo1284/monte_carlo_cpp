# Monte Carlo Option Pricing

A high-performance C++ implementation for pricing European and Asian options using Monte Carlo simulation with OpenMP parallelization. The project supports both Euler and Predictor-Corrector finite difference methods and uses modern C++20 features.

## Features

- Parallel Monte Carlo simulation using OpenMP
- Multiple finite difference schemes:
  - Euler method
  - Predictor-Corrector method
- Option types supported:
  - European options (puts and calls)
  - Asian options (puts and calls)
- High-performance Mersenne Twister random number generation
- Comprehensive statistical analysis (price, standard deviation, standard error)
- Automated testing using Google Test framework
- Continuous Integration using GitHub Actions

## Project Structure

### Core Components
- `OptionData.hpp`: Encapsulates option parameters (strike, maturity, rates, volatility)
- `SDEGeneral.hpp`: Implements the stochastic differential equation for price evolution
- `MCCentralHub.hpp`: Coordinates the Monte Carlo simulation process

### Option Pricing
- `Pricer.hpp`: Abstract base class for option pricing
- `EuropeanOptionPricer.hpp`: Implementation of European option pricing
- `AsianOptionPricer.hpp`: Implementation of Asian option pricing with arithmetic averaging

### Numerical Methods
- `FDMType.hpp`: Base class for finite difference methods
- `FDMEuler.hpp`: Euler scheme implementation
- `FDMPredictCorrect.hpp`: Predictor-Corrector scheme implementation

### Random Number Generation
- `RandNumGen.hpp`: Abstract random number generator interface
- `MTEngRandNumGen.hpp`: Mersenne Twister implementation optimized for parallel execution

### Utilities
- `StopWatch.cpp/hpp`: High-precision timing utilities
- `main.cpp`: Example usage and benchmarking

## Building the Project

### Prerequisites
- CMake 3.10 or higher
- C++20 compliant compiler (g++ or clang++)
- OpenMP support
- Git (for fetching GoogleTest)

### Build Instructions

1. Clone the repository:
```bash
git clone <repository-url>
cd monte_carlo_cpp
```

2. Configure with CMake:
```bash
# Using g++
cmake -B build -G Ninja

# Using clang++
cmake -B build -G Ninja -DUSE_CLANG=ON
```

3. Build the project:
```bash
cmake --build build
```

### Running Tests
Execute the test suite:
```bash
cd build
ctest --output-on-failure
```

## Usage Example

```cpp
// Option parameters
OptionData myOption{ 
    .K = 65.0,        // Strike price
    .T = 0.25,        // Time to maturity
    .r = 0.08,        // Risk-free rate
    .sig = 0.3,       // Volatility
    .D = 0.0,         // Dividend rate
    .S_0 = 60.0,      // Initial stock price
    .type = 1,        // Call option
    .H = 0.0,         // No barrier
    .betaCEV = 1.0,   // Standard CEV parameter
    .scale = 1.0      // Standard scale
};

// Simulation parameters
int NT = 1000;    // Number of time steps
int NSIM = 50000; // Number of simulations

// Create pricing engine
auto pricerEuroCall = std::make_shared<EuropeanOptionPricer>(payoffCall, discount);
auto sde = std::make_shared<SDEGeneral>(sdeParams, myOption);
auto fdm = std::make_shared<FDMPredictCorrect>(sde, NT);

// Run simulation
MCCentralHub<SDEGeneral, Pricer, FDMType, MTEngRandNumGen> 
    engine(components, NSIM, NT);
engine.BeginSimulation();

// Get results
double price = pricerEuroCall->OptionPrice();
auto [stdDev, stdError] = pricerEuroCall->StandardDeviationStats();
```

## Performance Considerations
- Utilizes OpenMP for parallel path generation
- SIMD optimizations for numerical operations
- Efficient memory management with pre-allocated paths
- Thread-local random number generation
- Optimized finite difference schemes
- Link Time Optimization (LTO) support
- CPU-specific optimizations with -march=native
- Vectorization-friendly data alignment

## Build Optimizations
- Comprehensive compiler optimization flags
- Support for both GCC and Clang compilers
- Configurable parallel builds
- Cache-friendly data structures
- Automated test discovery

## CI/CD Configuration

### GitHub Actions Workflow

```yaml
name: CI

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  build-and-test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        compiler: [gcc, clang]
        build_type: [Release, Debug]

    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake ninja-build libomp-dev
        
    - name: Configure CMake
      run: |
        cmake -B build -G Ninja \
          -DCMAKE_BUILD_TYPE=${{matrix.build_type}} \
          -DUSE_CLANG=${{matrix.compiler == 'clang'}}
          
    - name: Build
      run: cmake --build build
      
    - name: Test
      working-directory: build
      run: ctest --output-on-failure

  code-quality:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Install analysis tools
      run: |
        sudo apt-get update
        sudo apt-get install -y clang-tidy cppcheck
        
    - name: Static analysis
      run: |
        cppcheck --enable=all --error-exitcode=1 .
        find . -name '*.cpp' -o -name '*.hpp' | xargs clang-tidy
```

## Performance Benchmarks
| Test Case | Time (s) | Memory (MB) | Threads |
|-----------|----------|-------------|---------|
| European Call (50K paths) | ~2.2 | ~50 | 32 |
| European Put (50K paths) | ~2.2 | ~50 | 32 |
| Asian Call (50K paths) | ~2.2 | ~50 | 32 |
| Asian Put (50K paths) | ~2.2 | ~50 | 32 |

*Benchmarks performed on AMD Ryzen 9 5950X, 32 threads, GCC 11.4.0

## Code Quality and Testing
- Comprehensive unit tests using GoogleTest
- Continuous Integration via GitHub Actions
- Static analysis with clang-tidy and cppcheck
- Memory leak detection
- Thread safety validation
- Numerical accuracy verification

## Future Enhancements

1. Additional finite difference schemes
   - Milstein scheme
   - Runge-Kutta methods
   - Stochastic Runge-Kutta

2. Early exercise features
   - American options
   - Bermudan options
   - Optimal exercise boundary calculation

3. Variance reduction techniques
   - Control variates
   - Antithetic variates
   - Stratified sampling
   - Importance sampling
