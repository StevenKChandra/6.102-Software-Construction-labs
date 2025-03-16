#ifndef CONCRETE_EDGE_GRAPH_TPP
#define CONCRETE_EDGE_GRAPH_TPP

#include <stdexcept>
#include <string>

#include "concrete_edges_graph.h"

template<typename T>
Edge<T>::Edge(T source, T target, int weight):
    source{source}, target{target}, weight{weight} { }

template<typename T>
ConcreteEdgesGraph<T>::ConcreteEdgesGraph():
    vertices_{}, edges{} {}

template<typename T>
bool ConcreteEdgesGraph<T>::add(T vertex) {
    if (vertices_.find(vertex) != vertices_.end()) return false;

    vertices_.insert(vertex);
    return true;
}

template<typename T>
int ConcreteEdgesGraph<T>::set(T source, T target, int weight) {
    if (weight < 0) throw std::domain_error("Edge weight cannot be negative.");
    
    int edgeIndex = findEdgeIndex(source, target);

    int previousWeight = 0;

    if (edgeIndex != -1) {
        previousWeight = (*edges[edgeIndex]).weight;
        delete edges[edgeIndex];
        edges.erase(edges.begin() + edgeIndex);
    }

    if (weight == 0) return previousWeight;

    if (vertices_.find(source) == vertices_.end()) vertices_.insert(source);
    if (vertices_.find(target) == vertices_.end()) vertices_.insert(target);
    edges.push_back(new Edge(source, target, weight));

    return previousWeight;
}

template<typename T>
bool ConcreteEdgesGraph<T>::remove(T vertex) {
    if (vertices_.find(vertex) == vertices_.end()) {
        return false;
    }

    vertices_.erase(vertex);

    std::vector<int> indicesToRemove;
    for (int i = edges.size() - 1; i >= 0; i--) {
        if ((*edges[i]).source == vertex || (*edges[i]).target == vertex) indicesToRemove.push_back(i);
    }

    for (int i = 0; i < indicesToRemove.size(); i++) {
        delete edges[i];
        edges.erase(edges.begin() + indicesToRemove[i]);
    }
    return true;
}

template<typename T>
std::unordered_set<T> ConcreteEdgesGraph<T>::vertices() {
    return vertices_;
}

template<typename T>
std::unordered_map<T, int> ConcreteEdgesGraph<T>::sources(T target) {
    std::unordered_map<T, int> output;
    for (int i = 0; i < edges.size(); i++) {
        if ((*edges[i]).target == target) output[(*edges[i]).source] = (*edges[i]).weight;
    }
    return output;
}

template<typename T>
std::unordered_map<T, int> ConcreteEdgesGraph<T>::targets(T source) {
    std::unordered_map<T, int> output;
    for (int i = 0; i < edges.size(); i++) {
        if ((*edges[i]).source == source) output[(*edges[i]).target] = (*edges[i]).weight;
    }
    return output;
}

template<typename T>
int ConcreteEdgesGraph<T>::findEdgeIndex(T source, T target) {
    for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
        if ((*edges[edgeIndex]).source == source && (*edges[edgeIndex]).target == target) {
            return edgeIndex;
        }
    }
    return -1;
}

template<typename T>
ConcreteEdgesGraph<T>::~ConcreteEdgesGraph() {
    for (int i = 0; i < edges.size(); i++) {
        delete edges[i];
    }
}

#endif