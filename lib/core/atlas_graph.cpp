#include "atlas_graph.h"

#include <algorithm>
#include <stdexcept>

bool AtlasGraph::Node::contains(Node *other) const
{
    if (other->hashes.size() > hashes.size())
        return false;

    int thisIndex = 0;
    int otherIndex = 0;

    for (; otherIndex < other->hashes.size(); otherIndex++)
    {
        bool found = false;

        for (; thisIndex < hashes.size(); thisIndex++)
        {
            if (hashes[thisIndex] == other->hashes[otherIndex])
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }
    }

    return true;
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

    for (int i = 0; i < sortedNodes.size(); i++)
    {
        for (int j = i + 1; j < sortedNodes.size(); j++)
        {
            assert(sortedNodes[i]->hashes.size() <= sortedNodes[j]->hashes.size());

            if (sortedNodes[j]->contains(sortedNodes[i]))
            {
                sortedNodes[i]->parent = sortedNodes[j];
                break;
            }
        }
    }
}