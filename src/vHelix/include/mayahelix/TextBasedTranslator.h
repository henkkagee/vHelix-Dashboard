#ifndef _TEXTBASEDTRANSLATOR_H_
#define _TEXTBASEDTRANSLATOR_H_

#include <mayahelix/Definition.h>
#include <maya/MPxFileTranslator.h>

#define HELIX_TEXT_BASED_FILE_DESCRIPTION "Text based vhelix"
#define HELIX_TEXT_BASED_FILE_TYPE "rpoly"

// TODO: Implement writer. Currently this is only used to import files from the polygon relaxation simulator.

namespace Helix {
	class TextBasedTranslator : public MPxFileTranslator {
	public:
		virtual std::string reader(const MFileObject& file, const std::string & options, MPxFileTranslator::FileAccessMode mode);
		virtual bool haveReadMethod() const;
		virtual bool canBeOpened() const;
		virtual std::string defaultExtension() const;
		virtual MPxFileTranslator::MFileKind identifyFile(const MFileObject& file, const char *buffer, short size) const;

		static void *creator();
	};
}

#endif /* N _TEXTBASEDTRANSLATOR_H_ */
