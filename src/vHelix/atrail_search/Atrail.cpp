/*
 *
 * Atrail.cpp
 *
 *  Created on: Jul 4, 2013
 *      Author: mohamma1
 */

#include "Atrail.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <queue> // in BFS search
#include <assert.h>
#include <boost/graph/connected_components.hpp> //In split and check
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>




// Do not define DEBUG if testing on large graphs. Some of the printing is during the exponential search and the printing can grow exponentially large even on moderately large graphs
#define NDEBUG 
#ifdef DEBUG
#define DEBUGPRINT( x ) x
#else
#define DEBUGPRINT( x )
#endif

using namespace boost;


// A graph has an Eulerian trail ( a cycle which visits each edge exactly once ) iff all degrees are even and it is connected upto isolated vertices
// Can be multigraph, must have no self loops
bool has_eulerian_trail(const Graph & G)
{
	bool has_trail = true;
	unsigned int n = num_vertices(G);
	std::vector<int> component(n);
	unsigned int num_components = connected_components(G, &component[0]);
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
		if(degree(i, G) % 2 == 1)
		{
			has_trail = false;
			break;
		}
	}
	//std::cout<<"The graph " << (has_trail?"has":"does not have") <<" an Eulerian trail"<<std::endl;
	return has_trail;
}

std::list<Vertex> find_eulerian_trail(const Graph & G)
{
	if(!has_eulerian_trail(G))
	{
		std::cerr<<"ERROR! The graph has no Eulerian trail"<<std::endl;
		return std::list<Vertex>(0);
	}
	std::list<Vertex> Trail(0);
	std::list<Vertex>::iterator insert_loc;
	VertexIterator vit = vertices(G).first;
	VertexIterator vend = --(vertices(G).second);
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
		DEBUGPRINT( std::cout << "DEBUG: "; print_walk(Trail, "Trail "); )

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


bool Atrail_search(Graph & Gr, std::vector<std::vector<std::size_t> > & edge_code, std::list<std::size_t> & trail_edgelist, std::list<Vertex> & new_trail  , vHelix &parent)
{
    parent.sendToConsole_("INFO: Checking if the graph has an A-trail ...\n");
	std::cout << "---------------------------------------" << std::endl;
    parent.sendToConsole_("INFO: First Checking if the graph has an Eulerian trail ...\n");
	if(!has_eulerian_trail(Gr)) {
        parent.sendToConsole_("ERROR! The graph does not have an Eulerian trail and thus cannot have an A-trail!\n");
		return false;
	}
	else
	{
        parent.sendToConsole_("INFO: The graph has an Eulerian trail, now checking if it has an A-trail ...\n");
	}

	unsigned int n = num_vertices(Gr);
	unsigned int m = num_edges(Gr);
	std::vector<std::vector<Vertex> > P(n);
	for(unsigned int a = 0; a < n; a++) P[a] = std::vector<Vertex>(edge_code[a].size());
	bool is_multigraph = false;
	std::map<std::pair<Vertex, Vertex>, unsigned int > edge_multi;

	EdgeIterator mit, mend;
	Vertex u, v, temp;
	for( tie(mit, mend) = edges(Gr); mit != mend; mit++)
	{
		u = source(*mit, Gr);
		temp = target(*mit, Gr);
		if( u < temp ) v = temp;
		else {v = u; u = temp;}
		edge_multi[std::make_pair(u,v)] = 0;
	}

	Vertex w;
	unsigned int ord1, ord2;
	Graph G = Gr;
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
		ord1 = std::find(edge_code[u].begin(), edge_code[u].end(), e_ind) - edge_code[u].begin();
		P[u][ord1] = v;
		ord2 = std::find(edge_code[v].begin(), edge_code[v].end(), e_ind) - edge_code[v].begin();
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

	if( is_multigraph )
	{
        parent.sendToConsole_("INFO: The graph is a multigraph.\n");
		DEBUGPRINT(std::cout<<"INFO: The extra edges which make the graph non-simple have been subdivided (node in each extra edge) ..."<<std::endl;)
		DEBUGPRINT(std::cout<<"Graph after subdivision: \n"<<to_string_graph(G);)
	}

	DEBUGPRINT(std::cout<<"Embedding as order of adjacent vertices (after subdivision): "<<std::endl;)
	std::vector<std::vector<Vertex> >::iterator eit;
	for( eit = P.begin(); eit != P.end(); eit++)
	{
		std::vector<Vertex>::iterator ait;
		DEBUGPRINT(std::cout<<"Order around vertex "<< (eit - P.begin())<<": ";)
		for(ait = eit->begin(); ait != eit->end(); ait++)
		{
			DEBUGPRINT(std::cout<<*ait<<" ";)
		}
		DEBUGPRINT(std::cout<<std::endl;)
	}

	std::vector<unsigned int> bn;
    parent.sendToConsole_("INFO: Enumerating the intersectable branching nodes (i.e. nodes with degree greater or equal to six) ...\n");
	enumerate_intersectable_nodes(G, bn);
    std::stringstream sstr;
	if (bn.empty())
	{
        parent.sendToConsole_("INFO: The graph has no branch nodes\n");
	}else{
        parent.sendToConsole_("INFO: Branch nodes \n");
		for( std::vector<unsigned int>::iterator it = bn.begin(); it != bn.end(); ++it)
		{
            sstr<<*it<<" ";
		}
        sstr<<std::endl;
	}
    std::string tempStr(sstr.str());
    parent.sendToConsole_(tempStr);
	
	std::map<Vertex, std::vector<Vertex> > bn_new_nodes_map;
	std::vector<parity> parity_vec1;
	std::vector<parity> parity_vec2;
	bool has_Atrail = true;
	if(!bn.empty())
	{
        parent.sendToConsole_("INFO: Since there are branch nodes, the A-trail search may take a while ...\n");
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

	if( has_Atrail)
	{
        parent.sendToConsole_("INFO: The graph has an A-trail\n");
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
		DEBUGPRINT(std::cout<<"INFO: "; print_walk(Trail, "Trail (before remap) ");)
		// remap
		for( std::list<Vertex>::iterator it = Trail.begin(); it != Trail.end(); it++)
		{
			std::map<Vertex, Vertex>::iterator orig_pair = new_node_to_bn_map.find(*it);
			if( orig_pair != new_node_to_bn_map.end())
			{
				*it = orig_pair->second;
			}
		}
		DEBUGPRINT(std::cout<<"INFO: "; print_walk(Trail, "Trail (after remap) ");)

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
					DEBUGPRINT(std::cout<<"Fixing node "<<*it<<" ..."<<std::endl;)
					std::list<Vertex>::iterator first, second;
					first = std::find(new_trail.begin(), new_trail.end(), *it);
					second = std::find( ++first,new_trail.end(),*it);
					std::list<Vertex> to_rev(first, second);
					to_rev.reverse();
					DEBUGPRINT(print_walk(to_rev, "Reversed section: ");)
					first = new_trail.erase(first, second);
					new_trail.splice(first, to_rev);
					DEBUGPRINT(print_walk(new_trail, "New trail: ");)
					fixed[*it] = true;
				}
			}
		}
		new_trail.push_back(new_trail.front());
        std::cout<<"INFO: ";
        std::string ret = print_walk(new_trail, "Trail (after local fixing): ");
        parent.sendToConsole_(ret);

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
						current_edge = edge(*oldit, *it, Gr).first;
						edge_ind = get(edge_index, Gr, current_edge);
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
				current_edge = edge(*old, *it, Gr).first;
				edge_ind = get(edge_index, Gr, current_edge);
				trail_edgelist.push_back(edge_ind);
				it = old;
			}
		}
        std::cout<<"INFO: "; ret = print_walk(trail_edgelist, "Trail (as edge list): ");
        parent.sendToConsole_(ret);
        std::cout<<"INFO: "; ret = print_walk(new_trail, "Trail (as node list): ");
        parent.sendToConsole_(ret);
	}
	std::cout<<"---------------------------------------"<<std::endl;
	return has_Atrail;
}

bool split_and_check(Graph & G, const std::vector<std::vector<Vertex> > & P,
		std::vector<unsigned int> bn, unsigned int dep, parity par,
		std::map<Vertex, std::vector<Vertex> > & bn_new_nodes_map, std::vector<parity> parity_vec)
{
	parity_vec.push_back(par);
	DEBUGPRINT(std::cout<<"DEBUG: Parity: ";)
	for( unsigned int l=0; l < parity_vec.size(); l++)
	{
		DEBUGPRINT(std::cout<<bn[l]<<"->"<<(parity_vec[l] == even_odd ? "even_odd, " : "odd_even, ");)
	}
	DEBUGPRINT(std::cout<<std::endl;)
	// G := split graph at bn[in] according to parity and P
	unsigned int deg = degree(bn[dep], G);
	DEBUGPRINT(std::cout << "DEBUG: Splitting branch node " << bn[dep] << "with degree " << deg << std::endl;)
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

	DEBUGPRINT(std::cout<<"DEBUG: Edges: "<<to_string_graph_edgelist(G)<<std::endl;)
	DEBUGPRINT(std::cout<<"DEBUG: Graph after split: \n"<<to_string_graph(G);)
	unsigned int s = split_vertices.size();
	unsigned int k = 0;
	DEBUGPRINT(std::cout<<"DEBUG: The new vertices after split are ";)
	for( k = 0; k < s; ++k)
	{
		DEBUGPRINT(std::cout<<split_vertices[k]<<" ";)
	}
	DEBUGPRINT(std::cout<<std::endl;)
	std::vector<int> component(num_vertices(G));
	if( (unsigned int) connected_components(G, &component[0]) > dep+2)
	{
		DEBUGPRINT(std::cout << "DEBUG: Graph becomes disconnected ..." << std::endl;)
			DEBUGPRINT(std::cout << "DEBUG: Removing the new vertices and patching " << bn[dep] << " " << "..." << std::endl;)
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
					DEBUGPRINT(std::cout<<"DEBUG: Both branchs disconnect the graph, backtracking ..."<<std::endl;)
					DEBUGPRINT(std::cout<<"DEBUG: Removing the new vertices and patching "<<bn[dep]<<" "<< "..."<<std::endl;)
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
			DEBUGPRINT(std::cout<<"DEBUG: Embedding has an A-trail with this parity ..."<<std::endl;)
			return true;
		}
	}
}


void enumerate_intersectable_nodes(const Graph &G, std::vector<unsigned int> & bn, Vertex s)
{
	bn.clear();
	VertexIterator vertexBegin, vertexIt,vertexEnd;
	tie(vertexBegin, vertexEnd) = vertices(G);
	unsigned int n = num_vertices(G);
	enum color_type { white, gray, black };
	color_type * color = new color_type[n];
	std::queue<Vertex> checked_queue;
	Vertex u, v;

	for( vertexIt = vertexBegin; vertexIt != vertexEnd; ++vertexIt ) { color[*vertexIt] = white; }

	color[s] = gray;
	checked_queue.push(s);
	if( degree(s, G) >= 6) bn.push_back(s);

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

