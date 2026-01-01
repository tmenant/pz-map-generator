#pragma once

#include <fmt/base.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

class AtlasGraph
{
public:
    struct Node
    {
        std::vector<uint32_t> hashes;
        Node *parent = nullptr;

        Node() = default;
        Node(std::vector<uint32_t> _hashes) : hashes(std::move(_hashes)) {}

        bool contains(Node *other) const;
    };

private:
    std::unordered_map<uint32_t, Node> atlasNodes;

public:
    AtlasGraph() = default;

    inline size_t size() const { return atlasNodes.size(); }

    Node *getNodeById(uint32_t id);
    Node *getRootNode(Node *currentNode);
    Node *getRootNode(uint32_t childId);

    void addNode(uint32_t id, Node atlasData);
    void buildGraph();
};