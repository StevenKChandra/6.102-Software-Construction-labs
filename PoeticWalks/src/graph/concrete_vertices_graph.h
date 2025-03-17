#ifndef CONCRETE_VERTICES_GRAPH_H
#define CONCRETE_VERTICES_GRAPH_H

#include <vector>

#include "graph.h"

/**
 * A mutable vertex data structure.
 * Contains edges to other vertices connected to this vertex.
 */
template<typename T>
class Vertex {
    /**
     * Abstraction function:
     *      - represents a vertex with corresponding label in the graph
     *      - sources map represents edges from other vertices and their weights
     *      - targets map represents edges to other vertices and their weights
     * 
     * Representation invariant:
     *      - for all edges connected to this vertex, vertex at other end of the edge exist in the graph
     *      - for all edges connected to this vertex, the edge's weight is non negative
     *
     *  Safety from rep exposure:
     *      - all getter function returns by value.
     */

public: 
    const T label;
    
private:
    std::unordered_map<Vertex<T>*, int> targets;
    std::unordered_map<Vertex<T>*, int> sources;

public:
    Vertex() = delete;

    Vertex(T vertex);

    // /**
    //  * Set the a new weight for the edge from the given source vertex to this vertex.
    //  * If the edge does not exist before then a new one is created.
    //  * If the weight parameter is 0 then the edge is removed.
    //  * 
    //  * @param source the pointer of source vertex
    //  * @param weight new weight of the edge, must be non negative
    //  * 
    //  * @return the previous weight of the edge
    //  *          0 if such there was no such edge
    //  */
    // int setSource(Vertex<T> *source, int weight);

    /**
     * Set the a new weight for the edge from the this vertex to given target vertex.
     * If the edge does not exist before then a new one is created.
     * If the weight parameter is 0 then the edge is removed.
     * 
     * @param source the pointer of target vertex
     * @param weight new weight of the edge, must be non negative
     * 
     * @return the previous weight of the edge
     *         0 if such there was no such edge
     */
    int setTarget(Vertex<T> *target, int weight);

    /**
     * Get the source vertices with edges targeting this vertex and the
     * weights of those edges.
     * 
     * @return a map where the key are the source vertices with edges
     *         targeting this vertex and the value for each key are 
     *         the weight of edges from the key to this vertex
     */
    std::unordered_map<T, int> getSources();

    /**
     * Get the target vertices with edges from this vertex and the
     * weights of those edges.
     * 
     * @return a map where the key are the target vertices with edges
     *         from this vertex and the value for each key are 
     *         the weight of edges targetting the key from this vertex
     */
    std::unordered_map<T, int> getTargets();
};

template<typename T>
class ConcreteVerticesGraph: public Graph<T> {
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
    std::vector<Vertex<T>*> vertices_;
    
public:
    ConcreteVerticesGraph();

    ~ConcreteVerticesGraph();

    bool add(T vertex) override;

    int set(T source, T target, int weight) override;

    bool remove(T vertex) override;

    std::unordered_set<T> vertices() override;

    std::unordered_map<T, int> sources(T target) override;

    std::unordered_map<T, int> targets(T source) override;

private:
    /**
     * Returns the index in vertices vector that correspond to a vertex
     * 
     * @param source label of the vertex
     * @return the index of the said vertex in vertices vector.
     *         -1 if there is no such vertex.
    */
    int findVertexIndex(T vertex);
};

#include "concrete_vertices_graph.tpp"

#endif