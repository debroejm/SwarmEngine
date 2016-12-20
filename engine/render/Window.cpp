#include "../Render.h"

#include "../Core.h"
#include "../core/GLFWCallbacks.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Render {

        Window* active_window = nullptr;

        Window::Window(int width, int height, RenderObjectCollection &renderObjects, const char* name)
                : renderObjects(renderObjects) {
            GLFWwindow* prevWindow = nullptr;
            if(active_window != nullptr) prevWindow = active_window->getWindow();
            window = glfwCreateWindow(width, height, name, NULL, prevWindow);
            glfwSetKeyCallback(window, GLFW::key_callback);

            // Initialize the Context
            makeCurrent();

            // GLEW initialization
            glewExperimental = GL_TRUE; // Needed for core profile
            GLenum err = glewInit();
            if (err != GLEW_OK) {
                Log::log_core(FATAL) << "Failed to Initialize GLEW: " << (const char*)(glewGetErrorString(err));
                std::exit(-1); // TODO: Replace exit(-1) with Custom Exception
            }

            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS);
            // Cull triangles which normal is not towards the camera
            glEnable(GL_CULL_FACE);

        }

        int Window::getWidth() const{
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            return width;
        }

        int Window::getHeight() const{
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            return height;
        }

        void Window::getSize(int &width, int &height) const {
            glfwGetFramebufferSize(window, &width, &height);
        }

        void Window::makeCurrent() {
            glfwMakeContextCurrent(window);
            active_window = &(*this);
        }

        Window* Window::getCurrent() {
            return active_window;
        }

    }
}