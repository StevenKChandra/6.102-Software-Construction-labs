#include "gtest/gtest.h"

#include <unistd.h>
#include <linux/limits.h>

#include "graph_poet.h"
#include "concrete_vertices_graph.h"

namespace {

TEST(GraphPoetTest, ConstructorTest) {
    std::string relativePath{"/../test/poet/alphabet.txt"};
    ASSERT_NO_THROW(GraphPoet(new ConcreteVerticesGraph<std::string>(), relativePath, false)) << "Constructor must work with relative path";
    
    char buffer[PATH_MAX];
    getcwd(buffer, PATH_MAX);
    std::string absolutePath{buffer};
    absolutePath = absolutePath.append(relativePath);
    ASSERT_NO_THROW(GraphPoet(new ConcreteVerticesGraph<std::string>(), absolutePath, true)) << "Constructor must work with absolute path.";

    ASSERT_THROW(GraphPoet(new ConcreteVerticesGraph<std::string>(), "asdf"), std::runtime_error) << "Expected constructor fail if file does not exist.";
}

TEST(GraphPoetTest, PoemTest) {
    /**
     * Testing strategy:
     * partition on number of input words:
     *      - zero
     *      - one
     *      - two
     *      - more than two
     * 
     * partition on input word cases:
     *      - all lower
     *      - mixed
     *      - all upper
     * 
     * partition bridge options:
     *      - no bridge exist
     *      - single bridge edge exist
     *      - multiple bridge exist 
     */
    
    GraphPoet graphPoet = GraphPoet(new ConcreteVerticesGraph<std::string>(), "/../test/poet/alphabet.txt");

    // empty input
    EXPECT_EQ("", graphPoet.poem("")) << "Expected empty string when input is empty";
    
    // one word input
    EXPECT_EQ("a", graphPoet.poem("a")) << "Expected same string when input is only one word";

    // two word input, all lower case, single bridge exist
    EXPECT_EQ("a b c", graphPoet.poem("a c")) << "Expected correct string";
    
    // more than two word input, all lower case, no bridge exist
    EXPECT_EQ("a d g", graphPoet.poem("a d g")) << "Expected same string when no bridge exist";

    // more than two word input, mixed case, single bridge exist
    EXPECT_EQ("a b C d e f G", graphPoet.poem("a C e G")) << "Expected correct string";

    graphPoet = GraphPoet(new ConcreteVerticesGraph<std::string>(), "/../test/poet/multi-edge-test.txt");

    graphPoet.poem("");
    // more than two edges, all upper case, multiple bridge exist
    EXPECT_EQ("FOO bar FOOBAR bar FOO", graphPoet.poem("FOO FOOBAR FOO")) << "Expected correct string";
}
}