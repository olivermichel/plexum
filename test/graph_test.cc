#include <catch.h>

#include <plexum/graph.h>

class V {
public:
	V() = delete;
	V(unsigned long i) : i(i) { }
	unsigned long i;
};

class E {
public:
	E() = delete;
	E(unsigned long i, double weight) : i(i), weight(weight) { }
	unsigned long i;
	double weight;
};

TEST_CASE("graph vertices and edges", "[Graph]")
{
	plexum::Graph<int, int> g1;

	REQUIRE(typeid(g1) == typeid(plexum::Graph<int,int>));

	SECTION("graphs are empty upon construction")
	{
		REQUIRE(g1.vertices.count() == 0);
		REQUIRE(g1.edges.count() == 0);
	}

	SECTION("vertices can be added")
	{
		g1.vertices.add(1);
		g1.vertices.add(2);

		REQUIRE(g1.vertices.has_index(0));
		REQUIRE(g1.vertices.has_index(1));
		REQUIRE(!(g1.vertices.has_index(2)));

		REQUIRE(g1.vertices.count() == 2);
	}

	SECTION("vertices can be connected through edges")
	{
		g1.vertices.add(1);
		g1.vertices.add(2);
		g1.edges.add(g1.vertices[0], g1.vertices[1], 1);
		REQUIRE(g1.edges.count() == 1);
	}

	SECTION("edge iterators hold iterators to connected vertices")
	{
		auto a = g1.vertices.add(1);
		auto b = g1.vertices.add(2);
		auto e = g1.edges.add(g1.vertices[0], g1.vertices[1], 1);

		REQUIRE(e.from() == a);
		REQUIRE(e.to() == b);
		REQUIRE(e.from().id() == 0);
		REQUIRE(e.to().id() == 1);
	}

	SECTION("vertices and edges can be accessed through sequential indices")
	{
		g1.vertices.add(1);
		g1.vertices.add(2);
		g1.vertices.add(3);

		REQUIRE_NOTHROW(g1.vertices[2]);
		REQUIRE_THROWS(g1.vertices[3]);
		REQUIRE_THROWS(g1.vertices[42]);

		g1.edges.add(g1.vertices[0], g1.vertices[1], 1);
		g1.edges.add(g1.vertices[1], g1.vertices[2], 2);

		REQUIRE_NOTHROW(g1.edges[0]);
		REQUIRE_NOTHROW(g1.edges[1]);
		REQUIRE_THROWS(g1.edges[42]);
		REQUIRE_THROWS(g1.edges[2]);
	}

	SECTION("edges can be accessed through a vertex pair")
	{
		g1.vertices.add(1);
		g1.vertices.add(2);
		g1.vertices.add(3);
		g1.edges.add(g1.vertices[0], g1.vertices[1], 1);
		g1.edges.add(g1.vertices[1], g1.vertices[2], 2);

		REQUIRE(*(g1.edges.between(g1.vertices[0], g1.vertices[1])) == 1);
		REQUIRE(*(g1.edges.between(g1.vertices[1], g1.vertices[2])) == 2);

		REQUIRE(*(g1.edges.between(g1.vertices[1], g1.vertices[0])) == 1);
		REQUIRE(*(g1.edges.between(g1.vertices[2], g1.vertices[1])) == 2);

		REQUIRE_THROWS(g1.edges.between(g1.vertices[2], g1.vertices[0]));
		REQUIRE_THROWS(g1.edges.between(g1.vertices[0], g1.vertices[2]));
	}

	SECTION("vertices.neighbors() returns iterators to neighbor vertices") {

		auto v1 = g1.vertices.add(1);
		auto v2 = g1.vertices.add(2);
		auto v3 = g1.vertices.add(3);
		auto e1 = g1.edges.add(g1.vertices[0], g1.vertices[1], 1);
		auto e2= g1.edges.add(g1.vertices[1], g1.vertices[2], 2);

		REQUIRE(v1.neighbors().size() == 1);
		REQUIRE(v2.neighbors().size() == 2);
		REQUIRE(v3.neighbors().size() == 1);

		REQUIRE(*(v1.neighbors()[0]) == *v2);
		REQUIRE(*(v2.neighbors()[0]) == *v1);
		REQUIRE(*(v2.neighbors()[1]) == *v3);
		REQUIRE(*(v3.neighbors()[0]) == *v2);
	}

	SECTION("vertices can be removed")
	{
		auto v1 = g1.vertices.add(1);
		auto v2 = g1.vertices.add(2);
		auto v3 = g1.vertices.add(3);
		auto e1 = g1.edges.add(g1.vertices[0], g1.vertices[1], 1);
		auto e2= g1.edges.add(g1.vertices[1], g1.vertices[2], 2);

		REQUIRE_THROWS(g1.vertices.remove(v1));
		g1.edges.remove(e1);
		REQUIRE_NOTHROW(g1.vertices.remove(v1));
		REQUIRE_NOTHROW(g1.vertices.remove_with_edges(v2));

		REQUIRE(g1.vertices.count() == 1);
		REQUIRE(g1.edges.count() == 0);
	}

	SECTION("edges can be removed")
	{
		auto v1 = g1.vertices.add(1);
		auto v2 = g1.vertices.add(2);
		auto e1 = g1.edges.add(g1.vertices[0], g1.vertices[1], 1);

		REQUIRE(g1.edges.count() == 1);
		REQUIRE_NOTHROW(g1.edges.remove(g1.edges.between(v2, v1)));
		REQUIRE_THROWS(g1.edges.remove(g1.edges.between(v2, v1)));
		REQUIRE(g1.edges.count() == 0);
	}
}

TEST_CASE("sub and super graphs", "[Graph]")
{
	plexum::Graph<int, int> g1;
	plexum::Graph<int, int> g2;
	plexum::Graph<int, int> g3;

	SECTION("graph objects can be mapped onto each other")
	{
		REQUIRE(typeid(g1) == typeid(plexum::Graph<int,int>));
		REQUIRE(typeid(g2) == typeid(plexum::Graph<int,int>));

		REQUIRE(!g1.has_subgraphs());
		REQUIRE(!g2.supergraph());

		g1.map(&g2);

		REQUIRE(g1.has_subgraphs());
		REQUIRE(g1.has_subgraph(&g2));
		REQUIRE(g2.supergraph());
	}

	SECTION("unmapping a not mapped graph throws an exception")
	{
		g1.map(&g3);
		REQUIRE_THROWS(g1.unmap(&g2));
		REQUIRE_NOTHROW(g1.unmap(&g3));
	}

	SECTION("unmapping a not mapped vertex throws an exception")
	{
		auto a = g1.vertices.add(1);
		auto b = g2.vertices.add(1);
		auto c = g3.vertices.add(1);

		g1.vertices[0].map(b);

		REQUIRE_THROWS(g1.vertices[0].unmap(c));
		REQUIRE_NOTHROW(g1.vertices[0].unmap(b));
	}

	SECTION("unmapping a not mapped edge throws an exception")
	{
		g1.vertices.add(1);
		g2.vertices.add(1);
		g3.vertices.add(1);
		g1.vertices.add(2);
		g2.vertices.add(2);
		g3.vertices.add(2);

		auto a = g1.edges.add(g1.vertices[0], g1.vertices[1], 1);
		auto b = g1.edges.add(g1.vertices[0], g1.vertices[1], 1);
		auto c = g1.edges.add(g1.vertices[0], g1.vertices[1], 1);

		g1.edges[0].map_link(b);

		REQUIRE_THROWS(g1.edges[0].unmap(c));
		REQUIRE_NOTHROW(g1.edges[0].unmap(b));
	}

	SECTION("map and unmap graph vertices")
	{
		auto a = g1.vertices.add(1);
		auto b = g2.vertices.add(1);

		REQUIRE(!g1.vertices[0].has_subvertices());
		REQUIRE(!g1.vertices[0].has_supervertex());
		REQUIRE(!g2.vertices[0].has_subvertices());
		REQUIRE(!g2.vertices[0].has_supervertex());

		REQUIRE(g1.vertices[0].sub_vertices().empty());
		REQUIRE(g1.vertices[0].super_vertex() == nullptr);
		REQUIRE(g2.vertices[0].sub_vertices().empty());
		REQUIRE(g2.vertices[0].super_vertex() == nullptr);

		g1.vertices[0].map(g2.vertices[0]);

		REQUIRE(g1.vertices[0].has_subvertices());
		REQUIRE(!g1.vertices[0].has_supervertex());
		REQUIRE(g2.vertices[0].has_supervertex());
		REQUIRE(!g2.vertices[0].has_subvertices());

		REQUIRE(g1.vertices[0].sub_vertices()[0] == b._ptr());
		REQUIRE(g1.vertices[0].super_vertex() == nullptr);
		REQUIRE(g2.vertices[0].sub_vertices().empty());
		REQUIRE(g2.vertices[0].super_vertex() == a._ptr());

		g1.vertices[0].unmap(g2.vertices[0]);

		REQUIRE(!g1.vertices[0].has_subvertices());
		REQUIRE(!g1.vertices[0].has_supervertex());
		REQUIRE(!g2.vertices[0].has_subvertices());
		REQUIRE(!g2.vertices[0].has_supervertex());

		REQUIRE(g1.vertices[0].sub_vertices().empty());
		REQUIRE(g1.vertices[0].super_vertex() == nullptr);
		REQUIRE(g2.vertices[0].sub_vertices().empty());
		REQUIRE(g2.vertices[0].super_vertex() == nullptr);
	}

	SECTION("map and unmap graph edges")
	{
		g1.vertices.add(1);
		g1.vertices.add(2);
		g2.vertices.add(1);
		g2.vertices.add(2);

		auto a = g1.edges.add(g1.vertices[0], g1.vertices[1], 1);
		auto b = g2.edges.add(g2.vertices[0], g2.vertices[1], 1);

		REQUIRE(!g1.edges[0].has_subedges());
		REQUIRE(!g1.edges[0].has_superedge());
		REQUIRE(!g2.edges[0].has_subedges());
		REQUIRE(!g2.edges[0].has_superedge());

		REQUIRE(g1.edges[0].sub_edges().empty());
		REQUIRE(g1.edges[0].super_edge().empty());
		REQUIRE(g2.edges[0].sub_edges().empty());
		REQUIRE(g2.edges[0].super_edge().empty());

		g1.edges[0].map_link(g2.edges[0]);

		REQUIRE(g1.edges[0].has_subedges());
		REQUIRE(!g1.edges[0].has_superedge());
		REQUIRE(g2.edges[0].has_superedge());
		REQUIRE(!g2.edges[0].has_subedges());

		REQUIRE(g1.edges[0].sub_edges()[0] == b._ptr());
		REQUIRE(g1.edges[0].super_edge().empty());
		REQUIRE(g2.edges[0].sub_edges().empty());
		REQUIRE(g2.edges[0].super_edge()[0] == a._ptr());

		g1.edges[0].unmap(g2.edges[0]);

		REQUIRE(!g1.edges[0].has_subedges());
		REQUIRE(!g1.edges[0].has_superedge());
		REQUIRE(!g2.edges[0].has_subedges());
		REQUIRE(!g2.edges[0].has_superedge());

		REQUIRE(g1.edges[0].sub_edges().empty());
		REQUIRE(g1.edges[0].super_edge().empty());
		REQUIRE(g2.edges[0].sub_edges().empty());
		REQUIRE(g2.edges[0].super_edge().empty());
	}
}

TEST_CASE("bfs", "[Graph]")
{
	plexum::Graph<V, E> g;

	REQUIRE(typeid(g) == typeid(plexum::Graph<V,E>));

	auto v1 = g.vertices.add(0);
	auto v2 = g.vertices.add(1);
	auto v3 = g.vertices.add(2);
	auto v4 = g.vertices.add(4);
	auto e1 = g.edges.add(v1, v2, {0, 1});
	auto e2 = g.edges.add(v2, v3, {1, 2});
	auto e3 = g.edges.add(v3, v1, {2, 2});

	REQUIRE(g.vertices.count() == 4);
	REQUIRE(g.edges.count() == 3);

	SECTION("find an unconstrained BFS path")
	{
		std::vector<plexum::Graph<V, E>::edge_proxy::iterator> path;
		REQUIRE_NOTHROW(path = g.find_path(v1, v2));
		REQUIRE(path.size() == 1);
		REQUIRE(path[0].from() == v1);
		REQUIRE(path[0].to() == v2);
	}

	SECTION("throw an exception if there is no unconstrained path")
	{
		std::vector<plexum::Graph<V, E>::edge_proxy::iterator> path;
		REQUIRE_THROWS(path = g.find_path(v1, v4));
	}

	SECTION("finds a constrained BFS path")
	{
		std::vector<plexum::Graph<V, E>::edge_proxy::iterator> path;
		REQUIRE_NOTHROW(path = g.find_path(v1, v2, [](E* e) { return e->weight >= 1; }));
		REQUIRE(path.size() == 1);
		REQUIRE(path[0].from() == v1);
		REQUIRE(path[0].to() == v2);
	}

	SECTION("finds a constrained BFS path")
	{
		std::vector<plexum::Graph<V, E>::edge_proxy::iterator> path;
		REQUIRE_NOTHROW(path = g.find_path(v1, v2, [](E* e) { return e->weight >= 2; }));
		REQUIRE(path.size() == 2);
		REQUIRE(path[0].from() == v3);
		REQUIRE(path[0].to() == v1);
		REQUIRE(path[1].from() == v2);
		REQUIRE(path[1].to() == v3);
	}

	SECTION("throws an exception if the is no constrained path")
	{
		std::vector<plexum::Graph<V, E>::edge_proxy::iterator> path;
		REQUIRE_THROWS(path = g.find_path(v1, v3, [](E* e) { return e->weight >= 10; }));
	}
}
