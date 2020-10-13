/*
 * ply_to_embedding.cpp
 *
 *  Created on: Jul 13, 2015
 *  Author: Abdulmelik Mohammed
 *  Takes the embedding information of a 3D object's mesh (ply file) and creates an embedding (vcode file).
 *  Argument 1: filename of ply document (ply ascii format http://paulbourke.net/dataformats/ply).
 *  Argument 2 (Optional): output embedding filename in vcode format, uses the ply file basename + vcode if not given (vcode is simply list of adjacent vertices according to their clockwise order).
 *  Preconditions: TODO: mesh is 2-vertex-connected
 */

/*
 * Modified 2020 in order to be included and compiled in the new vHelix workspace
 * Created .hpp file, added reference argument to console & redirected output.
 * - Henrik Granö
*/

#include "ply_to_embedding.hpp"

using namespace std;



int ply_to_embedding::main(int argc, const char *argv[], vHelix &parent)
{
    parent.sendToConsole_("INFO: ply_to_embedding, A simple PLY to embedding (vcode) converter.\n");
    parent.sendToConsole_("INFO: PLY is a simple polygon format for describing 3d meshes, more info about the format can be found from http://paulbourke.net/dataformats/ply.\n");
    parent.sendToConsole_("INFO: vcode (vertex code) is a simple format where each line reprsents the local rotational order of the adjacent vertices of a vertex corresponding to the line\n");
    parent.sendToConsole_("INFO: This program converts from a simple PLY type with only vertex position and face desciption lines to a vcode, i.e. local rotation of adjacent vertices about each vertex.\n");
    /*if(argc >= 2)
    {*/
		// Parsing arguments
		std::string inputply(argv[1]);
		std::string outputembedding;
		if( argc >= 3 )
		{
            if(argc > 3) parent.sendToConsole_("WARNING: More arguments than prescribed, ignoring arguments after the output_embedding argument! Type ply_to_dimacs to read usage instructions!\n");
			outputembedding = std::string(argv[2]);
		}else
		{
			outputembedding = inputply.substr(0, inputply.find_last_of('.')).append(".vcode");
		}
		embedding_t embedding;
        std::stringstream fetching;
        fetching << "INFO: Fetching the embedding from the PLY file " << inputply << std::endl;
        parent.sendToConsole_(fetching.str());
        if(!ply_to_embedding(inputply.c_str(), embedding, parent)) return 1;
        std::stringstream success;
        success << "INFO: Successfully fetched the embedding from the PLY."<< std::endl;
        parent.sendToConsole_(success.str());
        std::ofstream ofs(outputembedding.c_str(), std::ofstream::out);
		if (!ofs.is_open())
		{
            std::stringstream err;
            err << "ERROR: Unable to create file " << outputembedding;
            parent.sendToConsole_(err.str());
            return false;
		}
		ofs << "p " << embedding.size() << "\n";
        for (unsigned int i = 0; i < embedding.size(); ++i)
		{
            for (unsigned int j = 0; j < embedding[i].size(); j++)
			{
				ofs << embedding[i][j] << " ";
			}
			ofs << std::endl;
		}
		ofs.close();
        std::stringstream successWrote;
        successWrote << "INFO: Successfully wrote the embedding to " << outputembedding << "." << std::endl;
        parent.sendToConsole_(successWrote.str());
		return EXIT_SUCCESS;
    /*}
    else
	{
        parent.sendToConsole_("ERROR: Improper usage\n");
		std::cerr << "Usage: " << "ply_to_dimacs " << "input_ply"<<" [output_embedding]"<<std::endl;
		return 1;
	}*/

}


// Reads the ply file to the graph object.
bool ply_to_embedding::ply_to_embedding(std::string input_filename, embedding_t & embedding, vHelix &parent)
{
	std::string line;
	std::ifstream myfile (input_filename.c_str());
	size_t number_faces;
	size_t number_nodes;
	std::vector<size_t> degrees;
	std::vector<std::vector<size_t> > facelist;
	//std::vector <std::vector<size_t> > facemat;
	std::vector<std::vector<neighbours> > face_neigh_list; // neighbours list according to order of face descriptions
	int nodes_per_face;

	if (myfile.is_open())
	{
		getline(myfile, line);
		if( line.substr(0,3).compare("ply")) 
		{
            parent.sendToConsole_("ERROR: the given file is not a PLY file.\n");
			return false; //Making sure it is a ply file
		}
		
		while ( myfile.good() )
		{
			getline (myfile, line);
			//std::cout<<"Line: "<<line<<std::endl;
			if (line.substr(0,6).compare("format") == 0)
			{
				std::stringstream ss(line.c_str());
				std::string type;
				std::string line_header;
				//string version;
				ss>>line_header>>type;									
				if(type.compare("ascii") != 0)
				{
                    parent.sendToConsole_("ERROR: the PLY file is not ascii format.\n");
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
                std::stringstream vertNum;
                vertNum<<"INFO: Number of vertices: "<<number_nodes<<std::endl;
                parent.sendToConsole_(vertNum.str());
				embedding = embedding_t(number_nodes);
				degrees = std::vector<size_t>(number_nodes);
				face_neigh_list = std::vector<std::vector<neighbours> >(number_nodes);
				//facemat = std::vector<std::vector<size_t> >(number_nodes, std::vector<size_t>(number_nodes, 0));
			}

			if (line.substr(0,12).compare("element face") == 0)
			{
				// Get the number of faces
				char *temp = new char[line.substr(13).size() + 1];
				strcpy(temp, line.substr(13).c_str());
				number_faces = atoi(temp);
				delete temp;
                std::stringstream faceNum;
                faceNum<<"INFO: Number of faces: "<<number_faces<<std::endl;
                parent.sendToConsole_(faceNum.str());
				facelist = std::vector<std::vector<size_t> >(number_faces, std::vector<size_t>(0));
			}
			
			if (line.substr(0,10).compare("end_header") == 0)
			{
				// Skip the geometric positions of the verticecs
                for (unsigned int i = 0; i < number_nodes; i++)
					getline(myfile, line);
				neighbours neigh;
                parent.sendToConsole_("INFO: reading the face description lines ...\n");
				// Obtain facelist information from the face descriptions.
                for (unsigned int i = 0; i < number_faces; i++)
				{
					getline (myfile, line);
					//std::cout<<"line: "<<line<<std::endl;
					char * temp = new char[line.size()+1];
					strcpy (temp,line.c_str());
					char *p = strtok(temp, " ");
					nodes_per_face = atoi(p);
					facelist[i] = std::vector<size_t>(nodes_per_face);
					for (int j = 0; j < nodes_per_face; j++)
					{
						p = strtok(NULL, " ");
						facelist[i][j] = atoi(p);
						//degrees[atoi(p)]++;  degree of vertex is the total number of times it appears in face descriptions
						if (j >= 2)
						{
							neigh.back = facelist[i][j - 2];
							neigh.forw = facelist[i][j];
							face_neigh_list[facelist[i][j - 1]].push_back(neigh);
							//facemat[facelist[i][j - 1]][facelist[i][j]] = i + 1;
						}
					}
					neigh.back = facelist[i][nodes_per_face - 1];
					neigh.forw = facelist[i][1];
					face_neigh_list[facelist[i][0]].push_back(neigh);
					neigh.back = facelist[i][nodes_per_face - 2];
					neigh.forw = facelist[i][0];
					face_neigh_list[facelist[i][nodes_per_face - 1]].push_back(neigh);
                    delete[] temp;
				}
				DEBUGPRINT(std::cout << "Face list ..." << std::endl;)
                for (unsigned int i = 0; i < number_faces; i++)
				{
                    for (unsigned int j = 0; j < facelist[i].size(); j++)
					{
						DEBUGPRINT(std::cout << facelist[i][j] << " ";)
					}
					DEBUGPRINT(std::cout << std::endl;)
				}
                parent.sendToConsole_("INFO: Gathering embedding from face list ...\n");
				bool push_back = true;
                for (unsigned int i = 0; i < number_nodes; i++)
				{
					std::set<size_t> unique_neighs;
                    for (unsigned int l = 0; l < face_neigh_list[i].size(); l++)
					{
						unique_neighs.insert(face_neigh_list[i][l].forw);
						unique_neighs.insert(face_neigh_list[i][l].back);
					}
					degrees[i] = unique_neighs.size();
					embedding[i] = std::vector<size_t>(0);
					embedding[i].push_back(face_neigh_list[i][0].back);
					embedding[i].push_back(face_neigh_list[i][0].forw);
					size_t k = 0;
					push_back = true;
					while (push_back)
					{
						k = 0;
						while (face_neigh_list[i][k].back != embedding[i].back())
						{
							k++;
							if (k == face_neigh_list[i].size())
							{
								push_back = false;
								break;
							}
						}
						
						if (push_back)
						{
							if (face_neigh_list[i][k].forw == embedding[i].front())
								push_back = false;
							else embedding[i].push_back(face_neigh_list[i][k].forw);
						}
							
					}
					std::cout << "i = " << i << std::endl;
					while (embedding[i].size() < degrees[i])
					{
						k = 0;
						while (face_neigh_list[i][k].forw != embedding[i].front())
						{
							k++;
							if (k == face_neigh_list[i].size())
							{
								break;
							}
						}
						embedding[i].insert(embedding[i].begin(), face_neigh_list[i][k].back);
					}

				}
				DEBUGPRINT(std::cout << "Embedding ..." << std::endl;)
                for (unsigned int i = 0; i < number_nodes; i++)
				{	
                    for (unsigned int j = 0; j < embedding[i].size(); j++)
					{
						DEBUGPRINT(std::cout << embedding[i][j] << " ";)
					}
					DEBUGPRINT(std::cout << std::endl;)
				}

			}
		}
		myfile.close();
	}
	else {
        std::stringstream errFile;
        errFile << "ERROR: Unable to open file "<<input_filename<<std::endl;
        parent.sendToConsole_(errFile.str());
		return false;
	}
	return true;
}
