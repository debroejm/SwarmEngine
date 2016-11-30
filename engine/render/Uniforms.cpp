#include "../Render.h"

namespace Swarm {
    namespace Render {
        namespace Uniforms {

            void Uniform::setf (Renderer &render, GLsizei count, GLsizei stride, GLfloat *data) {
                type = F;
                this->data.f = {count, stride, data};
                render.markUniformDirty(*this);
            }

            void Uniform::seti (Renderer &render, GLsizei count, GLsizei stride, GLint   *data) {
                type = I;
                this->data.i = {count, stride, data};
                render.markUniformDirty(*this);
            }

            void Uniform::setui(Renderer &render, GLsizei count, GLsizei stride, GLuint  *data) {
                type = UI;
                this->data.ui = {count, stride, data};
                render.markUniformDirty(*this);
            }

            void Uniform::setm (Renderer &render, GLsizei count, GLsizei width, GLsizei height, GLfloat *data) {
                type = M;
                this->data.m = {count, width, height, data};
                render.markUniformDirty(*this);
            }
        }
    }
}