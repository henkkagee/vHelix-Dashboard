/*
 * PaintStrand.h
 *
 *  Created on: 8 mar 2012
 *      Author: johan
 */

#ifndef _CONTROLLER_DISCONNECT_H_
#define _CONTROLLER_DISCONNECT_H_

#include <mayahelix/controller/Operation.h>
#include <mayahelix/controller/PaintStrand.h>

#include <mayahelix/model/Material.h>
#include <mayahelix/model/Base.h>
#include <mayahelix/model/Strand.h>

/*
 * Manages disconnection of bases
 */

namespace Helix {
	namespace Controller {
		// The ElementT (first argument) stores the source base in a connection (i.e. it's forward attribute will be connected), the UndoT stores the original destination, if any, to be restored if an undo is requested. No data needs to be save for the redo process (just disconnect the ElementT
		class VHELIXAPI Disconnect : public Operation<Model::Base, Model::Base, Empty> {
		protected:
			std::string doExecute(Model::Base & element);
			std::string doUndo(Model::Base & element, Model::Base & undoData);
			std::string doRedo(Model::Base & element, Empty & redoData);
		};
	}
}

#endif /* N _CONTROLLER_DISCONNECT_H_ */
