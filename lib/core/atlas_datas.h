#pragma once

#include <fmt/base.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

struct AtlasDatas
{
    std::vector<uint32_t> hashes;
    AtlasDatas *parent = nullptr;

    AtlasDatas() = default;
    AtlasDatas(std::vector<uint32_t> hashes)
    {
        this->hashes = hashes;
    }

    bool contains(AtlasDatas *other) const
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
};

class AtlasGraph
{
private:
    std::unordered_map<uint32_t, AtlasDatas> atlasNodes;

public:
    AtlasGraph() = default;

    AtlasDatas *getNodeById(uint32_t id)
    {
        auto it = atlasNodes.find(id);
        return it != atlasNodes.end() ? &it->second : nullptr;
    }

    size_t size()
    {
        return atlasNodes.size();
    }

    void addNode(uint32_t id, AtlasDatas atlasData)
    {
        std::sort(atlasData.hashes.begin(), atlasData.hashes.end());
        atlasNodes[id] = atlasData;
    }

    void buildGraph()
    {
        std::vector<AtlasDatas *> nodes;
        nodes.reserve(atlasNodes.size());

        for (auto &[id, node] : atlasNodes)
        {
            nodes.push_back(&node);
        }

        std::sort(nodes.begin(), nodes.end(), [](AtlasDatas *a, AtlasDatas *b)
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
};