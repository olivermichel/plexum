/*
 * plexum graph library
 *
 * oliver dot michel at colorado dot edu
 *
 * MIT License
 */

#ifndef PLEXUM_GRAPH_H
#define PLEXUM_GRAPH_H

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <numeric>

namespace plexum
{
	//
	// plexum::Graph<VertexType, EdgeType>
	//

	/*! @brief The undirected graph class.
	 *  @tparam VertexType the vertex type
	 *  @tparam EdgeType the edge type
	 */
	template<class VertexType, class EdgeType>
	class Graph
	{
	public:

		/*! @brief initialization for the internal vertex/edge index management */
		static const std::size_t ELEMENT_INDEX_INIT = 0;

		//
		// plexum::Graph<VertexType, EdgeType>::exception
		//

		/*! @brief A custom exception class
		 *  @details inherits from std::exception
		 */
		class exception : public std::exception
		{
		public:

			explicit exception(const char* what)
				: _what(what)
			{ }

			explicit exception(const std::string& what)
				: _what(what)
			{ }

			/*! @brief returns the error message associated with the exception
			 * @return a character array containing the error message associated with the exception
			 */
			virtual const char* what() const throw()
			{
				return _what.c_str();
			}

			virtual ~exception() throw()
			{ }

		private:
			std::string _what;
		};

		//
		// plexum::Graph<VertexType, EdgeType>::container<T>
		//

		//! @cond

		template<class T>
		class container
		{
			friend class Graph<VertexType, EdgeType>;
		public:

			container() = delete;

			container(unsigned id, T e)
				: _id(id),
				  _element(e)
			{ }

		protected:
			unsigned _id;
			T _element;
		};

		// forward declare edge_container for referenceability in vertex_container
		template<class> class edge_container;

		//
		// plexum::Graph<VertexType, EdgeType>::vertex_container
		//

		template<class>
		class vertex_container : public container<VertexType>
		{
			friend class Graph<VertexType, EdgeType>;
			friend class vertex_proxy;

		public:

			vertex_container(unsigned id, VertexType e)
				: container<VertexType>(id, e),
				  _neighbors(),
				  _in_edges(),
				  _out_edges(),
				  _super_vertex(),
				  _sub_vertices()
			{ }

		private:

			void _add_neighbor(vertex_container<VertexType>* n)
			{
				_neighbors.push_back(n);
			}

			void _remove_neighbor(vertex_container<VertexType>* n)
			{
				_neighbors.erase(std::find(_neighbors.begin(), _neighbors.end(), n));
			}

			void _add_in_edge(edge_container<EdgeType>* e)
			{
				_in_edges.push_back(e);
			}

			void _remove_in_edge(edge_container<EdgeType>* e)
			{
				_in_edges.erase(std::find(_in_edges.begin(), _in_edges.end(), e));
			}

			void _add_out_edge(edge_container<EdgeType>* e)
			{
				_out_edges.push_back(e);
			}

			void _remove_out_edge(edge_container<EdgeType>* e)
			{
				_out_edges.erase(std::find(_out_edges.begin(), _out_edges.end(), e));
			}

			void _add_sub_vertex(vertex_container<VertexType>* v)
			{
				_sub_vertices.push_back(v);
			}

			void _remove_sub_vertex(vertex_container<VertexType>* v)
			{
				_sub_vertices.erase(std::find(_sub_vertices.begin(), _sub_vertices.end(), v));
			}

			void _set_super_vertex(vertex_container<VertexType>* v)
			{
				_super_vertex = v;
			}

			void _unset_super_vertex()
			{
				_super_vertex = nullptr;
			}

			std::vector<vertex_container<VertexType>*> _neighbors;
			std::vector<edge_container<EdgeType>*>     _in_edges;
			std::vector<edge_container<EdgeType>*>     _out_edges;
			vertex_container<VertexType>*              _super_vertex;
			std::vector<vertex_container<VertexType>*> _sub_vertices;
		};

		//
		// plexum::Graph<VertexType, EdgeType>::edge_container
		//

		template<class>
		class edge_container : public container<EdgeType>
		{
			friend class Graph<VertexType, EdgeType>;
			friend class edge_proxy;

		public:

			edge_container(unsigned id, EdgeType e)
				: container<EdgeType>(id, e),
				  _from(nullptr),
				  _to(nullptr)
			{ }

		private:

			inline void _set_from(vertex_container<VertexType>* v)
			{
				_from = v;
			}

			inline void _set_to(vertex_container<VertexType>* v)
			{
				_to = v;
			}

			inline void _unset_from()
			{
				_from = nullptr;
			}

			inline void _unset_to()
			{
				_to = nullptr;
			}

			void _add_sub_edge(edge_container<EdgeType>* e)
			{
				_sub_edges.push_back(e);
			}

			void _remove_sub_edge(edge_container<EdgeType>* e)
			{
				_sub_edges.erase(std::find(_sub_edges.begin(), _sub_edges.end(), e));
			}

			void _set_super_edge(edge_container<EdgeType>* e)
			{
				_super_edge = {e};
			}

			void _unset_super_edge()
			{
				_super_edge.clear();
			}

			void _add_super_edge(edge_container<EdgeType>* e)
			{
				_super_edge.push_back(e);
			}

			vertex_container<VertexType>* _from;

			vertex_container<VertexType>* _to;

			// sub edge may be mapped to a series of super edges
			std::vector<edge_container<EdgeType>*> _super_edge;

			std::vector<edge_container<EdgeType>*> _sub_edges;
		};

		//! @endcond

		//
		// plexum::Graph<VertexType, EdgeType>::vertex_proxy
		//

		class vertex_proxy
		{
			friend class Graph<VertexType, EdgeType>;
		public:

			//
			// plexum::Graph<VertexType, EdgeType>::vertex_proxy::iterator
			//

			class iterator
			{
				friend class vertex_proxy;

			public:

				iterator() = delete;

				iterator(
					Graph<VertexType, EdgeType>* g,
					typename std::map<std::size_t, vertex_container<VertexType>>::iterator i
				)
					: _g(g),
					  _i(i)
				{ }

				inline iterator operator++()
				{
					return iterator(_g, ++_i);
				}

				inline iterator operator++(int i)
				{
					return iterator(_g, _i++);
				}

				inline VertexType& operator*()
				{
					return _i->second._element;
				}

				inline VertexType* operator->()
				{
					return &(_i->second._element);
				}

				inline bool operator==(const iterator& other)
				{
					return _i == other._i;
				}

				inline bool operator!=(const iterator& other)
				{
					return _i != other._i;
				}

				inline bool operator<(const iterator& other) const
				{
					return _i->first < (other._i)->first;
				}

				inline std::size_t id()
				{
					return _i->first;
				}

				bool has_neighbors()
				{
					return !(_i->second._neighbors.empty());
				}

				std::vector<iterator> neighbors()
				{
					std::vector<iterator> s;

					for(vertex_container<VertexType>* n : _i->second._neighbors)
						s.push_back(iterator(_graph(), _graph()->vertices._vertices.find(n->_id)));

					return s;
				};

				void map(iterator other)
				{
					_i->second._add_sub_vertex(&(other._container()));
					other._container()._set_super_vertex(&(this->_container()));
				}

				template<typename F>
				void map(iterator other, F f)
				{
					map(other);
					f(this->_ptr(), other._ptr());
				}

				void unmap(iterator other)
				{
					auto sub_it = std::find(
						_i->second._sub_vertices.begin(),
						_i->second._sub_vertices.end(),
						&(other._container())
					);

					if(sub_it != _i->second._sub_vertices.end()) {
						_i->second._remove_sub_vertex(&(other._container()));
						other._container()._unset_super_vertex();
					} else {
						throw exception("Graph::vertex_proxy::iterator::unmap(): "
											"specified vertex is not a subvertex");
					}
				}

				template<typename F>
				void unmap(iterator other, F f)
				{
					unmap(other);
					f(this->_ptr(), other._ptr());
				}

				bool has_subvertices()
				{
					return !_container()._sub_vertices.empty();
				}

				bool has_supervertex()
				{
					return _container()._super_vertex != nullptr;
				}

				inline VertexType* super_vertex()
				{
					return has_supervertex() ? &(_container()._super_vertex->_element) : nullptr;
				}

				std::vector<VertexType*> sub_vertices()
				{
					std::vector<VertexType*> v;

					for(auto c : _container()._sub_vertices)
						v.push_back(&(c->_element));

					return v;
				}

				inline Graph<VertexType, EdgeType>* _graph()
				{
					return _g;
				}

				inline vertex_container<VertexType>& _container()
				{
					return _i->second;
				}

				inline VertexType* _ptr()
				{
					return &(_i->second._element);
				}

			private:

				Graph<VertexType, EdgeType>* _g;

				typename std::map<std::size_t, vertex_container<VertexType>>::iterator _i;
			};

			vertex_proxy() = delete;

			explicit vertex_proxy(Graph<VertexType, EdgeType>* graph)
				: _graph(graph),
				  _i(Graph::ELEMENT_INDEX_INIT),
				  _vertices()
			{ }

			iterator add(const VertexType& vertex)
			{

				auto i = iterator(
					_graph,
					_vertices.insert(
						_vertices.end(),
						std::make_pair(_i, vertex_container<VertexType>(_i, vertex))
					)
				);

				_i++;
				return i;
			}

			iterator remove(iterator pos)
			{
				if(pos.has_neighbors()) {
					throw exception("vertex_proxy::remove(): vertex still has neighbors.");
				}

				return iterator(_graph, _vertices.erase(pos._i));
			}

			iterator remove_with_edges(iterator pos)
			{
				if(pos.has_neighbors())
					for(iterator n : pos.neighbors())
						_graph->edges.remove(_graph->edges.between(pos, n));

				return iterator(_graph, _vertices.erase(pos._i));
			}

			bool has_index(std::size_t index)
			{
				return _vertices.find(index) != _vertices.end();
			}

			iterator operator[](std::size_t index) throw(exception)
			{
				auto i = _vertices.find(index);

				if (i != _vertices.end())
					return iterator(_graph, _vertices.find(index));
				else
					throw exception("vertex_proxy::operator[]: index "
									+ std::to_string(index) + " does not exist.");
			}

			inline iterator begin()
			{
				return iterator(_graph, _vertices.begin());
			}

			inline iterator end()
			{
				return iterator(_graph, _vertices.end());
			}

			inline std::size_t count() const
			{
				return _vertices.size();
			}

		private:

			void _add_neighbor(iterator neighbor, iterator vertex) // invoked from edge_proxy
			{
				vertex._container()._add_neighbor(&neighbor._container());
			}

			Graph<VertexType, EdgeType>* _graph;

			std::size_t _i;

			std::map<std::size_t, vertex_container<VertexType>> _vertices;
		};

		//
		// plexum::Graph<VertexType, EdgeType>::edge_proxy
		//

		class edge_proxy
		{
			friend class Graph<VertexType, EdgeType>;
			friend class vertex_proxy;

		public:

			//
			// plexum::Graph<VertexType, EdgeType>::edge_proxy::iterator
			//

			class iterator
			{
				friend class edge_proxy;
				friend class vertex_proxy;

			public:

				iterator() = delete;

				inline iterator(
					Graph<VertexType, EdgeType>* g,
					typename std::map<std::size_t, edge_container<EdgeType>>::iterator i
				)
					: _g(g),
					  _i(i)
				{ }

				inline iterator operator++()
				{
					return iterator(_g, ++_i);
				}

				inline iterator operator++(int i)
				{
					return iterator(_g, _i++);
				}

				inline EdgeType& operator*()
				{
					return _i->second._element;
				}

				inline EdgeType* operator->()
				{
					return &(_i->second._element);
				}

				inline bool operator==(const iterator& other)
				{
					return _i == other._i;
				}

				inline bool operator!=(const iterator& other)
				{
					return _i != other._i;
				}

				inline bool operator<(const iterator& other)
				{
					return _i->first < (other._i)->first;
				}

				inline std::size_t id()
				{
					return _i->first;
				}

				void map_link(iterator other)
				{
					_i->second._add_sub_edge(&(other._container()));
					other._container()._set_super_edge(&(this->_container()));
				}

				template<typename F>
				void map_link(iterator other, F f)
				{
					map_link(other);
					f(this->_ptr(), other._ptr());
				}

				void map_path(iterator other)
				{
					_i->second._add_sub_edge(&(other._container()));
					other._container()._add_super_edge(&(this->_container()));
				}

				template<typename F>
				void map_path(iterator other, F f)
				{
					map_path(other);
					f(this->_ptr(), other._ptr());
				}

				void unmap(iterator other)
				{
					auto sub_it = std::find(
						_i->second._sub_edges.begin(),
						_i->second._sub_edges.end(),
						&(other._container())
					);

					if(sub_it != _i->second._sub_edges.end()) {
						_i->second._remove_sub_edge(&(other._container()));
						other._container()._unset_super_edge();
					} else {
						throw exception("Graph::edge_proxy::iterator::unmap(): "
											"specified edge is not a subedge");
					}
				}

				template<typename F>
				void unmap(iterator other, F f)
				{
					unmap(other);
					f(this->_ptr(), other._ptr());
				}

				bool has_subedges()
				{
					return !_container()._sub_edges.empty();
				}

				bool has_superedge()
				{
					return !_container()._super_edge.empty();
				}

				inline std::vector<EdgeType*> super_edge()
				{
					std::vector<EdgeType*> v;

					for(auto c : _container()._super_edge)
						v.push_back(&(c->_element));

					return v;
				}

				std::vector<EdgeType*> sub_edges()
				{
					std::vector<EdgeType*> v;

					for(auto c : _container()._sub_edges)
						v.push_back(&(c->_element));

					return v;
				}

				inline typename vertex_proxy::iterator from()
				{
					return _g->vertices[_container()._from->_id];
				}

				inline typename vertex_proxy::iterator to()
				{
					return _g->vertices[_container()._to->_id];
				}

				inline Graph<VertexType, EdgeType>* _graph()
				{
					return _g;
				}

				inline edge_container<EdgeType>& _container()
				{
					return _i->second;
				}

				inline EdgeType* _ptr()
				{
					return &(_i->second._element);
				}

			private:

				Graph<VertexType, EdgeType>* _g;

				typename std::map<std::size_t, edge_container<EdgeType>>::iterator _i;
			};

			edge_proxy() = delete;

			edge_proxy(Graph<VertexType, EdgeType>* graph)
				: _graph(graph),
				  _i(Graph::ELEMENT_INDEX_INIT),
				  _edges()
			{ }

			iterator add(
				typename vertex_proxy::iterator from,
				typename vertex_proxy::iterator to,
				const EdgeType& edge)
			{
				auto i = iterator(
					_graph,
					_edges.insert(
						_edges.end(),
						std::make_pair(_i, edge_container<EdgeType>(_i, edge))
					)
				);

				_connect(from, to, i);
				_set_neighbors_bidirectional(from, to);
				_set_edges_bidirectional(from, to, i);
				_i++;
				return i;
			}

			iterator remove(iterator edge_it)
			{
				_unset_neighbors_bidirectional(edge_it);
				_unset_edges_bidirectional(edge_it);
				return iterator(_graph, _edges.erase(edge_it._i));
			}

			bool has_index(std::size_t index)
			{
				return _edges.find(index) != _edges.end();
			}

			iterator operator[](std::size_t index) throw(exception)
			{
				auto i = _edges.find(index);

				if (i != _edges.end())
					return iterator(_graph, _edges.find(index));
				else
					throw exception("edge_proxy::operator[]: index "
									+ std::to_string(index) + " does not exist.");
			}

			iterator begin()
			{
				return iterator(_graph, _edges.begin());
			}

			iterator end()
			{
				return iterator(_graph, _edges.end());
			}

			std::size_t count() const
			{
				return _edges.size();
			}

			iterator between(typename vertex_proxy::iterator from,
							 typename vertex_proxy::iterator to)
				throw(exception)
			{
				for(auto edge : _edges) {

					bool from_match_1 = edge.second._from == &(from._container());
					bool to_match_1   = edge.second._to   == &(to._container());
					bool from_match_2 = edge.second._to   == &(from._container());
					bool to_match_2   = edge.second._from == &(to._container());

					if((from_match_1 && to_match_1) || (from_match_2 && to_match_2))
						return operator[](edge.first);
				}
				throw exception("edge_proxy::between(a, b): there is no edge between a and b");
			}

		private:

			void _connect(
				typename vertex_proxy::iterator from,
				typename vertex_proxy::iterator to,
				iterator edge)
			{
				edge._container()._set_from(&from._container());
				edge._container()._set_to(&to._container());
			}

			void _set_neighbors_bidirectional(
				typename vertex_proxy::iterator from,
				typename vertex_proxy::iterator to)
			{
				_graph->vertices._add_neighbor(from, to);
				_graph->vertices._add_neighbor(to, from);
			}

			void _unset_neighbors_bidirectional(iterator edge)
			{
				edge._container()._from->_remove_neighbor(edge._container()._to);
				edge._container()._to->_remove_neighbor(edge._container()._from);
			}

			void _set_edges_bidirectional(
				typename vertex_proxy::iterator from,
				typename vertex_proxy::iterator to,
				iterator edge)
			{
				from._container()._add_out_edge(&edge._container());
				to._container()._add_in_edge(&edge._container());
				to._container()._add_out_edge(&edge._container());
				from._container()._add_in_edge(&edge._container());
			}

			void _unset_edges_bidirectional(iterator edge)
			{
				edge._container()._from->_remove_in_edge(&edge._container());
				edge._container()._to->_remove_in_edge(&edge._container());
				edge._container()._from->_remove_out_edge(&edge._container());
				edge._container()._to->_remove_out_edge(&edge._container());
			}

			Graph<VertexType, EdgeType>* _graph;
			std::size_t _i;
			std::map<std::size_t, edge_container<EdgeType>> _edges;
		};

	public:

		/*! @brief constructs a new Graph object */
		Graph()
			: vertices(this),
			  edges(this),
			  _supergraph(nullptr),
			  _subgraphs()
		{ }

		/*! @brief maps the graph *subgraph* onto the graph
		 *  @param subgraph the subgraph to be mapped
		 */
		void map(Graph<VertexType, EdgeType>* subgraph)
		{
			_subgraphs.push_back(subgraph);
			subgraph->_supergraph = this;
		}

		/*! @brief checks if *subgraph* is mapped onto the graph
 		 *  @param subgraph the subgraph to be checked
 		 *  @return true if *subgraph* is mapped, false otherwise
 		 */
		bool has_subgraph(Graph<VertexType, EdgeType>* subgraph) const
		{
			return std::find(_subgraphs.begin(), _subgraphs.end(), subgraph) != _subgraphs.end();
		}

		/*! @brief checks whether the graph has any subgraphs mapped
	  	 *  @return true if there are mapped subgraphs, false otherwise
	  	 */
		bool has_subgraphs() const
		{
			return !_subgraphs.empty();
		}

		/*! @brief unmaps *subgraph* from the graph
   		 *  @param subgraph the subgraph to be unmapped
   		 */
		void unmap(Graph<VertexType, EdgeType>* subgraph)
		{
			auto subgraph_it = std::find(_subgraphs.begin(), _subgraphs.end(), subgraph);

			if(subgraph_it != _subgraphs.end()) {
				_subgraphs.erase(subgraph_it);
				subgraph->_supergraph = nullptr;
			} else {
				throw exception("Graph::unmap(): specified graph is not a subgraph");
			}
		}

		/*! @brief returns a pointer to the graph's super-graph
		 *  @return a pointer to the super-graph or std::nullptr if there is no super-graph
		 */
		const Graph<VertexType, EdgeType>* supergraph()
		{
			return _supergraph;
		};

		/*! @brief  returns a reference to a std::vector containing pointers
		 * 		    to the graph's sub-graphs
 		 *  @return a reference to a std::vector containing pointers to the graph's sub-graph or
 		 *  		a reference to an empty std::vector if there are no sub-graphs
 		 */
		const std::vector<Graph<VertexType, EdgeType>*>& subgraphs()
		{
			return _subgraphs;
		};

		template<class T, class U> // T, U in order not to shadow VertexType, EdgeType
		friend std::ostream& operator<<(std::ostream& os, Graph<T, U>& g);

		/*! @brief the vertex_proxy holding the graph's vertices */
		vertex_proxy vertices;

		/*! @brief the edge_proxy holding the graph's edges */
		edge_proxy edges;

	private:

		void _print_adjacency_list(std::ostream& os)
		{
			for(std::pair<std::size_t, vertex_container<VertexType>> p : vertices._vertices) {
				os << " " << p.second._element << " -> [ ";
				for(unsigned i = 0; i < p.second._neighbors.size(); i++) {
					os << p.second._neighbors[i]->_element
					<< (i == p.second._neighbors.size()-2 ? ", " : " ");
				}
				os << "]" << std::endl;
			}
		}

		Graph<VertexType, EdgeType>* _supergraph;

		std::vector<Graph<VertexType, EdgeType>*> _subgraphs;
	};

	template<class VertexType, class EdgeType>
	std::ostream& operator<<(std::ostream& os, Graph<VertexType, EdgeType>& g)
	{
		os << "Graph(n=" << g.vertices.count() << ", m=" << g.edges.count() << ")"
		<<  std::endl;
		g._print_adjacency_list(os);
		return os;
	};
}

#endif
