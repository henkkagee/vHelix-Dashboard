/*
 * PaintStrand.h
 *
 *  Created on: 8 mar 2012
 *      Author: johan
 */

#ifndef _CONTROLLER_PAINTSTRAND_H_
#define _CONTROLLER_PAINTSTRAND_H_

#include <mayahelix/Utility.h>

#include <mayahelix/controller/Operation.h>
#include <mayahelix/model/Material.h>
#include <mayahelix/model/Base.h>
#include <mayahelix/model/Strand.h>

namespace Helix {
	namespace Controller {
		/*
		 * This is the controller for painting strands, these are implemented as functors used with the STL
		 */

		class VHELIXAPI PaintStrandOperation : public Operation<Model::Base, Model::Material, Model::Material> {
		public:
			PaintStrandOperation(Model::Material & material) : m_material(material) {

			}

			PaintStrandOperation() {

			}

			void setMaterial(const Model::Material & material) {
				m_material = material;
			}

		protected:
			std::string doExecute(Model::Base & element);
			std::string doUndo(Model::Base & element, Model::Material & undoData);
			std::string doRedo(Model::Base & element, Model::Material & redoData);
		private:
			Model::Material m_material;
		};

		/*
		 * Utility class for painting multiple strands with multiple randomized colors
		 */

		class VHELIXAPI PaintMultipleStrandsFunctor {
		public:
			inline std::string loadMaterials() {
				std::string status;

				m_materials_begin = Model::Material::AllMaterials_begin(status, m_numMaterials);

                if (status == "") {
                    status = "Material::GetAllMaterials";
					return status;
				}

				return "kSuccess";
			}

			inline void operator() (Model::Strand strand) {
				/*
				 * Randomize a new color, assume numMaterials > 0
				 */

				m_operation.setMaterial(*(m_materials_begin + (rand() % m_numMaterials)));

				Model::Strand::ForwardIterator it = strand.forward_begin();
				for_each_itref(it, strand.forward_end(), m_operation.execute());

                if ((m_status = m_operation.status()) == "") {
					//m_status.perror("PaintStrandOperation::status 1");
					return;
				}

				if (!it.loop()) {
					// Step back a step so that we don't color our base again. In the case of an end base with no backward connection, the loop will just be empty
					strand.setDefiningBase(strand.getDefiningBase().backward());

					std::for_each(strand.reverse_begin(), strand.reverse_end(), m_operation.execute());

                    if ((m_status = m_operation.status()) == "") {
						//m_status.perror("PaintStrandOperation::status 1");
						return;
					}
				}
			}

			inline const std::string & status() const {
				return m_status;
			}

			inline std::string undo() {
				return m_operation.undo();
			}

			inline std::string redo() {
				return m_operation.redo();
			}

		protected: // FIXME
			PaintStrandOperation m_operation;

			/*Model::Material *m_materials;
			size_t m_numMaterials;*/
			Model::Material::Iterator m_materials_begin;
			Model::Material::Container::size_type m_numMaterials;

			std::string m_status;
		};

		/*
		 * Extended utility class of the above where we look at the current color of the base and tries to pick another color
		 */

		class VHELIXAPI PaintMultipleStrandsWithNewColorFunctor : public PaintMultipleStrandsFunctor {
		public:
			inline void operator() (Model::Strand strand) {
				/*
				 * Randomize a new color, assume numMaterials > 0
				 */

				Model::Material previous_material, chosen_material;

				{
					std::string status;
                    if ((status = strand.getDefiningBase().getMaterial(previous_material)) == "")
                        status = "Base::getMaterial";
				}

				do {
					chosen_material = *(m_materials_begin + (rand() % m_numMaterials));
					//chosen_material = *(Model::Material::AllMaterials_begin(m_status, m_numMaterials) + (rand() % m_numMaterials));
				} while (chosen_material == previous_material);

				m_operation.setMaterial(chosen_material);

				Model::Strand::ForwardIterator it = strand.forward_begin();
				for_each_itref(it, strand.forward_end(), m_operation.execute());


                if ((m_status = m_operation.status()) == "") {
					//m_status.perror("PaintStrandOperation::status 1");
					return;
				}

				if (!it.loop()) {

					// Step back a step so that we don't color our base again. In the case of an end base with no backward connection, the loop will just be empty
					strand.setDefiningBase(strand.getDefiningBase().backward());
					
					std::for_each(strand.reverse_begin(), strand.reverse_end(), m_operation.execute());

                    if ((m_status = m_operation.status()) == "") {
						//m_status.perror("PaintStrandOperation::status 1");
						return;
					}
				}
			}
		};

		/*
		 * This one is used by the JSON importer, it is not interested in undo functionality as the strand has not existed before
		 */

		class VHELIXAPI PaintMultipleStrandsNoUndoFunctor {
		public:
			class SetMaterialFunctor {
			public:
				inline SetMaterialFunctor(Model::Material::ApplyMaterialToBases apply) : m_apply(apply) {

				}

				inline void operator() (Model::Base & base) {
					std::string status;

                    if ((status = m_apply.add(base)) == "")
						m_status = status;
				}

				inline std::string status() const {
					return m_status;
				}

				inline const Model::Material::ApplyMaterialToBases & getApply() const {
					return m_apply;
				}

			private:
				Model::Material::ApplyMaterialToBases m_apply;
				std::string m_status;
			};

			inline void operator() (Model::Strand strand, Model::Material & material) {
				/*
				 * Randomize a new color, assume numMaterials > 0
				 */

				Model::Strand::ForwardIterator it = strand.forward_begin();

				SetMaterialFunctor functor(material.setMaterialOnMultipleBases());
				for_each_ref_itref(it, strand.forward_end(), functor);

                if ((m_status = functor.status()) == "") {
					HMEVALUATE_DESCRIPTION("SetMaterialFunctor", m_status);
					return;
				}

				if (!it.loop()) {
					// Step back a step so that we don't color our base again. In the case of an end base with no backward connection, the loop will just be empty
					strand.setDefiningBase(strand.getDefiningBase().backward());

					for_each_ref(strand.reverse_begin(), strand.reverse_end(), functor);

                    if ((m_status = functor.status()) == "") {
						HMEVALUATE_DESCRIPTION("SetMaterialFunctor", m_status);
						return;
					}
				}

				m_status = functor.getApply().apply();
			}

			inline const std::string & status() const {
				return m_status;
			}

		private:
			std::string m_status;
		};

		class VHELIXAPI PaintMultipleStrandsNoUndoNoOverrideFunctor : public PaintMultipleStrandsNoUndoFunctor {
		public:
			inline void operator() (Model::Strand strand, Model::Material & material) {
				std::string status;
				Model::Material m;

                if ((status = strand.getDefiningBase().getMaterial(m)) == "")
					PaintMultipleStrandsNoUndoFunctor::operator()(strand, material);
			}
		private:
		};
	}
}

#endif /* _CONTROLLER_PAINTSTRAND_H_ */
