/*
 * Duplicate.h
 *
 *  Created on: Aug 3, 2011
 *      Author: bjorn
 */

#ifndef DUPLICATE_H_
#define DUPLICATE_H_

/*
 * Command for duplicating helices and their belongings
 */

#include <mayahelix/Definition.h>

#include <mayahelix/controller/Duplicate.h>

#include <iostream>
#include <vector>

#include <maya/MPxCommand.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>

#define MEL_DUPLICATEHELICES_COMMAND "duplicateHelices"

namespace Helix {
	class VHELIXAPI Duplicate : public MPxCommand {
	public:
		Duplicate();
		virtual ~Duplicate();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static MSyntax newSyntax ();
		static void *creator();

	private:
		class DuplicateWithProgressBar : public Controller::Duplicate {
			void onProgressStart(unsigned int process, unsigned int range);
			void onProgressStep();
			void onProgressDone();
		} m_operation;
	};
}

#endif /* DUPLICATE_H_ */
