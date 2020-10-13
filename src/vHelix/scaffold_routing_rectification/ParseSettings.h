#ifndef _PARSE_SETTINGS_H_
#define _PARSE_SETTINGS_H_

#include "Helix.h"
#include "Scene.h"

#include <cassert>
#include <sstream>

/*
 * There were some issues with popt on windows.
 */

class parse_settings {

private:
	struct string_to_bool : public std::unary_function<const char *, bool> {
		inline bool operator()(const char *str) const {
			return stricmp("true", str) == 0;
		}
	};

	template<typename T>
	struct identity : public std::unary_function<T, T> {
		inline T operator()(T && t) const {
			return std::forward<T>(t);
		}
	};

public:

    static void parse(int argc, const char **argv, physics::settings_type & physics_settings, scene::settings_type & scene_settings, Phys::Helix::settings_type & helix_settings, std::string & input_file, std::string & output_file) {

		physics_settings.kStaticFriction = physics::real_type(0.5);
		physics_settings.kDynamicFriction = physics::real_type(0.5);
		physics_settings.kRestitution = physics::real_type(1.0);
		physics_settings.kRigidBodySleepThreshold = physics::real_type(0.001);
		physics_settings.visual_debugger = true;
		scene_settings.initial_scaling = physics::real_type(1.0);
		scene_settings.discretize_lengths = true;
		helix_settings.attach_fixed = true;
		helix_settings.density = physics::real_type(10);
		helix_settings.spring_stiffness = physics::real_type(100);
		helix_settings.fixed_spring_stiffness = physics::real_type(1000);
		helix_settings.spring_damping = physics::real_type(100);

		parse_settings parser( {
			make_argument("scaling", scene_settings.initial_scaling, std::ptr_fun(&atof)),
			make_argument("discretize_lengths", scene_settings.discretize_lengths, string_to_bool()),

			make_argument("density", helix_settings.density, std::ptr_fun(&atof)),
			make_argument("spring_stiffness", helix_settings.spring_stiffness, std::ptr_fun(&atof)),
			make_argument("fixed_spring_stiffness", helix_settings.fixed_spring_stiffness, std::ptr_fun(&atof)),
			make_argument("spring_damping", helix_settings.spring_damping, std::ptr_fun(&atof)),
			make_argument("attach_fixed", helix_settings.attach_fixed, string_to_bool()),

			make_argument("static_friction", physics_settings.kStaticFriction, std::ptr_fun(&atof)),
			make_argument("dynamic_friction", physics_settings.kDynamicFriction, std::ptr_fun(&atof)),
			make_argument("restitution", physics_settings.kRestitution, std::ptr_fun(&atof)),
			make_argument("rigid_body_sleep_threshold", physics_settings.kRigidBodySleepThreshold, std::ptr_fun(&atof)),
			make_argument("visual_debugger", physics_settings.visual_debugger, string_to_bool()),

			make_argument("input", input_file, identity<std::string>()),
			make_argument("output", output_file, identity<std::string>())
		}, argc, argv );
	}

	static std::string usage(const char *name) {
		std::stringstream ss;

		ss << "Usage: " << name << std::endl
			<< "\t--input=<filename>" << std::endl
			<< "\t--output=<filename>" << std::endl
			<< "\t[ --scaling=<decimal> ]" << std::endl
			<< "\t[ --discretize_lengths=<true|false> ]" << std::endl
			<< "\t[ --density=<decimal> ]" << std::endl
			<< "\t[ --spring_stiffness=<decimal> ]" << std::endl
			<< "\t[ --fixed_spring_stiffness=<decimal> ]" << std::endl
			<< "\t[ --spring_damping=<decimal> ]" << std::endl
			<< "\t[ --attach_fixed=<true|false> ]" << std::endl
			<< "\t[ --static_friction=<decimal> ]" << std::endl
			<< "\t[ --dynamic_friction=<decimal> ]" << std::endl
			<< "\t[ --restitution=<decimal> ]" << std::endl
			<< "\t[ --rigid_body_sleep_threshold=<decimal> ]" << std::endl
			<< "\t[ --visual_debugger=<true|false> ] " << std::endl;
		return ss.str();
	}

private:

	struct argument {
		const char *name;
		virtual void parse(const char *value) const = 0;
	};

	template<typename T, typename ConvertFunctorT>
	struct argument_impl : public argument {
		void parse(const char *value) const {
			target = T(functor(value));
		}

		T & target;
		ConvertFunctorT functor;

		inline argument_impl(const char *name, T & target, ConvertFunctorT && functor) : target(target), functor(std::forward<ConvertFunctorT>(functor)) { this->name = name; }
	};

	template<typename T, typename ConvertFunctorT>
	static argument_impl<T, ConvertFunctorT> *make_argument(const char *name, T & target, ConvertFunctorT && functor) {
		return new argument_impl<T, ConvertFunctorT>(name, target, std::forward<ConvertFunctorT>(functor));
	}

	std::vector<argument *> args;

	inline parse_settings(const std::initializer_list<argument *> & args, int argc, const char **argv) {
		for (int i = 1; i < argc; ++i) {
			const char *kslfds = argv[i];
			const char *equal(strchr(argv[i], '='));
			if (equal && argv[i][0] == '-' && argv[i][1] == '-') {
				for (argument *arg : args) {
					if (strncmp(arg->name, argv[i] + 2, equal - argv[i] - 2) == 0)
						arg->parse(equal + 1);
				}
			}
        }
	}

	~parse_settings() {
		for (std::vector<argument *>::iterator it(args.begin()); it != args.end(); ++it)
			delete *it;
	}
};

#endif /* _PARSE_SETTINGS_H_ */
