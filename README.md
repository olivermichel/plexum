# Plexum Graph Library

by Oliver Michel < oliver dot michel at colorado dot edu >

This is a STL-compliant header-only graph library working with arbitrary types of
vertices or edges. The library currently only supports undirected simple
graphs.

## Running the tests

    mkdir build && cd build
    cmake ..
    make
    ./run_tests -s

## Basic usage

    #include <omg/graph.h>
    
    omg::Graph<std::string, std::string> g;
    auto v1 = g.vertices.add("v1");
    auto v2 = g.vertices.add("v2");
    g.edges.add(v1, v2, "e1");
    
    for(auto v : g.vertices)
        std::cout << v << " "; // v1 v2
    

## TODO
* directed graph support
* DFS/BFS traversal iterators
* basic graph properties (diameter, centralities)
