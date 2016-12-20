#include "../../Render.h"

#include "../../Core.h"

#include <set>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Model {

        bool Model::operator==(Model &rhs) {
            return getVAOID() == rhs.getVAOID();
        }

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

            Render::Window* window = Render::Window::getCurrent();
            if(window == nullptr) {
                Log::log_render(ERR) << "Trying to create model data without a GL Context being bound!";
                return;
            }

            GLuint vao;
            glGenVertexArrays(1, &vao);
            registeredVAOs.insert(vao);
            glBindVertexArray(vao);

            unordered_map<DataType::Type, VecArray> const &dataMap = data.getData();

            // Create Data Buffers
            for(auto && iter : dataMap) {
                GLuint bufferID;
                glGenBuffers(1, &bufferID);
                data_buffers.insert(BufferEntry{ bufferID, iter.first.getAttribID(), iter.first.getType() });
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
            glGenBuffers(1, &element_buffer);
            registeredBuffers.insert(element_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.getIndexCount() * sizeof(unsigned short), &data.getIndices()[0], GL_STATIC_DRAW);

            glBindVertexArray(0);

            vao_map[window->getWindow()] = vao;
            elementCount = data.getIndexCount();

            Log::log_render(INFO) << "Model Created [Buffers: ";
            for(BufferEntry entry : data_buffers) Log::log_render << entry.buffer << ", ";
            Log::log_render << "Initial VAO: " << vao << ", ElementCount: " << elementCount << "]";

            loaded = true;
        }

        void ModelSegment::regenVAO() {
            Render::Window* window = Render::Window::getCurrent();
            if(window == nullptr) return; // Safety check

            // Delete the old VAO if it exists
            if(vao_map.count(window->getWindow())) {
                GLuint old_vao = vao_map.at(window->getWindow());
                Log::log_render(INFO) << "Deleting Old VAO [" << old_vao << "]";
                registeredVAOs.erase(old_vao);
                glDeleteVertexArrays(1, &old_vao);
            }

            // Generate new VAO
            GLuint vao;
            glGenVertexArrays(1, &vao);
            registeredVAOs.insert(vao);
            glBindVertexArray(vao);

            // Bind the Data Buffers
            for(BufferEntry entry : data_buffers) {
                glBindBuffer(GL_ARRAY_BUFFER, entry.buffer);
                glEnableVertexAttribArray(entry.attrib);
                glVertexAttribPointer(entry.attrib, (int)entry.type, GL_FLOAT, GL_FALSE, 0, (void*)0);
            }

            // Bind the Element Buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

            // Unbind new VAO
            glBindVertexArray(0);

            vao_map[window->getWindow()] = vao;
            Log::log_render(INFO) << "Created new VAO on request: " << vao;
        }

        ModelSegment::ModelSegment(const ModelSegment &other) {
            *this = other;
        }

        ModelSegment &ModelSegment::operator=(const ModelSegment &other) {
            vao_map = other.vao_map;
            data_buffers = other.data_buffers;
            elementCount = other.elementCount;
            return *this;
        }

        GLint ModelSegment::getVAOID() {
            Render::Window* window = Render::Window::getCurrent();
            if(window == nullptr) return -1; // Safety check
            if(vao_map.count(window->getWindow())) return vao_map.at(window->getWindow());
            else {
                regenVAO();
                if(vao_map.count(window->getWindow())) return vao_map.at(window->getWindow());
                else return -1; // Shouldn't ever happen, but just in case regenVAO fails
            }
        }


        void ModelSegment::cleanup() {

            // Cleanup Buffers
            for(BufferEntry entry : data_buffers) {
                Log::log_render(INFO) << "Deleting Buffer [" << entry.buffer << "]";
                registeredBuffers.erase(entry.buffer);
                glDeleteBuffers(1, &entry.buffer);
            }

            // Cleanup VAO
            for(auto && iter : vao_map) {
                Log::log_render(INFO) << "Deleting VAO [" << iter.second << "]";
                registeredVAOs.erase(iter.second);
                glDeleteVertexArrays(1, &iter.second);
            }

            data_buffers.clear();
            loaded = false;

        }


    }
}