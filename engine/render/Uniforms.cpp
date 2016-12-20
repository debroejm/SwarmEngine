#include "../Render.h"

namespace Swarm {
    namespace Render {
        namespace Uniforms {

            void Uniform::setf (GLsizei count, GLsizei stride, GLfloat *data) {
                if(hasData) clear();
                type = F;
                this->data.f = {count, stride, new GLfloat[count*stride]};
                for(int i = 0; i < count; i++) {
                    for(int j = 0; j < stride; j++) {
                        int index = i*stride + j;
                        this->data.f.data[index] = data[index];
                    }
                }
                hasData = true;
            }

            void Uniform::seti (GLsizei count, GLsizei stride, GLint   *data) {
                if(hasData) clear();
                type = I;
                this->data.i = {count, stride, new GLint[count*stride]};
                for(int i = 0; i < count; i++) {
                    for(int j = 0; j < stride; j++) {
                        int index = i*stride + j;
                        this->data.i.data[index] = data[index];
                    }
                }
                hasData = true;
            }

            void Uniform::setui(GLsizei count, GLsizei stride, GLuint  *data) {
                if(hasData) clear();
                type = UI;
                this->data.ui = {count, stride, new GLuint[count*stride]};
                for(int i = 0; i < count; i++) {
                    for(int j = 0; j < stride; j++) {
                        int index = i*stride + j;
                        this->data.ui.data[index] = data[index];
                    }
                }
                hasData = true;
            }

            void Uniform::setm (GLsizei count, GLsizei width, GLsizei height, GLfloat *data) {
                if(hasData) clear();
                type = M;
                this->data.m = {count, width, height, new GLfloat[count*width*height]};
                for(int i = 0; i < count; i++) {
                    for(int x = 0; x < width; x++) {
                        for(int y = 0; y < height; y++) {
                            int index = i*width*height + x*height + y;
                            this->data.m.data[index] = data[index];
                        }
                    }
                }
                hasData = true;
            }
            
            void Uniform::bind(Renderer &render) {
                if(!hasData) return;
                Program &program = render.getProgram();
                switch(type) {
                    case Uniforms::Uniform::F: {
                        switch(data.f.stride) {
                            case 1:  glUniform1fv(program.getUniformID(name), data.f.count, data.f.data); break;
                            case 2:  glUniform2fv(program.getUniformID(name), data.f.count, data.f.data); break;
                            case 3:  glUniform3fv(program.getUniformID(name), data.f.count, data.f.data); break;
                            default: glUniform4fv(program.getUniformID(name), data.f.count, data.f.data); break;
                        } } break;
                    case Uniforms::Uniform::I: {
                        switch(data.i.stride) {
                            case 1:  glUniform1iv(program.getUniformID(name), data.i.count, data.i.data); break;
                            case 2:  glUniform2iv(program.getUniformID(name), data.i.count, data.i.data); break;
                            case 3:  glUniform3iv(program.getUniformID(name), data.i.count, data.i.data); break;
                            default: glUniform4iv(program.getUniformID(name), data.i.count, data.i.data); break;
                        } } break;
                    case Uniforms::Uniform::UI: {
                        switch(data.ui.stride) {
                            case 1:  glUniform1uiv(program.getUniformID(name), data.ui.count, data.ui.data); break;
                            case 2:  glUniform2uiv(program.getUniformID(name), data.ui.count, data.ui.data); break;
                            case 3:  glUniform3uiv(program.getUniformID(name), data.ui.count, data.ui.data); break;
                            default: glUniform4uiv(program.getUniformID(name), data.ui.count, data.ui.data); break;
                        } } break;
                    case Uniforms::Uniform::M: {
                        switch(data.m.width) {
                            case 2: {
                                switch(data.m.height) {
                                    case 2:  glUniformMatrix2fv  (program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                    case 3:  glUniformMatrix2x3fv(program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                    default: glUniformMatrix2x4fv(program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                } } break;
                            case 3: {
                                switch(data.m.height) {
                                    case 2:  glUniformMatrix3x2fv(program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                    case 3:  glUniformMatrix3fv  (program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                    default: glUniformMatrix3x4fv(program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                } } break;
                            default: {
                                switch(data.m.height) {
                                    case 2:  glUniformMatrix4x2fv(program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                    case 3:  glUniformMatrix4x3fv(program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                    default: glUniformMatrix4fv  (program.getUniformID(name), data.m.count, GL_FALSE, data.m.data); break;
                                } } break;
                        } } break;
                    default: break;
                }
            }

            void Uniform::clear() {
                switch(type) {
                    case F:  delete data.f.data;  data.f.data = nullptr;  break;
                    case I:  delete data.i.data;  data.i.data = nullptr;  break;
                    case UI: delete data.ui.data; data.ui.data = nullptr; break;
                    case M:  delete data.m.data;  data.m.data = nullptr;  break;
                    default: break;
                }
                hasData = false;
            }
        }
    }
}