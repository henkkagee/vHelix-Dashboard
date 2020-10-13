#ifndef _CONTROLLER_DUPLICATE_H_
#define _CONTROLLER_DUPLICATE_H_

#include <mayahelix/Definition.h>

#include <mayahelix/model/Base.h>

#include <maya/MObjectArray.h>

#include <map>
#include <string>

namespace Helix {
	namespace Controller {
		/*
		 * Duplicate controller: Duplicates a given list of helices.
		 * This is a two step process: Duplicate helices + bases and then connect them
		 * in order to do that the first step needs to generate a base relationship table to track the old bases relation to the new bases
		 * thus being able to create the bindings in the new helices
		 */

		class VHELIXAPI Duplicate {
		public:
			std::string duplicate(const MObjectArray & helices);
			std::string undo();
			std::string redo();

		protected:
			/*
			 * Callbacks for progress bars etc.
			 * There are two processes, duplicate and connect
			 */
			virtual void onProgressStart(unsigned int process, unsigned int range);
			virtual void onProgressStep();
			virtual void onProgressDone();

		private:
			/*
			 * They need to be copied here for the redo process to operate on something
			 */

			MObjectArray m_helices;
			std::vector<Model::Helix> m_new_helices;
		};
	}
}

#endif /* N _CONTROLLER_DUPLICATE_H_ */
