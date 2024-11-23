#ifndef CentralHub_HPP
#define CentralHub_HPP

#include <memory>
#include <vector>
#include <iostream>
#include "SDEGeneral.hpp"
#include "Pricer.hpp"
#include "FDMType.hpp"
#include "MTEngRandNumGen.hpp"

template<typename SDEGeneral, typename Pricer, typename FDMType, typename MTEngRandNumGen>
class MCCentralHub {
private:
    std::shared_ptr<SDEGeneral> sde;
    std::shared_ptr<Pricer> pricer;
    std::shared_ptr<FDMType> fdm;
    std::shared_ptr<MTEngRandNumGen> randGen;
    int NumSim;
    int PathSize;
    std::vector<double> path;

public:
    MCCentralHub(const std::tuple<std::shared_ptr<SDEGeneral>, std::shared_ptr<Pricer>, 
                 std::shared_ptr<FDMType>, std::shared_ptr<MTEngRandNumGen>>& pieces, 
                 int numSimulations, int numTime) 
        : sde(std::get<0>(pieces))
        , pricer(std::get<1>(pieces))
        , fdm(std::get<2>(pieces))
        , randGen(std::get<3>(pieces))
        , NumSim(numSimulations)
        , PathSize(numTime + 1)
        , path(PathSize)
    {
        std::cout << "Constructor: Checking pointers..." << std::endl;
        if (!sde) throw std::runtime_error("SDE is null");
        if (!pricer) throw std::runtime_error("Pricer is null");
        if (!fdm) throw std::runtime_error("FDM is null");
        if (!randGen) throw std::runtime_error("RandGen is null");
        
        std::cout << "Constructor: Parameters validated" << std::endl;
        std::cout << "NumSim: " << NumSim << ", PathSize: " << PathSize << std::endl;
    }

    void BeginSimulation() {
        try {
            std::cout << "BeginSimulation: Starting..." << std::endl;
            
            if (!sde || !sde->data) {
                throw std::runtime_error("SDE or SDE data is null");
            }
            
            const double S_0 = sde->data->S_0;
            std::cout << "Initial price (S_0): " << S_0 << std::endl;
            
            const double dt = fdm->getTimeStep();
            std::cout << "Time step (dt): " << dt << std::endl;

            const auto& timePoints = fdm->getTimePoints();
            std::cout << "First few time points: ";
            for (int i = 0; i < std::min(5, static_cast<int>(timePoints.size())); ++i) {
                std::cout << timePoints[i] << " ";
            }
            std::cout << std::endl;

            for (int i = 0; i < NumSim; ++i) {
                if (i % 10000 == 0) {
                    std::cout << "Processing simulation " << i << std::endl;
                }
                
                path[0] = S_0;
                double VOld = S_0;
                
                for (int j = 1; j < PathSize; ++j) {
                    const double t = fdm->getTimePoint(j - 1);
                    const double normVar = randGen->GenerateRandNum();
                    const double normVar2 = randGen->GenerateRandNum();
                    
                    const double VNew = fdm->next_n(VOld, t, dt, normVar, normVar2);
                    path[j] = VNew;
                    VOld = VNew;
                }
                
                pricer->GeneratePath(path);
            }
            
            std::cout << "All simulations completed" << std::endl;
            pricer->AfterPathCleanUp();
            std::cout << "Cleanup completed" << std::endl;
            
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in BeginSimulation: " << e.what() << std::endl;
            throw;
        }
        catch (...) {
            std::cerr << "Unknown exception in BeginSimulation" << std::endl;
            throw;
        }
    }
};

#endif
