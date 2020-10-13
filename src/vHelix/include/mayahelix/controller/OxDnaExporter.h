#ifndef _CONTROLLER_OXDNAEXPORTER_H_
#define _CONTROLLER_OXDNAEXPORTER_H_

#include <mayahelix/controller/Operation.h>
#include <mayahelix/model/Strand.h>

#include <mayahelix/DNA.h>

#include <list>
#include <vector>

#include <boost/math/quaternion.hpp>

#if defined(WIN32) || defined(WIN64)
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif /* N Windows */

#define HELIX_OXDNA_CONF_FILE_TYPE "conf"
#define HELIX_OXDNA_TOP_FILE_TYPE "top"
#define HELIX_OXDNA_VHELIX_FILE_TYPE "vhelix"

/*
 * OxDnaExporter: Generates .top and .conf with the strands of the scene that can be used together with oxDNA for simulating
 * DNA models. See https://dna.physics.ox.ac.uk for more information.
 *
 * All strands must have been assigned sequences.
 */
namespace Helix {
	namespace Controller {
		class VHELIXAPI OxDnaExporter : public Operation<Model::Strand> {
		public:

			/*
			 * Writes what is currently stored in m_strands. Use the Operation interface to populate it.
			 */
			std::string write(const char *topology_filename, const char *configuration_filename, const char *vhelix_filename, const std::vector<double> & minTranslation, const std::vector<double> & maxTranslation) const;

		protected:

			virtual std::string doExecute(Model::Strand & element);
			std::string doUndo(Model::Strand & element, Empty & undoData);
			std::string doRedo(Model::Strand & element, Empty & redoData);

		private:
			/*
			 * For exporting a strand we need its sequence (the labels are the only identifiers used) and whether its a circular strand.
			 *
			 */
			struct Base {
				DNA::Name label;
				std::vector<double> translation, normal, tangent; // Tangent is the normalized vector between the helix axis and the base.
				std::string name, helixName, material;
			};

			struct Strand {
				std::string name;
				std::vector<Base> strand;
				bool circular;
			};

			std::list<Strand> m_strands;

			struct Helix {
				std::vector<double> translation, normal;
			};

			std::tr1::unordered_map<std::string, Helix> m_helices;
		};
	}
}

#endif /* _CONTROLLER_OXDNAEXPORTER_H_ */

