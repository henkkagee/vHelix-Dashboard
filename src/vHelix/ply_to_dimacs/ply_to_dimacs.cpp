/*
 * ply_to_dimacs.cpp
 *
 *  Created on: Jul 11, 2013
 *  Author: Gilberto Garcia Perez
 *  Takes the graph information of a 3D object's mesh (ply file) and creates a graph (dimacs file).
 *  Argument 1: filename of ply document (ply ascii format http://paulbourke.net/dataformats/ply).
 *  Argument 2 (Optional): output graph filename in dimacs format, uses the ply file basename + dimacs if not given (dimacs format mat.gsia.cmu.edu/COLOR/general/ccformat.ps).
 */

/*
 * Modified in June 2020 in order to be included and compiled in the new vHelix workspace
 * Created .hpp file, added reference argument to console, redirected output and fixed memory leak on
 * line 190.
 * - Henrik Granö
*/

#include "ply_to_dimacs.hpp"

using namespace std;
using namespace boost;
 

int ply_to_dimacs::main(int argc, const char *argv[], vHelix &parent)
{
	Graph G;
    parent.sendToConsole_("INFO: ply_to_dimacs, A simple PLY to dimacs converter.\n");
    parent.sendToConsole_("INFO: PLY is a simple polygon format for describing 3d meshes, more info about the format can be found from http://paulbourke.net/dataformats/ply.\n");
    parent.sendToConsole_("INFO: Dimacs is a simple file format for representing graphs, more info about dimacs can be found from http://dimacs.rutgers.edu/Challenges/.\n");
    parent.sendToConsole_("INFO: This program converts from a simple PLY type with only vertex position and face desciption lines to a simple dimacs format with only edge desciptions.\n");
		// Parsing arguments
		std::string inputply(argv[1]);
		std::string outputdimacs;
		if( argc >= 3 )
		{
            if(argc > 3) parent.sendToConsole_("WARNING: more arguments than prescribed, ignoring arguments after the output_dimacs argument! Type ply_to_dimacs to read usage instructions!\n");
			outputdimacs = std::string(argv[2]);
		}else
		{
			outputdimacs = inputply.substr(0, inputply.find_last_of('.')).append(".dimacs");
		}

        if(!read_ply(inputply.c_str(), G, parent)) return 1;
        parent.sendToConsole_("INFO: Done reading the PLY file\n");
		std::vector<int> component(num_vertices(G));
		unsigned int num_components = connected_components(G, &component[0]);

		assert( num_components == 1);
		if( num_components != 1 )
		{
            parent.sendToConsole_("WARNING: The graph in the PLY is not connected!\n");
		}else
		{
            parent.sendToConsole_("INFO: The graph in the PLY is connected.\n");
		}	
		if (boyer_myrvold_planarity_test(G))
            parent.sendToConsole_("INFO: The graph in the PLY is planar.\n");
		else
		{
            parent.sendToConsole_("WARNING: The graph in the PLY is not planar!\n");
		}
        //std::cout<<"Graph:\n"<<to_string_graph(G, "Vertex ", "->\t\t\t", "\t" )<<std::endl;
		write_dimacs(outputdimacs, G);
        stringstream sstr;
        sstr << "INFO: Successfully converted the PLY "<<inputply<<" to dimacs "<<outputdimacs<<"."<<std::endl<<std::endl;
        parent.sendToConsole_(sstr.str());
		return EXIT_SUCCESS;

    /*
     else{
		std::cerr<<"ERROR: Improper usage"<<std::endl;
		std::cerr << "Usage: " << "ply_to_dimacs " << "input_ply"<<" [output_dimacs]"<<std::endl;
		return 1;
    }*/

}

// Reads the ply file to the graph object.
bool ply_to_dimacs::read_ply (std::string input_filename, Graph & G, vHelix &parent)
{
	string line;
	ifstream myfile (input_filename.c_str());
	int number_faces;
	int number_nodes;
	std::vector<bool> check_double;
	int nodes_per_face;
	int start_node;
	int end_node;
	int aux;
	int max;
	int min;
	if (myfile.is_open())
	{
		getline(myfile, line);
		if( line.substr(0,3).compare("ply")) 
		{
            parent.sendToConsole_("ERROR: the given file is not a PLY file\n");
			return false; //Making sure it is a ply file
		}
		
		while ( myfile.good() )
		{
			getline (myfile, line);
			//std::cout<<"Line: "<<line<<std::endl;
			if (line.substr(0,6).compare("format") == 0)
			{
				std::stringstream ss(line.c_str());
				string type;
				string line_header;
				//string version;
				ss>>line_header>>type;									
				if(type.compare("ascii") != 0)
				{
                    parent.sendToConsole_("ERROR: the PLY file is not ascii format\n");
					return false;
				}
			}
			if (line.substr(0,14).compare("element vertex") == 0)
			{
				// Get the number of vertices
				char *temp = new char[line.substr(15).size() + 1];
				strcpy(temp, line.substr(15).c_str());
				number_nodes = atoi(temp);
                delete temp;
				check_double = std::vector<bool>(number_nodes*number_nodes, false);
                std::stringstream n_vert;
                n_vert << "INFO: Number of vertices: "<<number_nodes<<std::endl;
                parent.sendToConsole_(n_vert.str());
				G = Graph(number_nodes);
			}
			if (line.substr(0,12).compare("element face") == 0)
			{
				// Get the number of faces
				char *temp = new char[line.substr(13).size() + 1];
				strcpy(temp, line.substr(13).c_str());
				number_faces = atoi(temp);
                std::stringstream n_faces;
                n_faces <<"INFO: Number of faces: "<<number_faces<<std::endl;
                parent.sendToConsole_(n_faces.str());
				delete temp;
			}
			if (line.substr(0,10).compare("end_header") == 0)
			{
				// Skip the geometric positions of the verticecs
				for (int i = 0; i < number_nodes; i++)
					getline(myfile, line);
				// Obtain adjacency information from the face descriptions.
				for (int i = 0; i < number_faces; i++)
				{
					getline (myfile, line);
					//std::cout<<"line: "<<line<<std::endl;
					char * temp = new char[line.size()+1];
					strcpy (temp,line.c_str());
					char *p = strtok(temp, " ");
					nodes_per_face = atoi(p);
					p = strtok(NULL, " ");
					start_node = atoi(p) + 1;
					p = strtok(NULL, " ");
					aux = start_node;
					end_node = atoi(p) + 1;
					p = strtok(NULL, " ");
					max = std::max(start_node, end_node);
					min = std::min(start_node, end_node);
					if (check_double[min + (max-1)*(max-2)/2] == false)
					{
						add_edge(start_node-1, end_node-1, G); //cout << "(" << start_node << ", " << end_node << ")\n"; //add_edge
						check_double[min + (max-1)*(max-2)/2] = true;
					}
					for (int i = 2; i < nodes_per_face; i++)
					{
						start_node = end_node;
						end_node = atoi(p) + 1;
						p = strtok(NULL, " ");
						max = std::max(start_node, end_node);
						min = std::min(start_node, end_node);
						if (check_double[min + (max-1)*(max-2)/2] == false)
						{
							add_edge(start_node-1, end_node-1, G); //cout << "(" << start_node << ", " << end_node << ")\n"; //add_edge
							check_double[min + (max-1)*(max-2)/2] = true;
						}
					}
					start_node = end_node;
					end_node = aux;
					max = std::max(start_node, end_node);
					min = std::min(start_node, end_node);
					if (check_double[min + (max-1)*(max-2)/2] == false)
					{
						add_edge(start_node-1, end_node-1, G); //cout << "(" << start_node << ", " << end_node << ")\n"; //add_edge
						check_double[min + (max-1)*(max-2)/2] = true;
					}
                    delete[] temp;
				}
			}
		}
		myfile.close();
	}
	else {
        std::stringstream err;
        err << "ERROR: Unable to open file "<<input_filename<<std::endl;
        parent.sendToConsole_(err.str());
		return false;
	}

	return true;
}
