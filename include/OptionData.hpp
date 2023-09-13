#ifndef OptionData_HPP
#define OptionData_HPP
#include <algorithm> // for max()
using namespace std;

// Encapsulate all data in one place
struct OptionData { // Option data + behaviour
	double K;
	double T;
	double r;
	double sig;
	double D;		// dividend
	double S_0;  //initial condition
	int type;		// 1 == call, -1 == put
	// Extra data 
	double H;		// down and out barrier
	double betaCEV;	// elasticity factor (CEV model)
	double scale;	// scale factor in CEV model
	/*double myPayOffFunction(double S) { // Payoff function
		if (type == 1) { // Call
			return max(S - K, 0.0);
		}
		else { // Put
			return max (K - S, 0.0);
		}
	}*/
};
#endif