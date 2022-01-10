#include "design.h"
#include <QDebug>

Design::Design(const std::string meshpath, const std::string meshname)
{
    name = meshname;
    path = meshpath;
    number_edges = 0;
    number_vertices = 0;
    number_faces = 0;
    nlohmann::json settings;
    std::ifstream file("settings.json");
    settings << file;
    write_intermediates = settings["write_intermediate_files"]; 
}

int Design::read3Dobject() {
    if (path.find_last_of(".ply") == path.length() - 1) {
        return readPLY();
    }
    else if (path.find_last_of(".obj") == path.length() - 1) {
        std::cout << "Reading .obj\n";
        return readOBJ();
    }
    else {
        outstream << "ERROR: File format not supported\n";
    }
}

int Design::readPLY() {
    happly::PLYData ply(path);
    std::vector<std::array<double,3>> posarr = ply.getVertexPositions();
    number_vertices = posarr.size();
    vertices.resize(number_vertices);
    for (unsigned int i = 0; i < number_vertices; i++) {
        vertices[i] = coordinates(posarr[i][0],posarr[i][1],posarr[i][2]);
    }
    for (unsigned int i = 0; i < number_vertices; i++) {
        std::cerr << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z<< std::endl;
    }
    adj_vertices.resize(number_vertices);
    faces = ply.getFaceIndices();
    //number_vertices = vertices.size();
    std::cerr << "Number of vertices: " << number_vertices << "\nVertex list size: " << vertices.size()<< std::endl;
    number_faces = faces.size();
    std::cerr << "Number of faces: " << number_faces<< std::endl;
    return 1;
}
int Design::readOBJ() {
    std::string line;
    std::ifstream myfile (path.c_str());
    while (myfile.good()) {
        getline(myfile,line);
        std::vector<std::string> temp;
        boost::split(temp,line,boost::is_any_of(" "));
        if (temp[0].substr(0,1).compare("#") == 0) { // comment
            continue;
        }
        else if (temp[0].substr(0).compare("v") == 0) {
            number_vertices++;
            //outstream << "Node\n";
            vertices.push_back(coordinates(std::stod(temp[1]), std::stod(temp[2]),std::stod(temp[3])));
            std::set<size_t> adj;
            adj_vertices.push_back(adj);
        }
        else if (temp[0].substr(0).compare("f") == 0) { // face
            number_faces++;
            //outstream << "\nFace\n";
            std::vector<size_t> face_nodes;

            for (auto it = temp.begin() + 1; it != temp.end(); it++) {
                std::vector<std::string> temp2;
                boost::split(temp2,*it,boost::is_any_of("/"));
                face_nodes.push_back(atoi(temp2[0].c_str()) - 1);
                std::cout << atoi(temp2[0].c_str()) - 1 << ", ";
            }
            faces.push_back(face_nodes);
        }
        else if (temp[0][0] == 'l') { // edge
            number_edges++;
            std::cout << "temp size: " << temp.size() << " \n";
            std::vector<size_t> nodes(2);
            nodes[1] = atoi(temp[1].c_str()) -1;
            nodes[2] = atoi(temp[2].c_str()) -1;
            non_face_edges.push_back(nodes);
            adj_vertices[nodes[1]].insert(nodes[2]);
            adj_vertices[nodes[2]].insert(nodes[1]);
        }
    }
    outstream << "Information retrieved from file\n";
    outstream << "Faces: " << faces.size() << ", nodes: " << vertices.size() << "\n";
    myfile.close();
    return 1;
}

int Design::createEmbedding() {
    std::cerr << "Number of vertices: " << number_vertices<< std::endl;
    std::vector<std::vector<neighbours> > face_neigh_list(number_vertices);  // neighbours list according to order of face descriptions
    std::vector<size_t> degrees(number_vertices);
    embedding = embedding_t(number_vertices);
    neighbours neigh;
    //std::cerr << "line 78"<< std::endl;
    for (unsigned int i = 0; i < number_faces; i++) {
        //std::cerr << "Face size: " << faces[i].size()<< std::endl;
        for (unsigned int j = 0; j < faces[i].size(); j++)
        {
            //std::cerr << faces[i][j]<< std::endl;
            if (j >= 2)
            {
                neigh.back = faces[i][j - 2];
                neigh.forw = faces[i][j];
                face_neigh_list[faces[i][j - 1]].push_back(neigh);
            }
            //std::cerr << faces[i][j];
        }
        //std::cerr << "Line 90"<< std::endl;
        neigh.back = faces[i][faces[i].size() - 1];
        neigh.forw = faces[i][1];
        face_neigh_list[faces[i][0]].push_back(neigh);
        neigh.back = faces[i][faces[i].size() - 2];
        neigh.forw = faces[i][0];
        face_neigh_list[faces[i][faces[i].size() - 1]].push_back(neigh);
    }

    //std::cerr << "line 98"<< std::endl;
    bool push_back = true;
    outstream << "INFO: Gathering embedding from face list ...\n";
    for (unsigned int i = 0; i < number_vertices; i++)
    {
        if (adj_vertices[i].empty()) { // node not in faceless edge

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
                    if (k == face_neigh_list[i].size()-1)
                    {
                        break;
                    }
                }
                embedding[i].insert(embedding[i].begin(), face_neigh_list[i][k].back);
            }
        }
        else { // node in faceless edge, use zig_zag combing
            std::cerr << "Line 161"<< std::endl;
            for (unsigned int j = 0; j < face_neigh_list[i].size(); j++) { //insert adjacent vertices in faces
                adj_vertices[i].insert(face_neigh_list[i][j].back);
                adj_vertices[i].insert(face_neigh_list[i][j].forw);
            }
            std::vector<std::vector<size_t>> adj_vec(number_vertices);
            for (unsigned int i = 0; i < number_vertices; i++) {
                adj_vec[i].assign(adj_vertices[i].begin(),adj_vertices[i].end());
            }
            for (unsigned int i = 0; i < number_vertices; i++) {
                std::map<double,std::vector<std::pair<size_t,double>>> latitudes;
                for (unsigned int j = 0; j < adj_vec[i].size(); j++) {
                    coordinates direction = vertices[i] - vertices[adj_vec[i][j]];
                    double phi = atan2(direction.x,direction.y);
                    double theta = acos(direction.z);
                    latitudes[phi].push_back(std::make_pair(adj_vec[i][j],theta));
                }
                std::vector<size_t> sorted_edges;
                bool desc = false;
                for (auto &it : latitudes) {
                    //double phi = it.first;
                    std::sort(it.second.begin(),it.second.end(),[desc](const std::pair<size_t,double> a, const std::pair<size_t,double> b) {
                        if (desc) return a.second > b.second;
                        else return b.second > a.second;
                    });
                    for (auto edge : it.second) {
                        sorted_edges.push_back(edge.first);
                    }
                    desc = !desc;
                }
                embedding[i] = sorted_edges;
            }

        }
    }
    if (write_intermediates) {
        write_embedding();
    }
    
    return 1;
}

bool Design::write_embedding() {
    std::string outputembedding(name);
    outputembedding.append(".vcode");
    std::ofstream ofs(outputembedding.c_str(), std::ofstream::out);
    if (!ofs.is_open())
    {
        outstream << "ERROR: Unable to create file " << outputembedding;
        return false;
    }
    std::cerr << "Writing to " << outputembedding.c_str()<< std::endl;
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
    return true;
}

std::string Design::getoutstream() {
    std::string ret = outstream.str();
    outstream.str( std::string() );
    outstream.clear();
    return ret;
}
