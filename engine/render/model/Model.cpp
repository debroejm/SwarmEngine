#include "../../Render.h"

#include "../../Core.h"

#include <set>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Model {

        std::set<GLuint> registeredVAOs;
        std::set<GLuint> registeredBuffers;

        void cleanupBuffers() {

            // Cleanup Buffers
            for(GLuint buffer : registeredBuffers) {
                Log::log_render(INFO) << "Deleting Buffer [" << buffer << "]";
                glDeleteBuffers(1, &buffer);
            }

            // Cleanup VAOs
            for(GLuint vao : registeredVAOs) {
                Log::log_render(INFO) << "Deleting VAO [" << vao << "]";
                glDeleteVertexArrays(1, &vao);
            }

        }

        ModelSegment::ModelSegment(RawModelDataIndexed &data) {
            genBuffers(data);
        }

        void ModelSegment::genBuffers(RawModelDataIndexed &data) {
            if(loaded) cleanup();

            glGenVertexArrays(1, &vao);
            registeredVAOs.insert(vao);
            glBindVertexArray(vao);

            unordered_map<DataType::Type, VecArray> const &dataMap = data.getData();

            // Create Data Buffers
            for(auto && iter : dataMap) {
                GLuint bufferID;
                glGenBuffers(1, &bufferID);
                buffers.insert(bufferID);
                registeredBuffers.insert(bufferID);
                glBindBuffer(GL_ARRAY_BUFFER, bufferID);
                switch(iter.first.getType()) {
                    case ONE:
                        glBufferData(GL_ARRAY_BUFFER, data.getSize() * sizeof(float), &iter.second.v1[0], GL_STATIC_DRAW);
                        break;
                    case TWO:
                        glBufferData(GL_ARRAY_BUFFER, data.getSize() * sizeof(glm::vec2), &iter.second.v2[0], GL_STATIC_DRAW);
                        break;
                    case THREE:
                        glBufferData(GL_ARRAY_BUFFER, data.getSize() * sizeof(glm::vec3), &iter.second.v3[0], GL_STATIC_DRAW);
                        break;
                    case FOUR:
                        glBufferData(GL_ARRAY_BUFFER, data.getSize() * sizeof(glm::vec4), &iter.second.v4[0], GL_STATIC_DRAW);
                        break;
                    default: break;
                }

                glEnableVertexAttribArray(iter.first.getAttribID());
                glVertexAttribPointer(
                        iter.first.getAttribID(),
                        (int)iter.first.getType(),
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        (void*)0
                );
            }

            // Create Index Buffer
            GLuint eBufferID;
            glGenBuffers(1, &eBufferID);
            buffers.insert(eBufferID);
            registeredBuffers.insert(eBufferID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eBufferID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.getIndexCount() * sizeof(unsigned short), &data.getIndices()[0], GL_STATIC_DRAW);

            elementCount = data.getIndexCount();

            Log::log_render(INFO) << "Model Created [Buffers: ";
            for(GLuint ID : buffers) Log::log_render << ID << ", ";
            Log::log_render << "VAO: " << vao << ", ElementCount: " << elementCount << "]";

            loaded = true;
        }

        ModelSegment::ModelSegment(const ModelSegment &other) {
            *this = other;
        }

        ModelSegment &ModelSegment::operator=(const ModelSegment &other) {
            vao = other.vao;
            buffers = other.buffers;
            elementCount = other.elementCount;
            return *this;
        }


        void ModelSegment::cleanup() {

            // Cleanup Buffers
            for(GLuint buffer : buffers) {
                Log::log_render(INFO) << "Deleting Buffer [" << buffer << "]";
                registeredBuffers.erase(buffer);
                glDeleteBuffers(1, &buffer);
            }

            // Cleanup VAO
            Log::log_render(INFO) << "Deleting VAO [" << vao << "]";
            registeredVAOs.erase(vao);
            glDeleteVertexArrays(1, &vao);

            buffers.clear();
            loaded = false;

        }


    }
}