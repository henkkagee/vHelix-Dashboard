/*
 * RoutedMeshImporter.h
 *
 *  Created on: Feb 4, 2014
 *      Author: johan
 */

#ifndef ROUTEDMESHIMPORTER_H_
#define ROUTEDMESHIMPORTER_H_

#include <mayahelix/Utility.h>

#include <vector>

namespace Helix {
	namespace Controller {
		class VHELIXAPI RoutedMeshImporter {
		public:
			std::string read(const char *filename);

		private:
			std::vector<std::vector<double>> m_vertices;

			struct Edge {
				unsigned int vertex;
				bool hasCylinder;
				double cylinder[2]; // start and end offsets.
				bool hasAngle;
				double angle;
			};

			std::vector<Edge> m_edges;
			double m_initialRotation;
		};
	}
}


#endif /* ROUTEDMESHIMPORTER_H_ */
