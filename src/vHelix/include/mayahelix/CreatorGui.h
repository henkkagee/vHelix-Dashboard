/*
 * CreateHelixGui.h
 *
 *  Created on: Jul 26, 2011
 *      Author: bjorn
 */

#ifndef CREATORGUI_H_
#define CREATORGUI_H_

/*
 * Shows a confirmDialog in MEL that then calls the createHelix method
 */

#include <mayahelix/Definition.h>

#include <iostream>

#include <maya/MPxCommand.h>

#define MEL_CREATEHELIX_GUI_COMMAND "createHelix_gui"

namespace Helix {
	class VHELIXAPI CreatorGui : public MPxCommand {
	public:
		CreatorGui();
		virtual ~CreatorGui();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static void *creator();
	};
}

#endif /* CREATORGUI_H_ */
