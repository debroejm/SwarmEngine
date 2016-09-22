#include "../Render.h"

#include "../Core.h"

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

        void Renderer::changeShaderProfile(Program *program) {
            currentProgram = program;
            if(program != NULL) glUseProgram(currentProgram->getProgramID());
        }

        void Renderer::changeCamera(Camera* camera) {
            currentCamera = camera;
        }

        void Renderer::updateMatrixUniforms() {

            // Safety Check
            if(currentProgram == NULL) return;
            if(!currentProgram->isLinked()) return;

            // Grab the Matrices from the respective locations
            glm::mat4 matrix_Projection = Input::getProjectionMatrix(); // NEED
            glm::mat4 matrix_View; // NEED
            if(currentCamera == NULL) matrix_View = glm::mat4(1.0);
            else matrix_View = currentCamera->getViewMatrix();

            // Bind the Matrices
            glUniformMatrix4fv(currentProgram->getUniformID("_v"), 1, GL_FALSE, &matrix_View[0][0]);
            glUniformMatrix4fv(currentProgram->getUniformID("_p"), 1, GL_FALSE, &matrix_Projection[0][0]);
        }

        void Renderer::render(Model::Model & object, glm::mat4 matrix_Model) {

            // Safety Check
            if(currentProgram == NULL) return;
            if(!currentProgram->isLinked()) return;

            // Bind the Model Matrix
            glUniformMatrix4fv(currentProgram->getUniformID("_m"), 1, GL_FALSE, &matrix_Model[0][0]);

            // Texture Binding
            // TODO: Make the texture binding code object specific
            glActiveTexture(GL_TEXTURE0);
            GLuint TexID = object.getTexture();
            glBindTexture(GL_TEXTURE_2D, TexID);
            glUniform1i(currentProgram->getUniformID("texturemap"), 0);

            // Each Object has its own VAO
            glBindVertexArray(object.getVaoID());

            // Draw the triangles !
            glDrawElements(
                    GL_TRIANGLES,      // mode
                    object.getElementCount(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
            );

            glBindVertexArray(0);
        }
    }
}