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
    auto it = atlasNodes.find(id);

    if (it == atlasNodes.end())
    {
        throw std::runtime_error("id not found: " + std::to_string(id));
    }

    return &it->second;
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

void AtlasGraph::addNode(uint32_t id, AtlasGraph::Node atlasData)
{
    std::sort(atlasData.hashes.begin(), atlasData.hashes.end());
    atlasNodes[id] = atlasData;
}

void AtlasGraph::buildGraph()
{
    std::vector<AtlasGraph::Node *> nodes;
    nodes.reserve(atlasNodes.size());

    for (auto &[id, node] : atlasNodes)
    {
        nodes.push_back(&node);
    }

    std::sort(nodes.begin(), nodes.end(), [](AtlasGraph::Node *a, AtlasGraph::Node *b)
    {
        assert(a != nullptr);
        assert(b != nullptr);

        return a->hashes.size() < b->hashes.size();
    });

    assert(nodes.size() > 1);

    for (int i = 0; i < nodes.size(); i++)
    {
        for (int j = i + 1; j < nodes.size(); j++)
        {
            assert(nodes[i]->hashes.size() <= nodes[j]->hashes.size());

            if (nodes[j]->contains(nodes[i]))
            {
                nodes[i]->parent = nodes[j];
                break;
            }
        }
    }
}