#include "../Render.h"

#include "../Core.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Render {

        void init() {
            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS);

            // Cull triangles which normal is not towards the camera
            glEnable(GL_CULL_FACE);
        }

        void cleanup() {

        }

        Renderer::Renderer(Program *program) : currentProgram(program) {
            if(currentProgram != NULL) glUseProgram(currentProgram->getProgramID());
        }

        void Renderer::changeShaderProfile(Program *program) {
            currentProgram = program;
            if(currentProgram != NULL) glUseProgram(currentProgram->getProgramID());
        }

        /*
        void Renderer::updateUniforms() {

            // Safety Check
            if(currentProgram == NULL) return;
            if(!currentProgram->isLinked()) return;

            // Grab the Matrices from the respective locations
            glm::mat4 matrix_Projection; // = Input::getProjectionMatrix(); // NEED
            glm::mat4 matrix_View; // = Input::getViewMatrix(); // NEED
            if(currentCamera == NULL) { matrix_View = glm::mat4(1.0); matrix_Projection = glm::mat4(1.0); Logging::Log::log_render(Logging::ERR) << "NULL Camera when Rendering"; }
            else { matrix_View = currentCamera->getViewMatrix(); matrix_Projection = currentCamera->getProjectionMatrix(); }

            // Bind the VP Matrices
            glUniformMatrix4fv(currentProgram->getUniformID(Uniforms::MatrixView),       1, GL_FALSE, &matrix_View[0][0]);
            glUniformMatrix4fv(currentProgram->getUniformID(Uniforms::MatrixProjection), 1, GL_FALSE, &matrix_Projection[0][0]);

            // Bind Texture IDs
            // TODO: More Dynamic Binding
            glUniform1i(currentProgram->getUniformID(Uniforms::TextureDiffuse),     0);
            glUniform1i(currentProgram->getUniformID(Uniforms::TextureSpecular),    1);
            glUniform1i(currentProgram->getUniformID(Uniforms::TextureNormal),      2);
            glUniform1i(currentProgram->getUniformID(Uniforms::TextureEmissive),    3);
        }
         */

        void Renderer::start() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //updateUniforms();

            if(currentProgram == NULL) return;
            glUniform1i(currentProgram->getUniformID(Texture::MapType::DIFFUSE.getUniform()), Texture::MapType::DIFFUSE.getActiveID());
            glUniform1i(currentProgram->getUniformID(Texture::MapType::SPECULAR.getUniform()), Texture::MapType::SPECULAR.getActiveID());
            glUniform1i(currentProgram->getUniformID(Texture::MapType::NORMAL.getUniform()), Texture::MapType::NORMAL.getActiveID());
            glUniform1i(currentProgram->getUniformID(Texture::MapType::EMISSIVE.getUniform()), Texture::MapType::EMISSIVE.getActiveID());
        }

        void Renderer::end() {
            glfwSwapBuffers(Input::getWindow()); // Possibly move window handle around
        }

        void Renderer::render(Model::Model & object, glm::mat4 matrix_Model) {

            // Safety Check
            if(currentProgram == NULL) return;
            if(!currentProgram->isLinked()) return;

            // Update dirty uniforms
            for(auto && uniform : dirty_uniforms) {
                if(uniform == NULL) continue;
                switch(uniform->type) {
                    case Uniforms::Uniform::F: {
                        switch(uniform->data.f.stride) {
                            case 1:  glUniform1fv(currentProgram->getUniformID(uniform->name), uniform->data.f.count, uniform->data.f.data); break;
                            case 2:  glUniform2fv(currentProgram->getUniformID(uniform->name), uniform->data.f.count, uniform->data.f.data); break;
                            case 3:  glUniform3fv(currentProgram->getUniformID(uniform->name), uniform->data.f.count, uniform->data.f.data); break;
                            default: glUniform4fv(currentProgram->getUniformID(uniform->name), uniform->data.f.count, uniform->data.f.data); break;
                        } } break;
                    case Uniforms::Uniform::I: {
                        switch(uniform->data.i.stride) {
                            case 1:  glUniform1iv(currentProgram->getUniformID(uniform->name), uniform->data.i.count, uniform->data.i.data); break;
                            case 2:  glUniform2iv(currentProgram->getUniformID(uniform->name), uniform->data.i.count, uniform->data.i.data); break;
                            case 3:  glUniform3iv(currentProgram->getUniformID(uniform->name), uniform->data.i.count, uniform->data.i.data); break;
                            default: glUniform4iv(currentProgram->getUniformID(uniform->name), uniform->data.i.count, uniform->data.i.data); break;
                        } } break;
                    case Uniforms::Uniform::UI: {
                        switch(uniform->data.ui.stride) {
                            case 1:  glUniform1uiv(currentProgram->getUniformID(uniform->name), uniform->data.ui.count, uniform->data.ui.data); break;
                            case 2:  glUniform2uiv(currentProgram->getUniformID(uniform->name), uniform->data.ui.count, uniform->data.ui.data); break;
                            case 3:  glUniform3uiv(currentProgram->getUniformID(uniform->name), uniform->data.ui.count, uniform->data.ui.data); break;
                            default: glUniform4uiv(currentProgram->getUniformID(uniform->name), uniform->data.ui.count, uniform->data.ui.data); break;
                        } } break;
                    case Uniforms::Uniform::M: {
                        switch(uniform->data.m.width) {
                            case 2: {
                                switch(uniform->data.m.height) {
                                    case 2:  glUniformMatrix2fv  (currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                    case 3:  glUniformMatrix2x3fv(currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                    default: glUniformMatrix2x4fv(currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                } } break;
                            case 3: {
                                switch(uniform->data.m.height) {
                                    case 2:  glUniformMatrix3x2fv(currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                    case 3:  glUniformMatrix3fv  (currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                    default: glUniformMatrix3x4fv(currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                } } break;
                            default: {
                                switch(uniform->data.m.height) {
                                    case 2:  glUniformMatrix4x2fv(currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                    case 3:  glUniformMatrix4x3fv(currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                    default: glUniformMatrix4fv  (currentProgram->getUniformID(uniform->name), uniform->data.m.count, GL_FALSE, uniform->data.m.data); break;
                                } } break;
                        } } break;
                    default: break;
                }
            }

            // Bind the Model Matrix
            glUniformMatrix4fv(currentProgram->getUniformID(Uniforms::MatrixModel.getName()), 1, GL_FALSE, &matrix_Model[0][0]);

            // Each Object has its own VAO
            glBindVertexArray(object.getVAOID());

            // Draw the triangles !
            glDrawElements(
                    GL_TRIANGLES,      // mode
                    object.getElementCount(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
            );

            glBindVertexArray(0);
        }

        void Renderer::render(RenderObject &object) {
            object.prepareModel();
            render(object.getModel(), object.getMatrix());
        }

        void Renderer::renderAll() {
            for(auto && iter : roMap) {
                if(iter.first == NULL) continue;
                iter.first->bind();
                for(RenderObject* o : iter.second)
                    render(*o);
            }
        }

        void Renderer::registerRenderObject(RenderObject* object) {
            if(object == NULL) return;
            roMap[&(object->getTexture())].push_back(object);
        }

        void Renderer::unregisterRenderObject(RenderObject* object) {
            if(object == NULL) return;
            if(roMap.count(&(object->getTexture()))) {
                std::vector<RenderObject *> &list = roMap[&(object->getTexture())];
                for(unsigned int i = 0; i < list.size(); i++)
                    if(list[i] == object) list.erase(list.begin()+i);
            }
        }

        void Renderer::clearRenderObjects() {
            roMap.clear();
        }
    }
}