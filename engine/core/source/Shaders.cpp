#include "../headers/Shaders.h"


namespace ENGINE_NAMESPACE {
    namespace Shaders {


// Shader Uniform Names
        const GLchar * UniformName_MVP = "MVP";
        const GLchar * UniformName_Model = "M";
        const GLchar * UniformName_View = "V";
        const GLchar * UniformName_Texture = "textureSampler";
        const GLchar * UniformName_LightPos = "LightPosition_worldspace";
        const GLchar * UniformName_LightColor = "";
        const GLchar * UniformName_LightPower = "";
        const GLchar * UniformName_ScreenSize = "";

        std::vector<GLuint> registeredShaders;
        std::vector<GLuint> registeredPrograms;

        const char* getShaderName(GLenum shaderType) {
            switch(shaderType) {
                case GL_VERTEX_SHADER: return "Vertex";
                case GL_FRAGMENT_SHADER: return "Fragment";
                default: return NULL;
            }
        }

        GLuint compileShader(const char* shaderPath, GLenum shaderType) {

            // Determine the type of shader
            char infoMsg[256];
            const char* shaderTypeName = getShaderName(shaderType);
            if(shaderTypeName == NULL) {
                char errorMsg[256];
                sprintf(errorMsg, "Unknown Shader Type '%i': %s", shaderType, shaderPath);
                Logging::Log(LOGGING_ERROR, "Shaders", errorMsg);
                return 0;
            }

            // Attempt to read the file path given
            std::string shaderSource;
            std::ifstream shaderStream(shaderPath, std::ios::in);
            if(shaderStream.is_open()) {
                std::string line = "";
                while(getline(shaderStream, line)) shaderSource += "\n" + line;
                shaderStream.close();
            } else {
                char errorMsg[256];
                sprintf(errorMsg, "Failed to open '%s'", shaderPath);
                Logging::Log(LOGGING_ERROR, "Shaders", errorMsg);
                return 0;
            }

            GLuint shaderID = glCreateShader(shaderType);
            registeredShaders.push_back(shaderID);
            sprintf(infoMsg, "Attempting to compile %s Shader [%i]: %s", shaderTypeName, shaderID, shaderPath);
            Logging::Log(LOGGING_INFO, "Shaders", infoMsg);

            // Attempt to compile shader
            const char* sourcePointer = shaderSource.c_str();
            glShaderSource(shaderID, 1, &sourcePointer, NULL);
            glCompileShader(shaderID);

            // Check on compile status
            GLint result = GL_FALSE;
            int infoLogLength;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
            std::vector<char> shaderErrorMessage(infoLogLength);
            glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
            if(shaderErrorMessage.size() > 3) {
                char errorMsg[1000];
                sprintf(errorMsg, "%s Shader Error: %s", shaderTypeName, &shaderErrorMessage[0]);
                Logging::Log(LOGGING_ERROR, "Shaders", errorMsg);
                return 0;
            }

            Logging::Log(LOGGING_INFO, "Shaders", "Done!");
            return shaderID;
        }

        void cleanupShaders() {
            for(int i = 0; i < registeredShaders.size(); i++) {
                char infoMsg[256];
                sprintf(infoMsg, "Deleting Shader [%i]", registeredShaders[i]);
                Logging::Log(LOGGING_INFO, "Cleanup", infoMsg);
                glDeleteShader(registeredShaders[i]);
            }
        }

        GLuint compileProgram(Shader *shaders[], int shaderCount) {
            GLuint programID = glCreateProgram();
            char infoMsg[256];
            sprintf(infoMsg, "Attempting to Link Program [%i]", programID);
            Logging::Log(LOGGING_INFO, "Shaders", infoMsg);

            // Attach Shaders
            for(int i = 0; i < shaderCount; i++) {
                if(shaders[i] != NULL) {
                    glAttachShader(programID, shaders[i]->getShaderID());
                    char infoMsg2[256];
                    sprintf(infoMsg2, "Attaching %s Shader [%i]", getShaderName(shaders[i]->getShaderType()), shaders[i]->getShaderID());
                    Logging::Log(LOGGING_INFO, "Shaders", infoMsg2);
                } else {
                    Logging::Log(LOGGING_WARNING, "Shaders", "Found NULL Shader Object");
                }
            }
            glLinkProgram(programID);

            // Check on compile status
            GLint result = GL_FALSE;
            int infoLogLength;
            glGetProgramiv(programID, GL_LINK_STATUS, &result);
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
            std::vector<char> errorMessage( std::max(infoLogLength, int(1)) );
            glGetProgramInfoLog(programID, infoLogLength, NULL, &errorMessage[0]);
            if(errorMessage.size() > 3) {
                char errorMsg[1000];
                sprintf(errorMsg, "Program Error: %s", &errorMessage);
                Logging::Log(LOGGING_ERROR, "Shaders", errorMsg);
                return 0;
            }

            Logging::Log(LOGGING_INFO, "Shaders", "Done!");
            return programID;
        }

        void cleanupPrograms() {
            for(int i = 0; i < registeredPrograms.size(); i++) {
                char infoMsg[256];
                sprintf(infoMsg, "Deleting Program [%i]", registeredPrograms[i]);
                Logging::Log(LOGGING_INFO, "Cleanup", infoMsg);
                glDeleteProgram(registeredPrograms[i]);
            }
        }



        // ------------
        // Shader Class
        // ------------

        Shader::Shader(const char* shaderPath, GLenum shaderType) {
            shaderID = compileShader(shaderPath, shaderType);
            this->shaderType = shaderType;
        }

        Shader::~Shader() {
            // Normally would delete shader here, but its taken care of in cleanupShaders()
        }

        GLuint Shader::getShaderID() { return shaderID; }
        GLenum Shader::getShaderType() { return shaderType; }

        void Shader::operator =(const Shader &rhs)
        {
            this->shaderID = rhs.shaderID;
            this->shaderType = rhs.shaderType;
        }


        // -------------
        // Program Class
        // -------------

        Program::Program(Shader *shaders[], int shaderCount) {
            Program(shaders, shaderCount, true, true, true);
        }

        Program::Program(Shader *shaders[], int shaderCount, bool vertices, bool uvs, bool normals) {
            programID = compileProgram(shaders, shaderCount);
            this->vertices = vertices;
            this->uvs = uvs;
            this->normals = normals;
            findIDs();
        }

        Program::~Program() {
            // Normally would delete program here, but its taken care of in cleanupPrograms()
        }

        GLuint Program::getProgramID() { return programID; }

        void Program::operator =(const Program &rhs)
        {
            this->programID = rhs.programID;
        }

        void Program::findIDs() {
            attribID_vertex =   glGetAttribLocation(programID, "vertex");
            attribID_uv =       glGetAttribLocation(programID, "uv");
            attribID_normal =   glGetAttribLocation(programID, "normal");

            uniformID_model =       glGetUniformLocation(programID, "_m");
            uniformID_view =        glGetUniformLocation(programID, "_v");
            uniformID_projection =  glGetUniformLocation(programID, "_p");

            uniformID_texture =     glGetUniformLocation(programID, "texturemap");
        }
    }
}