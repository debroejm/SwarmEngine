#include "../../Render.h"

#include <glm/gtx/transform.hpp>

namespace Swarm {
    namespace Model {

        Bone::Bone(glm::vec3 pos, string name, Bone* parent) {
            originalPos = pos;
            position = pos;
            this->name = name;
            this->parent = parent;
        }

        Bone::Bone(Bone &bone) {
            *this = bone;
        }

        Bone::Bone(Bone &bone, string name) {
            *this = bone;
            this->name = name;
        }

        void Bone::revertPosition()
        {
            position = originalPos;
        }

        void Bone::addPosition(glm::vec3 newPos) { setPosition(position+newPos); }
        void Bone::setPosition(glm::vec3 newPos) { position = newPos; }
        void Bone::rotatePosition(float angle, glm::vec3 amount) { rotatePosition(glm::rotate(angle, amount)); }
        void Bone::rotatePosition(glm::mat4 rotMatrix) {
            rotationMatrix = rotMatrix;
        }

        void Bone::updateBoneBuffer() {
            glm::vec3 totalPos = getPosition();
            for(int i = 0; i < bonePositions.size(); i++)
            {
                bonePositions[i]->operator=(totalPos);
            }
            glm::mat4 rotMat = getRotationMatrix();
            for(int i = 0; i < normals.size(); i++) {
                (*normals[i]) = glm::vec3(rotMat * glm::vec4((*normalsStatic[i]),1.0f));
            }
        }

        void Bone::addChild(Bone &child) {
            children.push_back(&child);
        }

        void Bone::addBonePosition(glm::vec3 &bonePos) {
            bonePositions.push_back(&bonePos);
        }

        void Bone::addNormal(glm::vec3 &normal) {
            normals.push_back(&normal);
        }

        void Bone::addNormal(glm::vec3 &normal, glm::vec3 &normalStatic) {
            normals.push_back(&normal);
            normalsStatic.push_back(&normalStatic);
        }

        glm::vec3 Bone::getPosition() {
            if(parent == NULL) return glm::vec3(getRotationMatrix() * glm::vec4(position, 1.0f));
            else return glm::vec3(getRotationMatrix() * glm::vec4(position, 1.0f)) + parent->getPosition();
        }

        glm::vec3 Bone::getRelativePosition() {
            return position;
        }

        glm::mat4 Bone::getRotationMatrix() {
            if(parent == NULL) return rotationMatrix;
            else return rotationMatrix * parent->getRotationMatrix();
        }

        void Bone::operator =(const Bone &rhs)
        {
            position = rhs.position;
            originalPos = position;
            normalsStatic = rhs.normalsStatic;
            name = rhs.name;
            parent = NULL;
            children.clear();
        }
    }
}