#include "../headers/Rendering.h"

ENGINE_NAMESPACE::Shaders::Program *currentProgram;

namespace ENGINE_NAMESPACE {
    namespace Rendering{

        GLuint VertexArrayID;

        void init() {
            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS);

            // Cull triangles which normal is not towards the camera
            glEnable(GL_CULL_FACE);

            glGenVertexArrays(1, &VertexArrayID);
            glBindVertexArray(VertexArrayID);
        }

        void cleanup() {
            glDeleteVertexArrays(1, &VertexArrayID);
        }

        void ChangeShader(Shaders::Program &newProgram)
        {
            currentProgram = &newProgram;
            glUseProgram(currentProgram->getProgramID());
        }

        void Render(Models::Model & object) { Render(object, glm::mat4(1.0)); }
        void Render(Models::Model & object, glm::mat4 modelMatrix)
        {
            // MVP Matrices
            glm::mat4 ProjectionMatrix = Controls::getProjectionMatrix(); // NEED
            glm::mat4 ViewMatrix = Controls::getViewMatrix(); // NEED
            //glm::mat4 ModelMatrix = glm::mat4(1.0); // Likely needs to be different
            //glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

            if(!currentProgram->isLinked()) return; // Safety Check

            // Shader Uniforms
            /*
            GLuint MVPID = currentProgram->getUniformID(SHADER_UNIFORM_MVP);
            GLuint ModelID = currentProgram->getUniformID(SHADER_UNIFORM_MODEL);
            GLuint ViewID = currentProgram->getUniformID(SHADER_UNIFORM_VIEW);
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewID, 1, GL_FALSE, &ViewMatrix[0][0]);
             */
            glUniformMatrix4fv(currentProgram->getUniformID_model(), 1, GL_FALSE, &modelMatrix[0][0]);
            glUniformMatrix4fv(currentProgram->getUniformID_view(), 1, GL_FALSE, &ViewMatrix[0][0]);
            glUniformMatrix4fv(currentProgram->getUniformID_projection(), 1, GL_FALSE, &ProjectionMatrix[0][0]);

            // Texture Binding
            glActiveTexture(GL_TEXTURE0);
            GLuint TexID = object.getTexture();
            glBindTexture(GL_TEXTURE_2D, TexID);
            glUniform1i(currentProgram->getUniformID_texture(), 0);

            // 1rst attribute buffer : bones
            if(currentProgram->usesVertices()) {
                glEnableVertexAttribArray(currentProgram->getAttribID_vertex());
                glBindBuffer(GL_ARRAY_BUFFER, object.getBoneBuffer());
                glVertexAttribPointer(
                        0,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void *) 0            // array buffer offset
                );
            }

            // 2nd attribute buffer : UVs
            if(currentProgram->usesUVs()) {
                glEnableVertexAttribArray(currentProgram->getAttribID_uv());
                glBindBuffer(GL_ARRAY_BUFFER, object.getUVBuffer());
                glVertexAttribPointer(
                        1,                                // attribute
                        2,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        (void *) 0                          // array buffer offset
                );
            }

            // 3rd attribute buffer : normals
            if(currentProgram->usesNormals()) {
                glEnableVertexAttribArray(currentProgram->getAttribID_normal());
                glBindBuffer(GL_ARRAY_BUFFER, object.getNormalBuffer());
                glVertexAttribPointer(
                        2,                                // attribute
                        3,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        (void *) 0                          // array buffer offset
                );
            }

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.getElementBuffer());

            // Draw the triangles !
            glDrawElements(
                    GL_TRIANGLES,      // mode
                    object.getElementCount(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
            );

            if(currentProgram->usesVertices()) glDisableVertexAttribArray(currentProgram->getAttribID_vertex());
            if(currentProgram->usesUVs()) glDisableVertexAttribArray(currentProgram->getAttribID_uv());
            if(currentProgram->usesNormals()) glDisableVertexAttribArray(currentProgram->getAttribID_normal());
        }
    }
}