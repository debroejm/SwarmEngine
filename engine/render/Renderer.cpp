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

        void Renderer::changeCamera(Camera* camera) {
            currentCamera = camera;
        }

        void Renderer::updateUniforms() {

            // Safety Check
            if(currentProgram == NULL) return;
            if(!currentProgram->isLinked()) return;

            // Grab the Matrices from the respective locations
            glm::mat4 matrix_Projection = Input::getProjectionMatrix(); // NEED
            glm::mat4 matrix_View; // = Input::getViewMatrix(); // NEED
            if(currentCamera == NULL) { matrix_View = glm::mat4(1.0); Logging::Log::log_render(Logging::ERR) << "NULL Camera when Rendering"; }
            else matrix_View = currentCamera->getViewMatrix();

            // Bind the VP Matrices
            glUniformMatrix4fv(currentProgram->getUniformID(Uniforms::MatrixView),       1, GL_FALSE, &matrix_View[0][0]);
            glUniformMatrix4fv(currentProgram->getUniformID(Uniforms::MatrixProjection), 1, GL_FALSE, &matrix_Projection[0][0]);

            // Bind Texture IDs
            glUniform1i(currentProgram->getUniformID(Uniforms::TextureDiffuse),     0);
            glUniform1i(currentProgram->getUniformID(Uniforms::TextureSpecular),    1);
            glUniform1i(currentProgram->getUniformID(Uniforms::TextureNormal),      2);
            glUniform1i(currentProgram->getUniformID(Uniforms::TextureEmissive),    3);
        }

        void Renderer::start() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            updateUniforms();
        }

        void Renderer::end() {
            glfwSwapBuffers(Input::getWindow()); // Possibly move window handle around
        }

        void Renderer::render(Model::Model & object, glm::mat4 matrix_Model) {

            // Safety Check
            if(currentProgram == NULL) return;
            if(!currentProgram->isLinked()) return;

            // Bind the Model Matrix
            glUniformMatrix4fv(currentProgram->getUniformID(Uniforms::MatrixModel), 1, GL_FALSE, &matrix_Model[0][0]);

            // Texture Binding
            // TODO: Make the texture binding code object specific
            /*
            glActiveTexture(GL_TEXTURE0);
            GLuint TexID = object.getTexID();
            glBindTexture(GL_TEXTURE_2D, TexID);
            glUniform1i(currentProgram->getUniformID("texturemap"), 0);
             */

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

        void Renderer::render(RenderObjectSingle &object) {
            object.prepareModel();
            render(object.getModel(), object.getMatrix());
        }

        void Renderer::render(RenderObjectMulti &object) {
            for(unsigned int i = 0; i < object.getCount(); i++) {
                object.prepareModel(i);
                render(object.getModel(i), object.getMatrix(i));
            }
        }
    }
}