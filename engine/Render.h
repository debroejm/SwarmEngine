#pragma once


// ********************
//  External Libraries
// ********************

#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <map>
#include <vector>
using namespace std;

#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <set>



// ************
//  Code Begin
// ************

// TODO: Better Documentation

namespace Swarm {

    namespace Texture {

        namespace MapType {

            class Type {
            public:
                Type(GLenum target, GLuint activeID, string uniform) : target(target), active(activeID), uniform(uniform) { }

                Type(const Type &other) { *this = other; }
                Type &operator=(const Type &other) {
                    active = other.active;
                    uniform = other.uniform;
                    target = other.target;
                    return *this;
                }

                bool operator==(const Type &other) const { return hash() == other.hash(); }
                bool operator<(const Type &other) const { return hash() < other.hash(); }
                bool operator>(const Type &other) const { return hash() > other.hash(); }

                string getUniform() const { return uniform; }
                void setUniform(string uniform) { this->uniform = uniform; }
                GLuint getActiveID() const { return active; }
                GLenum getTarget() const { return target; }

                size_t hash() const { return hasher(active); }

            protected:
                GLuint active;
                string uniform;
                GLenum target;

                std::hash<unsigned int> hasher;
            };

            static const Type DIFFUSE   (GL_TEXTURE_2D, 0, "_texture_diffuse");
            static const Type SPECULAR  (GL_TEXTURE_2D, 1, "_texture_specular");
            static const Type NORMAL    (GL_TEXTURE_2D, 2, "_texture_normal");
            static const Type EMISSIVE  (GL_TEXTURE_2D, 3, "_texture_emissive");
        }
    }
}

namespace std {
    template<> struct hash<Swarm::Texture::MapType::Type> {
        size_t operator() (const Swarm::Texture::MapType::Type &var) const {
            return var.hash();
        }
    };
}

namespace Swarm {
    namespace Texture {

        GLuint loadPNGTexture(const char *filename);

        void registerTexture(GLuint textureID);
        void cleanupTextures();

        class Texture {
        public:
            virtual GLuint getTex(const MapType::Type &type) const = 0;
            virtual GLuint getTex(GLuint activeID) const = 0;
            //virtual map<MapType::Type, GLuint> &getAllTex() const = 0; // TODO: Remove for iteration
            virtual void bind() const = 0;
            virtual int compareTo(const Texture &rhs) const = 0;
            bool operator<(const Texture &rhs) const { return compareTo(rhs) < 0; }
            bool operator>(const Texture &rhs) const { return compareTo(rhs) > 0; }
            bool operator==(const Texture &rhs) const { return compareTo(rhs) == 0; }
        };

        class SingleTexture : public Texture {
        public:
            SingleTexture(GLuint texID);
            SingleTexture(const char * texName);

            friend class AnimatedTexture;

            void setTex(const MapType::Type &type, GLuint texID);
            void setTex(const MapType::Type &type, const char * texName);
            virtual GLuint getTex(const MapType::Type &type) const;
            virtual GLuint getTex(GLuint activeID) const;
            //virtual map<MapType::Type, GLuint> &getAllTex() const;

            virtual void bind() const;

            SingleTexture &operator=(const SingleTexture &rhs);
            SingleTexture &operator=(const GLuint &rhs);

            virtual int compareTo(const Texture &rhs) const;

        protected:
            map<MapType::Type, GLuint> texIDs;
        };

        class AnimatedTexture {
        public:
            AnimatedTexture(GLuint textureID);

            AnimatedTexture(const char *textureName);

            AnimatedTexture();

            void addTexture(GLuint textureID);

            void addTexture(const char *textureName);

            void addTexture(GLuint textureID, double interval);

            void addTexture(const char *textureName, double interval);

            virtual GLuint getID();
            virtual void bind();

            AnimatedTexture &operator=(const AnimatedTexture &rhs);

            AnimatedTexture &operator=(const SingleTexture &rhs);

            AnimatedTexture &operator=(const GLuint &rhs);

        protected:
            vector<GLuint> textureList;
            int currentIndex;
            vector<double> changeDelay;
            double lastTime;
        };
    }


    namespace Model {

        enum VecType {
            ONE     = 1,
            TWO     = 2,
            THREE   = 3,
            FOUR    = 4
        };

        struct VecVar {
            union Val {
                float v1;
                glm::vec2 v2;
                glm::vec3 v3;
                glm::vec4 v4;
                explicit Val(float in)      : v1(in) {}
                explicit Val(glm::vec2 in)  : v2(in) {}
                explicit Val(glm::vec3 in)  : v3(in) {}
                explicit Val(glm::vec4 in)  : v4(in) {}
            } val;
            VecType type;
            explicit VecVar()               : val(0.0f), type(ONE)  {}
            explicit VecVar(float in)       : val(in), type(ONE)    {}
            explicit VecVar(glm::vec2 in)   : val(in), type(TWO)    {}
            explicit VecVar(glm::vec3 in)   : val(in), type(THREE)  {}
            explicit VecVar(glm::vec4 in)   : val(in), type(FOUR)   {}
        };

        union VecArray {
            float       *v1;
            glm::vec2   *v2;
            glm::vec3   *v3;
            glm::vec4   *v4;
            explicit VecArray()               : v1(NULL) {}
            explicit VecArray(float in[])     : v1(in)   {}
            explicit VecArray(glm::vec2 in[]) : v2(in)   {}
            explicit VecArray(glm::vec3 in[]) : v3(in)   {}
            explicit VecArray(glm::vec4 in[]) : v4(in)   {}
        };

        namespace DataType {

            class Type {
            public:
                Type(VecType type, VecVar defaultVal, GLuint attribID, float epsilon = 0.00001f);

                Type(const Type &other);
                Type &operator=(const Type &other);

                bool operator==(const Type &other) const;

                VecType getType() const { return type; }
                VecVar getDefault() const { return defaultVal; }
                float getEpsilon() const { return epsilon; }
                GLuint getAttribID() const { return attribID; }
                void setAttribID(GLuint attribID) { this->attribID = attribID; }

            private:
                VecType type;
                VecVar defaultVal;
                float epsilon;
                GLuint attribID;
            };

            static const Type VERTEX    ( THREE,  VecVar(glm::vec3(0.0f, 0.0f, 0.0f)),        1 );
            static const Type UV        ( TWO,    VecVar(glm::vec2(0.0f, 0.0f)),              2 );
            static const Type NORMAL    ( THREE,  VecVar(glm::vec3(0.0f, 1.0f, 0.0f)),        3 );
            static const Type COLOR     ( FOUR,   VecVar(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),  4 );
            static const Type TANGET    ( THREE,  VecVar(glm::vec3(0.0f, 1.0f, 0.0f)),        5 );
            static const Type BITANGET  ( THREE,  VecVar(glm::vec3(0.0f, 1.0f, 0.0f)),        6 );

        }

        class RawModelDataIndexed;
    }
}

namespace std {
    template<> struct hash<Swarm::Model::DataType::Type> {
        size_t operator() (const Swarm::Model::DataType::Type &var) const {
            return hash<unsigned int>()((var.getType()-1) + 4*var.getAttribID());
        }
    };
}

namespace Swarm {
    namespace Model {

        class RawModelData {
        public:
            RawModelData() {}
            RawModelData(RawModelData &other);
            ~RawModelData();
            RawModelData &operator=(const RawModelData &other);

            RawModelData &setData(const DataType::Type &type, float data[], unsigned int size);
            RawModelData &setData(const DataType::Type &type, VecArray data, unsigned int size);
            bool hasData(const DataType::Type &type) const;
            VecArray getData(const DataType::Type &type) const;
            const unordered_map<DataType::Type, VecArray> &getData() const { return dataMap; }

            unsigned int getSize() const;

            void computeTangents();

            virtual RawModelDataIndexed* index();

            virtual void cleanup();

        protected:

            void upsize(unsigned int size);

            // TODO: Consider array-based implementation if this turns out too slow
            unordered_map<DataType::Type, VecArray> dataMap;

            unsigned int size = 0;
        };

        class RawModelDataIndexed : public RawModelData {
        public:
            RawModelDataIndexed() {}
            RawModelDataIndexed(const RawModelDataIndexed &other);
            RawModelDataIndexed &setIndices(unsigned short* indices, unsigned int size);
            unsigned short* getIndices() const;
            unsigned int getIndexCount() const;

            RawModelDataIndexed &operator=(const RawModelDataIndexed &other);

            virtual RawModelDataIndexed* index();

            virtual void cleanup();

        protected:
            unsigned short* indices; // TODO: Possibly make this an unsigned int
            unsigned int indexCount;
        };


        RawModelDataIndexed* loadFromOBJ(const char * path, bool computeTangents = false);


        class Model {
        public:
            virtual GLuint getVAOID() = 0;
            virtual unsigned int getElementCount() = 0;
        };

        void cleanupBuffers();

        class ModelSegment : public Model {
        public:
            ModelSegment(RawModelDataIndexed &data);
            void genBuffers(RawModelDataIndexed &data);

            ModelSegment(const ModelSegment &other);
            ModelSegment &operator=(const ModelSegment &other);

            virtual GLuint getVAOID() { return vao; }
            virtual unsigned int getElementCount() { return elementCount; }

            bool isLoaded() { return loaded; }
            void cleanup();

        protected:
            GLuint vao;
            std::set<GLuint> buffers;
            unsigned int elementCount = 0;
            bool loaded = false;
        };


        void cleanupBuffers();

    }


    namespace Render {

        namespace Uniforms {

            static string MatrixModel =             "_m";
            static string MatrixView =              "_v";
            static string MatrixProjection =        "_p";

            static string LightAmbientColor =       "_ambient_light_color";
            static string LightAmbientDirection =   "_ambient_light_direction";

            static string TextureDiffuse =          "_texture_diffuse";
            static string TextureSpecular =         "_texture_specular";
            static string TextureNormal =           "_texture_normal";
            static string TextureEmissive =         "_texture_emissive";

        }


        class RenderObject {
        public:
            virtual Model::Model &getModel() = 0;
            virtual Texture::Texture &getTexture() = 0;
            virtual glm::mat4 &getMatrix() = 0;
            virtual void setMatrix(glm::mat4 matrix) = 0;
            virtual void prepareModel() {}
        };

        /*
        class RenderObjectCollection : RenderObject {
        public:
            virtual unsigned int getCount() = 0;
            virtual Model::Model &getModel(unsigned int index) = 0;
            virtual glm::mat4 &getMatrix(unsigned int index) = 0;
            virtual void setMatrix(unsigned int index, glm::mat4 matrix) = 0;
            virtual void prepareModel(unsigned int index) {}
        };
         */

        class RenderObjectSimple : public RenderObject {
        public:
            RenderObjectSimple(Model::Model &model, Texture::Texture &texture, glm::mat4 matrix = glm::mat4(1.0)) : model(model), texture(texture), matrix(matrix) {}
            virtual Model::Model &getModel() { return model; }
            virtual Texture::Texture &getTexture() { return texture; }
            virtual glm::mat4 &getMatrix() { return matrix; }
            virtual void setMatrix(glm::mat4 matrix) { this->matrix = matrix; }
            void translate(float x, float y, float z) { translate(glm::vec3(x, y, z)); }
            virtual void translate(glm::vec3 amount);
            void rotate(float amount, float x, float y, float z) { rotate(amount, glm::vec3(x, y, z)); }
            virtual void rotate(float amount, glm::vec3 angle);
            void scale(float x, float y, float z) { scale(glm::vec3(x, y, z)); }
            virtual void scale(glm::vec3 amount);
            virtual void resetMatrix();
        protected:
            Model::Model &model;
            Texture::Texture &texture;
            glm::mat4 matrix;
        };



        class Shader {
        public:
            Shader(const char *shaderPath, GLenum shaderType);

            ~Shader();

            void operator=(const Shader &rhs);

            //! Get the GLuint ID representing this Shader.
            GLuint getShaderID();

            //! Get the type of Shader (Vertex/Fragment/etc).
            GLenum getShaderType();

        protected:
            GLuint shaderID;
            GLenum shaderType;
        };

        class Program {
        public:
            Program();

            Program(Program &other);

            Program(Shader *shaders[], int shaderCount,
                    bool vertices = true, bool uvs = true, bool normals = true);

            ~Program();

            void operator=(const Program &rhs);

            //! Get the GLuint ID representing this linked shader Program.
            GLuint getProgramID() { return programID; }

            //! Does this shader Program use Vertices as an input?
            bool usesVertices() { return vertices; }
            //! Does this shader Program use UVs as an input?
            bool usesUVs() { return uvs; }
            //! Does this shader Program use Normals as an input?
            bool usesNormals() { return normals; }

            //! Get the ID for a specified shader Uniform variable.
            /*!
             * Scans the linked Program for the specified shader Uniform and returns its ID.
             * If the Uniform is found, it is cached in an internal map for fast future lookup.
             */
            GLint getUniformID(string name);

            //! Has this shader Program already been linked?
            bool isLinked() { return linked; }

        protected:
            GLuint programID;
            bool linked;

            bool vertices;
            bool uvs;
            bool normals;

            unordered_map<string, GLint> uniformCache;
        };

        //! Cleans up any and all Shaders.
        void cleanupShaders();
        //! Cleans up any and all shader Programs.
        void cleanupPrograms();


        //! Enumeration representing different possible camera movements.
        /*!
         * This enumeration is used to tell Camera objects how they need to move through worldspace.
         * Possible values are:
         *   INSTANT : Camera instantly changes position to the new location
         *   SMOOTH : Camera moves smoothly over time based on its speed to the new location
         *   LERP : Camera lerps over time based on its speed to the new location
         * Lerping is a method of movement where the object accelerates from rest to a speed when starting,
         * and decelerates to rest when stopping.
         * \sa Camera
         */
        enum CameraMovementMode {
            INSTANT,
            SMOOTH,
            LERP
        };


        //! Structure representing a Camera Position
        /*!
         * This structure contains three worldspace coordinates. A View Matrix is calculated from these coordinates
         * when a render call is performed using a Camera. Coordinates are as follows:
         *   position : the physical positon of the camera
         *   lookAt : the location the camera is currently looking at
         *   up : a vector representing which direction is 'up' to the camera
         */
        struct CamPos {
            glm::vec3 position;
            glm::vec3 lookAt;
            glm::vec3 up;
        };


        class Camera {
        public:
            Camera(float speed = 1.0f, CameraMovementMode mode = INSTANT);
            Camera(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up = glm::vec3(0,1,0), float speed = 1.0f, CameraMovementMode mode = INSTANT);

            void setMovementMode(CameraMovementMode mode) { movementMode = mode; }
            CameraMovementMode getMovementMode() { return movementMode; }

            void setPosition(CamPos pos) { currentPos = pos; }
            CamPos getPosition() { return currentPos; }

            void setTargetPosition(CamPos pos) { targetPos = pos; }
            CamPos getTargetPosition() { return targetPos; }

            void update(double deltaTime);

            glm::mat4 getViewMatrix();
        protected:
            CameraMovementMode movementMode = INSTANT;

            float moveSpeed;

            CamPos currentPos;
            CamPos targetPos;
        };


        void init();
        void cleanup();

        struct TexPntrComp {
            bool operator() (Texture::Texture* lhs, Texture::Texture* rhs) const {
                if(lhs == NULL) return rhs != NULL;
                if(rhs == NULL) return false;
                return (*lhs) < (*rhs);
            }
        };

        class Renderer {
        public:
            Renderer() : currentProgram(NULL) {};
            Renderer(Program* program);

            void changeShaderProfile(Program* program);
            void changeCamera(Camera* camera);

            Program* getShaderProfile() { return currentProgram; }
            Camera* getCamera() { return currentCamera; }

            void updateUniforms();

            void start();
            void end();

            void render(Model::Model &object, glm::mat4 matrix_Model = glm::mat4(1.0));
            void render(RenderObject &object);
            void renderAll();

            void registerRenderObject(RenderObject* object);
            void unregisterRenderObject(RenderObject* object);
            void clearRenderObjects();

        protected:
            Program* currentProgram;
            Camera* currentCamera;

            // Texture Type -> Texture ID -> RO List
            std::map<Texture::Texture*, std::vector<RenderObject*>, TexPntrComp> roMap;
        };

    }
    
    
    namespace Anim {

        class Rigging {
        public:
            Rigging(Model::Model &input);
            virtual ~Rigging() {}

            virtual void apply() = 0;
            virtual void update(float delta) = 0;

        protected:
            Model::Model* model = NULL;
        };

        /*
        class RiggingHumanoid: public Rigging {
        public:
            RiggingHumanoid(Model::Model &input);

            void apply();
            void update(float delta);

        protected:
            Model::Bone* leftKnee = NULL;
            Model::Bone* rightKnee = NULL;
            Model::Bone* leftFoot = NULL;
            Model::Bone* rightFoot = NULL;
        };
        */
    }
}