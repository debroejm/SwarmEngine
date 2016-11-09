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

        GLuint loadPNGTexture(const char *filename);

        void registerTexture(GLuint textureID);

        void cleanupTextures();

        class Texture {
        public:
            virtual GLuint getID() = 0;
            virtual void bind() = 0;
        };

        class SingleTexture : public Texture {
        public:
            SingleTexture(GLuint textureID);
            SingleTexture(const char *textureName);

            friend class AnimatedTexture;

            virtual GLuint getID();
            virtual void bind();

            SingleTexture &operator=(const SingleTexture &rhs);
            SingleTexture &operator=(const GLuint &rhs);

            bool operator==(const SingleTexture &rhs);
            bool operator==(const GLuint &rhs);

        protected:
            GLuint textureID;
        };

        class AnimatedTexture : public Texture {
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


        RawModelDataIndexed* loadFromOBJ(const char * path);


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

        /*
        class Bone
        {
        public:
            Bone(glm::vec3 pos = glm::vec3(0), string name = "Unnamed", Bone* parent = NULL);
            Bone(Bone &bone);
            Bone(Bone &bone, string name);

            void revertPosition();
            void addPosition(glm::vec3 newPos);
            void setPosition(glm::vec3 newPos);
            void rotatePosition(float angle, glm::vec3 amount);
            void rotatePosition(glm::mat4 rotMatrix);

            void updateBoneBuffer();

            void setParent(Bone &parent) { this->parent = &parent; }
            Bone* getParent() { return parent; }

            void addChild(Bone &child);
            vector<Bone*> getChildren() { return children; }

            void addBonePosition(glm::vec3 &bonePos);
            void addNormal(glm::vec3 &normal);
            void addNormal(glm::vec3 &normal, glm::vec3 &normalStatic);
            vector<glm::vec3*> getBonePositions() { return bonePositions; }
            vector<glm::vec3*> getNormals() { return normals; }

            glm::vec3 getPosition();
            glm::vec3 getRelativePosition();
            glm::mat4 getRotationMatrix();

            string getName() { return name; }

            void operator=(const Bone &rhs);

        private:
            string name;
            glm::vec3 originalPos;
            glm::vec3 position;
            glm::mat4 rotationMatrix = glm::mat4(1.0f);
            Bone* parent;
            vector<Bone*> children;
            vector<glm::vec3*> bonePositions;
            vector<glm::vec3*> normals;
            vector<glm::vec3*> normalsStatic;
        };

        struct bVert{
            float x;
            float y;
            float z;
            Bone* modifier;
        };
         */

        /*
        class Model
        {
        public:
            Model();
            Model(string name);
            Model(Model &other);
            ~Model();
            bool loadOBJ(const char * path);
            bool loadMMD(const char * path);
            bool loadData(Bone skeleton[], glm::vec3 bones[], glm::vec2 uvs[], glm::vec3 normals[], glm::vec3 normalsStatic[], unsigned int elementCount, unsigned short indices[], unsigned int indexCount);

            void operator=(Model &rhs);

            void addTexture(const char * textureName);
            void addTexture(GLuint textureID);
            void setTexture(Texture::AnimatedTexture other);

            //GLuint getBoneBuffer();
            //GLuint getUVBuffer();
            //GLuint getNormalBuffer();
            //GLuint getElementBuffer();
            GLuint getVaoID();
            int getElementCount();
            GLuint getTexture();

            Bone* getBone(int index);
            Bone* getBone(string tag);

            bool isLoaded();
            string getName() { return name; }

            glm::vec3 getMaxDimensions();
            glm::vec3 getMinDimensions();

            glm::vec3 getBonePosition(int index);
            void addBonePosition(int index, glm::vec3 pos);
            void setBonePosition(int index, glm::vec3 pos);
            void rotateBonePosition(int index, float angle, glm::vec3 amount);

            void updateBoneBuffer();

        private:
            bool loaded;
            string name;

            GLuint vao;

            GLuint bonebuffer;
            GLuint uvbuffer;
            GLuint normalbuffer;
            GLuint elementbuffer;
            unsigned int indexCount;

            Bone* skeleton = NULL;
            int boneCount;
            glm::vec3* bonePositions = NULL;
            glm::vec2* uvs = NULL;
            glm::vec3* normals = NULL;
            glm::vec3* normalsStatic = NULL;
            unsigned short* indexArray = NULL;
            unsigned int elementCount;

            //vector<Bone> skeleton;
            //vector<glm::vec3> bones;
            //vector<glm::vec3> normals;

            glm::vec3 maxDim;
            glm::vec3 minDim;

            Texture::AnimatedTexture texture;
        };
         */
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


        class RenderObjectSingle {
        public:
            virtual Model::Model &getModel() = 0;
            virtual glm::mat4 &getMatrix() = 0;
            virtual void setMatrix(glm::mat4 matrix) = 0;
            virtual void prepareModel() {}
        };

        class RenderObjectMulti {
        public:
            virtual unsigned int getCount() = 0;
            virtual Model::Model &getModel(unsigned int index) = 0;
            virtual glm::mat4 &getMatrix(unsigned int index) = 0;
            virtual void setMatrix(unsigned int index, glm::mat4 matrix) = 0;
            virtual void prepareModel(unsigned int index) {}
        };

        class SimpleROS : public RenderObjectSingle {
        public:
            SimpleROS(Model::Model &model, glm::mat4 matrix = glm::mat4(1.0)) : model(model), matrix(matrix) {}
            virtual Model::Model &getModel() { return model; }
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
            void render(RenderObjectSingle &object);
            void render(RenderObjectMulti &object);

        protected:
            Program* currentProgram;
            Camera* currentCamera;
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