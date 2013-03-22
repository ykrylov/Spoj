// shortest_path.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>
#include <queue>
#include <stack>
#include <limits>
#include <stdexcept>

typedef unsigned uint32_t;

char * cached_input;
struct graph_node
{
	graph_node(uint32_t node_id = 0, uint32_t weight = 0) 
		: m_node_id(node_id)
		, m_weight(weight)
	{}
	uint32_t m_node_id;
	uint32_t m_weight;
};

struct d_weight
{
	uint32_t m_weight;
	bool	 m_selected;
	d_weight() 
		: m_weight(std::numeric_limits<uint32_t>::max())
		, m_selected(false)
	{}
};

class shortest_path
{
public:
	shortest_path(std::istream & strm);
	shortest_path(std::istream & strm, char * cached_input);
	bool find_path_dfs(uint32_t from, uint32_t to, std::vector<uint32_t> & path);
	bool find_path_bfs(uint32_t from, uint32_t to, std::vector<uint32_t> & path);

	uint32_t find_shortest_path(uint32_t from, uint32_t to);
private:
	bool find_path_dfs_internal(uint32_t current, uint32_t to, std::vector<uint32_t> & path, std::vector<bool> & visited_nodes);

	typedef std::map<std::string, uint32_t> city_name_id_map;
	city_name_id_map m_name_id_map;

	typedef std::vector<graph_node>::iterator graph_node_list_iter;
	typedef std::vector<std::vector<graph_node> > city_graph;
	city_graph m_graph;

	typedef std::pair<std::string, std::string> path;
	typedef std::vector<path> paths_arr;
	paths_arr	m_paths_arr;

	typedef std::vector<d_weight> weights;
	typedef std::vector<weights>  weight_city_table;
	weight_city_table m_city_weights;
};

uint32_t shortest_path::find_shortest_path(uint32_t from, uint32_t to)
{
	weights & from_weights = m_city_weights[from];
	if( !from_weights.empty() )
	{
		return from_weights[to].m_weight;
	}

	std::map<uint32_t, uint32_t> min_nodes_weight;

	uint32_t city_nbr = m_graph.size();
	from_weights.resize(city_nbr);

	std::vector<graph_node>::iterator it  = m_graph[from].begin(); 
	std::vector<graph_node>::iterator end = m_graph[from].end();

	for(; it != end; ++it)
	{
		graph_node& gn = *it;
		from_weights[gn.m_node_id].m_weight = gn.m_weight;
	}

	//std::for_each(
	//	m_graph[from].begin(), 
	//	m_graph[from].end(),
	//	[&from_weights](const graph_node& gn)
	//	{
	//		from_weights[gn.m_node_id].m_weight = gn.m_weight;
	//	}
	//);

	from_weights[from].m_selected = true;
	from_weights[from].m_weight = 0;

	uint32_t current_min_point = from;

	while(--city_nbr)
	{	
		std::vector<graph_node>::iterator it  = m_graph[current_min_point].begin(); 
		std::vector<graph_node>::iterator end = m_graph[current_min_point].end();

		for(; it != end; ++it)
		{
			graph_node& gn = *it;
			if( !from_weights[gn.m_node_id].m_selected )
			{
				if(from_weights[gn.m_node_id].m_weight == 
					std::numeric_limits<uint32_t>::max())
				{
					from_weights[gn.m_node_id].m_weight = 
						from_weights[current_min_point].m_weight + gn.m_weight;
				}
				else
				{
					from_weights[gn.m_node_id].m_weight = 
						std::min(from_weights[current_min_point].m_weight + gn.m_weight, 
									from_weights[gn.m_node_id].m_weight);
							         
				}
				min_nodes_weight[from_weights[gn.m_node_id].m_weight] = gn.m_node_id;
			}
		}

		//std::for_each(
		//	m_graph[current_min_point].begin(), 
		//	m_graph[current_min_point].end(),
		//	[&from_weights, &min_nodes_weight, current_min_point](const graph_node& gn)
		//	{
		//		if( !from_weights[gn.m_node_id].m_selected )
		//		{
		//			if(from_weights[gn.m_node_id].m_weight == 
		//				std::numeric_limits<uint32_t>::max())
		//			{
		//				from_weights[gn.m_node_id].m_weight = 
		//					from_weights[current_min_point].m_weight + gn.m_weight;
		//			}
		//			else
		//			{
		//				from_weights[gn.m_node_id].m_weight = 
		//					std::min(from_weights[current_min_point].m_weight + gn.m_weight, 
		//							 from_weights[gn.m_node_id].m_weight);
		//					         
		//			}
		//			min_nodes_weight[from_weights[gn.m_node_id].m_weight] = gn.m_node_id;
		//		}
		//	}
		//);

		current_min_point = (min_nodes_weight.begin())->second;
		min_nodes_weight.erase(min_nodes_weight.begin());
		from_weights[current_min_point].m_selected = true;
	}

	return from_weights[to].m_weight;
}

bool  shortest_path::find_path_bfs(uint32_t from, uint32_t to, std::vector<uint32_t> & path)
{
	
	std::vector<bool> marked_nodes(m_graph.size(), false);
	static const uint32_t no_parent = std::numeric_limits<uint32_t>::max();

	std::vector<uint32_t> parent_nodes(m_graph.size(),no_parent);
	std::stack<uint32_t>  nodes_order;

	std::queue<uint32_t> node_qu;

//	path.push_back(from);
	node_qu.push(from);
	marked_nodes[from] = true;

	while(!node_qu.empty())
	{
		uint32_t current = node_qu.front();
		node_qu.pop();

		if(current == to)
		{
			nodes_order.push(current);
			// do something
			uint32_t parent_node;
			do
			{
				parent_node = parent_nodes[current];
				if(parent_node != no_parent)
				{
					nodes_order.push(parent_node);
					current = parent_node;
				}

			} while( parent_node != no_parent );

			while(!nodes_order.empty())
			{
				path.push_back(nodes_order.top());
				nodes_order.pop();
			}
			return true;
		}
		for(size_t i = 0; i < m_graph[current].size(); ++i)
		{
			uint32_t next_node = m_graph[current][i].m_node_id;
			if(!marked_nodes[next_node])
			{
				marked_nodes[next_node] = true;
				parent_nodes[next_node] = current;
				node_qu.push(next_node);
			}
		}
	}
	return false;
}

bool shortest_path::find_path_dfs(uint32_t current, uint32_t to, std::vector<uint32_t> & path)
{
	std::vector<bool> visited_nodes(m_graph.size(), false);
	return find_path_dfs_internal(current, to, path, visited_nodes);
}

bool shortest_path::find_path_dfs_internal(uint32_t current, uint32_t to, std::vector<uint32_t> & path, std::vector<bool> & visited_nodes)
{
	path.push_back(current);
	if(current == to) return true;

	visited_nodes[current] = true;
	 bool found = false;

	 for(size_t i = 0; i < m_graph[current].size() && !found; ++i)
	 {
		 uint32_t next_node = m_graph[current][i].m_node_id;
		 if(!visited_nodes[next_node])
		 {
			 found = find_path_dfs_internal(next_node, to, path, visited_nodes);
		 }
	 }
	 
   if(!found) path.pop_back();

   return found;
}

shortest_path::shortest_path(std::istream & strm)
{
	uint32_t city_numbers;
	strm >> city_numbers;

	m_graph.resize(city_numbers);
	m_city_weights.resize(city_numbers);

	std::string city_name;
	uint32_t    route_nbrs;
	graph_node  gn;
	for (uint32_t i = 0; i < city_numbers; ++i)
	{
		strm >> city_name; 
		m_name_id_map[city_name] = i;

		strm >> route_nbrs;
		m_graph[i].reserve(route_nbrs);
		while(route_nbrs--)
		{
			strm >> gn.m_node_id >> gn.m_weight;
			--gn.m_node_id;
			m_graph[i].push_back(gn);
		}
	}

	uint32_t path_nbr;
	strm >> path_nbr;
	m_paths_arr.reserve(path_nbr);
	path one_path;

	uint32_t path_weight;
	while(path_nbr--)
	{
		strm >> one_path.first >> one_path.second;
		m_paths_arr.push_back(one_path);

		uint32_t from = m_name_id_map[one_path.first];
		uint32_t to   = m_name_id_map[one_path.second];

		path_weight = find_shortest_path(from, to);

		std::cout << path_weight << std::endl;

		//std::vector<uint32_t> way;
		//find_path_dfs(from, to, way);
		//way.clear();
		//find_path_bfs(from, to, way);
	}

	    //uint32_t from, to;
		//for(int i=0; i < m_paths_arr.size(); ++i)
		//{
		//	from = m_name_id_map[m_paths_arr[i].first];
		//	to   = m_name_id_map[m_paths_arr[i].second];
		//}

}

shortest_path::shortest_path(std::istream & strm, char * cached_input)
{
	
	uint32_t city_numbers;
	strm >> city_numbers;

	m_graph.resize(city_numbers);
	m_city_weights.resize(city_numbers);

	std::string city_name;
	uint32_t    route_nbrs;
	graph_node  gn;
//	char city_name_str[16];
	for (uint32_t i = 0; i < city_numbers; ++i)
	{
	/*	strm.getline((char*)city_name_str, 15);*/
		strm >> city_name; 
		m_name_id_map[city_name] = i;

		strm >> route_nbrs;
		m_graph[i].reserve(route_nbrs);
		while(route_nbrs--)
		{
			strm >> gn.m_node_id >> gn.m_weight;
			--gn.m_node_id;
			m_graph[i].push_back(gn);
		}
	}

	uint32_t path_nbr;
	strm >> path_nbr;
	m_paths_arr.reserve(path_nbr);
	path one_path;

	uint32_t path_weight = 1;
	while(path_nbr--)
	{
		strm >> one_path.first >> one_path.second;
		m_paths_arr.push_back(one_path);

		uint32_t from = m_name_id_map[one_path.first];
		uint32_t to   = m_name_id_map[one_path.second];

//		path_weight = find_shortest_path(from, to);

		std::cout << path_weight << std::endl;

		//std::vector<uint32_t> way;
		//find_path_dfs(from, to, way);
		//way.clear();
		//find_path_bfs(from, to, way);
	}
	strm.ignore();

	    //uint32_t from, to;
		//for(int i=0; i < m_paths_arr.size(); ++i)
		//{
		//	from = m_name_id_map[m_paths_arr[i].first];
		//	to   = m_name_id_map[m_paths_arr[i].second];
		//}

}

int main()
{
//#define _my_test

	static const unsigned CACHED_SIZE = 10*1024*1024;
	cached_input = new char[CACHED_SIZE];
	
	uint32_t test_cases_nbr;

#ifdef _my_test
	std::string input_str("1\n4\ngdansk\n2\n2 1\n3 3\nbydgoszcz\n3\n1 1\n3 1\n4 4\ntorun\n3\n1 3\n2 1\n4 1\nwarszawa\n2\n2 4\n3 1\n2\ngdansk warszawa\nbydgoszcz warszawa\n");
	std::istringstream input(input_str);
    input >> test_cases_nbr;
#else
	std::cin >> test_cases_nbr;
	memset(cached_input, 0, CACHED_SIZE);
	long long size = std::cin.readsome(cached_input, CACHED_SIZE-1);
	if(size < CACHED_SIZE-1 )
		cached_input[size] = 0;
	else
		throw std::runtime_error("Overflow");

	std::string input_str(cached_input);
	std::istringstream input(input_str);
#endif
    while( test_cases_nbr-- )
    {
#ifdef _my_test
		shortest_path sp(input);
#else
		shortest_path sp(input);
#endif
    }
	delete[] cached_input;
	return 0;
}

