#define SWARM_INCLUDE_GLEW
#include "RenderInternal.h"

namespace Swarm {
    namespace Render {

        void init() {

            createMainThreadContext();
            startRenderThread();

        }

        void cleanup() {

            stopRenderThread();

            Render::RenderObjectStatic::cleanup();
            Render::CameraInternal::cleanup();
            Render::WindowInternal::cleanup();
            Render::ProgramInternal::cleanup();
            Render::ShaderInternal::cleanup();

        }
    }
}