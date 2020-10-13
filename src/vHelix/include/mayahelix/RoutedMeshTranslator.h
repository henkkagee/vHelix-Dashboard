/*
 * RoutedMeshTranslator.h
 *
 *  Created on: Feb 4, 2014
 *      Author: johan
 */

#ifndef ROUTEDMESHTRANSLATOR_H_
#define ROUTEDMESHTRANSLATOR_H_

#include <mayahelix/Definition.h>
#include <maya/MPxFileTranslator.h>

#define HELIX_ROUTED_MESH_FILE_TYPE "rmesh"

namespace Helix {
	class RoutedMeshTranslator : public MPxFileTranslator {
	public:
		virtual std::string reader (const MFileObject& file, const std::string & options, MPxFileTranslator::FileAccessMode mode);
		virtual bool haveReadMethod () const;
		virtual bool canBeOpened () const;
		virtual std::string defaultExtension () const;
		virtual MPxFileTranslator::MFileKind identifyFile (	const MFileObject& file, const char *buffer, short size) const;

		static void *creator();
	};
}


#endif /* ROUTEDMESHTRANSLATOR_H_ */
