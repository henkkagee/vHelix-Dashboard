#ifndef _CREATECURVES_H_
#define _CREATECURVES_H_

#include <mayahelix/Definition.h>

#include <mayahelix/controller/CreateCurves.h>

#include <maya/MPxCommand.h>

#define MEL_CREATE_CURVES_COMMAND "createStrandCurves"

namespace Helix {
	class VHELIXAPI CreateCurves : public MPxCommand {
	public:
		CreateCurves();
		virtual ~CreateCurves();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static void *creator();
		static MSyntax newSyntax();

	private:
		Controller::CreateCurves m_operation;
	};
}

#endif /* N _CREATECURVES_H_ */
