/*
 * ExportStrands.h
 *
 *  Created on: 12 jul 2011
 *      Author: johan
 */

#ifndef EXPORTSTRANDS_H_
#define EXPORTSTRANDS_H_

/*
 * Command for exporting the strand sequences of the target, selected or all helices
 * The result will be passed out as an std::stringArray
 */

#include <mayahelix/Definition.h>

#include <iostream>

#include <maya/MPxCommand.h>

#include <mayahelix/controller/ExportStrands.h>

#define MEL_EXPORTSTRANDS_COMMAND "exportStrands"

namespace Helix {
	class VHELIXAPI ExportStrands : public MPxCommand {
	public:
		ExportStrands();
		virtual ~ExportStrands();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static MSyntax newSyntax ();
		static void *creator();

	private:
		class ExportStrandsWithProgressBar : public Controller::ExportStrands {
		public:
			void onProgressStep();
		} m_operation;
	};
}

#endif /* EXPORTSTRANDS_H_ */
