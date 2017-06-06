#define SWARM_INCLUDE_GLEW
#include "RenderInternal.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Render {

        Log _static_log_render("Render");

        Log &log_render() { return _static_log_render; }
        Log &log_render(Logging::LogSeverity severity) { return _static_log_render(severity); }

        void init() {

            log_render().initFile();

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