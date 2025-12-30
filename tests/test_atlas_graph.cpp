#include "core/atlas_datas.h"
#include <doctest/doctest.h>
#include <fmt/base.h>

TEST_CASE("Atlas Datas contains")
{
    AtlasDatas atlas1;
    AtlasDatas atlas2;

    SUBCASE("Sous-sequence simple")
    {
        atlas1.hashes = { 0, 1, 2, 3 };
        atlas2.hashes = { 1, 3 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Sous-sequence identique")
    {
        atlas1.hashes = { 5, 6, 7 };
        atlas2.hashes = { 5, 6, 7 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Sous-sequence avec un seul element")
    {
        atlas1.hashes = { 10, 20, 30 };
        atlas2.hashes = { 20 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Ordre incorrect")
    {
        atlas1.hashes = { 1, 2, 3 };
        atlas2.hashes = { 3, 2 };

        CHECK_FALSE(atlas1.contains(&atlas2));
    }

    SUBCASE("Element manquant")
    {
        atlas1.hashes = { 1, 2, 3 };
        atlas2.hashes = { 2, 4 };

        CHECK_FALSE(atlas1.contains(&atlas2));
    }

    SUBCASE("Sous-sequence plus longue que la source")
    {
        atlas1.hashes = { 1, 2 };
        atlas2.hashes = { 1, 2, 3 };

        CHECK_FALSE(atlas1.contains(&atlas2));
    }

    SUBCASE("Sous-sequence non contigue")
    {
        atlas1.hashes = { 1, 4, 7, 9, 12 };
        atlas2.hashes = { 4, 9, 12 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Gestion des doublons")
    {
        atlas1.hashes = { 1, 2, 2, 3 };
        atlas2.hashes = { 2, 3 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Doublons insuffisants")
    {
        atlas1.hashes = { 1, 2, 3 };
        atlas2.hashes = { 2, 2 };

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Sous-sequence vide")
    {
        atlas1.hashes = { 1, 2, 3 };
        atlas2.hashes = {};

        CHECK(atlas1.contains(&atlas2));
    }

    SUBCASE("Source vide")
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

        graph.addNode(0, AtlasDatas({ 0, 1, 2, 3 }));
        graph.addNode(1, AtlasDatas({ 0, 1, 2 }));
        graph.addNode(2, AtlasDatas({ 2 }));
        graph.addNode(3, AtlasDatas({ 0, 1 }));
        graph.addNode(4, AtlasDatas({ 3 }));
        graph.addNode(5, AtlasDatas({ 4 }));
        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(3)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(4)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(5)->parent == nullptr);
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

        graph.addNode(0, AtlasDatas({ 0, 1, 2, 3 }));
        graph.addNode(1, AtlasDatas({ 0, 1, 2 }));
        graph.addNode(2, AtlasDatas({ 0, 1 }));
        graph.addNode(3, AtlasDatas({ 0 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(3)->parent == graph.getNodeById(2));
    }

    TEST_CASE("multiple roots")
    {
        /*
        [0]           [3]
         └── [1]       └── [4]
              └── [2]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasDatas({ 0, 1, 2 }));
        graph.addNode(1, AtlasDatas({ 0, 1 }));
        graph.addNode(2, AtlasDatas({ 0 }));

        graph.addNode(3, AtlasDatas({ 3, 4 }));
        graph.addNode(4, AtlasDatas({ 3 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(1));

        CHECK(graph.getNodeById(3)->parent == nullptr);
        CHECK(graph.getNodeById(4)->parent == graph.getNodeById(3));
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

        graph.addNode(0, AtlasDatas({ 0, 1, 2, 3, 4 }));
        graph.addNode(1, AtlasDatas({ 0, 1, 2 }));
        graph.addNode(2, AtlasDatas({ 3, 4 }));
        graph.addNode(3, AtlasDatas({ 0, 1 }));
        graph.addNode(4, AtlasDatas({ 3 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);

        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(0));

        CHECK(graph.getNodeById(3)->parent == graph.getNodeById(1));
        CHECK(graph.getNodeById(4)->parent == graph.getNodeById(2));
    }

    TEST_CASE("same size no inclusion")
    {
        /*
            [0]     [1]     [2]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasDatas({ 0, 1 }));
        graph.addNode(1, AtlasDatas({ 2, 3 }));
        graph.addNode(2, AtlasDatas({ 4, 5 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == nullptr);
        CHECK(graph.getNodeById(2)->parent == nullptr);
    }

    TEST_CASE("minimal parent selection")
    {
        /*
        [0]
         └── [1]
              └── [2]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasDatas({ 0, 1, 2, 3 }));
        graph.addNode(1, AtlasDatas({ 0, 1, 2 }));
        graph.addNode(2, AtlasDatas({ 0, 1 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == graph.getNodeById(1));
    }

    TEST_CASE("orphan leaf")
    {
        /*
        [0]  [2]
         |
        [1]
        */
        AtlasGraph graph;

        graph.addNode(0, AtlasDatas({ 0, 1 }));
        graph.addNode(1, AtlasDatas({ 0 }));
        graph.addNode(2, AtlasDatas({ 9 }));

        graph.buildGraph();

        CHECK(graph.getNodeById(0)->parent == nullptr);
        CHECK(graph.getNodeById(1)->parent == graph.getNodeById(0));
        CHECK(graph.getNodeById(2)->parent == nullptr);
    }
}
