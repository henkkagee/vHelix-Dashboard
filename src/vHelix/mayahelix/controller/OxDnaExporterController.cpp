/*
 * OxDnaExporterController.cpp
 *
 *  Created on: Jan 28, 2014
 *      Author: johan
 */

#include <mayahelix/controller/OxDnaExporter.h>
#include <mayahelix/model/Helix.h>
#include <mayahelix/Utility.h>

#include <fstream>

namespace Helix {
	namespace Controller {
		MStatus OxDnaExporter::doExecute(Model::Strand & element) {
			Strand outstrand;
			MStatus status;

			// Iterate backwards along the strand to find the first base of the strand if not circular.
			element.rewind();
			Model::Base & base(element.getDefiningBase());
			MDagPath baseDagPath;
			HMEVALUATE_RETURN(baseDagPath = base.getDagPath(status), status);
			HMEVALUATE_RETURN(outstrand.name = baseDagPath.fullPathName(&status), status);
			/*
			Model::Helix parent(base.getParent(status));
			MEulerRotation parentRotation;
			HMEVALUATE_RETURN(status = parent.getRotation(parentRotation), status);
			MVector parentTranslation;
			parent.getTranslation(parentTranslation, MSpace::kWorld);
			const MVector normal((MVector(0, 0, 1) * parentRotation.asMatrix()).normal());
			*/

			Model::Strand::ForwardIterator it = element.forward_begin();
			int store_dir = 1;/*erik, janky solution to a crossover bug */
			for(; it != element.forward_end(); ++it) {

				/*ERIK: this block was moved into the loop*/
				Model::Helix parent(it->getParent(status));
				MEulerRotation parentRotation;
				HMEVALUATE_RETURN(status = parent.getRotation(parentRotation), status);
				MVector parentTranslation;
				parent.getTranslation(parentTranslation, MSpace::kWorld);
				const MVector normal((MVector(0, 0, 1) * parentRotation.asMatrix()).normal());
				/*END of moved block*/


				/*
				Model::Helix parent(it->getParent(status));
				*/
				Base base;
				HMEVALUATE_RETURN(status = it->getLabel(base.label), status);

				HMEVALUATE_RETURN(status = it->getTranslation(base.translation, MSpace::kWorld), status);

                boost::math::quaternion<double> rotation;
				HMEVALUATE_RETURN(status = it->getRotation(rotation), status);

				if (base.label == DNA::Invalid) {/*ERIK added this to make questionmarks into T's if complementary dna is not assigned it will fall appart*/
					base.label = 'T';
					/*const MString errorString(MString("The base ") + it->getDagPath(status).fullPathName() + " does not have an assigned label.");
					MGlobal::displayError(errorString);
					HPRINT("%s", errorString.asChar());

					return MStatus::kInvalidParameter;*/
				}

				base.tangent = (normal ^ ((base.translation - parentTranslation) ^ normal)).normal();

				int direction;
				HMEVALUATE_RETURN(direction = it->sign_along_axis(MVector::zAxis, MSpace::kTransform, status), status);

				if (direction == 0) {/* ERIK crappy solutiton to weird xover bug*/
					direction = store_dir;
				}

				store_dir = direction; /*save the dircetion for later next round as it may need to be used again */

				base.normal = normal * direction; /* HERE IS THE PROBLEM*/
				HMEVALUATE_RETURN(base.helixName = parent.getDagPath(status).fullPathName(), status);
				HMEVALUATE_RETURN(base.name = it->getDagPath(status).partialPathName(), status);
				Model::Material material;
				HMEVALUATE_RETURN(status = it->getMaterial(material), status);
				base.material = material.getMaterial();
				outstrand.strand.push_back(base);

				if (m_helices.find(base.helixName.asChar()) == m_helices.end()) {
					Helix helix;

					HMEVALUATE_RETURN(status = parent.getTranslation(helix.translation, MSpace::kWorld), status);

					MEulerRotation rotation;
					HMEVALUATE_RETURN(status = parent.getRotation(rotation), status);
                    helix.normal = MVector::zAxis * boost::numeric::ublas::matrix<double>().rotateTo(rotation).asMatrix();

					m_helices.insert(std::make_pair(base.helixName.asChar(), helix));
				}
			}

			outstrand.circular = it.loop();/*ERIK check it the last base is the same  as the first */
			m_strands.push_back(outstrand);

			return MStatus::kSuccess;
		}

		MStatus OxDnaExporter::doUndo(Model::Strand & element, Empty & undoData) {
			return MStatus::kSuccess;
		}

		MStatus OxDnaExporter::doRedo(Model::Strand & element, Empty & redoData) {
			return MStatus::kSuccess;
		}

		MStatus OxDnaExporter::write(const char *topology_filename, const char *configuration_filename, const char *vhelix_filename, const MVector & minTranslation, const MVector & maxTranslation) const {

			std::ofstream conf_file(configuration_filename);
			std::ofstream top_file(topology_filename);
			std::ofstream vhelix_file(vhelix_filename);

			if (!conf_file) {
				MGlobal::displayError(MString("Can't open file \"") + configuration_filename + "\" for writing.");
				return MStatus::kFailure;
			}

			if (!top_file) {
				MGlobal::displayError(MString("Can't open file \"") + topology_filename + "\" for writing.");
				return MStatus::kFailure;
			}

			if (!vhelix_file) {
				MGlobal::displayError(MString("Can't open file \"") + vhelix_filename + "\" for writing.");
				return MStatus::kFailure;
			}

			size_t numBases = 0;
			for (std::list<Strand>::const_iterator it = m_strands.begin(); it != m_strands.end(); ++it) {
				numBases += it->strand.size();
			}

			top_file << numBases << " " << m_strands.size() << std::endl;

			unsigned int i = 1;
			int j = 0;
			//
			//for (std::list<Strand>::const_iterator it = m_strands.begin(); it != m_strands.end(); ++it, ++i) {
			//	const int firstIndex = it->circular ? int(it->strand.size()) - 1 : -1;
			//	const int lastIndex = it->circular ? 0 : -1;

			//	/*top_file << "# " << it->name.asChar() << std::endl;*/   /* ERIK, i removed this to make the exports work with */

			//	int k = 0;
			//	for (std::vector<Base>::const_iterator lit = it->strand.begin(); lit != it->strand.end(); ++lit, ++j, ++k)
			//		top_file << i << " " << lit->label.toChar() << " " <<
			//					(k == 0 ? firstIndex : j - 1) << " " <</* ERIK: I flipped the order to the print out to correct it */
			//					(k == int(it->strand.size()) - 1 ? lastIndex : j + 1) << std::endl;

			//	/*top_file << std::endl;*/
			//}

			//top_file.close();

			//

			//const MVector dimensions(maxTranslation - minTranslation);/* i changed this to make the box big enough*/

			//double largest_side = std::max(dimensions.x, std::max(dimensions.y,dimensions.z)); /*Erik, square boxes are easier to work with, figure out what side is largest  */
			//
			////conf_file << "t = 0" << std::endl << "b = " << dimensions.x * 2.0 << " " << dimensions.y * 2.0 << " " << dimensions.z * 2.0 << std::endl << "E = 0. 0. 0." << std::endl; /* ERIK, i added a 2x multiplier to box dimensions*/
			//conf_file << "t = 0" << std::endl << "b = " << largest_side * 2.0 << " " << largest_side * 2.0 << " " << largest_side * 2.0 << std::endl << "E = 0. 0. 0." << std::endl; /* ERIK, i added a 2x multiplier to box dimensions*/

			//for (std::list<Strand>::const_iterator it = m_strands.begin(); it != m_strands.end(); ++it, ++i) {
			//	for (std::vector<Base>::const_iterator sit = it->strand.begin(); sit != it->strand.end(); ++sit) {
			//		const MVector translation(sit->translation/* - minTranslation*/);
			//		

			//		conf_file << (translation.x + sit->tangent.x * -0.35) * 1.174 << " " << (translation.y + sit->tangent.y * -0.35) * 1.174 << " " << (translation.z + sit->tangent.z * -0.35) * 1.174 << " " << /* ERIK I multiply with * 1.174  to change fron nm to oxDNA lenght units  i nudge it with the tangent vector to move from phosphate position to center of mass */
			//				sit->tangent.x * -1.0 << " " << sit->tangent.y * -1.0 << " " << sit->tangent.z * -1.0 << " " <</*ERIK, i added minuses to the tangent vectors to flip them*/
			//				sit->normal.x << " " << sit->normal.y << " " << sit->normal.z <<
			//				" 0.0 0.0 0.0 0.0 0.0 0.0" << std::endl;
			//	}
			//}

			//conf_file.close();
			int nt_counter = 0; /*Erik i added this to keep track of indices in circular strands */
			for (std::list<Strand>::const_iterator it = m_strands.begin(); it != m_strands.end(); ++it, ++i) {
				const int firstIndex = it->circular ? int(it->strand.size()) - 1 + nt_counter : -1;
				const int lastIndex = it->circular ? (0 + nt_counter) : -1;

				/*top_file << "# " << it->name.asChar() << std::endl;*/   /* ERIK, i removed this to make the exports work with */

				int k = 0;
				for (std::vector<Base>::const_reverse_iterator lit = it->strand.rbegin(); lit != it->strand.rend(); ++lit, ++j, ++k,nt_counter++)
					top_file << i << " " << lit->label.toChar() << " " <<
					(k == 0 ? firstIndex : j - 1) << " " <</* ERIK: I flipped the order to the print out to correct it */
					(k == int(it->strand.size()) - 1 ? lastIndex : j + 1) << std::endl;

				/*top_file << std::endl;*/
			}

			top_file.close();



			const MVector dimensions(maxTranslation - minTranslation);/* i changed this to make the box big enough*/

			double largest_side = std::max(dimensions.x, std::max(dimensions.y, dimensions.z)); /*Erik, square boxes are easier to work with, figure out what side is largest  */

			//conf_file << "t = 0" << std::endl << "b = " << dimensions.x * 2.0 << " " << dimensions.y * 2.0 << " " << dimensions.z * 2.0 << std::endl << "E = 0. 0. 0." << std::endl; /* ERIK, i added a 2x multiplier to box dimensions*/
			conf_file << "t = 0" << std::endl << "b = " << largest_side * 2.0 << " " << largest_side * 2.0 << " " << largest_side * 2.0 << std::endl << "E = 0. 0. 0." << std::endl; /* ERIK, i added a 2x multiplier to box dimensions*/

			for (std::list<Strand>::const_iterator it = m_strands.begin(); it != m_strands.end(); ++it, ++i) {
				for (std::vector<Base>::const_reverse_iterator sit = it->strand.rbegin(); sit != it->strand.rend(); ++sit) {
					const MVector translation(sit->translation/* - minTranslation*/);


					conf_file << (translation.x + sit->tangent.x * -0.35) * 1.174 << " " << (translation.y + sit->tangent.y * -0.35) * 1.174 << " " << (translation.z + sit->tangent.z * -0.35) * 1.174 << " " << /* ERIK I multiply with * 1.174  to change fron nm to oxDNA lenght units  i nudge it with the tangent vector to move from phosphate position to center of mass */
						sit->tangent.x * -1.0 << " " << sit->tangent.y * -1.0 << " " << sit->tangent.z * -1.0 << " " <</*ERIK, i added minuses to the tangent vectors to flip them*/
						sit->normal.x * -1.0 << " " << sit->normal.y * -1.0 << " " << sit->normal.z * -1.0<<
						" 0.0 0.0 0.0 0.0 0.0 0.0" << std::endl;
				}
			}

			conf_file.close();

			const std::string date(Date());
			vhelix_file << "# vHelix glue file for oxDNA export \"" << topology_filename << "\" and \"" << configuration_filename << "\"." << std::endl <<
					"# " << date.c_str() << std::endl << std::endl;

			for (std::tr1::unordered_map<std::string, Helix>::const_iterator it = m_helices.begin(); it != m_helices.end(); ++it) {
				const MVector translation(it->second.translation/* - minTranslation*/);

				vhelix_file << "helix " << it->first.c_str() << " " <<
						translation.x << " " << translation.y << " " << translation.z << " " <<
						it->second.normal.x << " " << it->second.normal.y << " " << it->second.normal.z << std::endl;
			}

			vhelix_file << std::endl;

			unsigned int index = 0;
			for (std::list<Strand>::const_iterator it = m_strands.begin(); it != m_strands.end(); ++it, ++i) {
				for (std::vector<Base>::const_reverse_iterator bit = it->strand.rbegin(); bit != it->strand.rend(); ++bit) {
					vhelix_file << "base " << index++ << " " << bit->name << " " << bit->helixName << " " << bit->material.asChar() << std::endl;
				}
			}


			vhelix_file.close();

			return MStatus::kSuccess;
		}
	}
}
