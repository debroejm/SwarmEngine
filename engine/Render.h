#pragma once


// ********************
//  External Libraries
// ********************

//#include <stdio.h>
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
#include <queue>
#include <glm/gtc/matrix_transform.hpp>



// ************
//  Code Begin
// ************

// TODO: Better Documentation

namespace Swarm {

    namespace Render {
        class Renderer;
    }

    namespace Texture {

        namespace MapType {

            class Type {
            public:

                Type(GLenum target, GLuint activeID);

                Type(const Type &other) { *this = other; }
                Type &operator=(const Type &other) {
                    active = other.active;
                    target = other.target;
                    return *this;
                }

                // Comparison Operators
                bool operator==(const Type &other) const { return hash() == other.hash(); }
                bool operator< (const Type &other) const { return hash() < other.hash(); }
                bool operator<=(const Type &other) const { return hash() <= other.hash(); }
                bool operator> (const Type &other) const { return hash() > other.hash(); }
                bool operator>=(const Type &other) const { return hash() >= other.hash(); }

                GLuint getActiveID() const { return active; }
                GLenum getTarget() const { return target; }

                size_t hash() const { return hasher(active); }

                operator GLuint() { return active;}

            protected:
                GLuint active;
                GLenum target;

                static std::hash<unsigned int> hasher;
            };

            static const Type DIFFUSE   (GL_TEXTURE_2D, 0);
            static const Type SPECULAR  (GL_TEXTURE_2D, 1);
            static const Type NORMAL    (GL_TEXTURE_2D, 2);
            static const Type EMISSIVE  (GL_TEXTURE_2D, 3);
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

        struct TexData {
            GLuint ID;
            GLenum target;
        };

        class Texture {
        public:
            void bind() const;

            void setTex(MapType::Type type, GLuint ID) { setTex(type.getActiveID(), ID, type.getTarget()); }
            void setTex(GLuint active, GLuint ID, GLenum target);

            GLuint getTex(GLuint active) const;
            GLuint operator[](GLuint active) const;

            // Comparison Operators
            int compareTo(const Texture &rhs) const;
            bool operator< (const Texture &rhs) const { return compareTo(rhs) < 0; }
            bool operator<=(const Texture &rhs) const { return compareTo(rhs) <= 0; }
            bool operator> (const Texture &rhs) const { return compareTo(rhs) > 0; }
            bool operator>=(const Texture &rhs) const { return compareTo(rhs) >= 0; }
            bool operator==(const Texture &rhs) const { return compareTo(rhs) == 0; }
            Texture &operator=(const Texture &rhs);

        protected:
            map<GLuint, TexData> data;
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
            unsigned short* indices = NULL; // TODO: Possibly make this an unsigned int
            unsigned int indexCount;
        };


        RawModelDataIndexed* loadFromOBJ(const char * path, bool computeTangents = false);


        class Model {
        public:
            virtual GLint getVAOID() = 0;
            virtual unsigned int getElementCount() const = 0;
            virtual bool operator==(Model &rhs);
        };

        void cleanupBuffers();

        class ModelSegment : public Model {
        public:
            ModelSegment(RawModelDataIndexed &data);
            void genBuffers(RawModelDataIndexed &data);

            ModelSegment(const ModelSegment &other);
            ModelSegment &operator=(const ModelSegment &other);

            virtual GLint getVAOID();
            virtual unsigned int getElementCount() const { return elementCount; }

            bool isLoaded() { return loaded; }
            void cleanup();

        protected:
            void regenVAO();

            struct BufferEntry {
                GLuint buffer;
                GLuint attrib;
                VecType type;
                friend bool operator<(const BufferEntry &lhs, const BufferEntry &rhs) { return lhs.buffer < rhs.buffer; }
            };

            std::map<GLFWwindow*, GLuint> vao_map;
            std::set<BufferEntry> data_buffers;
            GLuint element_buffer;
            unsigned int elementCount = 0;
            bool loaded = false;
        };


        void cleanupBuffers();

    }


    namespace Render {

        namespace Uniforms {

            class Uniform {
            public:
                friend class Swarm::Render::Renderer;

                Uniform(string name) : name(name) {}
                string &getName() { return name; }
                void setName(const string &name) { this->name = name; }
                void setName(string name) { this->name = name; }

                void setf (GLsizei count, GLsizei stride, GLfloat *data);
                void seti (GLsizei count, GLsizei stride, GLint   *data);
                void setui(GLsizei count, GLsizei stride, GLuint  *data);
                void setm (GLsizei count, GLsizei width, GLsizei height, GLfloat *data);

                void bind(Renderer &render);

                void clear();
                bool empty() { return hasData; }

            protected:
                string name;
                bool hasData = false;

                enum {
                    F, I, UI, M
                } type;
                union {
                    struct { GLsizei count; GLsizei stride; GLfloat* data; } f;
                    struct { GLsizei count; GLsizei stride; GLint*   data; } i;
                    struct { GLsizei count; GLsizei stride; GLuint*  data; } ui;
                    struct { GLsizei count; GLsizei width; GLsizei height; GLfloat* data; } m;
                } data;
            };

        }


        class ModelMatrixWrapper {
        public:
            virtual glm::mat4 getMatrix() { return matrix; }
            virtual void setMatrix(glm::mat4 matrix) { this->matrix = matrix; }
            void translate(float x, float y, float z) { translate(glm::vec3(x, y, z)); }
            virtual void translate(glm::vec3 amount) { matrix = glm::translate(matrix, amount); }
            void rotate(float amount, float x, float y, float z) { rotate(amount, glm::vec3(x, y, z)); }
            virtual void rotate(float amount, glm::vec3 angle) { matrix = glm::rotate(matrix, amount, angle); }
            void scale(float x, float y, float z) { scale(glm::vec3(x, y, z)); }
            virtual void scale(glm::vec3 amount) { matrix = glm::scale(matrix, amount); }
            virtual void resetMatrix() { matrix = glm::mat4(1); }
        protected:
            glm::mat4 matrix;
        };

        class RenderObject : public ModelMatrixWrapper {
        public:
            RenderObject(Model::Model &model, Texture::Texture &texture, glm::mat4 matrix = glm::mat4(1.0)) : model(model), texture(texture) { setMatrix(matrix); }
            virtual Model::Model &getModel() const { return model; }
            virtual Texture::Texture &getTexture() const { return texture; }
            virtual bool operator==(const RenderObject &rhs) const { return model == rhs.model && texture == rhs.texture; }
        protected:
            Model::Model &model;
            Texture::Texture &texture;
        };

        class RenderObjectMultiPart;

        class RenderObjectMulti : public ModelMatrixWrapper {
        public:
            virtual void add(RenderObjectMultiPart &part);
        protected:
            std::vector<RenderObjectMultiPart> parts;
        };

        class RenderObjectMultiPart : public RenderObject {
        public:
            RenderObjectMultiPart(RenderObjectMulti &parent, Model::Model &model, Texture::Texture &texture, glm::mat4 matrix = glm::mat4(1.0));
            RenderObjectMulti &getParent() { return parent; }
            virtual glm::mat4 getMatrix() { return RenderObject::getMatrix() * parent.getMatrix(); }
        protected:
            RenderObjectMulti &parent;
        };

        class RenderObjectCollection {
        public:
            void add(RenderObject &object);
            void remove(RenderObject &object);
            void clear();
            std::vector<RenderObject*> getList(const Texture::Texture &tex);
            std::map<Texture::Texture, std::vector<RenderObject*>> &getData() { return data; };
        protected:
            std::map<Texture::Texture, std::vector<RenderObject*>> data;
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

        void init();
        void cleanup();

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

        //! Structure representing a unique window.
        /*!
         * This structure serves as a wrapper to a GLFWwindow, in addition to holding a reference to a Camera object.
         * In addition, this structure is constructed with a reference to a RenderObjectCollection. Render Objects are
         * registered with said collection, telling the internal renderer what objects to render.
         * \sa Camera, RenderObjectCollection
         */
        struct Window {
            operator GLFWwindow*() const { return window; }
            GLFWwindow* getWindow() const { return window; }
            int getWidth() const;
            int getHeight() const;
            void getSize(int &width, int &height) const;
            RenderObjectCollection &getRenderObjectCollection() { return renderObjects; }
            void makeCurrent();
            // TODO: Options for different monitors
            Window(int width, int height, RenderObjectCollection &renderObjects, const char* name = "Swarm Engine Instance");
            static Window* getCurrent();
        private:
            GLFWwindow* window = nullptr;
            RenderObjectCollection &renderObjects;
            //static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        };

        class Camera {
        public:
            Camera(Window &window, float speed = 1.0f, CameraMovementMode mode = INSTANT);
            Camera(Window &window, glm::vec3 position, glm::vec3 lookAt, glm::vec3 up = glm::vec3(0,1,0), float speed = 1.0f, CameraMovementMode mode = INSTANT);

            void setMovementMode(CameraMovementMode mode) { movementMode = mode; }
            CameraMovementMode getMovementMode() { return movementMode; }

            void setPosition(CamPos pos) { currentPos = pos; }
            CamPos getPosition() { return currentPos; }

            void setTargetPosition(CamPos pos) { targetPos = pos; }
            CamPos getTargetPosition() { return targetPos; }

            void setFOV(float fov) { this->fov = fov; }
            float getFOV() { return fov; }

            void setViewDistance(float dist) { viewDistance = dist; }
            float getViewDistance() { return viewDistance; }

            Window &getWindow() const { return window; }

            void update(double deltaTime);

            glm::mat4 getViewMatrix();
            glm::mat4 getProjectionMatrix();
        protected:
            CameraMovementMode movementMode = INSTANT;

            float fov = 45.0f;
            float viewDistance = 1000.0f;

            float moveSpeed;

            CamPos currentPos;
            CamPos targetPos;

            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;

            struct Window& window;
        };

        class Renderer {
        public:
            typedef void (*RenderCycleFunc)(Renderer&, Camera&);

            Renderer(Program &program, RenderCycleFunc custom_render_cycle = nullptr);

            Program &getProgram() { return program; }

            void doRenderCycle(Camera &camera);

            void start(Camera &camera);
            void end(Camera &camera);

            // TODO: Add option for custom uniforms per object
            void render(Model::Model &object, glm::mat4 matrix_Model = glm::mat4(1.0));
            void render(RenderObject &object);
            void render(RenderObjectCollection &collection);

            void setUniformNameModel     (std::string name) { uniform_model      = name; }
            void setUniformNameView      (std::string name) { uniform_view       = name; }
            void setUniformNameProjection(std::string name) { uniform_projection = name; }

            std::string getUniformNameModel     () const { return uniform_model; }
            std::string getUniformNameView      () const { return uniform_view; }
            std::string getUniformNameProjection() const { return uniform_projection; }

            void setUniformNameTexture(GLuint activeID, std::string name) { uniform_map_textures[activeID] = name; }
            std::string getUniformNameTexture(GLuint activeID) {
                if(uniform_map_textures.count(activeID)) return uniform_map_textures[activeID];
                else return "";
            }

            // Developer retains ownership
            void addCustomUniform(Uniforms::Uniform* uniform);

        protected:
            Program program;

            RenderCycleFunc custom_render_cycle;

            std::string uniform_model;
            std::string uniform_view;
            std::string uniform_projection;
            std::map<GLuint, std::string> uniform_map_textures;

            std::set<Uniforms::Uniform*> uniform_custom_set;
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