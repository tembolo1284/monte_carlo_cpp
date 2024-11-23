#ifndef CentralHub_HPP
#define CentralHub_HPP

#include <memory>
#include <vector>
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
        , path(static_cast<size_t>(PathSize))
    {}

    void BeginSimulation() {
        const double S_0 = sde->data->S_0;
        const double dt = fdm->getTimeStep();
        
        // Print first few time points
        const auto& timePoints = fdm->getTimePoints();
        std::cout << "First few time points: ";
        for (size_t i = 0; i < std::min(static_cast<size_t>(5), timePoints.size()); ++i) {
            std::cout << timePoints[i] << " ";
        }
        std::cout << std::endl;

        for (int i = 0; i < NumSim; ++i) {
            if (i % 5000 == 0) {
                std::cout << "Processing simulation " << i << std::endl;
            }
            
            path[0] = S_0;
            double VOld = S_0;
            
            for (int j = 1; j < PathSize; ++j) {
                const double t = fdm->getTimePoint(static_cast<size_t>(j - 1));
                const double normVar = randGen->GenerateRandNum();
                const double normVar2 = randGen->GenerateRandNum();
                
                const double VNew = fdm->next_n(VOld, t, dt, normVar, normVar2);
                path[static_cast<size_t>(j)] = VNew;
                VOld = VNew;
            }
            
            pricer->GeneratePath(path);
        }
        
        pricer->AfterPathCleanUp();
    }
};

#endif
