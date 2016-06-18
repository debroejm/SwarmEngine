#include "Shaders.h"

using namespace ENGINE_NAMESPACE::ENGINE_NAMESPACE_LOG;

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_SHADER {


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
                case GL_GEOMETRY_SHADER: return "Geometry";
                case GL_FRAGMENT_SHADER: return "Fragment";
                default: return NULL;
            }
        }

        GLuint compileShader(const char* shaderPath, GLenum shaderType) {

            // Determine the type of shader
            const char* shaderTypeName = getShaderName(shaderType);
            if(shaderTypeName == NULL) {
                Log::log_render(ERR) << "Unknown Shader Type '" << shaderType << "' Specified For: " << shaderPath;
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
                Log::log_render(ERR) << "Failed to Open Shader: " << shaderPath;
                return 0;
            }

            GLuint shaderID = glCreateShader(shaderType);
            registeredShaders.push_back(shaderID);
            Log::log_render(INFO) << "Compiling " << shaderTypeName << " Shader [" << shaderID << "]: " << shaderPath;

            // Attempt to compile shader
            const char* sourcePointer = shaderSource.c_str();
            glShaderSource(shaderID, 1, &sourcePointer, NULL);
            glCompileShader(shaderID);

            // Check on compile status
            GLint result = GL_FALSE;
            int infoLogLength;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
            char shaderErrorMessage[infoLogLength+1];
            glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
            if(infoLogLength > 2) {
                Log::log_render(ERR) << shaderTypeName << " Shader Error: " << shaderErrorMessage;
                return 0;
            }

            return shaderID;
        }

        void cleanupShaders() {
            for(int i = 0; i < registeredShaders.size(); i++) {
                Log::log_render(INFO) << "Deleting Shader [" << registeredShaders[i] << "]";
                glDeleteShader(registeredShaders[i]);
            }
        }

        GLuint compileProgram(Shader *shaders[], int shaderCount) {
            GLuint programID = glCreateProgram();
            registeredPrograms.push_back(programID);
            Log::log_render(INFO) << "Linking Program [" << programID << "]";

            // Attach Shaders
            for(int i = 0; i < shaderCount; i++) {
                if(shaders[i] != NULL) {
                    glAttachShader(programID, shaders[i]->getShaderID());
                    Log::log_render(INFO) << "Attaching " << getShaderName(shaders[i]->getShaderType()) << " Shader [" << shaders[i]->getShaderID() << "]";
                } else {
                    Log::log_render(WARNING) << "Found NULL Shader Object";
                }
            }
            glLinkProgram(programID);

            // Check on compile status
            GLint result = GL_FALSE;
            int infoLogLength;
            glGetProgramiv(programID, GL_LINK_STATUS, &result);
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
            //std::vector<char> errorMessage( std::max(infoLogLength, int(1)) );
            //vector<char> errorMessage( infoLogLength );
            char errorMessage[infoLogLength+1];
            glGetProgramInfoLog(programID, infoLogLength, NULL, &errorMessage[0]);
            if(infoLogLength > 3) {
                Log::log_render(ERR) << "Program Error: " << errorMessage;
                return 0;
            }

            for(int i = 0; i < shaderCount; i++) {
                if(shaders[i] != NULL) glDetachShader(programID, shaders[i]->getShaderID());
            }

            return programID;
        }

        void cleanupPrograms() {
            for(int i = 0; i < registeredPrograms.size(); i++) {
                Log::log_render(INFO) << "Deleting Program [" << registeredPrograms[i] << "]";
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

        Program::Program() { linked = false; }
        Program::Program(Program &other) { *this = other; }
        Program::Program(Shader *shaders[], int shaderCount,
                         bool vertices, bool uvs, bool normals,
                         const char * model, const char * view, const char * projection,
                         const char * texMap) 
                       : vertices(vertices), uvs(uvs), normals(normals) {
            this->programID = compileProgram(shaders, shaderCount);
            linked = (programID > 0) && findUniformIDs(model, view, projection, texMap);
        }

        Program::~Program() {
            // Normally would delete program here, but its taken care of in cleanupPrograms()
        }

        void Program::operator =(const Program &rhs)
        {
            this->programID = rhs.programID;
            this->vertices = rhs.vertices;
            this->uvs = rhs.uvs;
            this->normals = rhs.normals;
            this->uniformID_model = rhs.uniformID_model;
            this->uniformID_view = rhs.uniformID_view;
            this->uniformID_projection = rhs.uniformID_projection;
            this->uniformID_texture = rhs.uniformID_texture;
            this->linked = rhs.linked;
        }
        
        GLint Program::getUniformID(const char * name) {
            return glGetUniformLocation(programID, name);
        }

        bool Program::findUniformIDs(const char * model, const char * view, const char * projection,
                                     const char * texMap) {
            uniformID_model =       glGetUniformLocation(programID, model);
            uniformID_view =        glGetUniformLocation(programID, view);
            uniformID_projection =  glGetUniformLocation(programID, projection);
            uniformID_texture =     glGetUniformLocation(programID, texMap);
            return !(uniformID_model < 0 || uniformID_view < 0 || uniformID_projection < 0 || uniformID_texture < 0);
        }
    }
}