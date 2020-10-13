/*
 * OxDnaImporter.h
 *
 *  Created on: Jan 29, 2014
 *      Author: johan
 */

#ifndef CONTROLLER_OXDNAIMPORTER_H_
#define CONTROLLER_OXDNAIMPORTER_H_

#include <mayahelix/Definition.h>
#include <mayahelix/DNA.h>

#include <mayahelix/model/Helix.h>
#include <mayahelix/model/Base.h>


#include <string>
#include <vector>

namespace Helix {
	namespace Controller {
		class VHELIXAPI OxDnaImporter {
		public:
			std::string read(const char *topology_filename, const char *configuration_filename, const char *vhelix_filename);

		protected:
			virtual void onProcessStart(int count) = 0;
			virtual void onProcessStep() = 0;
			virtual void onProcessEnd() = 0;

		private:
			struct Base {
				unsigned int strand;
				int forward, backward;

				DNA::Name label;
				std::vector<double> translation;
				std::string name, helixName, material;
				Model::Base base;
			};

			struct Helix {
				std::vector<double> translation, normal;
				Model::Helix helix;
			};
		};
	}
}


#endif /* CONTROLLER_OXDNAIMPORTER_H_ */
