#ifndef _STRAND_LENGTH_COUNT_H_
#define _STRAND_LENGTH_COUNT_H_

/*
* Command for counting the length of a strand.
*/

#include <mayahelix/Definition.h>

#include <mayahelix/controller/StrandLengthCount.h>

#include <iostream>
#include <list>

#include <maya/MPxCommand.h>
#include <maya/MDagPath.h>

#define MEL_STRANDLENGTHCOUNT_COMMAND "strandLength"

namespace Helix {
	class VHELIXAPI StrandLengthCount : public MPxCommand {
	public:
		virtual ~StrandLengthCount();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt();
		virtual std::string redoIt();
		virtual bool isUndoable() const;
		virtual bool hasSyntax() const;

		static MSyntax newSyntax();
		static void *creator();

	private:
		Controller::StrandLengthCount m_operation;
	};
}

#endif /* _STRAND_LENGTH_COUNT_H_ */
