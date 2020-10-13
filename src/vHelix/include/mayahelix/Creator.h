/*
 * Creator.h
 *
 *  Created on: 9 jul 2011
 *      Author: johan
 */

#ifndef CREATOR_H_
#define CREATOR_H_

/*
 * Command for creating new helices
 */

#include <mayahelix/Definition.h>

#include <mayahelix/model/Helix.h>

#include <list>

#include <maya/MPxCommand.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MPointArray.h>
#include "boost/numeric/ublas/matrix.hpp"

#define MEL_CREATEHELIX_COMMAND "createHelix"

namespace Helix {
	class VHELIXAPI Creator : public MPxCommand {
	public:
		Creator();
		virtual ~Creator();

		virtual std::string doIt(const MArgList & args);
		virtual std::string undoIt ();
		virtual std::string redoIt ();
		virtual bool isUndoable () const;
		virtual bool hasSyntax () const;

		static MSyntax newSyntax ();
		static void *creator();

		static int default_bases; // If no argument is given, uses this. It is initialized to DNA::CREATE_DEFAULT_NUM_BASES on startup

		/*
		 * Direct access interfaces, used by other C++ libraries
		 */

		std::string create(int bases, Model::Helix & helix, const std::string & name = "helix1");
		std::string create(const std::vector<double> & origo, int bases, double rotation, Model::Helix & helix, const std::string & name = "helix1");
		std::string create(const std::vector<double> & origo, const std::vector<double> & end, double rotation, Model::Helix & helix, const std::string & name = "helix1");

        std::string create(int bases, const QQuaternion & transform, Model::Helix & helix, const std::string & name = "helix1", bool showProgressBar = true);

	private:
		/*
		 * The createHelix along CV curve puts requirements on the generated bases
		 * by implementing this class, the caller can accept or reject creation of certain bases
		 */

		class CreateBaseControl {
		public:
			/*
			 * Return true to generate 'shared' coordinates for 'translation' in the operator below
			 * By 'shared' it's a coordinate system above the helices. It is not necessary 'world' coordinates because this would
			 * be inefficient (and there's no Maya method to acquire them), but also since the helices will *always* be in the same coordinate space
			 * but not the bases on each helix, world coordinates are not required
			 * return false is faster
			 */

			virtual bool generateSharedCoordinates() const;

			/*
			 * Return true to accept the creation of the base positioned at 'translation' and at index 'index'. if forward then forward strand, else backward strand
			 */

			virtual bool operator() (const std::vector<double> & translation, int index, bool forward) const;
		};

		/*
		 * Before calling, the m_materials must be set
		 */

        std::string createHelix(int bases, Model::Helix & helix, const QQuaternion & transform = QQuaternion(), const CreateBaseControl & control = CreateBaseControl(), const std::string & name = "helix1", bool showProgressBar = true);

		/*
		 * Create a helix along the line between the two given points
		 * either calculate the number of bases to generate and round to the nearest integer
		 * or give them explicitly.
		 * If no rotation is given, the helix will be oriented with the first base along the Y-axis.
		 * Notice that the rotation is given in degrees.
		 *
		 * TODO: Eliminate usage of these, use the method above instead.
		 */

		inline std::string createHelix(const std::vector<double> & origo, const std::vector<double> & end, double rotation, Model::Helix & helix, const CreateBaseControl & control = CreateBaseControl(), const std::string & name = "helix1") {
			const std::vector<double> delta(end - origo), center(origo + delta / 2);
			return createHelix(
					center, delta.normal(), DNA::DistanceToBaseCount(delta.length()),
					rotation, helix, control, name);
		}

		std::string createHelix(const std::vector<double> & origo, const std::vector<double> & direction, int bases, double rotation, Model::Helix & helix, const CreateBaseControl & control = CreateBaseControl(), const std::string & name = "helix1");

		/*
		 * Create a number of helices along a CV curve. Notice that the curve's bending properties (bezier etc) are not taken into account
		 * The rotation is applied to the first helix, the following helices will be rotated to match the end rotation of the previous one
		 */

		std::string createHelices(const MPointArray & points, double rotation = 0.0);

		/*
		 * These are for the redo/undo functionality
		 */

		enum CreateMode {
			CREATE_NONE = 0,
			CREATE_NORMAL = 1,
			CREATE_BETWEEN = 2,
			CREATE_ORIENTED = 3,
			CREATE_ALONG_CURVE = 4,
			CREATE_TRANSFORMED = 5
		} m_redoMode;

		struct {
			int bases;
			std::vector<double> origo, end, direction;
			double rotation;
			std::vector<MPointArray> points;
            boost::numeric::ublas::matrix<double> transform;
		} m_redoData;

		std::list<Model::Helix> m_helices;
		Model::Material m_materials[2];

		std::string randomizeMaterials();
	};
}


#endif /* CREATOR_H_ */
