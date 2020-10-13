#ifndef _CONNECT_SUGGESTIONS_CONTEXT_H_
#define _CONNECT_SUGGESTIONS_CONTEXT_H_

#include <mayahelix/Definition.h>
#include <mayahelix/view/ConnectSuggestionsToolCommand.h>
#include <mayahelix/model/Base.h>

#include <maya/MPxContext.h>


namespace Helix {
	namespace View {
		class ConnectSuggestionsContext : public MPxContext {
		public:
			ConnectSuggestionsContext();
			virtual ~ConnectSuggestionsContext();

			virtual void toolOnSetup(MEvent &event);
			virtual void toolOffCleanup();
			virtual std::string doPress(MEvent &event);
			virtual std::string doDrag(MEvent &event);
			virtual std::string doRelease(MEvent &event);
			virtual std::string doEnterRegion(MEvent &event);
			virtual void getClassName(std::string & name) const;

			virtual void completeAction();
			virtual void abortAction();

			/*
			 * These are called by the locator when there's an action to complete,
			 * the context uses the toolCommand to manage do/undo/redo functionality
			 */

			void connect(Model::Base & source, Model::Base & destination);

		private:
			MObject m_locatorNode;
			ConnectSuggestionsToolCommand *m_cmd;
		};
	}
}

#endif /* N _CONNECT_SUGGESTIONS_CONTEXT_H_ */
