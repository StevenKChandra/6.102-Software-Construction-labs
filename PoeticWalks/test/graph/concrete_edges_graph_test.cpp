#include "gtest/gtest.h"

#include <string>
#include <stdexcept>

#include "concrete_edges_graph.h"

namespace {

TEST(EdgesIntTest, ConstructorTest) {
    ASSERT_NO_THROW(Edge<int>(1, 2, 0)) << "Constructor must succeed if input is valid";
    EXPECT_THROW(Edge<int>(1, 2, -1), std::domain_error) << "Expected to throw exception when using negative weight";
}
    
class ConcreteEdgesGraphIntTest: public ::testing::Test {
protected:
    void SetUp(void)
    {
        
    }
    
    void TearDown(void)
    {

    }
    ConcreteEdgesGraph<int> graph{};
};

TEST_F(ConcreteEdgesGraphIntTest, ConstructorTest) {
    ASSERT_NO_THROW(ConcreteEdgesGraph<int>()) << "Constructor must work";
}

TEST_F(ConcreteEdgesGraphIntTest, EmptyVerticesIsEmptyTest) {
    EXPECT_EQ(0, ConcreteEdgesGraph<int>().vertices().size());
}

TEST_F(ConcreteEdgesGraphIntTest, AddTest) {
    EXPECT_TRUE(graph.add(1)) << "Expected adding new vertex returns true";
    EXPECT_FALSE(graph.add(1)) << "Expected adding new vertex returns false";
}

TEST_F(ConcreteEdgesGraphIntTest, SetTest) {
    /**
     * Testing strategy:
     * partition on @param source:
     *      - exist in the graph
     *      - does not exist in the graph
     * 
     * partition on @param target:
     *      - exist in the graph
     *      - does not exist in the graph
     * 
     * partition on @param weight:
     *      - zero
     *      - positive
     *      - negative
     * 
     * partition on partition on edges with target/source as one of their endpoints:
     *      - exist in the graph
     *      - does not exist
     */

    // source and target vertices do not exist, positive weight, edge does not exist.
    EXPECT_EQ(0, graph.set(1, 2, 3)) << "Expected adding a new edge with positive weight returns the edge's weight";
    EXPECT_EQ(3, graph.targets(1)[2]) << "Expected correct edge weight";
    EXPECT_EQ(2, graph.vertices().size()) << "Expected correct number of edges";
    
    // source and target vertices exist, positive weight, edge exist.
    EXPECT_EQ(3, graph.set(1, 2, 10)) << "Expected modifying existing edge returns the edge's value";
    EXPECT_EQ(10, graph.targets(1)[2]) << "Expected correct edge weight";
    EXPECT_EQ(2, graph.vertices().size()) << "Expected correct number of edges";

    // source and target vertices exist, zero weight.
    EXPECT_EQ(10, graph.set(1, 2, 0)) << "Expected removing existing edge returns zero";
    EXPECT_EQ(0, graph.targets(1).size()) << "Expected correct number of edges ";

    // negative weight.
    EXPECT_THROW(graph.set(1, 2, -1), std::domain_error);
}

TEST_F(ConcreteEdgesGraphIntTest, RemoveTest) {
    /**
     * Testing strategy:
     * partition on @param vertex:
     *      - exist in the graph
     *      - does not exist in the graph
     * 
     * partition on edges with vertex as one of their endpoints:
     *      - exist in the graph
     *      - does not exist
     */

     // vertex does not exist in the graph, edge does not exist in the graph.
    EXPECT_FALSE(graph.remove(1)) << "Expected removing non existing vertex returns false";

    
    // vertex exist in the graph, edge exist in the graph.
    graph.set(1, 2 , 10);
    EXPECT_TRUE(graph.remove(2)) << "Expected removing existing vertex returns true";
    EXPECT_EQ(1, graph.vertices().size()) << "Expected correct number of vertices";
    EXPECT_EQ(0, graph.targets(1).size()) << "Expected correct number of edges";
}

TEST_F(ConcreteEdgesGraphIntTest, VerticesTest) {
    /**
     * Testing strategy:
     * partition on vertices:
     *      - empty
     *      - non empty
     * 
     * partition on previous condition:
     *      - no removed vertex
     *      - some removed vertex
     */

    // empty vertices, no removed vertex
    EXPECT_EQ(0, graph.vertices().size()) << "Expected correct number of vertices";

    // non empty vertices, some removed vertex
    graph.add(1);
    graph.add(2);
    graph.add(3);
    graph.add(4);
    graph.add(5);
    graph.remove(2);
    EXPECT_EQ(4, graph.vertices().size()) << "Expected correct number of vertices";
    for (int i = 1; i < 6; i++) {
        if (i == 2) continue;
        EXPECT_TRUE(graph.vertices().find(i) != graph.vertices().end()) << "Expected correct vertices in graph";
    }
}

TEST_F(ConcreteEdgesGraphIntTest, SourcesTest) {
    /**
     * Testing strategy:
     * partition on source vertex:
     *      - vertex exist
     *      - vertex does not exist
     * 
     * partition on edges with vertex as source:
     *      - empty
     *      - non empty, no removed edges
     *      - non empty, some removed edges
     */
    
    // source vertex does not exist
    EXPECT_EQ(0, graph.sources(1).size()) << "Expected to return empty map if source vertex does not exist";
    
    // empty edges, no removed edges
    graph.add(1);
    EXPECT_EQ(0, graph.sources(1).size()) << "Expected correct number of edges";

    // non empty edges, no removed edges
    for (int i = 2; i < 10; i++) {
        graph.set(i, 1, i);
    }
    EXPECT_EQ(8, graph.sources(1).size()) << "Expected correct number of edges";
    for (int i = 2; i < 10; i++) {
        EXPECT_EQ(i, graph.sources(1)[i]) << "Expected correct weight of edge";
    }

    // non empty edges, some removed edges
    graph.set(3, 1, 0);
    graph.set(9, 1, 0);
    EXPECT_EQ(6, graph.sources(1).size()) << "Expected correct number of edges";
    for (int i = 2; i < 10; i++) {
        if (i == 3 || i == 9) continue;
        EXPECT_EQ(i, graph.sources(1)[i]) << "Expected correct weight of edge";
    }
}

TEST_F(ConcreteEdgesGraphIntTest, TargetTest) {
    /**
     * Testing strategy:
     * partition on target vertex:
     *      - vertex exist
     *      - vertex does not exist
     * 
     * partition on edges with vertex as target:
     *      - empty
     *      - non empty, no removed edges
     *      - non empty, some removed edges
     */

    // target vertex does not exist
    EXPECT_EQ(0, graph.targets(1).size()) << "Expected to return empty map if target vertex does not exist";
    
    // empty edges, no removed edges
    graph.add(1);
    EXPECT_EQ(0, graph.targets(1).size()) << "Expected correct number of edges";

    // non empty edges, no removed edges
    for (int i = 2; i < 10; i++) {
        graph.set(1, i, i);
    }
    EXPECT_EQ(8, graph.targets(1).size()) << "Expected correct number of edges";
    for (int i = 2; i < 10; i++) {
        EXPECT_EQ(i, graph.targets(1)[i]) << "Expected correct weight of edge";
    }

    // non empty edges, some removed edges
    graph.set(1, 3, 0);
    graph.set(1, 9, 0);
    EXPECT_EQ(6, graph.targets(1).size()) << "Expected correct number of edges";
    for (int i = 2; i < 10; i++) {
        if (i == 3 || i == 9) continue;
        EXPECT_EQ(i, graph.targets(1)[i]) << "Expected correct weight of edge";
    }
}
}