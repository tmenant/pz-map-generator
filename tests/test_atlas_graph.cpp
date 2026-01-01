#include "core/atlas_graph.h"
#include <doctest/doctest.h>
#include <fmt/base.h>

TEST_CASE("Atlas Datas contains")
{
    AtlasGraph::Node atlas1;
    AtlasGraph::Node atlas2;

    SUBCASE("Simple subsequence")
    {
        atlas1.hashes = { 0, 1, 2, 3 };
        atlas2.hashes = { 1, 3 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Identical subsequence")
    {
        atlas1.hashes = { 5, 6, 7 };
        atlas2.hashes = { 5, 6, 7 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Single-element subsequence")
    {
        atlas1.hashes = { 10, 20, 30 };
        atlas2.hashes = { 20 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Incorrect order")
    {
        atlas1.hashes = { 1, 2, 3 };
        atlas2.hashes = { 3, 2 };

        CHECK_FALSE(atlas1.contains(&atlas2));
    }

    SUBCASE("Missing element")
    {
        atlas1.hashes = { 1, 2, 3 };
        atlas2.hashes = { 2, 4 };

        CHECK_FALSE(atlas1.contains(&atlas2));
    }

    SUBCASE("Subsequence longer than source")
    {
        atlas1.hashes = { 1, 2 };
        atlas2.hashes = { 1, 2, 3 };

        CHECK_FALSE(atlas1.contains(&atlas2));
    }

    SUBCASE("Non-contiguous subsequence")
    {
        atlas1.hashes = { 1, 4, 7, 9, 12 };
        atlas2.hashes = { 4, 9, 12 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Duplicate handling")
    {
        atlas1.hashes = { 1, 2, 2, 3 };
        atlas2.hashes = { 2, 3 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Insufficient duplicates")
    {
        atlas1.hashes = { 1, 2, 3 };
        atlas2.hashes = { 2, 2 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Empty subsequence")
    {
        atlas1.hashes = { 1, 2, 3 };
        atlas2.hashes = {};

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Empty source")
    {
        atlas1.hashes = {};
        atlas2.hashes = { 1 };

        CHECK_FALSE(atlas1.contains(&atlas2));
    }
}

TEST_SUITE("Atlas Graph")
{
    TEST_CASE("single root with multiple branches and orphans")
    {
        /*
             [0]    [5]
            /   \
           [1]  [4]
          /   \
        [2]   [3]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasGraph::Node({ 0, 1, 2, 3 }));
        graph.addNode(1, AtlasGraph::Node({ 0, 1, 2 }));
        graph.addNode(2, AtlasGraph::Node({ 2 }));
        graph.addNode(3, AtlasGraph::Node({ 0, 1 }));
        graph.addNode(4, AtlasGraph::Node({ 3 }));
        graph.addNode(5, AtlasGraph::Node({ 4 }));
        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(3)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(4)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(5)->parent == nullptr);

        CHECK(graph.getRootNode(graph.getNodeById(0)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(1)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(2)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(3)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(4)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(5)) == graph.getNodeById(5));
    }

    TEST_CASE("linear chain")
    {
        /*
        [0]
         └── [1]
              └── [2]
                   └── [3]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasGraph::Node({ 0, 1, 2, 3 }));
        graph.addNode(1, AtlasGraph::Node({ 0, 1, 2 }));
        graph.addNode(2, AtlasGraph::Node({ 0, 1 }));
        graph.addNode(3, AtlasGraph::Node({ 0 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(3)->parent == graph.getNodeById(2));

        CHECK(graph.getRootNode(graph.getNodeById(0)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(1)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(2)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(3)) == graph.getNodeById(0));
    }

    TEST_CASE("multiple roots")
    {
        /*
        [0]           [3]
         └── [1]       └── [4]
              └── [2]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasGraph::Node({ 0, 1, 2 }));
        graph.addNode(1, AtlasGraph::Node({ 0, 1 }));
        graph.addNode(2, AtlasGraph::Node({ 0 }));

        graph.addNode(3, AtlasGraph::Node({ 3, 4 }));
        graph.addNode(4, AtlasGraph::Node({ 3 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(3)->parent == nullptr);
        CHECK(graph.getNodeById(4)->parent == graph.getNodeById(3));

        CHECK(graph.getRootNode(graph.getNodeById(0)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(1)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(2)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(3)) == graph.getNodeById(3));
        CHECK(graph.getRootNode(graph.getNodeById(4)) == graph.getNodeById(3));
    }

    TEST_CASE("branching")
    {
        /*
             [0]
            /   \
          [1]   [2]
          /       \
        [3]       [4]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasGraph::Node({ 0, 1, 2, 3, 4 }));
        graph.addNode(1, AtlasGraph::Node({ 0, 1, 2 }));
        graph.addNode(2, AtlasGraph::Node({ 3, 4 }));
        graph.addNode(3, AtlasGraph::Node({ 0, 1 }));
        graph.addNode(4, AtlasGraph::Node({ 3 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(3)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(4)->parent == graph.getNodeById(2));

        CHECK(graph.getRootNode(graph.getNodeById(0)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(1)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(2)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(3)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(4)) == graph.getNodeById(0));
    }

    TEST_CASE("same size no inclusion")
    {
        /*
            [0]     [1]     [2]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasGraph::Node({ 0, 1 }));
        graph.addNode(1, AtlasGraph::Node({ 2, 3 }));
        graph.addNode(2, AtlasGraph::Node({ 4, 5 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == nullptr);
        CHECK(graph.getNodeById(2)->parent == nullptr);

        CHECK(graph.getRootNode(graph.getNodeById(0)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(1)) == graph.getNodeById(1));
        CHECK(graph.getRootNode(graph.getNodeById(2)) == graph.getNodeById(2));
    }

    TEST_CASE("orphan leaf")
    {
        /*
        [0]  [2]
         |
        [1]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasGraph::Node({ 0, 1 }));
        graph.addNode(1, AtlasGraph::Node({ 0 }));
        graph.addNode(2, AtlasGraph::Node({ 9 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == nullptr);

        CHECK(graph.getRootNode(graph.getNodeById(0)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(1)) == graph.getNodeById(0));
        CHECK(graph.getRootNode(graph.getNodeById(2)) == graph.getNodeById(2));
    }
}
