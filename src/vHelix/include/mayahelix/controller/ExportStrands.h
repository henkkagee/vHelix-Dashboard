#ifndef _CONTROLLER_EXPORTSTRANDS_H_
#define _CONTROLLER_EXPORTSTRANDS_H_

#include <mayahelix/controller/Operation.h>

#include <mayahelix/model/Strand.h>

namespace Helix {
	namespace Controller {
		class VHELIXAPI ExportStrands : public Operation<Model::Strand> {
		public:
			enum Mode {
				COMMA_SEPARATED,
				COLON_SEPARATED
			};

			std::string write(const std::string & filename, Mode mode = COMMA_SEPARATED);

			virtual void onProgressStep();

		protected:
			std::string doExecute(Model::Strand & element);
			std::string doUndo(Model::Strand & element, Empty & undoData);
			std::string doRedo(Model::Strand & element, Empty & redoData);

		private:
			/*
			 * The data stored for export
			 */

			struct Data {
				/*inline Data(const std::string & _strand_name, const std::string & _sequence) : strand_name(_strand_name), sequence(_sequence) {

				}*/

				std::string strand_name, // Will be the two end bases for a strand not in a loop, if it's a loop, then any base name
						sequence;
			};

			std::list<Data> m_export_data;

			friend std::ostream & operator<<(std::ostream & stream, const ExportStrands::Data & data);
		};
	}
}

#endif /* N  _CONTROLLER_EXPORTSTRANDS_H_ */
