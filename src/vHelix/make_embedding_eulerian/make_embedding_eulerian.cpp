/*
* make_embedding_eulerian.cpp
*
*  Created on: July 14, 2015
*  Author: Abdulmelik Mohammed
*  Creates an eulerian embedding as an edge code from an embedding given as an vertex code and the associated multigraph containing the multiedges.
*  Argument 1: filename of input embedding in vcode format.
*  Argument 2: filename of input multigraph in dimacs format.
*  Argument 3 (optional): filename of output edge code, if not given the input vcode filename with an extension .ecode is used.
*/

#include "make_embedding_eulerian.hpp"

using namespace std;


int make_embedding_eulerian::main(int argc, const char* argv[], vHelix &parent)
{
    parent.sendToConsole_("INFO: make_embedding_eulerian, converts an embedding given as vcode (local rotation of vertices) to embedding as local rotation of edges.\n");
	if (argc < 3)
        parent.sendToConsole_("ERROR! Usage: make_embedding_eulerian input_vcode input_dimacs [output_ecode]\n");
	else
	{
		size_t i = 0, j = 0;
		std::string in_vcode(argv[1]);
		std::string in_dimacs(argv[2]);
		std::string out_ecode;
        std::string msg;
        std::stringstream msgout;

		if (argc >= 4) out_ecode = std::string(argv[3]);
		else out_ecode = in_vcode.substr(0, in_vcode.find_last_of('.')).append(".ecode");
		embedding_t vcode;
		Graph ingraph;
		
		if (!read_edge_code(in_vcode, vcode)) return 1; // vertex code is the same type as edge code and can be read as such ...
        else {
            msgout << "INFO: Successfully read vcode from " << in_vcode << "."<<std::endl;
            msg = msgout.str();
            parent.sendToConsole_(msg);
        }
		if (!read_dimacs(in_dimacs, ingraph)) return 1;
        else {
            msgout.str(std::string());
            msgout << "INFO: Successfully read multigraph from " << in_dimacs << "." << std::endl;
            msg = msgout.str();
            parent.sendToConsole_(msg);
        }
        DEBUGPRINT(std::cout << to_string_graph(ingraph) << std::endl;)

		
		embedding_t ecode(vcode.size()); //output edgecode
		std::vector<std::vector<vertex_edge_pair> > embedding_as_pair (vcode.size());
		DEBUGPRINT(std::cout << "DEBUG: vertex code ... "<<std::endl;)
		for (i = 0; i < vcode.size(); ++i)
		{
			embedding_as_pair[i] = std::vector<vertex_edge_pair>(vcode[i].size());
			DEBUGPRINT(std::cout << i << ": ";)
			for (j = 0; j < vcode[i].size(); ++j)
			{
				DEBUGPRINT(std::cout << vcode[i][j] << " ";)
				embedding_as_pair[i][j].vertex = vcode[i][j];
				embedding_as_pair[i][j].edge = -1; // sentinel, no edge assigned yet ...
			}
			DEBUGPRINT(std::cout << std::endl;)
		}

        parent.sendToConsole_("INFO: Creating the edge code from the given the vertex code and the multigraph ...\n");

		EdgeIterator e_it, e_end;
		boost::tie(e_it, e_end) = edges(ingraph);
		Vertex s, t;
        size_t e_ind;
        for (; e_it != e_end; e_it++)
		{
			s = source(*e_it, ingraph);
			t = target(*e_it, ingraph);
			i = 0;
			j = 0;
			vertex_edge_pair vep;
            e_ind = get(edge_index, ingraph, *e_it);
			while (embedding_as_pair[s][i].vertex != t) i++;
			while (embedding_as_pair[t][j].vertex != s) j++;
			if (embedding_as_pair[s][i].edge == -1)
			{
                embedding_as_pair[s][i].edge = e_ind;
                embedding_as_pair[t][j].edge = e_ind;
			}
			else
			{
				if (s < t)
				{
					vep.vertex = embedding_as_pair[s][i].vertex;
					vep.edge = e_ind;
					embedding_as_pair[s].insert(i == 0 ? embedding_as_pair[s].end() : embedding_as_pair[s].begin() + i, vep);
					vep.vertex = embedding_as_pair[t][j].vertex;
					embedding_as_pair[t].insert(j == embedding_as_pair[t].size() ? embedding_as_pair[t].begin() : embedding_as_pair[t].begin() + j + 1, vep);
				}
				else
				{
					vep.vertex = embedding_as_pair[s][i].vertex;
					vep.edge = e_ind;
					embedding_as_pair[s].insert(i == embedding_as_pair[s].size() ? embedding_as_pair[s].begin() : embedding_as_pair[s].begin() + i + 1, vep);
					vep.vertex = embedding_as_pair[t][j].vertex;
					embedding_as_pair[t].insert(j == 0 ? embedding_as_pair[t].end() : embedding_as_pair[t].begin() + j, vep);
				}
			}
			
			
		}

		for (i = 0; i < embedding_as_pair.size(); ++i)
		{
			ecode[i] = std::vector<size_t>(embedding_as_pair[i].size());
			for (j = 0; j < embedding_as_pair[i].size(); ++j)
            {
				ecode[i][j] = embedding_as_pair[i][j].edge;
			}
        }

		write_edge_code(out_ecode, ecode);
        msgout.str(std::string());
        msgout << "INFO: Wrote the embedding as an edge code to " << out_ecode << std::endl;
        msg = msgout.str();
        parent.sendToConsole_(msg);
		
		
		return EXIT_SUCCESS;

	}
	return EXIT_SUCCESS;
}



