#include <iostream>

#include "graph_poet.h"
#include "concrete_vertices_graph.h"

int main () {
    GraphPoet graphPoet= GraphPoet(new ConcreteVerticesGraph<std::string>(), "/../src/poet/mugar-omni-theater.txt");
    std::cout<<graphPoet.poem("This is my theater system.")<<std::endl;
    return 0;
}