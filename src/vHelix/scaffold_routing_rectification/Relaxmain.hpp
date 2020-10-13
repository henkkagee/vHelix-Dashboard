#ifndef MAIN_HPP
#define MAIN_HPP

#include "Definition.h"
#include "DNA.h"
#include "Utility.h"
#include "ParseSettings.h"
#include "Scene.h"
#include "SimulatedAnnealing.h"

#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "Physics.h"

static volatile bool running = true;

namespace relaxation {
/*
 * Increases/decreases the base counts of individual helices with min(minbasecount, basecount + [ -baserange, baserange ]), and evaluates the new energy level of the system by simulated annealing.
 */
template<typename StoreBestFunctorT, typename RunningFunctorT>
void simulated_annealing(scene & mesh, physics & phys, int kmax, float emax, unsigned int minbasecount, int baserange,
        StoreBestFunctorT store_best_functor, RunningFunctorT running_functor);


/*
 * Simple gradient descent implementation: Energy lower? Choose it, if not don't.
 */
template<typename StoreBestFunctorT, typename RunningFunctorT>
void gradient_descent(scene & mesh, physics & phys, int minbasecount, StoreBestFunctorT store_best_functor, RunningFunctorT running_functor);



static void handle_exit();

/*
 * Does a simple rectification of the structure without modification.
 */
template<typename RunningFunctorT>
SceneDescription simulated_rectification(scene & mesh, physics & phys, RunningFunctorT running_functor);



static int relax_main(int argc, const char **argv, vHelix &parent);

}
#endif // MAIN_HPP
