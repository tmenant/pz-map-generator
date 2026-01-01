#include "atlas_graph.h"

#include <algorithm>
#include <stdexcept>

bool AtlasGraph::Node::contains(Node *other) const
{
    if (other->hashes.size() > hashes.size())
        return false;

    return std::includes(hashes.begin(), hashes.end(), other->hashes.begin(), other->hashes.end());
}

AtlasGraph::Node *AtlasGraph::getNodeById(uint32_t id)
{
    auto it = nodesById.find(id);
    if (it == nodesById.end())
    {
        throw std::runtime_error("id not found: " + std::to_string(id));
    }

    return it->second;
}

AtlasGraph::Node *AtlasGraph::getRootNode(Node *currentNode)
{
    assert(currentNode != nullptr);

    if (currentNode->parent == nullptr)
    {
        return currentNode;
    }

    return getRootNode(currentNode->parent);
}

AtlasGraph::Node *AtlasGraph::getRootNode(uint32_t childId)
{
    auto currentNode = getNodeById(childId);
    return getRootNode(currentNode);
}

void AtlasGraph::addNode(uint32_t id, AtlasGraph::Node &&atlasData)
{
    std::sort(atlasData.hashes.begin(), atlasData.hashes.end());

    // remove duplicate hashes efficiently
    auto last = std::unique(atlasData.hashes.begin(), atlasData.hashes.end());
    atlasData.hashes.erase(last, atlasData.hashes.end());

    nodes.emplace_back(std::move(atlasData));
    nodesById[id] = &nodes.back();
}

void AtlasGraph::buildGraph()
{
    std::vector<AtlasGraph::Node *> sortedNodes;
    sortedNodes.reserve(nodesById.size());

    for (auto &[id, node] : nodesById)
    {
        sortedNodes.push_back(node);
    }

    std::sort(sortedNodes.begin(), sortedNodes.end(), [](AtlasGraph::Node *a, AtlasGraph::Node *b)
    {
        assert(a != nullptr);
        assert(b != nullptr);

        return a->hashes.size() < b->hashes.size();
    });

    assert(sortedNodes.size() > 1);

    // mapping each hash to his parent node
    std::unordered_map<uint32_t, std::vector<Node *>> reverseIndex;
    for (Node *node : sortedNodes)
    {
        for (uint32_t h : node->hashes)
        {
            reverseIndex[h].push_back(node);
        }
    }

    for (Node *current : sortedNodes)
    {
        if (current->hashes.empty()) continue;

        uint32_t firstNodeHash = current->hashes[0];

        // testing only nodes which contain at least the first hash of the current node
        const auto &candidates = reverseIndex[firstNodeHash];

        for (Node *potentialParent : candidates)
        {
            if (potentialParent->hashes.size() <= current->hashes.size())
                continue;

            if (potentialParent->contains(current))
            {
                current->parent = potentialParent;
                break;
            }
        }
    }
}