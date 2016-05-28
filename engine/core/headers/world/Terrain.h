#pragma once

#include "../../Definitions.h"
#include "../ModelLoading.h"

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_WORLD {
        class TerrainNode {
        public:
            TerrainNode(float x, float y, float z);
            ~TerrainNode();

            void addNeighbor(TerrainNode* neighbor);
            void setNodeBone(ENGINE_NAMESPACE_MODEL::Bone* bone);

            void setHeight(float height);

            TerrainNode &operator=(TerrainNode &rhs);

        protected:
            float x, y, z;
            std::vector<TerrainNode*> neighbors;
            ENGINE_NAMESPACE_MODEL::Bone* nodeBone;
        };

        class TerrainMesh {
        public:
            TerrainMesh(unsigned int complexity = 1);
            TerrainMesh(TerrainNode node, unsigned int complexity = 1);
            ~TerrainMesh();

            void addNode(TerrainNode node);
            void linkNodes(unsigned int idA, unsigned int idB);

            void compileMesh();

        protected:
            std::vector<TerrainNode> nodes;
            unsigned int complexity;
            ENGINE_NAMESPACE_MODEL::Model mesh;
        };
    }
}

