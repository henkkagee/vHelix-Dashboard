#ifndef _VIEW_CONNECT_SUGGESTIONS_TOOL_COMMAND_H_
#define _VIEW_CONNECT_SUGGESTIONS_TOOL_COMMAND_H_

#include <mayahelix/Definition.h>
#include <mayahelix/model/Base.h>
#include <mayahelix/controller/Connect.h>
#include <mayahelix/controller/PaintStrand.h>

#include <maya/MPxToolCommand.h>

#define MEL_CONNECT_SUGGESTIONS_TOOL_COMMAND "connectSuggestionsCmd"

namespace Helix {
	namespace View {
		class ConnectSuggestionsToolCommand : public MPxToolCommand {
		public:
			ConnectSuggestionsToolCommand();
			virtual ~ConnectSuggestionsToolCommand();

			std::string doIt(const MArgList& args);
			std::string redoIt();
			std::string undoIt();
			bool isUndoable() const;
			std::string finalize();

			static MSyntax newSyntax();
			static void *creator();

			void connect(Model::Base & source, Model::Base & destination);

		private:
			Controller::Connect m_operation;
			Controller::PaintMultipleStrandsWithNewColorFunctor m_functor;
		};
	}
}

#endif /* N _VIEW_CONNECT_SUGGESTIONS_TOOL_COMMAND_H_ */
