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

        Renderer::Renderer(Program &program, RenderCycleFunc custom_render_cycle)
                : program(program), custom_render_cycle(custom_render_cycle), uniform_model("_m"), uniform_view("_v"), uniform_projection("_p") {
            if(!program.isLinked()) ; // TODO: Throw Exception if Program is unlinked
        }

        void defaultRenderCycle(Renderer &render, Camera &camera) {
            render.start(camera);
            render.render(camera.getWindow().getRenderObjectCollection());
            render.end(camera);
        }

        void Renderer::doRenderCycle(Camera &camera) {
            if(custom_render_cycle != nullptr) custom_render_cycle(*this, camera);
            else defaultRenderCycle(*this, camera);
        }

        void Renderer::start(Camera &camera) {

            // Set Our Current Context and Program
            camera.getWindow().makeCurrent();
            glUseProgram(program.getProgramID());

            // Clear Buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Set View and Projection Uniforms;
            glUniformMatrix4fv(program.getUniformID(uniform_view),       1, GL_FALSE, &camera.getViewMatrix()      [0][0]);
            glUniformMatrix4fv(program.getUniformID(uniform_projection), 1, GL_FALSE, &camera.getProjectionMatrix()[0][0]);

            // Set Texture ID Uniforms
            for(auto && iter : uniform_map_textures)
                glUniform1i(program.getUniformID(iter.second), iter.first);

            // Set Custom Uniforms
            for(Uniforms::Uniform* uniform : uniform_custom_set)
                uniform->bind(*this);

        }

        void Renderer::end(Camera &camera) {
            glfwSwapBuffers(camera.getWindow());
        }

        void Renderer::render(Model::Model & object, glm::mat4 matrix_Model) {

            // Bind the Model Matrix
            glUniformMatrix4fv(program.getUniformID(uniform_model), 1, GL_FALSE, &matrix_Model[0][0]);

            // Each Object has its own VAO
            glBindVertexArray(object.getVAOID());

            // Draw the triangles
            glDrawElements(
                    GL_TRIANGLES,      // mode
                    object.getElementCount(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
            );

            // glBindVertexArray(0);
        }

        void Renderer::render(RenderObject &object) {
            object.getTexture().bind();
            render(object.getModel(), object.getMatrix());
        }

        void Renderer::render(RenderObjectCollection &collection) {
            std::map<Texture::Texture, std::vector<RenderObject*>> &data = collection.getData();
            for(auto && iter : data) {
                iter.first.bind();
                for(RenderObject* o : iter.second)
                    if(o != nullptr) render(o->getModel(), o->getMatrix());
            }
        }

        void Renderer::addCustomUniform(Uniforms::Uniform* uniform) {
            if(uniform != nullptr) uniform_custom_set.insert(uniform);
        }

    }
}