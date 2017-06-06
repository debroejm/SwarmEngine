#define SWARM_INCLUDE_GLFW
#include "render/RenderInternal.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Model {

        // Mapping: GLFWwindow -> Element Buffer ID -> VAO ID
        std::unordered_map<GLFWwindow*, std::unordered_map<GLuint, GLuint>> static_vao_map;

        std::set<GLuint> registeredVAOs;
        std::set<GLuint> registeredBuffers;

        void cleanup() {
            Model::cleanup();
        }

        void Model::cleanup() {

            // Cleanup Buffers
            for(GLuint buffer : registeredBuffers) {
                Render::log_render(DEBUG) << "Deleting GL Buffer [" << buffer << "]";
                glDeleteBuffers(1, &buffer);
            }
            Render::log_render().flush();

            // Cleanup VAOs
            for(GLuint vao : registeredVAOs) {
                Render::log_render(DEBUG) << "Deleting VAO [" << vao << "]" << Flush();
                glDeleteVertexArrays(1, &vao);
            }
            Render::log_render().flush();

        }

        Model::Model(const RawModelDataIndexed &data) {
            genBuffers(data);
        }

        Model::Model(const Model &other) {
            operator=(other);
        }

        Model &Model::operator=(const Model &other) {
            _loaded = other._loaded;
            _element_count = other._element_count;
            _element_buffer = other._element_buffer;
            _data_buffers = other._data_buffers;
            return *this;
        }

        SWMuint Model::vao() {
            GLFWwindow* window = glfwGetCurrentContext();
            if(window == nullptr || !_loaded) return 0; // Safety check
            if(static_vao_map[window].count(_element_buffer) < 1) genVAO();
            return static_vao_map[window][_element_buffer];
        }

        bool Model::operator==(const Model &rhs) const {
            // Assumes Models with same Element Buffer will have same other Buffers
            return _element_buffer == rhs._element_buffer;
        }

        bool Model::operator<(const Model &rhs) const {
            // Assumes Models with same Element Buffer will have same other Buffers
            return _element_buffer < rhs._element_buffer;
        }

        void Model::genBuffers(const RawModelDataIndexed &data) {

            // Create Data Context VAO
            GLuint vao;
            glGenVertexArrays(1, &vao);
            registeredVAOs.insert(vao);
            glBindVertexArray(vao);

            // Create Data Buffers
            for(auto && iter : data) {
                GLuint bufferID;
                glGenBuffers(1, &bufferID);
                _data_buffers.insert(BufferEntry{ bufferID, iter.first.attribID(), iter.first.type() });
                registeredBuffers.insert(bufferID);
                glBindBuffer(GL_ARRAY_BUFFER, bufferID);
                size_t size = data.size();
                unsigned int stride = iter.first.type();
                float fdata[ size * stride ];
                for(size_t i = 0; i < size; i++) {
                    for(unsigned int j = 0; j < stride; j++) {
                        fdata[i*stride+j] = j < 2 ?
                                            j == 0 ? iter.second.at(i).val.v4.x
                                                   : iter.second.at(i).val.v4.y :
                                            j == 2 ? iter.second.at(i).val.v4.z
                                                   : iter.second.at(i).val.v4.w ;

                    }
                }

                glBufferData(GL_ARRAY_BUFFER, size * stride * sizeof(float), &fdata[0], GL_STATIC_DRAW);
                glEnableVertexAttribArray(iter.first.attribID());
                glVertexAttribPointer(
                        iter.first.attribID(),
                        (int)iter.first.type(),
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        (void*)0
                );
            }

            // Create Index Buffer
            glGenBuffers(1, &_element_buffer);
            registeredBuffers.insert(_element_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indexSize() * sizeof(unsigned int), &data.indices()[0], GL_STATIC_DRAW);

            _element_count = data.indexSize();

            static_vao_map[glfwGetCurrentContext()][_element_buffer] = vao;
            glBindVertexArray(0);

            glfwSwapBuffers(glfwGetCurrentContext());

            Render::log_render(DEBUG) << "Model Created [Buffers: ";
            for(BufferEntry entry : _data_buffers) Render::log_render() << entry.buffer << ", ";
            Render::log_render() << "ElementCount: " << _element_count << "]";
            Render::log_render().flush();

            _loaded = true;
        }

        void Model::genVAO() {

            // Get current context in this thread
            GLFWwindow* window = glfwGetCurrentContext();
            if(window == nullptr || !_loaded) return;

            Render::log_render(DEBUG) << "VAO Generating..." << Flush();

            // Create new VAO object
            GLuint vao;
            glGenVertexArrays(1, &vao);
            registeredVAOs.insert(vao);
            glBindVertexArray(vao);

            for(BufferEntry buffer : _data_buffers) {
                glBindBuffer(GL_ARRAY_BUFFER, buffer.buffer);
                glEnableVertexAttribArray(buffer.attrib);
                glVertexAttribPointer(
                        buffer.attrib,
                        (int)buffer.type,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        (void*)0
                );
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer);

            glBindVertexArray(0);

            static_vao_map[window][_element_buffer] = vao;
            Render::log_render() << " Done! [" << vao << "]" << Flush();
        }

        /*
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
         */


    }
}