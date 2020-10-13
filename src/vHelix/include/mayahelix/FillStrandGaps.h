#ifndef _FILL_STRAND_GAPS_H_
#define _FILL_STRAND_GAPS_H_

#include <mayahelix/Definition.h>

#include <mayahelix/controller/FillStrandGaps.h>

#include <iostream>
#include <list>

#include <maya/MPxCommand.h>
#include <maya/MDagPath.h>

#define MEL_FILLSTRANDGAPS_COMMAND "fillStrandGaps"

namespace Helix {
	class VHELIXAPI FillStrandGaps : public MPxCommand {
	public:
		FillStrandGaps();
		virtual ~FillStrandGaps();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt();
		virtual std::string redoIt();
		virtual bool isUndoable() const;
		virtual bool hasSyntax() const;

		static MSyntax newSyntax();
		static void *creator();

	private:
		Controller::FillStrandGaps m_operation;
	};
}

#endif /* _FILL_STRAND_GAPS_H_ */
