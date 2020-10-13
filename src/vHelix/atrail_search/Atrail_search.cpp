#include "Atrail_search.hpp"



int atrail_search::main(int argc, const char* argv[], vHelix &parent)
{
	parent.sendToConsole_("INFO: Atrail_search, searchs for an Atrail for a given planar embedding.\n");
    if(argc < 2)
	{
		parent.sendToConsole_("ERROR! Usage: Atrail_search input_edgecode [output_edgetrail] [output_nodetrail]");
		return 1;
    }else
	{
		std::string edgecode_file(argv[1]);
		std::string edgetrail_file;
		std::string nodetrail_file;
		if( argc == 2)
		{
			edgetrail_file = edgecode_file.substr(0, edgecode_file.find_last_of('.')).append(".trail");
			nodetrail_file = edgecode_file.substr(0, edgecode_file.find_last_of('.')).append(".ntrail");
		}else{
			edgetrail_file = std::string(argv[2]);
			if(argc >= 4) nodetrail_file = std::string(argv[3]);
			if(argc >= 5) parent.sendToConsole_("WARNING: You have more arguments than required, ignoring the arguments after the third ...\n");
		}

		std::vector<std::vector<size_t> > edge_code;
		// Attempt to read the edge code from the provided file and process if read was successful.
		if(read_edge_code(std::string(edgecode_file), edge_code))
		{
			std::list<std::size_t> edge_trail;
			Graph G(edge_code.size());
			std::size_t m = 0;
			for( unsigned int i = 0; i < edge_code.size(); ++i ) m += edge_code[i].size();
			m = m / 2;
			std::vector<std::vector<unsigned int> > ind2pair(m, std::vector<unsigned int>(0));
			for( unsigned int i = 0; i < edge_code.size(); ++i)
			{
				for( unsigned int j = 0; j < edge_code[i].size(); ++j)
				{
					ind2pair[edge_code[i][j]].push_back(i);
				}

			}

			for( unsigned int i = 0; i < m; ++i)
			{
				add_edge(ind2pair[i][0], ind2pair[i][1], i, G);
			}

			parent.sendToConsole_("INFO: Read graph from the edgecode ");
			parent.sendToConsole_(std::string(edgecode_file));
			parent.sendToConsole_("\n");
			//std::cout<<"INFO: Graph "<<edgecode_file<<"\n"<<to_string_graph(G)<<std::endl;
			std::list<Vertex> node_trail;
            std::string output;
            if(Atrail_search(G, edge_code, edge_trail, node_trail, parent) == true)
            {
				parent.sendToConsole_("INFO: Found an A-trail for the graph\n");
				std::ofstream ofs(edgetrail_file.c_str(), std::ios::out);
				if( !ofs.is_open())
				{
					std::stringstream fileError;
                    fileError << "ERROR: Unable to create file: " << edgetrail_file << std::endl;
                    output = (fileError.str());
                    parent.sendToConsole_(output);
					return 2;
				}else
				{
					std::stringstream trailAppend;
					trailAppend << "INFO: Appending the trail as edge list to file: " << edgetrail_file  << std::endl;
                    output = trailAppend.str();
                    parent.sendToConsole_(output);
					for( std::list<std::size_t>::iterator it = edge_trail.begin(); it != edge_trail.end(); it++)
					{
							ofs<<*it<<" ";
					}
				}
				ofs.close();
				std::ofstream nfs(nodetrail_file.c_str(), std::ios::out);
				if( !nfs.is_open())
				{
					std::stringstream nodetrailError;
					nodetrailError << "ERROR! Unable to create file "<<nodetrail_file<<std::endl;
                    output = nodetrailError.str();
                    parent.sendToConsole_(output);
					return 2;
				}else
				{
					std::stringstream infoTrail;
					infoTrail << "INFO: Appending the trail as node list to file "<<nodetrail_file<<std::endl;
                    output = infoTrail.str();
                    parent.sendToConsole_(output);
					for( std::list<Vertex>::iterator it = node_trail.begin(); it != node_trail.end(); it++)
					{
							nfs<<*it<<" ";
					}
				}
				nfs.close();
				return 0;
			}else
			{
				parent.sendToConsole_("ERROR! Unable to find an Atrail for the given code\n");
				return 1;
			}
		} else
		{
			parent.sendToConsole_("ERROR! Invalid edge code..., aborting with error\n");
			return 3;
		}
	}
}

