#include "RenderInternal.h"

#include "api/Core.h"
#include "api/Exception.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Render {

        // *****************
        //  Window Registry
        // *****************

        //std::map<boost::thread::id, const WindowInternal*> _static_active_window_map;
        GLFWwindow* _static_primary_context = nullptr;
        std::map<GLFWwindow*, WindowInternal*> _static_registered_windows;

        /*
        const Window* Window::current() {
            if(_static_active_window_map.count(boost::this_thread::get_id())) return _static_active_window_map.at(boost::this_thread::get_id());
            else return nullptr;
        }
         */

        void WindowInternal::cleanup() {
            for(auto && iter : _static_registered_windows) delete iter.second;
            glfwDestroyWindow(_static_primary_context);
        }

        void createMainThreadContext() {
            // Create New Window
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            _static_primary_context = glfwCreateWindow(1, 1, "Main Thread Context (THIS SHOULD NOT BE VISIBLE)", nullptr, nullptr);

            // Bind the Context
            glfwMakeContextCurrent(_static_primary_context);

            // GLEW initialization
            glewExperimental = GL_TRUE; // Needed for core profile
            GLenum err = glewInit();
            if( err != GLEW_OK ) throw Exception::WindowException::GlewInit("Main Thread Context (This is an Internal Error)", std::string((const char*)glewGetErrorString(err)));
        }



        // ******************
        //  Window Callbacks
        // ******************

        void callback_key(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            // Send keycode and other info to the input processing part of the engine
            if(action == GLFW_PRESS) Input::setKeyPressed(key, true);
            if(action == GLFW_RELEASE) Input::setKeyPressed(key, false);
        }

        void callback_windowSize(GLFWwindow* window, int width, int height) {
            _static_registered_windows[window]->_width = width;
            _static_registered_windows[window]->_height = height;
        }

        void callback_windowPosition(GLFWwindow* window, int x, int y) {
            _static_registered_windows[window]->_posX = x;
            _static_registered_windows[window]->_posY = y;
        }

        void callback_framebufferSize(GLFWwindow* window, int width, int height) {
            _static_registered_windows[window]->_framebuffer_width = width;
            _static_registered_windows[window]->_framebuffer_height = height;
            _static_registered_windows[window]->_queued_framebuffer_resize = true;
        }

        void callback_windowFocus(GLFWwindow* window, int focused) {
            _static_registered_windows[window]->_focused = focused;
        }



        // ****************************
        //  Window Thread-Run Function
        // ****************************

        std::set<WindowInternal*> _static_visible_windows;

        boost::mutex _static_window_queue_visible_mutex;
        std::set<WindowInternal*> _static_window_queue_show;
        std::set<WindowInternal*> _static_window_queue_hide;

        boost::mutex _static_window_context_mutex;
        boost::mutex &getWindowContextMutex() { return _static_window_context_mutex; }

        std::atomic<bool> _static_run_render_thread(false);
        struct ThreadFunctr_WindowRender {
            void operator()() {
                log_render(INFO) << "Starting Render Thread" << Flush();
                try {
                    while (_static_run_render_thread) {


                        _static_window_queue_visible_mutex.lock();

                        for (WindowInternal *window : _static_window_queue_hide) {
                            if (window == nullptr) continue;
                            log_render(INFO) << "Hiding Window '" << window->name() << "'" << Flush();
                            glfwHideWindow(window->window());
                            _static_visible_windows.erase(window);
                        }
                        _static_window_queue_hide.clear();

                        for (WindowInternal *window : _static_window_queue_show) {
                            if (window == nullptr) continue;
                            log_render(INFO) << "Showing Window '" << window->name() << "'" << Flush();
                            glfwShowWindow(window->window());
                            _static_visible_windows.insert(window);
                        }
                        _static_window_queue_show.clear();

                        _static_window_queue_visible_mutex.unlock();



                        _static_window_context_mutex.lock();
                        for (WindowInternal *window : _static_visible_windows) {

                            // Do Rendering
                            window->renderAll();
                        }
                        glfwMakeContextCurrent(nullptr);
                        _static_window_context_mutex.unlock();

                        log_render().flushAll();

                        boost::this_thread::interruption_point();
                    }
                } catch(std::exception &e) {
                    log_render(ERR) << "Render Thread Fatal Error: " << e.what() << Flush();
                }
                log_render(INFO) << "Stopping Render Thread" << Flush();
            }
        };

        boost::thread _static_render_thread;

        void startRenderThread() {
            _static_run_render_thread = true;
            _static_render_thread = boost::thread(ThreadFunctr_WindowRender());
        }

        void stopRenderThread() {
            _static_run_render_thread = false;
            _static_render_thread.interrupt();
            _static_render_thread.join();
        }

        void checkWindowCloseFlags() {
            for (auto && iter : _static_registered_windows) {
                // Check if the window has been scheduled to close. If it has, don't close, just hide it
                if (glfwWindowShouldClose(iter.first)) {
                    glfwSetWindowShouldClose(iter.first, false);
                    iter.second->setVisible(false);
                }
            }
        }



        // ************************
        //  WindowInternal Methods
        // ************************

        WindowInternal::~WindowInternal() {
            glfwDestroyWindow(_window);
        }

        WindowInternal::WindowInternal(unsigned int width, unsigned int height, const std::string &name)
                : _width(width), _height(height), _posX(0), _posY(0), _name(name),
                  _queued_visible(false), _queued_framebuffer_resize(false),
                  _focused(true) {

            // Context lock for Window Creation and Initialization
            boost::lock_guard<boost::mutex> lock(_static_window_context_mutex);

            glfwMakeContextCurrent(nullptr);

            // Create New Window
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            _window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, _static_primary_context);

            // Set Callbacks
            glfwSetKeyCallback(_window, callback_key);
            glfwSetWindowSizeCallback(_window, callback_windowSize);
            glfwSetWindowPosCallback(_window, callback_windowPosition);
            glfwSetFramebufferSizeCallback(_window, callback_framebufferSize);
            glfwSetWindowFocusCallback(_window, callback_windowFocus);

            // Bind the Context
            glfwMakeContextCurrent(_window);

            // GLEW initialization
            glewExperimental = GL_TRUE; // Needed for core profile
            GLenum err = glewInit();
            if( err != GLEW_OK ) throw Exception::WindowException::GlewInit(_name, std::string((const char*)glewGetErrorString(err)));

            // Set GL Flags
            glEnable(GL_DEPTH_TEST);    // Enable depth test
            glDepthFunc(GL_LESS);       // Accept fragment if it closer to the camera than the former one
            glEnable(GL_CULL_FACE);     // Cull triangles which normal is not towards the camera

            // Unbind the Context
            glfwMakeContextCurrent(_static_primary_context);
        }

        /*
        void WindowInternal::makeCurrent() const {
            glfwMakeContextCurrent(_window);
            _static_active_window_map[boost::this_thread::get_id()] = &(*this);
        }
         */

        void WindowInternal::setVisible(bool visible) {
            if((this->visible() || _queued_visible) == visible) return;
            boost::lock_guard<boost::mutex> lock(_static_window_queue_visible_mutex);
            if(visible) {
                _static_window_queue_show.insert(&(*this));
                _static_window_queue_hide.erase (&(*this));
                _queued_visible = true;
            } else {
                _static_window_queue_hide.insert(&(*this));
                _static_window_queue_show.erase (&(*this));
                _queued_visible = false;
            }
        }

        bool WindowInternal::visible() const {
            return glfwGetWindowAttrib(_window, GLFW_VISIBLE) > 0 || _queued_visible;
        }

        void WindowInternal::cursorPos(double &pos_x, double &pos_y) {
            glfwGetCursorPos(_window, &pos_x, &pos_y);
        }

        void WindowInternal::attachRenderer(const Renderer* renderer, RenderObjectCollection &collection, bool reverse) {
            if(renderer == nullptr) return;
            boost::lock_guard<boost::mutex> lock(_render_mutex);
            const RendererInternal* ri = (const RendererInternal*)renderer;
            if(reverse) _renderers.insert(_renderers.begin(), RenderAttachment{ri, &collection});
            else _renderers.push_back(RenderAttachment{ri, &collection});
        }

        void WindowInternal::setClearColor(float r, float g, float b, float a) {
            _clear_color = glm::vec4(r, g, b, a);
        }

        void WindowInternal::renderAll() {
            std::set<RenderObjectCollection*> render_collection_cache;

            // Make this Window's Context Current
            glfwMakeContextCurrent(_window);

            if(_queued_framebuffer_resize) {
                glViewport(0, 0, _framebuffer_width, _framebuffer_height);
                _queued_framebuffer_resize = false;
            }

            glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Lock Anything Render Related
            boost::lock_guard<boost::mutex> lock(_render_mutex);

            for(RenderAttachment render : _renderers) {
                if(render.renderer == nullptr || render.collection == nullptr) continue;

                // Check to Flush Render Collection
                if(render_collection_cache.count(render.collection) < 1) {
                    render.collection->flush();
                    render_collection_cache.insert(render.collection);
                }

                // Do Render
                render.renderer->doPhase(Renderer::START, *this);
                render.collection->render(*render.renderer);
                render.renderer->doPhase(Renderer::END, *this);

            }



            // Swap the OpenGL Buffers
            glfwSwapBuffers(_window);
        }



        // ****************
        //  Window Methods
        // ****************

        Window::Window() {
            _window = nullptr;
        }

        Window::Window(unsigned int width, unsigned int height, const std::string &name) {
            _window = new WindowInternal(width, height, name);
            _static_registered_windows[_window->window()] = _window;
        }

        Window::Window(const Window &rhs) {
            *this = rhs;
        }

        Window::Window(WindowInternal* window) {
            _window = window;
        }

        Window &Window::operator=(const Window &rhs) {
            _window = rhs._window;
            return *this;
        }

        bool Window::operator==(const Window &rhs) const {
            return _window == rhs._window;
        }

        int         Window::width()     const { return _window->width(); }
        int         Window::height()    const { return _window->height(); }
        int         Window::posX()      const { return _window->posX(); }
        int         Window::posY()      const { return _window->posY(); }
        std::string Window::name()      const { return _window->name(); }

        bool        Window::focused()   const { return _window->focused(); }

        void        Window::setVisible(bool visible)    { _window->setVisible(visible); }
        bool        Window::visible()   const { return _window->visible(); }

        void        Window::setCamera(Camera* camera)   { _window->setCamera(camera); }
        Camera*     Window::camera()    const { return _window->camera(); }

        void        Window::cursorPos(double &pos_x, double &pos_y) { _window->cursorPos(pos_x, pos_y); }

        GLFWwindow* Window::window()    const { return _window->window(); }
        Window::operator GLFWwindow*()  const { return _window->window(); }

        void Window::setClearColor(float r, float g, float b, float a) {
            _window->setClearColor(r, g, b, a);
        }

        void Window::attachRenderer(const Renderer* renderer, RenderObjectCollection &collection, bool reverse) {
            _window->attachRenderer(renderer, collection, reverse);
        }

    }
}