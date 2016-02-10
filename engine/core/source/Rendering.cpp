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

        void ChangeShader(Shaders::Program *newProgram)
        {
            currentProgram = newProgram;
            if(currentProgram != NULL) glUseProgram(currentProgram->getProgramID());
        }

        void Render(Models::Model & object)
        {
            // MVP Matrices
            glm::mat4 ProjectionMatrix = Controls::getProjectionMatrix(); // NEED
            glm::mat4 ViewMatrix = Controls::getViewMatrix(); // NEED
            glm::mat4 ModelMatrix = glm::mat4(1.0); // Likely needs to be different
            glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

            if(currentProgram == NULL) return; // Safety check

            // TODO: Tweak Uniforms

            // Shader Uniforms
            GLuint MVPID = currentProgram->getUniformID(SHADER_UNIFORM_MVP);
            GLuint ModelID = currentProgram->getUniformID(SHADER_UNIFORM_MODEL);
            GLuint ViewID = currentProgram->getUniformID(SHADER_UNIFORM_VIEW);
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewID, 1, GL_FALSE, &ViewMatrix[0][0]);

            //NOTE: This should probably be in a seperate function
            glm::vec3 lightPos = glm::vec3(4,4,4);
            GLuint LightID = currentProgram->getUniformID(SHADER_UNIFORM_LIGHT_POS);
            glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

            // Texture Binding
            glActiveTexture(GL_TEXTURE0);
            GLuint TexID = object.getTexture();
            glBindTexture(GL_TEXTURE_2D, TexID);
            GLuint TexUniID = currentProgram->getUniformID(SHADER_UNIFORM_TEXTURE);
            glUniform1i(TexUniID, 0);

            // 1rst attribute buffer : bones
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, object.getBoneBuffer());
            glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, object.getUVBuffer());
            glVertexAttribPointer(
                    1,                                // attribute
                    2,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    (void*)0                          // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, object.getNormalBuffer());
            glVertexAttribPointer(
                    2,                                // attribute
                    3,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    (void*)0                          // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.getElementBuffer());

            // Draw the triangles !
            glDrawElements(
                    GL_TRIANGLES,      // mode
                    object.getElementCount(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
            );

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
        }

        void Render(Models::Model & object, glm::mat4 modelMatrix)
        {
            // MVP Matrices
            glm::mat4 ProjectionMatrix = Controls::getProjectionMatrix(); // NEED
            glm::mat4 ViewMatrix = Controls::getViewMatrix(); // NEED
            glm::mat4 ModelMatrix = modelMatrix; // Likely needs to be different
            glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

            // Shader Uniforms
            GLuint MVPID = currentProgram->getUniformID(SHADER_UNIFORM_MVP);
            GLuint ModelID = currentProgram->getUniformID(SHADER_UNIFORM_MODEL);
            GLuint ViewID = currentProgram->getUniformID(SHADER_UNIFORM_VIEW);
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewID, 1, GL_FALSE, &ViewMatrix[0][0]);

            //NOTE: This should probably be in a seperate function
            glm::vec3 lightPos = glm::vec3(4,4,4);
            GLuint LightID = currentProgram->getUniformID(SHADER_UNIFORM_LIGHT_POS);
            glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

            // Texture Binding
            glActiveTexture(GL_TEXTURE0);
            GLuint TexID = object.getTexture();
            glBindTexture(GL_TEXTURE_2D, TexID);
            GLuint TexUniID = currentProgram->getUniformID(SHADER_UNIFORM_TEXTURE);
            glUniform1i(TexUniID, 0);

            // 1rst attribute buffer : bones
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, object.getBoneBuffer());
            glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, object.getUVBuffer());
            glVertexAttribPointer(
                    1,                                // attribute
                    2,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    (void*)0                          // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, object.getNormalBuffer());
            glVertexAttribPointer(
                    2,                                // attribute
                    3,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    (void*)0                          // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.getElementBuffer());

            // Draw the triangles !
            glDrawElements(
                    GL_TRIANGLES,      // mode
                    object.getElementCount(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
            );

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
        }

    }
}