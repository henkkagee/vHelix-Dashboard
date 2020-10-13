/*
 * ExtendGui.h
 *
 *  Created on: Jul 26, 2011
 *      Author: bjorn
 */

#ifndef EXTENDGUI_H_
#define EXTENDGUI_H_

/*
 * Shows a confirmDialog in MEL that then calls the extendHelix method
 */

#include <mayahelix/Definition.h>

#include <iostream>

#include <maya/MPxCommand.h>

#define MEL_EXTENDSTRAND_GUI_COMMAND "extendStrand_gui"

namespace Helix {
	class VHELIXAPI ExtendGui : public MPxCommand {
	public:
		ExtendGui();
		virtual ~ExtendGui();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static void *creator();
		static MSyntax newSyntax();
	};
}

#endif /* EXTENDGUI_H_ */
