/*
 * Strand.h
 *
 *  Created on: 17 feb 2012
 *      Author: johan
 */

#ifndef STRAND_H_
#define STRAND_H_

#include <mayahelix/model/Base.h>

namespace Helix {
	namespace Model {
		/*
		 * Strand: Defines a strand. Uses a base and enables you to iterate forward or backward from it.
		 * Avoids loops by remembering the start base.
		 * The Strand interface enables iterating over bases and give as much possibility to use of the C++ STL as possible.
		 */

		class VHELIXAPI Strand {
		public:
			inline Strand(const Base & base) : m_base(base) {

			}

			inline Strand(const Strand & strand) : m_base(strand.m_base) {

			}

			inline Strand(const MObject & object) : m_base(object) {

			}

			inline Strand(const MDagPath & dagPath) : m_base(dagPath) {

			}

			/*
			 * The iterator interfaces
			 */

			//static const int FORWARD = 1, BACKWARD = 2;
			enum {
				FORWARD = 1,
				BACKWARD = 2
			};

			template<int Direction>
			class Iterator : public std::iterator<std::forward_iterator_tag, Base> {
				friend class Strand;
			public:
				Base & operator*() {
					return m_targetBase;
				}

				Base *operator->() {
					return &m_targetBase;
				}

				Iterator<Direction> operator++(int x) {
					switch (Direction) {
						case FORWARD:
							return Iterator<Direction>(*m_strand, m_targetBase.forward());
						case BACKWARD:
							return Iterator<Direction>(*m_strand, m_targetBase.backward());
						default:
							std::cerr << "ERROR BROKEN ITERATOR" << std::endl; // Mostly just because VC++ complains about not returning a value
							return Iterator<Direction>(*m_strand, m_targetBase.forward());
					}
				}

				Iterator<Direction> & operator++() {
					switch (Direction) {
						case FORWARD:
							m_targetBase = m_targetBase.forward();
							break;
						case BACKWARD:
							m_targetBase = m_targetBase.backward();
							break;
						default:
							std::cerr << "ERROR BROKEN ITERATOR" << std::endl; // Mostly just because VC++ complains about not returning a value
					}

					m_firstTarget = false;

					return *this;
				}

				bool operator==(const Iterator<Direction> & it) {
					if (m_isEnd) {
						if (it.m_isEnd)
							return m_strand == it.m_strand;
						else {

							if (!it.m_targetBase)
								return true;

							if (it.m_targetBase == it.m_strand->m_base && !it.m_firstTarget)
								// This is a loop and we're back in the beginning
								return true;

							return false;
						}
					}
					else {
						if (it.m_isEnd) {
							if (!m_targetBase)
								return true;

							if (m_targetBase == m_strand->m_base && !m_firstTarget)
								// This is a loop and we're back in the beginning
								return true;

							return false;
						}
						else
							// None of the compared iterators are end iterators, just do a regular comparison
							return m_strand == it.m_strand && m_targetBase == it.m_targetBase && m_isEnd == it.m_isEnd;
					}
				}

				bool operator!=(const Iterator<Direction> & it) {
					return !this->operator==(it);
				}

				/*
				 * This method can ONLY be called after a successful iteration. Returns true if the iteration stopped because of a loop and false
				 * if it was because of an end base.
				 */

				bool loop() {
					return !m_firstTarget && m_strand->m_base == m_targetBase;
				}

			private:

				Iterator(Strand & strand, bool isEnd = false, bool firstTarget = true) : m_strand(&strand), m_targetBase(strand.m_base), m_isEnd(isEnd), m_firstTarget(firstTarget) {

				}

				/*
				 * Only used by the right hand ++
				 */

				Iterator(Strand & strand, Base & base) : m_strand(&strand), m_targetBase(base), m_isEnd(false), m_firstTarget(false) {

				}

				Strand *m_strand;
				Base m_targetBase;

				bool m_isEnd, m_firstTarget;
							// The iterators from forward_end or backward_end will have m_isEnd set
							// so that we know in the comparison to do an end check and not a regular difference check
							// Iterators will have firstTarget set before their first increase to enable us to evaluate loops and not abort
							// on the first base.
			};

			typedef Iterator<FORWARD> ForwardIterator;
			typedef Iterator<BACKWARD> BackwardIterator;

			inline ForwardIterator forward_begin() {
				return ForwardIterator(*this);
			}

			inline ForwardIterator forward_end() {
				return ForwardIterator(*this, true);
			}

			inline BackwardIterator reverse_begin() {
				return BackwardIterator(*this);
			}

			inline BackwardIterator reverse_end() {
				return BackwardIterator(*this, true);
			}

			/*
			 * rewind: Iterates backwards until a base without a backward connection is found.
			 * Or, if the strand is circular, just iterates one complete lap.
			 *
			 * The base defining the strand is updated if an end base is found.
			 */
			void rewind();

			inline Base & getDefiningBase() {
				return m_base;
			}

			inline const Base & getDefiningBase() const {
				return m_base;
			}

			void setDefiningBase(const Base & base) {
				m_base = base;
			}

			bool contains_base(Base & base, std::string & status);
			bool contains_base(const Base & base, std::string & status);

			/*
			 * Comparison operator for std::find_if
			 */

			inline static bool Contains_base(Strand strand, Base base) {
				std::string status;
				return strand.contains_base(base, status);
			}

			/*
			 * Can be used with std::find_if to find the end base
			 */

			class BaseTypeFunc {
			public:
				inline BaseTypeFunc(Base::Type type) : m_type(type) { }
				inline BaseTypeFunc(const BaseTypeFunc & func) : m_type(func.m_type) { }

				inline bool operator() (Base & base) const {
					std::string status;
					return base.type(status) == m_type;
				}

			private:
				Base::Type m_type;
			};

			bool operator==(Strand & strand);
			bool operator==(const Strand & strand);

		protected:
			Base m_base; // The base defining the strand
		};
	}
}


#endif /* STRAND_H_ */
