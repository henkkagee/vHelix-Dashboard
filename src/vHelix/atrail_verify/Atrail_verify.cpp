#include "Atrail_verify.hpp"

int atrail_verify::main(int argc, const char* argv[], vHelix &parent)
{
	parent.sendToConsole_("INFO: Atrail_verify, verifies whether the given trail is a valid Atrail with respect to the given planar embedding.\n");
	if(argc != 3)
	{
		parent.sendToConsole_("ERROR! Usage: Atrail_verify edgecodefile edgetrailfile\n");
		return 1;
	}else
	{
		std::vector<std::vector<size_t> > edge_code;
		std::list<std::size_t> edge_trail;
		if(!read_edge_code(std::string(argv[1]), edge_code)) return 1;
		std::stringstream info;
		info<<"INFO: Reading trail from file "<<std::string(argv[2])<<std::endl;
		std::string sendStr = info.str();
		parent.sendToConsole_(sendStr);
        if(!read_trail(std::string(argv[2]), edge_trail, parent)) return 1;
        parent.sendToConsole_(print_walk(edge_trail, "INFO: Trail from file: "));
        if ((Atrail_verify(edge_code, edge_trail, parent) == true)) {
			parent.sendToConsole_("Yes, the given trail is a valid A-trail with respect to the embedding\n");
		}
		else {
			parent.sendToConsole_("No, the given trail is not a valid A-trail with respect to the embedding\n");
		}
		return 0;
	}
}

bool atrail_verify::read_trail(std::string input_filename, std::list<std::size_t> & trail, vHelix &parent)
{
	std::ifstream ifs(input_filename.c_str(), std::ifstream::in);
	if( !ifs.is_open())
	{
		parent.sendToConsole_("ERROR! Unable to read file ");
		parent.sendToConsole_(input_filename);
		parent.sendToConsole_("\n");
		return false;
	}else
	{
		trail.clear();
		//trail = std::list<std::size_t>(0);
		std::string line;
		std::getline(ifs, line);
		std::istringstream iss(line);
		std::size_t next_edge;
		while(iss >> next_edge) {
			trail.push_back(next_edge);
		}
		ifs.close();
		return true;
	}
}

bool atrail_verify::Atrail_verify(const std::vector<std::vector<std::size_t> > & edge_code,  const std::list<std::size_t> & trail_edgelist, vHelix &parent)
{
	std::size_t m = 0; // number of edges
	for( unsigned int i = 0; i < edge_code.size(); ++i ) m += edge_code[i].size();
	m = m / 2;
	std::vector<unsigned short> num_visits(m, 0);
	std::vector<std::vector<unsigned int> > ind2pair(m, std::vector<unsigned int>(0));
	for( unsigned int i = 0; i < edge_code.size(); ++i)
	{
		for( unsigned int j = 0; j < edge_code[i].size(); ++j)
		{
			ind2pair[edge_code[i][j]].push_back(i);
		}
	}

	if( m != trail_edgelist.size()) return false;
	else if (trail_edgelist.size() == 0)
	{
		parent.sendToConsole_("ERROR! The trail is empty!\n");
		return false;
	}
	else{
		//If the trail list begins with parallel edges e1 and e2 (i.e e1 and e2 have the same end nodes u,v), 
		//is the trail to be interpreted as u,e1,v,e2,u or v,e1,u,e2,v? The trail may be valid with one interpretation
		//but invalid with the other. We use the optimistic interpretation and check both in two rounds.		
		Vertex next = ind2pair[trail_edgelist.front()][1];		
		unsigned char round = 0; 
		bool is_valid_trail = true; // Is it a valid Eulerian trail?
		bool is_valid_Atrail = true; // Is it a valid Atrail?
		while(round < 2)
		{
			std::size_t loc, pre, suc;
			std::list<std::size_t>::const_iterator nextedge;
			is_valid_trail = true;
			is_valid_Atrail = true;
			for( std::list<std::size_t>::const_iterator it = trail_edgelist.begin(); it!= trail_edgelist.end(); ++it)
			{
				if((++num_visits[*it]) > 1)
				{
					is_valid_trail = false; // An edge is visited more than once
				}
				if( it != (--trail_edgelist.end()))
				{
					nextedge = ++it;
					--it;
				}else nextedge = trail_edgelist.begin();

				loc = std::distance(edge_code[next].begin(), std::find(edge_code[next].begin(),edge_code[next].end(), *(it)));
				pre = ((loc == edge_code[next].size() - 1)? 0 : (loc + 1));
				suc = ((loc == 0) ? (edge_code[next].size() - 1):(loc - 1));
				if( edge_code[next][pre] != *nextedge
						&& edge_code[next][suc] != *nextedge )
				{
					// The two consecutive edges are not neighbors in the cyclic order of edges around the joining node
					is_valid_Atrail = false;
					break;
				}
				next = (ind2pair[*nextedge][0] == next ? ind2pair[*nextedge][1] : ind2pair[*nextedge][0]);
			}
			if(!is_valid_trail) break;
			if( is_valid_trail && is_valid_Atrail) break;
			round++;
			next = ind2pair[trail_edgelist.front()][0]; // if it fails try the other interpretation.
			num_visits = std::vector<unsigned short>(m,0);
		}

		return is_valid_trail && is_valid_Atrail;
	}
}
