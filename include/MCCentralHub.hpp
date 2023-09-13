#ifndef CentralHub_HPP
#define CentralHub_HPP
//monte carlo central hub coordinator
#include <memory>
#include <vector>
//#include "SDEAbstract.hpp"
//#include "SDEGBM.hpp"
#include "SDEGeneral.hpp"
#include "Pricer.hpp"
#include "FDMType.hpp"
#include "FDMEuler.hpp"
//#include "RandNumGen.hpp"
#include "MTEngRandNumGen.hpp"


template<typename SDEGeneral, typename Pricer, typename FDMType, typename MTEngRandNumGen>
class MCCentralHub {

private:
    std::shared_ptr<SDEGeneral> sde;
    std::shared_ptr<Pricer> pricer;
    std::shared_ptr<FDMType> fdm;
    std::shared_ptr<MTEngRandNumGen> randGen;

    int NumSim;
    std::vector<double> path;

public:
    /*
    MCCentralHub(const std::shared_ptr<SDEGeneral>& sdeInput, const std::shared_ptr<Pricer>& pxInput,
        const std::shared_ptr<FDMType>& fdmInput, const std::shared_ptr<MTEngRandNumGen>& rgInput, int numSimulations, int numTime) : sde(sdeInput),
        pricer(pxInput), fdm(fdmInput), randGen(rgInput) {
        NumSim = numSimulations;
        path = std::vector<double>(numTime + 1);
    } */
    //new constructor using tuple
    MCCentralHub(const std::tuple<std::shared_ptr<SDEGeneral>, std::shared_ptr<Pricer>, std::shared_ptr<FDMType>,
                std::shared_ptr<MTEngRandNumGen>>& pieces, int numSimulations, int numTime) : sde(std::get<0>(pieces)), 
                pricer(std::get<1>(pieces)), fdm(std::get<2>(pieces)), randGen(std::get<3>(pieces)) {
        NumSim = numSimulations;
        path = std::vector<double>(numTime + 1);
    }

    void BeginSimulation() {
        double VOld{ 0.0 };
        double VNew{ 0.0 };

        for (int i = 1; i < NumSim; i++) {
            VOld = sde->data->S_0; //set VOld to initial price, S_0
            path[0] = VOld;

            //if ((i%10000) == 0) {
            //    std::cout << "i is at: " << i << "th iteration." << std::endl;
           //}

            for (int j = 1; j < path.size(); j++) {// I move fwd to j+1 and solve for VNew
                //double x_n, double t_n, double dt, double normVar, double normVar2
                VNew = fdm->next_n(VOld, fdm->x[j - 1], fdm->m, randGen->GenerateRandNum(), randGen->GenerateRandNum());
                path[j] = VNew;
                VOld = VNew;
            }
            pricer->GeneratePath(path);
            pricer->AfterPathCleanUp();
        }
    }
};

#endif
