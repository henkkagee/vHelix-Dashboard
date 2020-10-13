/*
 * Connect.h
 *
 *  Created on: 10 jul 2011
 *      Author: johan
 */

#ifndef CONNECT_H_
#define CONNECT_H_

/*
 * Command for connecting helix bases, disconnects old bindings if required
 */

#include <mayahelix/Definition.h>

#include <mayahelix/controller/Connect.h>
#include <mayahelix/controller/PaintStrand.h>

#include <iostream>

#include <maya/MPxCommand.h>
#include <maya/MObject.h>

#define MEL_CONNECTBASES_COMMAND "connectBases"

namespace Helix {
	class VHELIXAPI Connect : public MPxCommand {
	public:
		Connect();
		virtual ~Connect();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static MSyntax newSyntax ();
		static void *creator();

	private:
		Controller::Connect m_operation;
		Controller::PaintMultipleStrandsWithNewColorFunctor m_functor;
	};
}

#endif /* CONNECT_H_ */
