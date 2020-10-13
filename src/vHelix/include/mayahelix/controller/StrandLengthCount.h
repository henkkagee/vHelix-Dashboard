#ifndef _CONTROLLER_STRAND_LENGTH_COUNT_H_
#define _CONTROLLER_STRAND_LENGTH_COUNT_H_

#include <mayahelix/Definition.h>

#include <mayahelix/model/Strand.h>

namespace Helix {
	namespace Controller {
		class VHELIXAPI StrandLengthCount {
		public:
			unsigned int length(Model::Strand & strand);
		};
	}
}

#endif /* _CONTROLLER_STRAND_LENGTH_COUNT_H_ */
