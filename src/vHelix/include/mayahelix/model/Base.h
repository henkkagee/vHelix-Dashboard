/*
 * Base.h
 *
 *  Created on: 9 feb 2012
 *      Author: johan
 */

#ifndef _MODEL_BASE_H_
#define _MODEL_BASE_H_

#include <mayahelix/model/Object.h>
#include <mayahelix/model/Material.h>
#include <mayahelix/view/BaseShape.h>

#include <mayahelix/DNA.h>
#include "mayahelix/model/Space.h"

#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
//#include <maya/MEulerRotation.h>
//#include <maya/MFnDagNode.h>
//#include <maya/MNodeMessage.h>

namespace Helix {
	namespace Model {
		class Helix;
		/*
		 * Base: Defines the interface for manipulating helix bases
		 */

		class VHELIXAPI Base : public Object {
			friend void BaseModel_Shape_NodePreRemovalCallback(MObject & node,void *clientData);
		public:
			enum Type {
				BASE = 0,
				FIVE_PRIME_END = 1,
				THREE_PRIME_END = 2,
				END = 3
			};

			/*
			 * Null base is similar to the MObject::kNullObj and can be used as an invalid node.
			 */
			static Base null;

            static std::string Create(Helix & helix, const std::string & name, const std::vector<double> & translation, Base & base, Space space = Space::kTransform);

            //DEFINE_DEFAULT_INHERITED_OBJECT_CONSTRUCTORS(Base)

            static std::string AllSelected(MObjectArray & selectedBases);

			/*
			 * Handle materials (colors) of the base
			 */

            std::string setMaterial(const Material & material);
            std::string getMaterial(Material & material);

			/*
			 * Faster than the above
			 */

            std::string getMaterialColor(float & r, float & g, float & b, float & a);

            /*Color getColor(std::string & status);
            std::string setColor(const Color & color);*/

			/*
			 * Returns one of the enum types above
			 */

            Type type(std::string & status);

			/*
			 * Connect/disconnect
			 */

            std::string connect_forward(Base & target, bool ignorePreviousConnections = false);
            std::string disconnect_forward(bool ignorePerpendicular = false);
            std::string disconnect_backward(bool ignorePerpendicular = false);
            std::string connect_opposite(Base & target, bool ignorePreviousConnections = false);
            std::string disconnect_opposite();

			/*
			 * Labels
			 */

            std::string setLabel(DNA::Name label);
            std::string getLabel(DNA::Name & label);

			/*
			 * Find the next, prev or opposite bases.
			 * If you want to iterate strands, use the Strand class as it is iterator based and works with STL
			 */

            Base forward(std::string & status);
			Base forward();

            Base backward(std::string & status);
			Base backward();

            Base opposite(std::string & status);
			Base opposite();

            bool opposite_isDestination(std::string & status);

			/*
			 * Returns the direction of the strand relative to a given axis.
			 * Uses forward or backward connections to figure out in what direction
			 * the strand is going and returns the sign of the dot product between this
			 * vector and the axis given.
			 *
			 * space: In what space is this calculated? Usually relative to the Helix, that is kTransform.
			 */
            int sign_along_axis(const std::vector<double> & axis, Space space, std::string & status);

			/*
			 * Helix
			 */

            Helix getParent(std::string & status);

            inline std::string setShapesVisibility(bool visible) {
				return Object::setShapesVisibility(visible, View::BaseShape::id);
			}

            inline std::string toggleShapesVisibility() {
				return Object::toggleShapesVisibility(View::BaseShape::id);
			}

            inline bool isAnyShapeVisible(std::string & status) {
				return Object::isAnyShapeVisible(View::BaseShape::id, status);
			}

		private:
			/*
			 * The shape is only created once, after this, it is instead instanced. Saves on file size and hopefully increases performance
			 */

			/*static MObject s_shape;
			static MCallbackId s_shape_NodePreRemovalCallbackId;*/
		};
	}
}

#endif /* _MODEL_BASE_H_ */
