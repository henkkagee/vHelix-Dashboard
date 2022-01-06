#include "atrail.h"
#include <QDebug>

Atrail::Atrail(const std::string meshpath, const std::string meshname) : Design(meshpath, meshname)
{}

using namespace undir;

void undir::print_embedding(const planar_embedding_t & planar_embedding, const Graph & G, std::vector<std::vector<Vertex> > & embedding_as_vertex_order)
{
    Graph::vertex_iterator vertexIt,vertexEnd;
    tie(vertexIt, vertexEnd) = vertices(G);
    embedding_as_vertex_order = std::vector<std::vector<Vertex> >(num_vertices(G));
    unsigned int i = 0, j = 0;
    for( ;vertexIt != vertexEnd; ++vertexIt, i++ )
    {
        j=0;
        embedding_as_vertex_order[i] = std::vector<Vertex>(degree(*vertexIt, G));
        boost::property_traits<planar_embedding_t>::value_type::iterator I = planar_embedding[*vertexIt].begin();
        std::cout<<"cyclic order of nodes around vertex " <<*vertexIt<<": ";
        for( ; I != planar_embedding[*vertexIt].end(); I++, j++)
        {
            //std::cout<<" "<<((*vertexIt == I->m_source) ? I->m_target : I->m_source);

            embedding_as_vertex_order[i][j] = ((*vertexIt == I->m_source) ? I->m_target : I->m_source);
            std::cout<<" "<<embedding_as_vertex_order[i][j];
        }
        std::cout<<std::endl;
    }
    return;
}

void undir::embedding_to_edge_code(const planar_embedding_t & planar_embedding, Graph & G, std::vector<std::vector<std::size_t> > & edge_code)
{
    Graph::vertex_iterator vertexIt,vertexEnd;
    tie(vertexIt, vertexEnd) = vertices(G);
    edge_code = std::vector<std::vector<size_t> >(num_vertices(G));
    unsigned int i = 0, j = 0;
    for( ;vertexIt != vertexEnd; ++vertexIt, i++ )
    {
        j=0;
        edge_code[i] = std::vector<Vertex>(degree(*vertexIt, G));
        boost::property_traits<planar_embedding_t>::value_type::iterator I = planar_embedding[*vertexIt].begin();
        //std::cout<<"cyclic order of edges around vertex " <<*vertexIt<<": ";
        for( ; I != planar_embedding[*vertexIt].end(); I++, j++)
        {
            edge_code[i][j] = get(edge_index, G, *I);
            //std::cout<<" "<<edge_code[i][j];
        }
        //std::cout<<std::endl;
    }
    return;
}

std::string undir::print_walk(std::list<Vertex> Trail, std::string label)
{
    std::stringstream sstr;
    std::cout<<label;
    for( std::list<Vertex>::iterator it = Trail.begin(); it != Trail.end(); it++)
    {
        sstr<<*it<<" ";
    }
    sstr<<std::endl;
    std::string ret(sstr.str());
    std::cout << ret;
    return ret;
}

std::string undir::to_string_graph(const Graph & G, std::string lineheader, std::string liststart, std::string adj_sep, std::string line_sep)
{
    std::string printed_graph = "";
    VertexIterator vit, vend;
    for( tie(vit, vend) = vertices(G); vit != vend; vit++)
    {
        AdjacencyIterator ait, aend;
        printed_graph.append(lineheader);
        std::ostringstream convert;
        convert<<*vit;
        printed_graph.append(convert.str());
        printed_graph.append(liststart);
        convert.str("");
        for(tie(ait, aend) = adjacent_vertices(*vit, G); ait != aend; ait++)
        {
            convert<<*ait;
            printed_graph.append(convert.str());
            convert.str("");
            printed_graph.append(adj_sep);
        }
        printed_graph.append(line_sep);
    }
    return printed_graph;
}

std::string undir::to_string_graph_edgelist(const Graph & G, std::string pair_sep, std::string edge_frame_left, std::string edge_frame_right, std::string list_sep)
{
    std::string printed_graph = "";
    graph_traits<Graph>::edge_iterator edgeIt,edgeEnd;
    std::ostringstream convert;
    for( tie(edgeIt, edgeEnd) = edges(G); edgeIt != edgeEnd; ++edgeIt)
    {
        convert<<source(*edgeIt, G);
        printed_graph.append(edge_frame_left);
        printed_graph.append(convert.str());
        printed_graph.append(pair_sep);
        convert.str("");
        convert<<target(*edgeIt,G);
        printed_graph.append(convert.str());
        convert.str("");
        printed_graph.append(edge_frame_right);
        printed_graph.append(list_sep);
    }
    return printed_graph;
}

bool undir::read_dimacs(std::string input_filename, Graph & G)
{
    std::string line;
    std::ifstream ifs(input_filename.c_str(), std::ifstream::in);
    if( !ifs.is_open())
    {
        std::cerr<<"Unable to open file "<<input_filename<< std::endl;
        return false;
    }
    unsigned int n, m;
    unsigned int u, v;
    const unsigned int MAX_LENGTH = 10;
    char* line_type = new char[MAX_LENGTH];
    char* format = new char[MAX_LENGTH];
    std::size_t edge_ind = 0;
    while(std::getline(ifs, line))
    {
        if(!line.empty())
        {
            //std::cout<<line<<std::endl;
            switch (line[0])
            {
            case 'p':
                sscanf(line.c_str(),"%s %s %u %u", line_type, format, &n, &m);
                G = Graph(n);
                break;
            case 'e':
            case 'a':
                sscanf(line.c_str(),"%s %u %u", line_type, &u, &v);
                add_edge(u-1,v-1,edge_ind,G);
                edge_ind++;
                break;
            case 'c':
                break;
            default:
                break;
            }
        }
        line.clear();
    }
    ifs.close();
    delete[] line_type;
    delete[] format;
    return true;
}

bool undir::write_dimacs(std::string output_filename, Graph & G)
{
    std::string line;
    std::ofstream ofs(output_filename.c_str(), std::ifstream::out);
    if( !ofs.is_open())
    {
        std::cerr<<"Unable to open file "<<output_filename<< std::endl;
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

// TODO: Perhaps merge write_weighed_dimacs and write_dimacs
bool undir::write_weighed_dimacs(std::string output_filename, Graph & G)
{
    std::string line;
    std::ofstream ofs(output_filename.c_str(), std::ifstream::out);
    if( !ofs.is_open())
    {
        std::cerr<<"Unable to open file "<<output_filename<< std::endl;
        return false;
    }
    ofs<<"p edge "<<num_vertices(G)<<" "<<num_edges(G)<<"\n";
    EdgeIterator e_it, e_end;
    boost::tie(e_it, e_end) = edges(G);
    Vertex u, v;
    property_map < Graph, edge_weight_t >::type w = get(edge_weight, G);
    for( ; e_it != e_end; e_it++)
    {
        u = boost::source(*e_it, G);
        v = boost::target(*e_it, G);
        ofs<<"e "<< u + 1<<" "<<v+1<<" "<<w[*e_it]<<"\n";
    }
    ofs.close();
    return true;
}

bool undir::read_edge_code(std::string input_filename, std::vector<std::vector<std::size_t> > & edge_code)
{
    std::string line;
    unsigned long n_comp = 0;
    std::size_t n = 0;
    std::ifstream ifs(input_filename.c_str(), std::ifstream::in);
    if( !ifs.is_open())
    {
        std::cerr<<"ERROR! Unable to read file "<<input_filename<< std::endl;
        return false;
    }
    std::string header;
    //std::cout<<"Reading header"<<std::endl;
    std::getline(ifs,header);

    if(!header.empty())
    {
        char * param = new char[5];
        sscanf(header.c_str(), "%s %lu", param, &n_comp);
        n = n_comp;
        //std::cout<<"header: "<<header<<std::endl;
        edge_code = std::vector<std::vector<std::size_t> >(n);
    }else
    {
        std::cerr<<"ERROR! Incorrect header(first line) in file "<<input_filename<< std::endl;
        ifs.close();
        return false;
    }
    std::size_t i = 0;
    //std::cout<<"Reading adjacency"<<std::endl;
    while(std::getline(ifs, line))
    {

        if( i > n-1 )
        {
            std::cerr<<"Warning! the file has more lines than the given parameter in the header."<< std::endl;
        }
        edge_code[i] = std::vector<std::size_t>(0);

        std::istringstream iss(line);
        std::size_t code;
        while (iss >> code)
        {
            edge_code[i].push_back(code);

        }
        i++;
    }
    ifs.close();
    //std::cout<<"The edge code read is: "<<std::endl;
    for( std::size_t i = 0; i < edge_code.size(); ++i)
    {
        for( std::size_t j = 0; j < edge_code[i].size(); ++j )
        {
            //std::cout<<edge_code[i][j]<<" ";
        }
        //std::cout<<std::endl;
    }

    return true;
}

bool undir::write_edge_code(std::string filename, std::vector<std::vector<std::size_t> > edge_code)
{
    std::string line;

    std::ofstream ofs(filename.c_str(), std::ofstream::out);
    if( !ofs.is_open())
    {
        std::cerr<<"Unable to create file "<<filename<< std::endl;
        return false;
    }
    ofs<<"p "<<edge_code.size()<<"\n";
    for( unsigned int i = 0; i < edge_code.size(); ++i)
    {
        for( unsigned int j = 0; j < edge_code[i].size(); j++)
        {
            ofs<<edge_code[i][j]<<" ";
        }
        ofs<<std::endl;
    }
    ofs.close();
    return true;
}


int Atrail::createGraph() {
    using namespace undir;
    outstream << "creating Graph...\n";
    std::vector<bool> check_double(number_vertices*number_vertices,false);
    graph = Graph(number_vertices);
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
void Atrail::reindex_edges() {
    EdgeIterator e_it, e_end;
    Graph newgraph(number_vertices);
    boost::tie(e_it, e_end) = edges(graph);
    size_t e_ind = 0;
    for (; e_it != e_end; e_it++)
    {
        add_edge(source(*e_it,graph),target(*e_it,graph),e_ind,newgraph);
        e_ind++;
    }
    graph = newgraph;
}

int Atrail::postman_tour() {
    using namespace undir;
    outstream << "Postman tour...(makes the graph eulerian)\n";
    Graph ingraph = graph;
    size_t e_ind = 0;
    VertexIterator vit, vend;
    std::vector<Vertex> old_ind(0);
    std::size_t n = num_vertices(ingraph);
    std::size_t num_odd_vertices = 0;
    property_map < Graph, edge_weight_t >::type w = boost::get(edge_weight, ingraph);
    EdgeIterator e_it, e_end;
    boost::tie(e_it, e_end) = edges(ingraph);
    for (; e_it != e_end; e_it++)
    {
        w[*e_it] = 1;
    }
    outstream <<"INFO: Finding odd degree vertices ...\n";
    for (boost::tie(vit, vend) = boost::vertices(ingraph); vit != vend; ++vit)
    {
        if ((degree(*vit, ingraph) % 2) == 1)
        {
            old_ind.push_back(*vit);
            num_odd_vertices++;
        }
    }
    if (num_odd_vertices > 0)
    {
        Graph oddgraph(num_odd_vertices);
        std::vector<std::vector<int> > D(n, std::vector<int>(n, 0));
        get(edge_weight, ingraph);
        boost::tie(e_it, e_end) = edges(ingraph);

        outstream << "INFO: Finding the shortest paths between odd degree vertices ...\n";
        johnson_all_pairs_shortest_paths(ingraph, D);

        // Add edges to the complete graph
        outstream << "INFO: Constructing a complete graph from the odd degree vertices ...\n";
        for (long unsigned int i = 0; i < num_odd_vertices; ++i)
        {
            for (long unsigned int j = i + 1; j < num_odd_vertices; ++j)
            {
                Edge e = boost::add_edge(i, j, e_ind, oddgraph).first;
                boost::put(edge_weight, oddgraph, e, D[old_ind[i]][old_ind[j]]);
                e_ind++;
            }
        }

        lemon::FullGraph lg = lemon::FullGraph(num_odd_vertices); // lemon complete graph for min weight perfect matching
        Distances emap = {lg, 1}; // Unspecified distances set to 1
        EdgeIterator e_it, e_end;
        boost::tie(e_it, e_end) = edges(oddgraph);
        // Copying edge weights from boost graph to lemon full graph

        for (; e_it != e_end; e_it++)
        {
            //std::cout << "e " << e_it->m_source << " " << e_it->m_target << " " << get(edge_weight, ingraph, *e_it)<<std::endl;
            emap[lg.edge(lg(e_it->m_source), lg(e_it->m_target))] = -1 * get(edge_weight, ingraph, *e_it);
        }
        // Max weight perfect matching on lemon graph == min weight matching on orignal dimacs graph
        MWPM mwpm = MWPM(lg, emap);
        outstream << "INFO: Running the min weight matching algorithm ...\n";
        mwpm.run();

        std::vector< std::pair<Vertex, Vertex> > match;
        std::vector<bool> added = std::vector<bool>(num_odd_vertices, false);
        for (size_t i = 0; i < num_odd_vertices; ++i)
        {
            if (added[i] == false)
            {
                match.push_back(std::make_pair(i, lg.index(mwpm.mate(lg(i)))));
                added[i] = true;
                added[lg.index(mwpm.mate(lg(i)))] = true;
            }
        }

        boost::tie(e_it, e_end) = edges(ingraph);
        std::vector<std::size_t> parallel_edge_count(n*n, 0);
        std::size_t u, v, tmp;
        for (; e_it != e_end; e_it++)
        {
            u = (std::size_t) source(*e_it, ingraph);
            v = (std::size_t) target(*e_it, ingraph);
            if (u > v) { tmp = u; u = v; v = tmp; }
            //std::cout<<"pec[" <<u * (n-1) - ((u * (u-1)) / 2) + v - u<<"]";
            (parallel_edge_count[u * (n - 1) - ((u * (u - 1)) / 2) + v - u - 1])++; // edges ordered row by row -> index = (sum r=0 to r=u-1 (n-1-r)) + v - u
            w[*e_it] = 1;
        }

        //std::cout<<"INFO: Graph before addition of multiedges"<<std::endl;

        std::vector<Vertex> p(number_vertices);

        // Adding multiedges along the path of each matched pair to make all vertices have even degree.
        outstream << "INFO: Adding edges along the shortest paths ...\n";

        //unsigned int num_components = connected_components(ingraph, &component[0]);
        for (std::vector<std::pair<Vertex, Vertex> >::iterator vit = match.begin(); vit != match.end(); ++vit)
        {
            //DEBUGPRINT(std::cout << "INFO: Adding multiedges along path from " << old_ind[vit->first] << " to " << old_ind[vit->second] << " ..." << std::endl;)
                //assert(component[old_ind[vit->first]] == component[old_ind[vit->second]]);
            dijkstra_shortest_paths(ingraph, old_ind[vit->first],
                predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, ingraph))));

            Vertex current = old_ind[vit->second];
            while (current != old_ind[vit->first])
            {
                //DEBUGPRINT(std::cout << current << " ";)
                    u = ((current < p[current]) ? current : p[current]);
                v = ((current > p[current]) ? current : p[current]);
                if (parallel_edge_count[u*(n - 1) - ((u*(u - 1)) / 2) + v - u - 1] == 2)
                {
                    remove_edge(current, p[current], graph); // with the new edge to add, the older two can be removed
                    parallel_edge_count[u*(n - 1) - ((u*(u - 1)) / 2) + v - u - 1] = 0;
                }
                add_edge(current, p[current], graph);
                parallel_edge_count[u*(number_vertices - 1) - ((u*(u - 1)) / 2) + v - u - 1]++;
                current = p[current];
            }

            //DEBUGPRINT(std::cout << current << std::endl;)
        }
        //DEBUGPRINT(std::cout << "INFO: Graph after addition of multiedges" << std::endl;)

    }
    else
    {
        outstream << "INFO: There were no odd degree vertices!\n";
    }
    //DEBUGPRINT(std::cout << to_string_graph(multigraph);)
    assert(is_even(multigraph));
    if (!is_even())
    {
        outstream << "ERROR! an unknown error occurred. Output file not written.\n";
        return 1;
    }
    std::string outfile(name);
    write_dimacs(outfile.append("_multi.dimacs"), graph);
    std::stringstream sstr;
    outstream << "INFO: Wrote an Eulerian multigraph after addition of multiedges to " << outfile << std::endl;
    return 1;

}

bool Atrail::is_even()
{
    bool even = true;
    for (std::size_t i = 0; i < number_vertices; i++)
    {
        if (degree(i, graph) % 2 == 1)
        {
            even = false;
            break;
        }
    }
    return even;
}

int Atrail::make_embedding_eulerian() {
    Graph ingraph = graph;
    ecode = embedding_t(embedding.size()); //output edgecode
    std::vector<std::vector<vertex_edge_pair>> embedding_as_pair(embedding.size());
    //DEBUGPRINT(std::cout << "DEBUG: vertex code ... "<<std::endl;)
    size_t i = 0, j = 0;
    std::cerr << "Line 511\n";
    for (i = 0; i < embedding.size(); ++i)
    {
        embedding_as_pair[i] = std::vector<vertex_edge_pair>(embedding[i].size());
        //DEBUGPRINT(std::cout << i << ": ";)
        for (j = 0; j < embedding[i].size(); ++j)
        {
            //DEBUGPRINT(std::cout << vcode[i][j] << " ";)
            embedding_as_pair[i][j].vertex = embedding[i][j];
            embedding_as_pair[i][j].edge = -1; // sentinel, no edge assigned yet ...
        }
        //DEBUGPRINT(std::cout << std::endl;)
    }

    outstream << "INFO: Creating the edge code from the given the vertex code and the multigraph ...\n";

    undir::EdgeIterator e_it, e_end;
    boost::tie(e_it, e_end) = edges(ingraph);
    undir::Vertex s, t;
    size_t e_ind;
    //std::cerr << "Line 530\n";
    //std::cerr << undir::to_string_graph(ingraph).c_str()\n;
    for (; e_it != e_end; e_it++)
    {
        s = source(*e_it, ingraph);
        t = target(*e_it, ingraph);
        //std::cerr << "S = " << s << ", T = " << t << std::endl;
        i = 0;
        j = 0;
        vertex_edge_pair vep;
        e_ind = get(edge_index, ingraph, *e_it);
        //std::cerr << "E_ind = " << e_ind << std::endl;
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
    //std::cerr << "Line 567\n";
    //std::cerr << undir::to_string_graph(ingraph).c_str() << std::endl;
    /*for (int k= 0; k < embedding.size(); k++) {
        std::cerr << embedding[k][0] << embedding[k][1] << embedding[k][2] << std::endl;
    }*/
    /*std::cerr << "Embedding as pair"<< std::endl;
    for (unsigned int k= 0; k < embedding.size(); k++){
        std::cerr << embedding_as_pair[k].size()<< std::endl;
        std::cerr << embedding_as_pair[k][0].vertex << embedding_as_pair[k][1].vertex << embedding_as_pair[k][2].vertex << embedding_as_pair[k][3].vertex<< std::endl;
        std::cerr << embedding_as_pair[k][0].edge << embedding_as_pair[k][1].edge << embedding_as_pair[k][2].edge << embedding_as_pair[k][3].edge<< std::endl;
    }
    std::cerr << "Line 574"<< std::endl;*/
    for (i = 0; i < embedding_as_pair.size(); ++i)
    {
        ecode[i] = std::vector<size_t>(embedding_as_pair[i].size());
        for (j = 0; j < embedding_as_pair[i].size(); ++j)
        {
            ecode[i][j] = embedding_as_pair[i][j].edge;
        }
    }
    //std::cerr << "Line 578"<< std::endl;
    std::string outfile(name);
    undir::write_edge_code(outfile.append(".ecode"), ecode);

    outstream << "INFO: Wrote the embedding as an edge code to " << outfile << std::endl;

    return 1;
}

// A graph has an Eulerian trail ( a cycle which visits each edge exactly once ) iff all degrees are even and it is connected upto isolated vertices
// Can be multigraph, must have no self loops
bool Atrail::has_eulerian_trail()
{
    bool has_trail = true;
    unsigned int n = num_vertices(graph);
    std::vector<int> component(n);
    unsigned int num_components = connected_components(graph, &component[0]);
    if( num_components  > 1)
    {
        std::vector<unsigned int> vcount_per_comp(num_components);
        bool main_comp_found = false;
        unsigned int main_comp_cand = 0;
        for(unsigned int i = 0; i < n; ++i)
        {
            if(++vcount_per_comp[component[i]] > 1)
            {
                if(main_comp_found == false)
                {
                    main_comp_found = true;
                    main_comp_cand = component[i];
                } else if ((unsigned int)component[i] != main_comp_cand) has_trail = false;
            }
        }

    }

    for( unsigned int i = 0; i < n; i++)
    {
        if(degree(i, graph) % 2 == 1)
        {
            has_trail = false;
            break;
        }
    }
    //std::cout<<"The graph " << (has_trail?"has":"does not have") <<" an Eulerian trail"<<std::endl;
    return has_trail;
}

std::list<Vertex> Atrail::find_eulerian_trail(const Graph & G)
{
    if(!has_eulerian_trail())
    {
        std::cerr<<"ERROR! The graph has no Eulerian trail"<<std::endl;
        return std::list<Vertex>(0);
    }
    std::list<Vertex> Trail(0);
    std::list<Vertex>::iterator insert_loc;
    VertexIterator vit = boost::vertices(G).first;
    VertexIterator vend = --(boost::vertices(G).second);
    while(degree(*vit, G) == 0)
    {
        if( vit == vend)
        {
            std::cerr<<"WARNING! The graph consists of isolated vertices, the trail is empty"<<std::endl;
            return std::list<Vertex>(0);
        }
        vit++;
    }
    Vertex v = *vit;
    Trail.push_back(v);
    insert_loc = Trail.end();
    Vertex prev = v;
    Vertex next = v;
    AdjacencyIterator adIt, adEnd;
    AdjacencyIterator neiSt, neiEnd;

    std::vector< std::list<Vertex> > unused_edges(num_vertices(G)); // unmarked edges of a vertex
    unsigned int i=0;
    for(std::vector<std::list<Vertex> >::iterator myIt = unused_edges.begin(); myIt != unused_edges.end(); myIt++, i++)
    {
        tie(neiSt, neiEnd) = adjacent_vertices(i, G);
        for(;neiSt != neiEnd; neiSt++)
        {
            myIt->push_back((unsigned int)*neiSt);
        }
    }
    std::set<Vertex> in_trail_has_unused; //vertices on the current tour that have unused edges
    in_trail_has_unused.insert(v);

    do
    {
        v = *in_trail_has_unused.begin();
        insert_loc = std::find( Trail.begin(), Trail.end(), v);
        insert_loc++;
        next = v;
        do
        {
            Vertex adIt = unused_edges[next].front();
            prev = next;
            next = adIt;
            unused_edges[next].erase(std::find(unused_edges[next].begin(), unused_edges[next].end(), prev));
            unused_edges[prev].erase(std::find(unused_edges[prev].begin(), unused_edges[prev].end(), next));
            //unused_edges[next].remove(prev);
            //unused_edges[prev].remove(next);
            in_trail_has_unused.insert(next);
            insert_loc = Trail.insert(insert_loc,next);
            insert_loc++;
        }while(next != v);
        //DEBUGPRINT( std::cout << "DEBUG: "; print_walk(Trail, "Trail "); )

        std::set<Vertex>::iterator setIt = in_trail_has_unused.begin();

        for(; setIt != in_trail_has_unused.end();)
        {
            if (unused_edges[*setIt].size() == 0)
            {
                in_trail_has_unused.erase(setIt++);
            }
            else
            {
                ++setIt;
            }
        }

    }while(!in_trail_has_unused.empty());

    return Trail;
}

bool Atrail::Atrail_search()
{
    outstream << "INFO: Checking if the graph has an A-trail ...\n";
    outstream << "---------------------------------------" << std::endl;
    outstream << "INFO: First Checking if the graph has an Eulerian trail ...\n";
    if(!has_eulerian_trail()) {
        outstream << "ERROR! The graph does not have an Eulerian trail and thus cannot have an A-trail!\n";
        return false;
    }
    else
    {
        outstream << "INFO: The graph has an Eulerian trail, now checking if it has an A-trail ...\n";
    }
    std::cerr << outstream.str().c_str()<< std::endl;
    outstream.clear();
    std::list<Vertex> new_trail;
    unsigned int n = num_vertices(graph);
    unsigned int m = num_edges(graph);
    std::vector<std::vector<Vertex> > P(n);
    for(unsigned int a = 0; a < n; a++) P[a] = std::vector<Vertex>(ecode[a].size());
    bool is_multigraph = false;
    std::map<std::pair<Vertex, Vertex>, unsigned int > edge_multi;

    EdgeIterator mit, mend;
    Vertex u, v, temp;
    for( tie(mit, mend) = edges(graph); mit != mend; mit++)
    {
        u = source(*mit, graph);
        temp = target(*mit, graph);
        if( u < temp ) v = temp;
        else {v = u; u = temp;}
        edge_multi[std::make_pair(u,v)] = 0;
    }
    std::cerr << "line 736"<< std::endl;

    Vertex w;
    unsigned int ord1, ord2;
    Graph G = graph;
    tie(mit, mend) = edges(G);
    unsigned int edge_ind = m;
    std::vector<std::size_t> subdiv_to_multiedge(0);
    std::size_t e_ind = 0; // edge index of current edge
    for( unsigned int i = 0; i < m; i++,mit++)
    {
        u = source(*mit, G);
        temp = target(*mit, G);
        if( u < temp ) v = temp;
        else {v = u; u = temp;}

        e_ind = get(edge_index, G, *mit);
        ord1 = std::find(ecode[u].begin(), ecode[u].end(), e_ind) - ecode[u].begin();
        P[u][ord1] = v;
        ord2 = std::find(ecode[v].begin(), ecode[v].end(), e_ind) - ecode[v].begin();
        P[v][ord2] = u;
        if(++edge_multi[std::make_pair(u,v)] > 1)
        {
            if(is_multigraph == false) is_multigraph = true;
            w = add_vertex(G);
            subdiv_to_multiedge.push_back(e_ind);
            EdgeIterator old = --mit;
            mit++;
            remove_edge(*mit,G);
            mit = old;
            add_edge(u,w,edge_ind,G);
            add_edge(v,w,++edge_ind,G);
            ++edge_ind;
            P[u][ord1] = w;
            P[v][ord2] = w;
            P.push_back(std::vector<Vertex>(2));
            P.back()[0] = u;
            P.back()[1] = v;
        }
    }
    std::cerr << "line 776"<< std::endl;
    outstream.clear();

    if( is_multigraph )
    {
        outstream << "INFO: The graph is a multigraph.\n";
        //DEBUGPRINT(std::cout<<"INFO: The extra edges which make the graph non-simple have been subdivided (node in each extra edge) ..."<<std::endl;)
        //DEBUGPRINT(std::cout<<"Graph after subdivision: \n"<<to_string_graph(G);)
    }

    //DEBUGPRINT(std::cout<<"Embedding as order of adjacent vertices (after subdivision): "<<std::endl;)
    std::vector<std::vector<Vertex> >::iterator eit;
    for( eit = P.begin(); eit != P.end(); eit++)
    {
        std::vector<Vertex>::iterator ait;
        //DEBUGPRINT(std::cout<<"Order around vertex "<< (eit - P.begin())<<": ";)
        for(ait = eit->begin(); ait != eit->end(); ait++)
        {
            //DEBUGPRINT(std::cout<<*ait<<" ";)
        }
        //DEBUGPRINT(std::cout<<std::endl;)
    }
    std::cerr << "line 798\n" << n<< std::endl;
    std::vector<unsigned int> bn;
    outstream << "INFO: Enumerating the intersectable branching nodes (i.e. nodes with degree greater or equal to six) ...\n";
    enumerate_intersectable_nodes(G, bn);
    std::stringstream sstr;
    std::cerr << "line 803"<< std::endl;
    if (bn.empty())
    {
        outstream << "INFO: The graph has no branch nodes\n";
    }else{
        outstream << "INFO: Branch nodes \n";
        for( std::vector<unsigned int>::iterator it = bn.begin(); it != bn.end(); ++it)
        {
            sstr<<*it<<" ";
        }
        sstr<<std::endl;
    }
    std::string tempStr(sstr.str());
    outstream << tempStr;
    std::cerr << "line 817"<< std::endl;
    std::map<Vertex, std::vector<Vertex> > bn_new_nodes_map;
    std::vector<parity> parity_vec1;
    std::vector<parity> parity_vec2;
    bool has_Atrail = true;
    if(!bn.empty())
    {
        outstream << "INFO: Since there are branch nodes, the A-trail search may take a while ...\n";
        if (!split_and_check(G, P, bn, 0, odd_even, bn_new_nodes_map, parity_vec1))
        {
            has_Atrail = false;
            bn_new_nodes_map.clear();
            if(split_and_check(G, P, bn, 0, even_odd, bn_new_nodes_map, parity_vec2))
            {
                has_Atrail = true;
            }
        }
    }
    std::cerr << "line 835"<< std::endl;
    if( has_Atrail)
    {
        outstream << "INFO: The graph has an A-trail\n";
        std::list<Vertex> Trail = find_eulerian_trail(G);
        // In case the graph is a collection of isolated vertices, we have an empty trail.
        if( Trail.size() == 0 )
        {
            new_trail = std::list<Vertex>(0);
            trail_edgelist = std::list<std::size_t>(0);
            return true;
        }
        // For fixing crossings locally and remapping to original vertices, we need a map from new nodes (the split ones) to their equivalent branch node
        std::map<Vertex, Vertex> new_node_to_bn_map;
        for( std::map<Vertex, std::vector<Vertex> >::iterator mapIt = bn_new_nodes_map.begin(); mapIt != bn_new_nodes_map.end(); mapIt++)
        {
            for(std::vector<Vertex>::iterator new_node_it = mapIt->second.begin(); new_node_it != mapIt->second.end(); new_node_it++)
                new_node_to_bn_map[*new_node_it] = mapIt->first;
        }

        // TODO: PERFORMANCE: remap new vertices to their old ones more efficiently
        //DEBUGPRINT(std::cout<<"INFO: "; print_walk(Trail, "Trail (before remap) ");)
        // remap
        for( std::list<Vertex>::iterator it = Trail.begin(); it != Trail.end(); it++)
        {
            std::map<Vertex, Vertex>::iterator orig_pair = new_node_to_bn_map.find(*it);
            if( orig_pair != new_node_to_bn_map.end())
            {
                *it = orig_pair->second;
            }
        }
        //DEBUGPRINT(std::cout<<"INFO: "; print_walk(Trail, "Trail (after remap) ");)

        // Local fixing
        std::map<Vertex, bool> fixed;
        Trail.pop_back(); // TODO: In the exceptional case where there is only a single node.
        new_trail = Trail;
        for( std::list<Vertex>::iterator it = Trail.begin(); it != Trail.end(); it++)
        {
            // degree of branch nodes (originally 6 or more) is zero after reaching a leave node in the search tree
            if(degree(*it, G) == 4)
            {
                fixed[*it] = false;
            }
        }
        for( std::list<Vertex>::iterator it = Trail.begin(); it != Trail.end(); it++)
        {
            // degree of branch nodes (originally 6 or more) is zero after reaching a leave node in the search tree
            if(degree(*it, G) == 4 && fixed[*it] == false)
            {
                short prevs_ord, nexts_ord;
                std::list<Vertex>::iterator prev, next;
                //prev = --it;
                //next = ++(++it);
                //it--;
                if (it == Trail.begin())
                {
                    prev = --Trail.end();
                }else
                {
                    prev = --it;
                    ++it;
                }
                if (it == --Trail.end())
                {
                    next = Trail.begin();
                }else
                {
                    next = ++it;
                    --it;
                }

                prevs_ord = std::find(P[*it].begin(), P[*it].end(), *prev) - P[*it].begin();
                nexts_ord = std::find(P[*it].begin(), P[*it].end(), *next) - P[*it].begin();
                if( (nexts_ord != (prevs_ord + 1) % 4) && (prevs_ord != (nexts_ord + 1) % 4) )
                {
                    //DEBUGPRINT(std::cout<<"Fixing node "<<*it<<" ..."<<std::endl;)
                    std::list<Vertex>::iterator first, second;
                    first = std::find(new_trail.begin(), new_trail.end(), *it);
                    second = std::find( ++first,new_trail.end(),*it);
                    std::list<Vertex> to_rev(first, second);
                    to_rev.reverse();
                    //DEBUGPRINT(print_walk(to_rev, "Reversed section: ");)
                    first = new_trail.erase(first, second);
                    new_trail.splice(first, to_rev);
                    //DEBUGPRINT(print_walk(new_trail, "New trail: ");)
                    fixed[*it] = true;
                }
            }
        }
        new_trail.push_back(new_trail.front());
        std::cout<<"INFO: ";
        std::string ret = print_walk(new_trail, "Trail (after local fixing): ");
        outstream << ret;

        // Get the trail as an edge list.
        trail_edgelist.clear();
        std::size_t edge_ind = 0;
        Edge current_edge;

        if(new_trail.front() >= n)
        {
            new_trail.pop_front();
            new_trail.push_back(new_trail.front());
        }

        if(is_multigraph)
        {

            for( std::list<Vertex>::iterator it = new_trail.begin(); it != (--new_trail.end()); it++)
            {
                std::list<Vertex>::iterator oldit = it;
                if(*it < n)
                {
                    if(*(++it) < n)
                    {
                        current_edge = edge(*oldit, *it, graph).first;
                        edge_ind = get(edge_index, graph, current_edge);
                        trail_edgelist.push_back(edge_ind);
                    }else{
                        trail_edgelist.push_back(subdiv_to_multiedge[*it - n]);
                    }
                }
                it = oldit;
            }

            for( std::list<Vertex>::iterator it = ++new_trail.begin(); it != new_trail.end(); it++ )
            {
                std::list<Vertex>::iterator prev = --it;
                it++;
                if(*it >= n)
                {
                    new_trail.erase(it);
                    it = prev;
                }
            }

        }else{
            for( std::list<Vertex>::iterator it = new_trail.begin(); it != (--new_trail.end()); it++)
            {
                std::list<Vertex>::iterator old = it;
                it++;
                current_edge = edge(*old, *it, graph).first;
                edge_ind = get(edge_index, graph, current_edge);
                trail_edgelist.push_back(edge_ind);
                it = old;
            }
        }
        std::cout<<"INFO: "; ret = print_walk(trail_edgelist, "Trail (as edge list): ");
        outstream << ret;
        std::cout<<"INFO: "; ret = print_walk(new_trail, "Trail (as node list): ");
        outstream <<ret;
    }
    std::cout<<"---------------------------------------"<<std::endl;
    nodetrail = new_trail;
    return has_Atrail;
}

bool Atrail::split_and_check(Graph & G, const std::vector<std::vector<Vertex> > & P,
        std::vector<unsigned int> bn, unsigned int dep, parity par,
        std::map<Vertex, std::vector<Vertex> > & bn_new_nodes_map, std::vector<parity> parity_vec)
{
    parity_vec.push_back(par);
    /*DEBUGPRINT(std::cout<<"DEBUG: Parity: ";)
    for( unsigned int l=0; l < parity_vec.size(); l++)
    {
        DEBUGPRINT(std::cout<<bn[l]<<"->"<<(parity_vec[l] == even_odd ? "even_odd, " : "odd_even, ");)
    }*/
    //DEBUGPRINT(std::cout<<std::endl;)
    // G := split graph at bn[in] according to parity and P
    unsigned int deg = degree(bn[dep], G);
    //DEBUGPRINT(std::cout << "DEBUG: Splitting branch node " << bn[dep] << "with degree " << deg << std::endl;)
    std::vector<Vertex> split_vertices(deg/2);
    std::vector<Vertex>::const_iterator J = P[bn[dep]].begin();
    unsigned int out_edge_pairs_size = deg/2;
    std::vector<unsigned int>::iterator loc;
    unsigned int iloc;
    unsigned int ord;
    Vertex target;
    for( unsigned int i = 0 ; i < out_edge_pairs_size; ++i )
    {
        split_vertices[i] = add_vertex(G);
        // TODO: DESIGN IMPROVEMENT: abstract copy/paste code.
        target = *J;
        loc = std::find( bn.begin(),bn.end(),target );
        iloc = loc - bn.begin();
        if( loc - bn.begin() < dep )
        {
            ord = std::find(P[target].begin(), P[target].end(), bn[dep]) - P[target].begin();
            assert(bn_new_nodes_map[target].size() != 0);
            ord = ((parity_vec[iloc] == even_odd) ? ord+1 : ord) % (2*bn_new_nodes_map[target].size());
            ord = ord / 2;
            target = bn_new_nodes_map[target][ord];
        }

        add_edge(split_vertices[i], target, G);
        J++;

        if(par == odd_even)
        {
            target = *J;
            loc = std::find( bn.begin(),bn.end(),target );
            iloc = loc - bn.begin();
            if( loc - bn.begin() < dep )
            {
                ord = std::find(P[target].begin(), P[target].end(), bn[dep]) - P[target].begin();
                assert(bn_new_nodes_map[target].size() != 0);
                ord = ((parity_vec[iloc] == even_odd) ? ord+1 : ord) % (2*bn_new_nodes_map[target].size());
                ord = ord / 2;
                target = bn_new_nodes_map[target][ord];
            }
            add_edge(split_vertices[i], target, G);
            J++;
        }
        else
        {
            if( i == 0)
            {
                target = *(--(P[bn[dep]].end()));
                loc = std::find( bn.begin(),bn.end(),target );
                iloc = loc - bn.begin();
                if( loc - bn.begin() < dep )
                {
                    ord = std::find(P[target].begin(), P[target].end(), bn[dep]) - P[target].begin();
                    assert(bn_new_nodes_map[target].size() != 0);
                    ord = ((parity_vec[iloc] == even_odd) ? ord+1 : ord) % (2*bn_new_nodes_map[target].size());
                    ord = ord / 2;
                    target = bn_new_nodes_map[target][ord];
                }
                add_edge(split_vertices[i], target, G);
                J++;
            }else
            {
                target = *(--(--J));
                loc = std::find( bn.begin(),bn.end(),target );
                iloc = loc - bn.begin();
                if( loc - bn.begin() < dep )
                {
                    ord = std::find(P[target].begin(), P[target].end(), bn[dep]) - P[target].begin();
                    assert(bn_new_nodes_map[target].size() != 0);
                    ord = ((parity_vec[iloc] == even_odd) ? ord+1 : ord) % (2*bn_new_nodes_map[target].size());
                    ord = ord / 2;
                    target = bn_new_nodes_map[target][ord];
                }
                add_edge(split_vertices[i], target, G);
                J+=3;
            }

        }
    }

    // Clear all the edges of the branch node
    clear_vertex(bn[dep], G);
    std::size_t n = num_vertices(G);
    // DEBUG
    for( std::size_t i = 0; i < n; ++i )
        assert(degree(i, G) % 2 == 0);

    //DEBUGPRINT(std::cout<<"DEBUG: Edges: "<<to_string_graph_edgelist(G)<<std::endl;)
    //DEBUGPRINT(std::cout<<"DEBUG: Graph after split: \n"<<to_string_graph(G);)
    unsigned int s = split_vertices.size();
    unsigned int k = 0;
    //DEBUGPRINT(std::cout<<"DEBUG: The new vertices after split are ";)
    for( k = 0; k < s; ++k)
    {
        //DEBUGPRINT(std::cout<<split_vertices[k]<<" ";)
    }
    //DEBUGPRINT(std::cout<<std::endl;)
    std::vector<int> component(num_vertices(G));
    if( (unsigned int) connected_components(G, &component[0]) > dep+2)
    {
        //DEBUGPRINT(std::cout << "DEBUG: Graph becomes disconnected ..." << std::endl;)
         //   DEBUGPRINT(std::cout << "DEBUG: Removing the new vertices and patching " << bn[dep] << " " << "..." << std::endl;)
            // G := patch graph at bn[in]
        for( k = 0; k < s; ++k)
        {
            AdjacencyIterator adIt, adEnd;
            tie(adIt, adEnd) = adjacent_vertices(split_vertices[k], G);
            for(; adIt != adEnd; adIt++)
            {
                add_edge(bn[dep], *adIt, G);
            }

        }
        for( k = 0; k < s; ++k )
        {
            clear_vertex(split_vertices[k], G);
        }
        signed int kp = s - 1;
        for(  kp = s-1; kp >= 0; kp--)
        {
            remove_vertex(split_vertices[kp], G);
        }
        parity_vec.pop_back();
        return false;
    }else
    {
        bn_new_nodes_map[bn[dep]] = split_vertices;
        if ( dep < bn.size()-1)
        {
            bool left_branch, right_branch;
            left_branch = split_and_check(G, P, bn, dep+1, odd_even,bn_new_nodes_map,parity_vec);
            if( left_branch) return true;
            else{
                right_branch = split_and_check(G, P, bn, dep+1, even_odd,bn_new_nodes_map,parity_vec);
                if(right_branch) return true;
                else
                {
                    //DEBUGPRINT(std::cout<<"DEBUG: Both branchs disconnect the graph, backtracking ..."<<std::endl;)
                    //DEBUGPRINT(std::cout<<"DEBUG: Removing the new vertices and patching "<<bn[dep]<<" "<< "..."<<std::endl;)
                    // G := patch graph at bn[in]

                    for( k = 0; k < s; ++k)
                    {
                        AdjacencyIterator adIt, adEnd;
                        tie(adIt, adEnd) = adjacent_vertices(split_vertices[k], G);

                        for(; adIt != adEnd; adIt++)
                        {
                            add_edge(bn[dep], *adIt, G);
                        }

                    }
                    for( k = 0; k < s; ++k )
                    {
                        clear_vertex(split_vertices[k], G);
                    }

                    signed int kp = s - 1;
                    for( kp = s-1; kp >= 0; --kp)
                    {
                        remove_vertex(split_vertices[kp], G);
                    }
                    parity_vec.pop_back();
                    return false;
                }
            }

        }
        else{
            //DEBUGPRINT(std::cout<<"DEBUG: Embedding has an A-trail with this parity ..."<<std::endl;)
            return true;
        }
    }
}


void Atrail::enumerate_intersectable_nodes(const Graph &G, std::vector<unsigned int> & bn, Vertex s)
{
    std::cerr << "Enumerating intersectable nodes"<< std::endl;
    bn.clear();
    VertexIterator vertexBegin, vertexIt,vertexEnd;
    tie(vertexBegin, vertexEnd) = boost::vertices(G);
    unsigned int n = num_vertices(G);
    enum color_type { white, gray, black };
    color_type * color = new color_type[n];
    std::queue<Vertex> checked_queue;
    Vertex u, v;
    std::cerr << "for loop next\n";
    for( vertexIt = vertexBegin; vertexIt != vertexEnd; ++vertexIt ) { color[*vertexIt] = white; }
    std::cerr << "for loop finished\n";
    color[s] = gray;
    checked_queue.push(s);
    std::cerr << "1197\n" << n<< std::endl;
    std::cerr << to_string_graph(G).c_str() << "\n";
    std::cerr << degree(s, G)<< std::endl;
    if( degree(s, G) >= 6) bn.push_back(s);
    std::cerr << "1199\n";
    while(!checked_queue.empty())
    {

        u = checked_queue.front();
        checked_queue.pop();
        AdjacencyIterator adIt, adEnd;
        tie(adIt, adEnd) = adjacent_vertices(u, G);
        for(; adIt != adEnd; adIt++)
        {
            v = *adIt;
            if(color[v] == white)
            {
                color[v] = gray;
                if(degree(v, G) >= 6)
                    bn.push_back(v);
                checked_queue.push(v);
            }
        }
        color[u] = black;
    }

    delete[] color;
}
bool Atrail::Atrail_verify()
{
    std::size_t m = 0; // number of edges
    for( unsigned int i = 0; i < ecode.size(); ++i ) m += ecode[i].size();
    m = m / 2;
    std::vector<unsigned short> num_visits(m, 0);
    std::vector<std::vector<unsigned int> > ind2pair(m, std::vector<unsigned int>(0));
    for( unsigned int i = 0; i < ecode.size(); ++i)
    {
        for( unsigned int j = 0; j < ecode[i].size(); ++j)
        {
            ind2pair[ecode[i][j]].push_back(i);
        }
    }

    if( m != trail_edgelist.size()) return false;
    else if (trail_edgelist.size() == 0)
    {
        outstream << "ERROR! The trail is empty!\n";
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

                loc = std::distance(ecode[next].begin(), std::find(ecode[next].begin(),ecode[next].end(), *(it)));
                pre = ((loc == ecode[next].size() - 1)? 0 : (loc + 1));
                suc = ((loc == 0) ? (ecode[next].size() - 1):(loc - 1));
                if( ecode[next][pre] != *nextedge
                        && ecode[next][suc] != *nextedge )
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

int Atrail::main() {
    if (readPLY() < 1) return 0;
    std::cerr << "read ply\n";
    std::cerr << vertices.size() << nodetrail.size() << number_vertices<< std::endl;
    if (createEmbedding() < 1) return 0;
    std::cerr << "created embedding\n";
    std::cerr << vertices.size() << nodetrail.size()<< std::endl;
    if (createGraph() < 1) return 0;
    std::cerr << "Created graph\n";
    std::cerr << vertices.size() << nodetrail.size()<< std::endl;
    if (postman_tour() < 1) return 0;
    std::cerr << "Finsihed postman tour\n";
    std::cerr << vertices.size() << nodetrail.size()<< std::endl;
    reindex_edges();
    if (make_embedding_eulerian() < 1) return 0;
    std::cerr << "Made embedding eulerian\n";
    std::cerr << vertices.size() << nodetrail.size()<< std::endl;
    if (!Atrail_search()) return 0;
    std::cerr << "Found Atrail\n";
    std::cerr << vertices.size() << " " << nodetrail.size()<< std::endl;
    if (!Atrail_verify()) return 0;
    std::cerr << "Verfied Atrail\n";
    std::cerr << vertices.size() << " " << nodetrail.size()<< std::endl;
    return 1;

}

int Atrail::relax(const QVector<QVariant> args) {
    for (unsigned int i = 0; i < number_vertices; i++) {
        std::cerr << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z<< std::endl;
    }

    using namespace PhysXRelax;

    std::ifstream json("settings.json");
    nlohmann::json settings;
    if (json.is_open()) {
        std::cout << "Found settings file\n";
        settings << json;
    }
    else {
        std::cerr << "Could not read physics settings from file\n";
        return -1;
    }

    double *dblArgs = new double[9];
    bool *boolArgs = new bool[3];
    /*unsigned int k = 0; unsigned int j = 0;
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

    std::cout << "dblArgs:\n";
    for (int i = 0; i < 9; i++) {
        std::cout << dblArgs[i] << ", ";
    }

    std::cout << "boolArgs:\n";
    for (int i = 0; i < 3; i++) {
        std::cout << boolArgs[i] << ", ";
    }

    */
   
    std::cout << settings;
    settings = settings["physX"];
    dblArgs[0] = args[0].toDouble(); // scaling
    dblArgs[1] = settings["density"];
    dblArgs[2] = settings["spring_stiffness"];
    dblArgs[3] = settings["fixed_spring_stiffness"];
    dblArgs[4] = settings["spring_damping"];
    dblArgs[5] = settings["static_friction"];
    dblArgs[6] = settings["dynamic_friction"];
    dblArgs[7] = settings["restitution"];
    dblArgs[8] = settings["rigid_body_sleep_threshold"];
    boolArgs[0] = settings["discretize_lengths"];
    boolArgs[1] = settings["attach_fixed"];
    boolArgs[2] = settings["visual_debugger"];
    const int iterations = args[1].toInt();

    std::cout << "dblArgs:\n";
    for (int i = 0; i < 9; i++) {
        std::cout << dblArgs[i] << ", ";
    }

    std::cout << "boolArgs:\n";
    for (int i = 0; i < 3; i++) {
        std::cout << boolArgs[i] << ", ";
    }
    PhysXRelax::physics::settings_type physics_settings;
    PhysXRelax::scene::settings_type scene_settings;
    PhysXRelax::Helix::settings_type helix_settings;
    PhysXRelax::parse_settings(dblArgs,boolArgs,physics_settings,scene_settings,helix_settings);
    PhysXRelaxation relaxation(name,physics_settings,scene_settings,helix_settings, iterations);
    std::cerr << "Created relaxation object\n";
    std::vector<unsigned int> input_path;

    std::copy(nodetrail.begin(),nodetrail.end(),std::back_inserter(input_path));

    //outstream << relaxation->getoutstream().c_str();
    std::cerr << "Running scaffold_main(). Argument sizes: " << vertices.size() << ", " << input_path.size()<< std::endl;
    relaxation.scaffold_main(vertices,input_path);
    outstream << relaxation.getoutstream().c_str();
    delete boolArgs;
    delete dblArgs;
    return 1;
}
