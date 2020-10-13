#ifndef _JSONIMPORTER_H_
#define _JSONIMPORTER_H_

#include <mayahelix/Definition.h>

#include <mayahelix/model/Base.h>
#include <mayahelix/model/Helix.h>

#include <mayahelix/json/json.h>

#include <map>
#include <vector>
#include <string>
#include <list>

#include <tuple>

namespace Helix {
	namespace Controller {
		/*
		 * The caDNAno JSON importer rewritten to use the cleaner API and be less buggy
		 */

		class VHELIXAPI JSONImporter {
		public:
			/*
			 * Parse the json file
			 */

			std::string parseFile(const char *filename);

		protected:
			/*
			 * Binary representation structures
			 */

			struct Base {
				int connections[4]; // to helix, to base, from helix, from base
				std::vector<Model::Base> bases; // only used by the create method to manage the created bases

				// if [-1, -1, -1, -1] it's no base
				bool isValid() const {
					return connections[0] + connections[1] + connections[2] + connections[3] != -4;
				}

				bool isEndBase() const {
					return connections[0] + connections[1] == -2 || connections[2] + connections[3] == -2;
				}

				bool hasPreviousConnection() const {
					return connections[0] + connections[1] != -2;
				}

				bool hasNextConnection() const {
					return connections[2] + connections[3] != -2;
				}
			};

			struct Helix {
				std::vector<Base> stap, scaf; // the strands
				std::vector<int> loop, skip;
				int col, row, direction; // coordinates for the helix
				std::vector<int> stap_colors;

				Model::Helix helix;
			};

			struct file {
				std::map<int, Helix> helices; // Maps "num" in JSON file to a helix
				std::string name, filename;
			} m_file;
		};
	}
}


// function has to live in the std namespace 
// so that it is picked up by argument-dependent name lookup (ADL).
namespace std {
	namespace
	{

		// Code from boost
		// Reciprocal of the golden ratio helps spread entropy
		//     and handles duplicates.
		// See Mike Seymour in magic-numbers-in-boosthash-combine:
		//     https://stackoverflow.com/questions/4948780

		template <class T>
		inline void hash_combine(std::size_t& seed, T const& v)
		{
			seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		// Recursive template code derived from Matthieu M.
		template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
		struct HashValueImpl
		{
			static void apply(size_t& seed, Tuple const& tuple)
			{
				HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
				hash_combine(seed, get<Index>(tuple));
			}
		};

		template <class Tuple>
		struct HashValueImpl<Tuple, 0>
		{
			static void apply(size_t& seed, Tuple const& tuple)
			{
				hash_combine(seed, get<0>(tuple));
			}
		};
	}

	template <typename ... TT>
	struct hash<std::tuple<TT...>>
	{
		size_t
			operator()(std::tuple<TT...> const& tt) const
		{
			size_t seed = 0;
			HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
			return seed;
		}

	};
}

#endif /* N _JSONIMPORTER_H_ */
