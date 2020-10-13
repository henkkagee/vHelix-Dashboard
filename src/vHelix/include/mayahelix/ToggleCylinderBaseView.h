/*
 * HelixView.h
 *
 *  Created on: 14 jul 2011
 *      Author: johan
 */

#ifndef TOGGLECYLINDERBASEVIEW_H_
#define TOGGLECYLINDERBASEVIEW_H_

/*
 * Really simple command, returns the currently set view and allows changing it.
 * The result will be passed out as an std::string
 * If helices (or children of helices) are selected, no state change is made but these have their view toggled individually
 * also -target will have the same effect
 */

#include <mayahelix/Definition.h>

#include <maya/MPxCommand.h>
#include <maya/MObjectArray.h>

#include <list>

#define MEL_TOGGLECYLINDERBASEVIEW_COMMAND "toggleCylinderBaseView"

namespace Helix {
	class VHELIXAPI ToggleCylinderBaseView : public MPxCommand {
	public:
		ToggleCylinderBaseView();
		virtual ~ToggleCylinderBaseView();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static MSyntax newSyntax ();
		static void *creator();

		// This is the current status, can be accessed from the outside
		//

		static int CurrentView;

	private:
		std::string toggle(bool toggle, bool refresh, std::list<MObject> & targets);

		bool m_toggled; // For undo/redo
		std::list<MObject> m_toggleTargets;
	};
}


#endif /* HELIXVIEW_H_ */
