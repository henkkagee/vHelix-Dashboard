#ifndef _TEXTBASEDCONTROLLER_H_
#define _TEXTBASEDCONTROLLER_H_

#include <mayahelix/DNA.h>
#include <mayahelix/Utility.h>

#include <mayahelix/model/Base.h>
#include <mayahelix/model/Helix.h>

#if defined(WIN32) || defined(WIN64)
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif /* N Windows */

#include <vector>

#include <boost/math/quaternion.hpp>

namespace Helix {
	namespace Controller {
        class TextBasedImporter {
		public:
            std::string read(const char *filename, int nicking_min_length = 0, int nicking_max_length = 0);

		private:
			struct Base {
				std::string name, helixName, materialName;
                std::vector<double> position;
				DNA::Name label;

                inline Base(const char *name, const std::vector<double> & position, const char *materialName, const DNA::Name & label) : name(name), materialName(materialName), position(position), label(label) {}
				inline Base() {}
			};

			struct Helix {
                std::vector<double> position;
                std::vector<double> orientation;
				std::string name;
				unsigned int bases; // Bases automatically added with the 'hb' command.

                inline Helix(const std::vector<double> & position, const std::vector<double> & orientation, const char *name, unsigned int bases = 0) : position(position), orientation(orientation), name(name), bases(bases) {}
				inline Helix() : bases(0) {}
				inline bool operator==(const char *name) const { return this->name == name; }
			};

			struct Connection {
				enum Type {
					kForwardThreePrime,
					kForwardFivePrime,
					kBackwardThreePrime,
					kBackwardFivePrime,
					kNamed,
					kInvalid
				} fromType, toType;

				std::string fromHelixName, toHelixName, fromName, toName; // Only used when fromType/toType are kNamed.

				inline Connection(const char *fromHelixName, const char *fromName, const char *toHelixName, const char *toName, Type fromType, Type toType) : fromType(fromType), toType(toType), fromHelixName(fromHelixName), toHelixName(toHelixName), fromName(fromName), toName(toName) {}

                static Type TypeFromString(const char *type);
			};

#if defined(WIN32) || defined(WIN64)
			typedef std::unordered_map<std::string, DNA::Name> explicit_base_labels_t;
#else
			typedef std::tr1::unordered_map<std::string, DNA::Name> explicit_base_labels_t;
#endif /* Not windows */

			std::vector<Helix> helices;
			std::vector<Connection> connections;
			std::vector<Base> explicitBases; // Bases explicitly created with the 'b' command.
			explicit_base_labels_t explicitBaseLabels;

            friend std::string getBaseFromConnectionType(Model::Helix & helix, Connection::Type type, Model::Base & base);
		};
	}
}

#endif /* N _TEXTBASEDCONTROLLER_H_ */
