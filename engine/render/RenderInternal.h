#pragma once

#define SWARM_INCLUDE_GLM
#define SWARM_INCLUDE_GLFW
#define SWARM_BOOST_AVAILABLE
#include "api/Render.h"

#include "api/Util.h"

#include <boost/thread.hpp>


// ************
//  Code Begin
// ************

namespace Swarm {
    namespace Render {

        class RenderObjectStatic : public RenderObject {
        public:
            RenderObjectStatic(Model::Model &model, Texture::Texture &texture, glm::vec3 position, glm::vec3 scale, glm::vec3 rotate);

            virtual Model::Model &model() const { return _model; }
            virtual Texture::Texture &texture() const { return _texture; }

            virtual void render(const Renderer &renderer) const;

            static void cleanup();

        protected:
            Model::Model &_model;
            Texture::Texture &_texture;
            glm::mat4 _matrix = glm::mat4(1);
        };

        class ShaderInternal : public Shader {
        public:
            ShaderInternal(const std::string &src, const ShaderType &type);
            virtual ~ShaderInternal();

            virtual SWMuint ID()             const { return _ID; }
            virtual operator SWMuint()       const { return _ID; }
            virtual const ShaderType &type() const { return _type; }

            static void cleanup();

        protected:
            GLuint _ID;
            ShaderType _type;
        };

        class ProgramInternal : public Program {
        public:
            ProgramInternal(Shader* shaders[], size_t count);
            virtual ~ProgramInternal();

            virtual SWMuint ID() const { return _ID; }
            virtual SWMint uniform(const std::string &name);
            virtual void clearUniformCache() { _uniform_cache.clear(); }

            static void cleanup();
        protected:
            GLuint _ID;
            std::unordered_map<std::string, GLint> _uniform_cache;
        };

        class RendererInternal;

        class WindowInternal {
        public:
            WindowInternal(unsigned int width, unsigned int height, const std::string &name);
            ~WindowInternal();

            int width() const { return _width; }
            int height() const { return _height; }
            int posX() const { return _posX; }
            int posY() const { return _posY; }
            std::string name() const { return _name; }

            bool focused() const { return _focused; }

            void setCamera(Camera* camera) { _camera = camera; }
            Camera* camera() const { return _camera; }

            GLFWwindow* window()   const { return _window; }
            operator GLFWwindow*() const { return _window; }

            //virtual void makeCurrent() const;

            void setVisible(bool visible);
            bool visible() const;

            void cursorPos(double &pos_x, double &pos_y);

            void attachRenderer(const Renderer* renderer, RenderObjectCollection &collection, bool reverse = false);
            void setClearColor(float r, float g, float b, float a = 1.0f);
            void renderAll();

            // Static Cleanup
            static void cleanup();

        protected:
            friend void callback_windowSize(GLFWwindow* window, int width, int height);
            friend void callback_windowPosition(GLFWwindow* window, int x, int y);
            friend void callback_framebufferSize(GLFWwindow* window, int width, int height);
            friend void callback_windowFocus(GLFWwindow* window, int focused);

            std::atomic<int> _width, _height;
            std::atomic<int> _posX, _posY;
            std::string _name;

            std::atomic<bool> _focused;

            std::atomic<int> _framebuffer_width, _framebuffer_height;
            std::atomic<bool> _queued_framebuffer_resize;

            GLFWwindow* _window = nullptr;

            Camera* _camera = nullptr;
            std::atomic<bool> _queued_visible;
            glm::vec4 _clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            struct RenderAttachment {
                const RendererInternal* renderer;
                RenderObjectCollection* collection;
            };
            boost::mutex _render_mutex;
            std::vector<RenderAttachment> _renderers;
        };

        void createMainThreadContext();
        void startRenderThread();
        void stopRenderThread();
        void checkWindowCloseFlags();

        class CameraInternal : public Camera {
        public:
            CameraInternal(const CameraPosition &pos, float speed, MoveType type);

            virtual MoveType moveType() const { return _move_type; }
            virtual void setMoveType(MoveType type) { _move_type = type; }

            virtual float speed() const { return _move_speed; }
            virtual void setSpeed(float speed) { _move_speed = speed; }

            virtual float fov() const { return _fov; }
            virtual void setFOV(float fov) { _fov = fov; }

            virtual float viewDistance() const { return _view_distance; }
            virtual void setViewDistance(float distance) { _view_distance = distance; }

            virtual void setPosition(const CameraPosition &pos, bool instant = false) { (instant ? _position_current : _position_target) = pos; }
            virtual CameraPosition position(bool instant = false) { return (instant ? _position_current : _position_target).getModified(); }

            virtual void update(double delta_time);

            virtual glm::mat4 viewMatrix() const;
            virtual glm::mat4 projectionMatrix(int width, int height) const;

            static void cleanup();
            static void updateAll(double delta_time);

        protected:
            MoveType _move_type = INSTANT;
            float _move_speed = 1.0f;
            float _fov = 45.0f;
            float _view_distance = 1000.0f;
            Util::BufferData<CameraPosition> _position_current;
            Util::BufferData<CameraPosition> _position_target;
        };

        class RendererInternal : public Renderer {
        public:
            RendererInternal(Program* program);

            void doPhase(RenderCyclePhase phase, WindowInternal &window) const;

            virtual Program* program() const { return _program; }

            virtual void setRenderCycleFunction(RenderCyclePhase phase, RenderCycleFunc function);

            virtual void setUniformName(MatrixUniformType type, const std::string &name);
            virtual std::string uniformName(MatrixUniformType type) const;

            virtual void setTextureName(SWMuint active, const std::string &name);
            virtual void setTextureName(const Texture::Type::MapType &type, const std::string &name) {
                setTextureName(type.active(), name);
            }

            virtual std::string textureName(SWMuint active) const;
            virtual std::string textureName(const Texture::Type::MapType &type) const {
                return textureName(type.active());
            }

            virtual void insertUniform(Uniform &uniform);

            virtual void bindUniformsMatrix(const Window &window) const;
            virtual void bindUniformsTexture() const;
            virtual void bindUniformsCustom() const;

            static void cleanup();

        protected:
            Program* _program;

            std::map<RenderCyclePhase, RenderCycleFunc> _cycle_func_map;
            std::map<MatrixUniformType, std::string> _uniform_map_matrix;
            std::map<SWMuint, std::string> _uniform_map_texture;

            std::set<Uniform*> _uniform_set;
        };
    }
}