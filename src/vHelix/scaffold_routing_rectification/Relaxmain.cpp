#include "Relaxmain.h"


/*
 * Increases/decreases the base counts of individual helices with min(minbasecount, basecount + [ -baserange, baserange ]), and evaluates the new energy level of the system by simulated annealing.
 */


template<typename StoreBestFunctorT, typename RunningFunctorT>
void relaxation::simulated_annealing(scene & mesh, physics & phys, int kmax, float emax, unsigned int minbasecount, int baserange,
		StoreBestFunctorT store_best_functor, RunningFunctorT running_functor) {

	int modifiedHelix, previousBaseCount;
	physics::transform_type previousTransform;

	scene::HelixContainer & helices(mesh.getHelices());
	const scene::HelixContainer::size_type helixCount(helices.size());

	simulated_annealing(mesh,
		[](scene & mesh) { return mesh.getTotalSeparation(); },
		[&helixCount](float k) { return float(std::max(0., (exp(-k) - 1 / M_E) / (1 - 1 / M_E))) * helixCount; },
		[&modifiedHelix, &helices, &helixCount, &previousBaseCount, &previousTransform, &phys, &minbasecount, &baserange, &running_functor](scene & mesh) {
            for (Phys::Helix & helix : helices)
				helix.setTransform(helix.getInitialTransform());

			modifiedHelix = rand() % helixCount;
            Phys::Helix & helix(helices[modifiedHelix]);
			previousBaseCount = helix.getBaseCount();
			previousTransform = helix.getTransform();

			helix.recreateRigidBody(
				phys, std::max(minbasecount, helix.getInitialBaseCount() + (rand() % 2 * 2 - 1) * (1 + rand() % (baserange))), helix.getInitialTransform());

			while (!mesh.isSleeping() && running_functor()) {
				phys.scene->simulate(1.0f / 60.0f);
				phys.scene->fetchResults(true);
			}
		},
		probability_functor<float, float>(),
		[&modifiedHelix, &helices, &previousBaseCount, &previousTransform, &phys](scene & mesh) {
            Phys::Helix & helix(helices[modifiedHelix]);
			helix.recreateRigidBody(phys, previousBaseCount, helix.getInitialTransform());
		},
		store_best_functor,
		running_functor,
		kmax, emax);
}

/*
 * Simple gradient descent implementation: Energy lower? Choose it, if not don't.
 */
template<typename StoreBestFunctorT, typename RunningFunctorT>
void relaxation::gradient_descent(scene & mesh, physics & phys, int minbasecount, StoreBestFunctorT store_best_functor, RunningFunctorT running_functor, const int &iterations, vHelix &parent) {
	scene::HelixContainer & helices(mesh.getHelices());

    while (!mesh.isSleeping() && running_functor()) {
		phys.scene->simulate(1.0f / 60.0f);
		phys.scene->fetchResults(true);
	}

	//physics::real_type separation(mesh.getTotalSeparation());
	physics::real_type min, max, average, total;
	mesh.getTotalSeparationMinMaxAverage(min, max, average, total);
    store_best_functor(mesh, min, max, average, total, parent);
    if (iterations == 1) {
        return;
    }
    int k = 1;

    for (Phys::Helix & helix : helices) {
		for (int i = 0; i < 2; ++i) {
            if (!running_functor() || k >= iterations)
				return;

			helix.recreateRigidBody(phys, std::max(minbasecount, int(helix.getInitialBaseCount() + (i * 2 - 1))), helix.getInitialTransform());

			while (!mesh.isSleeping() && running_functor()) {
				phys.scene->simulate(1.0f / 60.0f);
				phys.scene->fetchResults(true);
			}

			//const physics::real_type newseparation(mesh.getTotalSeparation());
			physics::real_type newtotal;
			mesh.getTotalSeparationMinMaxAverage(min, max, average, newtotal);

			if (newtotal < total) {
				total = newtotal;
                store_best_functor(mesh, min, max, average, total, parent);
			} else
				helix.recreateRigidBody(phys, helix.getInitialBaseCount(), helix.getInitialTransform());

            for (Phys::Helix & helix : helices)
				helix.setTransform(helix.getInitialTransform());
            k++;
		}
	}
}

static void relaxation::handle_exit() {
	running = false;
}

/*
 * Does a simple rectification of the structure without modification.
 */
template<typename RunningFunctorT>
SceneDescription relaxation::simulated_rectification(scene & mesh, physics & phys, RunningFunctorT running_functor) {
	while (!mesh.isSleeping() && running_functor()) {
		phys.scene->simulate(1.0f / 60.0f);
		phys.scene->fetchResults(true);
	}

	return SceneDescription(mesh);
}


int relaxation::relax_main(std::string input_file, std::string output_file, const double *dblArgs,
                           const int &iterations, const bool *boolArgs, vHelix &parent) {
	seed();

	physics::settings_type physics_settings;
	scene::settings_type scene_settings;
    Phys::Helix::settings_type helix_settings;

    //parse_settings::parse(argc, argv, physics_settings, scene_settings, helix_settings, input_file, output_file);
    scene_settings.initial_scaling = dblArgs[0];
    physics_settings.kRestitution = dblArgs[7];
    physics_settings.kStaticFriction = dblArgs[5];
    physics_settings.visual_debugger = boolArgs[2];
    physics_settings.kDynamicFriction = dblArgs[6];
    physics_settings.kRigidBodySleepThreshold = dblArgs[8];
    scene_settings.discretize_lengths = boolArgs[0];
    helix_settings.density = dblArgs[1];
    helix_settings.attach_fixed = boolArgs[1];
    helix_settings.spring_damping = dblArgs[4];
    helix_settings.spring_stiffness = dblArgs[2];
    helix_settings.fixed_spring_stiffness = dblArgs[3];

    /*if (input_file.empty() || output_file.empty() || argc < 3) {
		std::cerr << parse_settings::usage(argv[0]) << std::endl;
		return 0;
    }*/
    std::string str;
    std::stringstream info;
    if (input_file.empty()) {
        info << "Relaxation input file empty (.ply)\n";
        str = info.str();
        str.insert(0, "POPUP_ERR");
        parent.sendToConsole_(str.c_str());
        info.str(std::string());
        return 0;
    }
    if (output_file.empty()) {
        info << "Relaxation output file not specified (.rpoly)\n";
        str = info.str();
        str.insert(0, "POPUP_ERR");
        parent.sendToConsole_(str.c_str());
        info.str(std::string());
        return 0;
    }

	scene mesh(scene_settings, helix_settings);
	physics phys(physics_settings);

	try {
		if (!mesh.read(phys, input_file)) {
            info << "Failed to read scene \"" << input_file << "\"" << std::endl;
            str = info.str();
            str.insert(0, "POPUP_ERR");
            parent.sendToConsole_(str.c_str());
            info.str(std::string());
			return 1;
		}
    }
	catch (const std::runtime_error & e) {
        info << "Failed to read scene \"" << input_file << "\": " << e.what() << std::endl;
        str = info.str();
        str.insert(0, "POPUP_ERR");
        parent.sendToConsole_(str.c_str());
        info.str(std::string());
		return 1;
    }

	physics::real_type initialmin, initialmax, initialaverage, initialtotal, min, max, average, total;
	mesh.getTotalSeparationMinMaxAverage(initialmin, initialmax, initialaverage, initialtotal);

    info << "Running simulation for scene loaded from \"" << input_file << " outputting to " << output_file << "\"." << std::endl
		<< "Initial: min: " << initialmin << ", max: " << initialmax << ", average: " << initialaverage << ", total: " << initialtotal << " nm" << std::endl
		<< "Connect with NVIDIA PhysX Visual Debugger to " << PVD_HOST << ':' << PVD_PORT << " to visualize the progress. " << std::endl
		<< "Press ^C to stop the relaxation...." << std::endl;
    str = info.str(); parent.sendToConsole_(str.c_str()); info.str(std::string());

    //setinterrupthandler<handle_exit>();

	SceneDescription best_scene;
#if 0
	best_scene = simulated_rectification(mesh, phys, []() { return running; });
#else
#if 0
	simulated_annealing(mesh, phys, 100, 0, 7, 1,
		[&best_scene](scene & mesh, float e) { std::cerr << "Store best energy: " << e << std::endl; best_scene = SceneDescription(mesh); },
		[]() { return running; });
#else
	gradient_descent(mesh, phys, 7,
        [&best_scene, &min, &max, &average, &total](scene & mesh, physics::real_type min_, physics::real_type max_, physics::real_type average_, physics::real_type total_, vHelix &parent) { min = min_; max = max_; average = average_; total = total_; std::stringstream sstr; sstr << "State: min: " << min << ", max: " << max << ", average: " << average << " total: " << total << " nm" << std::endl; std::string str = sstr.str(); parent.sendToConsole_(str); best_scene = SceneDescription(mesh); },
        []() { return running; }, iterations, parent);
#endif
#endif

    info << "Result: min: " << min << ", max: " << max << ", average: " << average << ", total: " << total << " nm" << std::endl;
    str = info.str(); parent.sendToConsole_(str.c_str()); info.str(std::string());
	{
		std::ofstream outfile(output_file);
		outfile << "# Relaxation of original " << input_file << " file. " << mesh.getHelixCount() << " helices." << std::endl
			<< "# Total separation: Initial: min: " << initialmin << ", max: " << initialmax << ", average: " << initialaverage << ", total: " << initialtotal << " nm" << ", final: min: " << min << ", max: " << max << ", average: " << average << ", total: " << total << " nm" << std::endl;

        if (!best_scene.write(outfile)) {
            info << "Failed to write resulting mesh to \"" << output_file << "\"" << std::endl;
            str = info.str();
            str.insert(0, "POPUP_ERR");
            parent.sendToConsole_(str.c_str());
        }
		outfile.close();
	}

	sleepms(2000);

	return 0;
}
