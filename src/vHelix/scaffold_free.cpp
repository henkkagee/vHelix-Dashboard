#include "scaffold_free.h"
#include "design.h"

using namespace dir;
using namespace boost;

bool dir::write_dimacs(std::string output_filename, DiGraph & G)
{
	std::string line;
	std::ofstream ofs(output_filename.c_str(), std::ifstream::out);
	if( !ofs.is_open())
	{
		std::cerr<<"Unable to open file "<<output_filename;
		return false;
	}
	ofs<<"p edge "<<num_vertices(G)<<" "<<num_edges(G)<<"\n";
	EdgeIterator e_it, e_end;
	tie(e_it, e_end) = edges(G);
	Vertex u, v;
	for( ; e_it != e_end; e_it++)
	{
		u = source(*e_it, G);
		v = target(*e_it, G);
		ofs<<"e "<< u + 1<<" "<<v+1<<"\n";
	}
	ofs.close();
	return true;
}

std::string dir::print_walk(std::vector<Vertex> Trail, std::string label)
{
    std::stringstream sstr;
	std::cout<<label;
	for( auto it = Trail.begin(); it != Trail.end(); it++)
	{
        sstr<<*it<<" ";
	}
    sstr<<std::endl;
    std::string ret(sstr.str());
    std::cout << ret;
    return ret;
}

Scaffold_free::Scaffold_free(const std::string meshpath, const std::string meshname) : Design(meshpath, meshname)
{}

int Scaffold_free::createGraph() {
    //using namespace undir;
    outstream << "creating Graph...\n";
    std::vector<bool> check_double(number_vertices*number_vertices,false);
    graph = DiGraph(number_vertices);
    int nodes_per_face;
    int start_node;
    int end_node;
    int aux;
    int max;
    int min;
    size_t edge_ind = 0;
    for (unsigned int i = 0; i < number_faces; i++) {
        nodes_per_face = faces[i].size();
        start_node = faces[i][0] + 1;
        aux = start_node;
        end_node = faces[i][1] + 1;
        max = std::max(start_node,end_node);
        min = std::min(start_node,end_node);
        if (check_double[min + (max-1)*(max-2)/2] == false) {
            add_edge(start_node-1,end_node-1,edge_ind,graph);
            edge_ind++;
            check_double[min + (max-1)*(max-2)/2] = true;
        }
        for (int j = 2; j < nodes_per_face; j++) {
            start_node = end_node;
            end_node = faces[i][j] + 1;
            max = std::max(start_node, end_node);
            min = std::min(start_node, end_node);
            if (check_double[min + (max-1)*(max-2)/2] == false)
            {
                add_edge(start_node-1,end_node-1,edge_ind,graph);
                edge_ind++;
                check_double[min + (max-1)*(max-2)/2] = true;
            }
        }
        start_node = end_node;
        end_node = aux;
        max = std::max(start_node, end_node);
        min = std::min(start_node, end_node);
        if (check_double[min + (max-1)*(max-2)/2] == false)
        {
            add_edge(start_node-1,end_node-1,edge_ind,graph);
            edge_ind++;
            check_double[min + (max-1)*(max-2)/2] = true;
        }
    }
    for (unsigned int i = 0; i < number_edges; i++) {
        start_node = non_face_edges[i][0];
        end_node = non_face_edges[i][1];
        add_edge(start_node,end_node,edge_ind,graph);
        edge_ind++;
    }
    outstream << "Successfully created the Graph\n";

    std::string dimacs(name);
    write_dimacs(dimacs.append(".dimacs"),graph);
    return 1;
}

int Scaffold_free::find_trail()
{

    std::vector<std::vector<Vertex>> isolated_trails;
    unsigned int nof_trails = 0;
    std::vector<Vertex> Trail(0);
    std::vector<Vertex>::iterator insert_loc;
    VertexIterator vit = boost::vertices(graph).first;
    VertexIterator vend = --(boost::vertices(graph).second);
    while(out_degree(*vit, graph) == 0)
    {
        if( vit == vend)
        {
            outstream<<"WARNING! The graph consists of isolated vertices, the trail is empty"<<std::endl;
            return 1;
        }
        vit++;
    }
    Vertex v = *vit;
    //Trail.push_back(v);
    insert_loc = Trail.end();
    Vertex prev;
    Vertex next;
    AdjacencyIterator adIt, adEnd;
    //AdjacencyIterator neiSt, neiEnd;

    // unmarked edges of a vertex, every edge should be traversed once in both directions
    // an element in unused_edges represents an edge directed *AWAY FROM* a vertex
    std::vector< std::list<std::pair<Vertex,bool> > >unused_edges(num_vertices(graph));
    unsigned int i=0;
    for(auto myIt = unused_edges.begin(); myIt != unused_edges.end(); myIt++, i++)
    {
        //tie(neiSt, neiEnd) = adjacent_vertices(i, G);
        for(auto neiIt = embedding[i].begin(); neiIt != embedding[i].end(); neiIt++)
        {
            myIt->push_back(std::make_pair((unsigned int)*neiIt,true));
        }
    }
    std::set<Vertex> in_trail_has_unused; //vertices on the current tour that have unused edges
    in_trail_has_unused.insert(v);

    bool first = true;
    while (!in_trail_has_unused.empty())
    {
        nof_trails++;
        v = *in_trail_has_unused.begin();
        //insert_loc = std::find( Trail.begin(), Trail.end(), v);
        insert_loc = Trail.end();
        //insert_loc = Trail.insert(insert_loc,99999999);
        insert_loc = Trail.insert(insert_loc,v);
        //std::cout << "Insert location of new trail: " << std::distance(insert_loc, Trail.begin()) << std::endl;
        insert_loc++;
        prev = v;
        next = v;
        first = true;
        while(1)
        {
            Vertex adIt;
            if (first == true) {
                adIt = std::find_if(unused_edges[next].begin(),unused_edges[next].end(),[](std::pair<Vertex,bool>& edge) {
                            return edge.second == true;
                        })->first;
                first = false;
                //std::cout << next << " to " << adIt << " (start)" << std::endl;
            }
            else {
                auto tmp = std::find_if(unused_edges[next].begin(),unused_edges[next].end(), [prev](std::pair<Vertex,bool>& edge) {
                    return (prev == edge.first);
                });
                std::pair<Vertex,bool> maybe_unused_edge;
                if (std::distance(tmp, unused_edges[next].end()) == 1) {
                    maybe_unused_edge = unused_edges[next].front();

                    //adIt = unused_edges[next].front().first; //last vertex in vertex code -> first vertex in vertex code

                    //std::cout << "(found, end) ";
                }
                else {
                    tmp++;
                    maybe_unused_edge = *tmp;
                    //adIt = tmp->first;
                    //std::cout << "(found) ";
                }
                if (maybe_unused_edge.second == false) {
                    //std::cout << "Next edge used, starting new trail\n";
                    break;
                }
                else {
                    adIt = maybe_unused_edge.first;
                    //std::cout << next << " to " << adIt <<  std::endl;
                }
            }


            prev = next;
            next = adIt;
            std::find(unused_edges[prev].begin(), unused_edges[prev].end(), std::make_pair(next,true))->second = false;
            in_trail_has_unused.insert(next);
            insert_loc = Trail.insert(insert_loc,next);
            insert_loc++;
        }//!(unused_edges[next].second == false));
        //std::cout << "DEBUG: "; print_walk(Trail, "Trail ");


        for(std::set<Vertex>::iterator setIt = in_trail_has_unused.begin(); setIt != in_trail_has_unused.end();)
        {
            if (std::find_if(unused_edges[*setIt].begin(),unused_edges[*setIt].end(),[](std::pair<Vertex,bool>& edge) {
                          return (edge.second == true);
                      }) == unused_edges[*setIt].end()) // condition fulfilled when vertex has no (outgoing) unused edges
            {
                in_trail_has_unused.erase(setIt++);
            }
            else
            {
                ++setIt;
            }
        }

        isolated_trails.push_back(Trail);
        Trail.clear();
    }
    std::cout << "Trail as node list\n";
    for (unsigned int i = 0; i < nof_trails; i++) {
        print_walk(isolated_trails[i], "");
    }
    nodetrail = isolated_trails;
    //print_walk(Trail, "Trail as edge list: \n");
    return 0;
}

int Scaffold_free::main() {
    if (readPLY() < 1) return 0;
    std::cerr << "read ply\n";
    std::cerr << vertices.size() << nodetrail.size() << number_vertices<< std::endl;
    if (createEmbedding() < 1) return 0;
    std::cerr << "created embedding\n";
    std::cerr << vertices.size() << nodetrail.size()<< std::endl;
    if (createGraph() < 1) return 0;
    std::cerr << "Created graph\n";
    std::cerr << vertices.size() << nodetrail.size()<< std::endl;;
    if (!find_trail()) return 0;
    std::cerr << "Found Atrail\n";
    std::cerr << vertices.size() << " " << nodetrail.size()<< std::endl;
    return 1;
}

int Scaffold_free::relax(const QVector<QVariant> args) {
    for (unsigned int i = 0; i < number_vertices; i++) {
        std::cerr << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z<< std::endl;
    }

    using namespace PhysXRelax;
    double *dblArgs = new double[9];
    bool *boolArgs = new bool[3];
    unsigned int k = 0; unsigned int j = 0;
    for (unsigned int i = 0; i < 15; i++) {
        switch (i) {
            case 0: // [0]scaling
            case 2: // [1]density
            case 3: // [2]spring stiffness
            case 4: // [3]fixed spring stiffness
            case 5: // [4]spring damping
            case 7: // [5]static friction
            case 8: // [6]dynamic friction
            case 9: // [7]restitution
            case 10:// [8]rigid body sleep threshold
                dblArgs[k] = args[i].toDouble();
                k++;
                break;
            case 1:  // discretize lengths
            case 6:  // attach fixed
            case 11: // visual debugger
                if (args[i].toString() == "Disabled") {
                    boolArgs[j] = false;
                }
                else {
                    boolArgs[j] = true;
                }
                j++;
                break;
        }
    }
    const int iterations = args[12].toInt();
    PhysXRelax::physics::settings_type physics_settings;
    PhysXRelax::scene::settings_type scene_settings;
    PhysXRelax::Helix::settings_type helix_settings;
    PhysXRelax::parse_settings(dblArgs,boolArgs,physics_settings,scene_settings,helix_settings);
    //PhysXRelaxation = new PhysXRelaxation(name,physics_settings,scene_settings,helix_settings, iterations);
    PhysXRelaxation relaxation(name,physics_settings,scene_settings,helix_settings, iterations);
    std::cerr << "Created relaxation object\n";
    std::vector<std::vector<unsigned int>> input_paths;
    for (auto it = nodetrail.begin(); it != nodetrail.end(); it++) {
        std::vector<unsigned int> input_path;
        std::copy((*it).begin(),(*it).end(),std::back_inserter(input_path));
        input_paths.push_back(input_path);
    }
    
    //std::copy(nodetrail.begin(),nodetrail.end(),std::back_inserter(input_path));

    //outstream << relaxation->getoutstream().c_str();
    std::cerr << "Running scaffold_free_main(). Argument sizes: " << vertices.size() << ", " << input_paths.size()<< std::endl;
    relaxation.scaffold_free_main(vertices,input_paths);
    outstream << relaxation.getoutstream().c_str();
    return 1;
}

/*int trail_search::main(int argc, const char* argv[])
{
    std::cout << "INFO: trail_search, searchs for an eulerian trail for a given planar embedding.\n";
    if(argc < 3)
	{
        std::cout << "ERROR! Usage: trail_search input_edgecode input_dimacs [output_edgetrail] [output_nodetrail]";
		return 1;
    }else
	{
        std::string vertexcode_file(argv[1]);
        std::string dimacs_file(argv[2]);
        std::string vertextrail_file;
		std::string nodetrail_file;
        if( argc == 3)
		{
            nodetrail_file = vertexcode_file.substr(0, vertexcode_file.find_last_of('.')).append(".ntrail");
		}else{
            nodetrail_file = std::string(argv[2]);
			if(argc >= 4) nodetrail_file = std::string(argv[3]);
            if(argc >= 5) std::cout << "WARNING: You have more arguments than required, ignoring the arguments after the third ...\n";
		}

		std::vector<std::vector<size_t> > edge_code;
        DiGraph G;
        read_dimacs(dimacs_file,G);
		// Attempt to read the edge code from the provided file and process if read was successful.
        if(read_vertex_code(std::string(vertexcode_file), edge_code))
		{
            std::vector<std::list<Vertex>> node_trail;
            std::string output;
            find_trail(G, edge_code,  node_trail);
            {
                std::cout << "INFO: Found a valid trail for the graph\n";
				std::ofstream nfs(nodetrail_file.c_str(), std::ios::out);
                if( !nfs.is_open())
                {
					std::stringstream nodetrailError;
					nodetrailError << "ERROR! Unable to create file "<<nodetrail_file<<std::endl;
                    output = nodetrailError.str();
                    std::cout << output;
					return 2;
				}else
				{
					std::stringstream infoTrail;
					infoTrail << "INFO: Appending the trail as node list to file "<<nodetrail_file<<std::endl;
                    output = infoTrail.str();
                    std::cout << output;
                    for( std::vector<std::list<Vertex>>::iterator v_it = node_trail.begin(); v_it != node_trail.end(); v_it++) {
                        std::list<Vertex> partial_trail = *v_it;
                        for( std::list<Vertex>::iterator l_it = partial_trail.begin(); l_it != partial_trail.end(); l_it++)
                        {
                                nfs<<*l_it<<" ";
                        }
                        nfs << "\n";
                    }
				}
				nfs.close();
				return 0;
            }
		} else
		{
            std::cout << "ERROR! Invalid vertex code..., aborting with error\n";
			return 3;
		}
	}
}*/