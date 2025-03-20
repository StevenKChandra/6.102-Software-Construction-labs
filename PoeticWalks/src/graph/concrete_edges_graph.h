#ifndef CONCRETE_EDGE_GRAPH_H
#define CONCRETE_EDGE_GRAPH_H

#include <vector>

#include "graph.h"

/**
 * An immutable directed edge data structure.
 * Connects two distinct vertices and have a positive weight.
 */
template<typename T>
class Edge {
    /**
     * Abstraction function:
     *       - represents an edge from source to target with weight.
     * 
     * Representation invariant:
     *      - vertices at the both end of the edge exist in the graph
     *      - weight > 0 
     * 
     * Safety from rep exposure:
     *      - all repressentations are const, no need for getter function
     */
public:
    Edge() = delete;

    Edge(T source, T target, int weight);

    const T source;

    const T target;

    const int weight;   
};

template<typename T>
class ConcreteEdgesGraph: public Graph<T> {
    /**
     * Abstraction function:
     *      - represents a graph.
     *      - vertices set holds the vertices in the graph
     *      - edges vector holds all the edges
     * 
     * Representation invariant:
     *      - for all edges, both the source and target vertex exist in vertices
     *      - for all edges, weight > 0 
     * 
     * Safety from rep exposure:
     *      - set and vector copies L using copy constructor.
     *      - getter functions returns by value, so a copy is made every time.
     */

private:
    std::unordered_set<T> vertices_;
    std::vector<Edge<T>*> edges;

public:
    ConcreteEdgesGraph();

    ~ConcreteEdgesGraph() override;

    bool add(T vertex) override;

    int set(T source, T target, int weight) override;

    bool remove(T vertex) override;

    std::unordered_set<T> vertices() override;

    std::unordered_map<T, int> sources(T target) override;

    std::unordered_map<T, int> targets(T source) override;

private:
    /**
     * Returns the index in edges vector that correspond to an edge
     * with source and target pair equals to the given source and
     * target vertex pair.
     * 
     * @param source label of the source vertex
     * @param target label of the target vertex
     * @return the index of the said edge in edges vector.
     *         -1 if there is no such edge.
    */
    int findEdgeIndex(T source, T target);
};

#include "concrete_edges_graph.tpp"

#endif