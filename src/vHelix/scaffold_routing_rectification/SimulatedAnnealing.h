#ifndef _SIMULATED_ANNEALING_H_
#define _SIMULATED_ANNEALING_H_

#include <cmath>
#include <iostream>

// Pseudo code from http://en.wikipedia.org/wiki/Simulated_annealing#Pseudocode

template<typename StateT, typename EnergyFunctorT, typename TemperatureFunctorT, typename NeighborFunctorT, typename ProbabilityFunctorT>
StateT simulated_annealing(StateT state, EnergyFunctorT energy_functor, TemperatureFunctorT temperature_functor, NeighborFunctorT neighbor_functor, ProbabilityFunctorT probability_functor, int kmax, decltype(energy_functor(state)) emax) {
	typedef decltype(energy_functor(state)) EnergyT;
	typedef decltype(temperature_functor(kmax)) TemperatureT;
	StateT sbest(state);
	EnergyT e(energy_functor(state));
	EnergyT ebest(e);

	for (int k = 0; k < kmax && e > emax; ++k) {
		const TemperatureT T(temperature_functor(float(k) / kmax));
		StateT snew(neighbor_functor(state));
		const EnergyT enew(energy_functor(snew));

		if (probability_functor(e, enew, T) > float(rand()) / RAND_MAX) {
			state = snew;
			e = enew;
		}

		if (enew < ebest) {
			sbest = snew;
			ebest = enew;
		}
	}

	return sbest;
}

// Alternative version where state is modified without copying then reverted if not chosen. This suits our simulation better.

template<typename StateT, typename EnergyFunctorT, typename TemperatureFunctorT, typename NeighborFunctorT, typename ProbabilityFunctorT, typename RevertFunctorT, typename StoreBestFunctorT, typename RunningFunctorT>
void simulated_annealing(
		StateT & state, EnergyFunctorT energy_functor, TemperatureFunctorT temperature_functor, NeighborFunctorT neighbor_functor, ProbabilityFunctorT probability_functor, RevertFunctorT revert_functor, StoreBestFunctorT store_best_functor, RunningFunctorT running_functor, int kmax, decltype(energy_functor(state)) emax) {
	typedef decltype(energy_functor(state)) EnergyT;
	typedef decltype(temperature_functor(float(0))) TemperatureT;
	EnergyT e(energy_functor(state));
	store_best_functor(state, e);
	EnergyT ebest(e);

	for (int k = 0; k < kmax && e > emax && running_functor(); ++k) {
		const TemperatureT T(temperature_functor(float(k) / kmax));
		neighbor_functor(state);
		const EnergyT enew(energy_functor(state));

		if (probability_functor(e, enew, T) > float(rand()) / RAND_MAX)
			e = enew;
		else
			revert_functor(state);

		//std::cerr << "e: " << enew << std::endl;
		if (enew < ebest) {
			store_best_functor(state, enew);
			ebest = enew;
		}
	}
}

// Pseudo code from http://en.wikipedia.org/wiki/Simulated_annealing#Acceptance_probabilities_2

template<typename EnergyT, typename TemperatureT>
struct probability_functor : std::function<float(EnergyT, EnergyT, TemperatureT)> {
	inline float operator()(const EnergyT & e, const EnergyT & enew, const TemperatureT & T) const {
		return enew < e ? result_type(1) : exp(result_type(e - enew) / T);
	}
};

#endif /* _SIMULATED_ANNEALING_H_ */