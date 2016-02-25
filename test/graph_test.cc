#include <catch.h>

#include <omg/graph.h>

TEST_CASE("a graph can be constructed", "[graph]")
{
	omg::Graph<int, int> g;

	REQUIRE(g.vertices.count() == 0);
	REQUIRE(g.edges.count() == 0);
}
