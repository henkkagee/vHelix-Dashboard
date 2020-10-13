/*
 * boost_graph_helper.cpp
 *
 *  Created on: Jul 4, 2013
 *      Author: mohamma1
 */
#include "boost_graph_helper.hpp"
#include <boost/graph/connected_components.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>

void print_embedding(const planar_embedding_t & planar_embedding, const Graph & G, std::vector<std::vector<Vertex> > & embedding_as_vertex_order)
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

void embedding_to_edge_code(const planar_embedding_t & planar_embedding, Graph & G, std::vector<std::vector<std::size_t> > & edge_code)
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

std::string print_walk(std::list<Vertex> Trail, std::string label)
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

std::string to_string_graph(const Graph & G, std::string lineheader, std::string liststart, std::string adj_sep, std::string line_sep)
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

std::string to_string_graph_edgelist(const Graph & G, std::string pair_sep, std::string edge_frame_left, std::string edge_frame_right, std::string list_sep)
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

bool read_dimacs(std::string input_filename, Graph & G)
{
    std::string line;
	std::ifstream ifs(input_filename.c_str(), std::ifstream::in);
	if( !ifs.is_open())
	{
		std::cerr<<"Unable to open file "<<input_filename;
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

bool write_dimacs(std::string output_filename, Graph & G)
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

// TODO: Perhaps merge write_weighed_dimacs and write_dimacs
bool write_weighed_dimacs(std::string output_filename, Graph & G)
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

bool read_edge_code(std::string input_filename, std::vector<std::vector<std::size_t> > & edge_code)
{
	std::string line;
	unsigned long n_comp = 0;
	std::size_t n = 0;
	std::ifstream ifs(input_filename.c_str(), std::ifstream::in);
	if( !ifs.is_open())
	{
		std::cerr<<"ERROR! Unable to read file "<<input_filename;
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
		std::cerr<<"ERROR! Incorrect header(first line) in file "<<input_filename;
		ifs.close();
		return false;
	}
	std::size_t i = 0;
	//std::cout<<"Reading adjacency"<<std::endl;
	while(std::getline(ifs, line))
	{

		if( i > n-1 )
		{
			std::cerr<<"Warning! the file has more lines than the given parameter in the header.";
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

bool write_edge_code(std::string filename, std::vector<std::vector<std::size_t> > edge_code)
{
	std::string line;

	std::ofstream ofs(filename.c_str(), std::ofstream::out);
	if( !ofs.is_open())
	{
		std::cerr<<"Unable to create file "<<filename;
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
