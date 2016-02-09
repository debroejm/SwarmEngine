#include "../headers/ShaderHandler.h"


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

        GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
        {

            // Create the shaders
            GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
            GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

            // Read the Vertex Shader code from the file
            std::string VertexShaderCode;
            std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
            if(VertexShaderStream.is_open())
            {
                std::string Line = "";
                while(getline(VertexShaderStream, Line))
                    VertexShaderCode += "\n" + Line;
                VertexShaderStream.close();
            }

            // Read the Fragment Shader code from the file
            std::string FragmentShaderCode;
            std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
            if(FragmentShaderStream.is_open()){
                std::string Line = "";
                while(getline(FragmentShaderStream, Line))
                    FragmentShaderCode += "\n" + Line;
                FragmentShaderStream.close();
            }

            GLint Result = GL_FALSE;
            int InfoLogLength;

            // Compile Vertex Shader
            char vBuf[256];
            sprintf(vBuf, "Compiling Vertex Shader: %s", vertex_file_path);
            Logging::Log(LOGGING_INFO, "ShaderLoading", vBuf);
            //printf("Compiling shader : %s\n", vertex_file_path);
            char const * VertexSourcePointer = VertexShaderCode.c_str();
            glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
            glCompileShader(VertexShaderID);

            // Check Vertex Shader
            glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
            glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<char> VertexShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            if( VertexShaderErrorMessage.size() > 3 )
            {
                char vErrorBuf[1000];
                sprintf(vErrorBuf, "Vertex Shader Error: %s", &VertexShaderErrorMessage[0]);
                Logging::Log(LOGGING_ERROR, "ShaderLoading", vErrorBuf);
            }
            //fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

            // Compile Fragment Shader
            char fBuf[256];
            sprintf(fBuf, "Compiling Fragment Shader: %s", fragment_file_path);
            Logging::Log(LOGGING_INFO, "ShaderLoading", fBuf);
            //printf("Compiling shader : %s\n", fragment_file_path);
            char const * FragmentSourcePointer = FragmentShaderCode.c_str();
            glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
            glCompileShader(FragmentShaderID);

            // Check Fragment Shader
            glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
            glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            if( FragmentShaderErrorMessage.size() > 3 )
            {
                char fErrorBuf[1000];
                sprintf(fErrorBuf, "Fragment Shader Error: %s", &FragmentShaderErrorMessage[0]);
                Logging::Log(LOGGING_ERROR, "ShaderLoading", fErrorBuf);
            }
            //fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

            // Link the program
            Logging::Log(LOGGING_INFO, "ShaderLoading", "Linking Program");
            //fprintf(stdout, "Linking program\n");
            GLuint ProgramID = glCreateProgram();
            glAttachShader(ProgramID, VertexShaderID);
            glAttachShader(ProgramID, FragmentShaderID);
            glLinkProgram(ProgramID);

            // Check the program
            glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
            glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            if( ProgramErrorMessage.size() > 3 )
            {
                char pErrorBuf[1000];
                sprintf(pErrorBuf, "Program Error: %s", &ProgramErrorMessage[0]);
                Logging::Log(LOGGING_ERROR, "ShaderLoading", pErrorBuf);
            }
            //fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

            glDeleteShader(VertexShaderID);
            glDeleteShader(FragmentShaderID);

            return ProgramID;
        }



// ----------
// ShaderData
// ----------

        ShaderData::ShaderData(const char *vertex_shader_path, const char *fragment_shader_path)
        {
            ShaderID = LoadShaders(vertex_shader_path, fragment_shader_path);

            MatrixID.ID = glGetUniformLocation(ShaderID, UniformName_MVP);
            MatrixID.used = true;
            ViewMatrixID.ID = glGetUniformLocation(ShaderID, UniformName_View);
            ViewMatrixID.used = true;
            ModelMatrixID.ID = glGetUniformLocation(ShaderID, UniformName_Model);
            ModelMatrixID.used = true;
            TextureID.ID = glGetUniformLocation(ShaderID, UniformName_Texture);
            TextureID.used = true;
            LightPositionID.ID = glGetUniformLocation(ShaderID, UniformName_LightPos);
            LightPositionID.used = true;
            LightColorID.ID = glGetUniformLocation(ShaderID, UniformName_LightColor);
            LightColorID.used = false;
            LightPowerID.ID = glGetUniformLocation(ShaderID, UniformName_LightPower);
            LightPowerID.used = false;
            ScreenSizeID.ID = glGetUniformLocation(ShaderID, UniformName_ScreenSize);
            ScreenSizeID.used = false;

            loaded = true;
        }

        ShaderData::ShaderData()
        {
            loaded = false;
        }

        ShaderData::~ShaderData()
        {
            if(!loaded) return;
            glDeleteProgram(ShaderID);
        }

        void ShaderData::setUniformID(int uniformType, bool useIt)
        {
            if(!loaded)
            {
                Logging::Log(LOGGING_WARNING, "Shaders", "Attempt to call setUniformID() on an empty ShaderData object");
                return;
            }
            if( uniformType == SHADER_UNIFORM_MVP ) MatrixID.used = useIt;
            else if( uniformType == SHADER_UNIFORM_MODEL ) ModelMatrixID.used = useIt;
            else if( uniformType == SHADER_UNIFORM_VIEW ) ViewMatrixID.used = useIt;
            else if( uniformType == SHADER_UNIFORM_TEXTURE ) TextureID.used = useIt;
            else if( uniformType == SHADER_UNIFORM_LIGHT_POS ) LightPositionID.used = useIt;
            else if( uniformType == SHADER_UNIFORM_LIGHT_COLOR ) LightColorID.used = useIt;
            else if( uniformType == SHADER_UNIFORM_LIGHT_POWER ) LightPowerID.used = useIt;
            else if( uniformType == SHADER_UNIFORM_SCREENSIZE ) ScreenSizeID.used = useIt;
        }

        GLuint ShaderData::getUniformID(int uniformType)
        {
            if(!loaded)
            {
                Logging::Log(LOGGING_WARNING, "Shaders", "Attempt to call getUniformID() on an empty ShaderData object");
                return -1;
            }
            if( uniformType == SHADER_UNIFORM_MVP )
            {
                if( MatrixID.used == true ) return MatrixID.ID;
                else return -1;
            }
            else if( uniformType == SHADER_UNIFORM_MODEL )
            {
                if( ModelMatrixID.used == true ) return ModelMatrixID.ID;
                else return -1;
            }
            else if( uniformType == SHADER_UNIFORM_VIEW )
            {
                if( ViewMatrixID.used == true ) return ViewMatrixID.ID;
                else return -1;
            }
            else if( uniformType == SHADER_UNIFORM_TEXTURE )
            {
                if( TextureID.used == true ) return TextureID.ID;
                else return -1;
            }
            else if( uniformType == SHADER_UNIFORM_LIGHT_POS )
            {
                if( LightPositionID.used == true ) return LightPositionID.ID;
                else return -1;
            }
            else if( uniformType == SHADER_UNIFORM_LIGHT_COLOR )
            {
                if( LightColorID.used == true ) return LightColorID.ID;
                else return -1;
            }
            else if( uniformType == SHADER_UNIFORM_LIGHT_POWER )
            {
                if( LightPowerID.used == true ) return LightPowerID.ID;
                else return -1;
            }
            else if( uniformType == SHADER_UNIFORM_SCREENSIZE )
            {
                if( ScreenSizeID.used == true ) return ScreenSizeID.ID;
                else return -1;
            }

            return -1;
        }

        GLuint ShaderData::getProgramID()
        {
            if(!loaded)
            {
                Logging::Log(LOGGING_WARNING, "Shaders", "Attempt to call getProgramID() on an empty ShaderData object");
                return -1;
            }
            return ShaderID;
        }

        void ShaderData::operator =(const ShaderData &rhs)
        {
            (*this).ShaderID = rhs.ShaderID;
            (*this).MatrixID = rhs.MatrixID;
            (*this).ViewMatrixID = rhs.ViewMatrixID;
            (*this).ModelMatrixID = rhs.ModelMatrixID;
            (*this).TextureID = rhs.TextureID;
            (*this).LightPositionID = rhs.LightPositionID;
            (*this).LightColorID = rhs.LightColorID;
            (*this).LightPowerID = rhs.LightPowerID;
            (*this).ScreenSizeID = rhs.ScreenSizeID;
            (*this).loaded = rhs.loaded;
        }

    }
}