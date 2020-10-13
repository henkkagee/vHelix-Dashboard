#ifndef _CONTROLLER_APPLYSEQUENCE_H_
#define _CONTROLLER_APPLYSEQUENCE_H_

#include <mayahelix/controller/Operation.h>
#include <mayahelix/model/Base.h>

#include <mayahelix/DNA.h>

namespace Helix {
	namespace Controller {
		class VHELIXAPI ApplySequence : public Operation<Model::Base, DNA::Name, DNA::Name> {
		public:
			inline void setSequence(const std::string & sequence) {
				m_sequence = sequence;
				m_sequence_index = 0;
			}

		protected:
			std::string doExecute(Model::Base & element);
			std::string doUndo(Model::Base & element, DNA::Name & undoData);
			std::string doRedo(Model::Base & element, DNA::Name & redoData);

		private:
			std::string m_sequence;
			unsigned int m_sequence_index;
		};
	}
}

#endif /* N _CONTROLLER_APPLYSEQUENCE_H_ */
