/*
 * Helix.h
 *
 *  Created on: 9 feb 2012
 *      Author: johan
 */

#ifndef _MODEL_HELIX_H_
#define _MODEL_HELIX_H_

#include <mayahelix/model/Object.h>
#include <mayahelix/model/Base.h>
#include <mayahelix/view/HelixShape.h>

#include <string>
//#include <maya/MMatrix.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <QQuaternion>

#include <iterator>

/*
 * This is the interface used for modifying helices
 */

namespace Helix {
	namespace Model {
		class VHELIXAPI Helix : public Object {
		public:

			/*
			 * Alias for an invalid Helix. Note that it is readable but should *not* be modified!
			 * TODO: Put exceptions on modification?
			 */
			static Helix null;

			/*
			 * Create: Creates a new helix node, attach a locator node and transforms it to the given position
			 */

			//static std::string Create(const MMatrix & transform, Helix & helix);
            static std::string Create(const std::string & name, const QQuaternion & transform, Helix & helix);

			/*
			 * Select: Give any object that either identifies a helix or it's children and get a handle to the helix manipulator
			 */

			static std::string Selected(Object & object, Helix & helix);

			/*
			 * Fills the array of all currently selected helices in the scene. Notice that it uses the above method, so indirect selections will also be returned
			 * No duplicates will be generated
			 */

			static std::string AllSelected(MObjectArray & selectedHelices);

			/*
			 * Used to obtain a list of all Helices in the scene.
			 *
			 * TODO: Make a general GetAllObjectsOfType to use for both helices and bases in a scene.
			 */
			static std::string All(MObjectArray & helices);

			/*
			 * This is an expansion of the getRelatives below, but for multiple helices. The difference is that it makes sure that all returned helices are unique
			 * Note that the helices given will be included in the relatives array
			 */

			static std::string GetRelatives(const MObjectArray & helices, MObjectArray & relatives);

			/*
			 * This is used by the Locator. Find all selected bases that belong to this helix.
			 */

			std::string getSelectedChildBases(MObjectArray & selectedBases);

			/*
			 * Show all helices using the cylinder representation or the base representation
			 */

			static std::string ToggleCylinderOrBases();
			static std::string RefreshCylinderOrBases();

            //DEFINE_DEFAULT_INHERITED_OBJECT_CONSTRUCTORS(Helix)

			/*
			 * Manage the cylinder. Default is along the Z-axis but could change in the future
			 */

			std::string setCylinderRange(double origo, double height);
			std::string getCylinderRange(double & origo, double & height);

			/*
			 * BaseIterator: Iterate over the bases belonging to this helix
			 */

			class BaseIterator : public std::iterator<std::input_iterator_tag, Base> {
				friend class Helix;
			public:

				inline BaseIterator operator++() {
					BaseIterator bi(*this);

					GetNextBaseIndex();

					return bi;
				}

				inline BaseIterator & operator++(int x) {
					GetNextBaseIndex();

					return *this;
				}

				inline Base * operator->() {
					m_cacheBase = getChildBase();
					return &m_cacheBase;
				}

				inline Base & operator*() {
					m_cacheBase = getChildBase();
					return m_cacheBase;
				}

				inline bool operator== (const BaseIterator & i) const {
					return m_helix == i.m_helix && m_childIndex == i.m_childIndex;
				}

				inline bool operator!= (const BaseIterator & i) const {
					return !operator == (i);
				}

				inline BaseIterator(const BaseIterator & copy) : m_helix(copy.m_helix), m_childIndex(copy.m_childIndex) {

				}

			protected:
				inline BaseIterator(Helix & helix, unsigned int index) : m_helix(&helix), m_childIndex(index) {

				}

				Base getChildBase();
				void GetNextBaseIndex();

			private:
				Helix *m_helix;
				Base m_cacheBase;
				unsigned int m_childIndex;
			};

			BaseIterator begin();
			BaseIterator end();

			/*
			 * Relatives: Get surrounding helices connected to this helix over base connections
			 */

			std::string getRelatives(MObjectArray & helices);

			/*
			 * Only used for memory allocation
			 */
			
			unsigned int numChildren(std::string & status);

			inline std::string setShapesVisibility(bool visible) {
				return Object::setShapesVisibility(visible, View::HelixShape::id);
			}

			inline std::string toggleShapesVisibility() {
				return Object::toggleShapesVisibility(View::HelixShape::id);
			}

			inline bool isAnyShapeVisible(std::string & status) {
				return Object::isAnyShapeVisible(View::HelixShape::id, status);
			}

			/*
			 * Convenience methods. Notice that it just finds the base of the type, and picks the one with the most extreme Z coordinate
			 */

			std::string getForwardThreePrime(Model::Base & base);
			std::string getForwardFivePrime(Model::Base & base);
			std::string getBackwardThreePrime(Model::Base & base);
			std::string getBackwardFivePrime(Model::Base & base);

		private:
			bool hasCylinder(std::string & status);
			std::string createCylinder(double origo, double height);
		};

		DEFINE_DEFAULT_INHERITED_OBJECT_INVERSED_ORDER_OPERATORS(Helix);
	}
}

#endif /* _MODEL_HELIX_H_ */
