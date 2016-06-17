#include "Terrain.h"

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_WORLD {

        TerrainNode::TerrainNode(float x, float y, float z)
                : x(x), y(y), z(z) {

        }

        TerrainNode::~TerrainNode() {

        }

        void TerrainNode::addNeighbor(TerrainNode* neighbor) {
            for(TerrainNode* node : neighbors) {
                if(node == neighbor) return;
            }
            neighbors.push_back(neighbor);
        }

        void TerrainNode::setNodeBone(ENGINE_NAMESPACE_MODEL::Bone* bone) {
            nodeBone = bone;
        }

        void TerrainNode::setHeight(float height) {
            y = height;
            if(nodeBone != NULL) {
                glm::vec3 oldPos = nodeBone->getPosition();
                oldPos.y = height;
                nodeBone->setPosition(oldPos);
            }
        }

        TerrainNode &TerrainNode::operator=(TerrainNode &rhs) {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            nodeBone = rhs.nodeBone;
        }



        TerrainMesh::TerrainMesh(unsigned int complexity)
                : complexity(complexity) {

        }

        TerrainMesh::TerrainMesh(TerrainNode node, unsigned int complexity)
                : complexity(complexity) {
            addNode(node);
        }

        TerrainMesh::~TerrainMesh() {

        }

        void TerrainMesh::addNode(TerrainNode node) {
            nodes.push_back(node);
        }

        void TerrainMesh::linkNodes(unsigned int idA, unsigned int idB) {
            if(idA >= nodes.size() || idB >= nodes.size()) return;
            nodes[idA].addNeighbor(&nodes[idB]);
            nodes[idB].addNeighbor(&nodes[idA]);
        }

        void TerrainMesh::compileMesh() {

        }
    }
}