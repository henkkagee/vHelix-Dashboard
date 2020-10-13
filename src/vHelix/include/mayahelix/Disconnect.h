/*
 * Disconnect.h
 *
 *  Created on: 10 jul 2011
 *      Author: johan
 */

#ifndef DISCONNECT_H_
#define DISCONNECT_H_

/*
 * Command for disconnecting helix bases, the selected base will have its forward attribute connection removed
 */

#include <mayahelix/Definition.h>

#include <iostream>
#include <vector>

#include <maya/MPxCommand.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>

#include <mayahelix/controller/Disconnect.h>

#define MEL_DISCONNECTBASE_COMMAND "disconnectBase"

namespace Helix {
	class VHELIXAPI Disconnect : public MPxCommand {
	public:
		Disconnect();
		virtual ~Disconnect();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static MSyntax newSyntax ();
		static void *creator();

	private:
		Controller::Disconnect m_operation;
		Controller::PaintMultipleStrandsWithNewColorFunctor m_functor;
	};
}

#endif /* DISCONNECT_H_ */
