#define SWARM_INCLUDE_GLEW
#define SWARM_BOOST_AVAILABLE
#define SWARM_REQUEST_BOOST_MUTEX
#include "api/Render.h"
#undef SWARM_REQUEST_BOOST_MUTEX

namespace Swarm {
    namespace Render {

            std::unordered_map<std::string, boost::mutex> _static_uniform_mutex_map;

            void Uniform::setf (SWMsizei count, SWMsizei stride, SWMfloat *data) {
                boost::lock_guard<boost::mutex> lock(_static_uniform_mutex_map[_name]);
                if(_has_data) clear(lock);
                type = F;
                this->data.f = {count, stride, new SWMfloat[count*stride]};
                for(int i = 0; i < count; i++) {
                    for(int j = 0; j < stride; j++) {
                        int index = i*stride + j;
                        this->data.f.data[index] = data[index];
                    }
                }
                _has_data = true;
            }

            void Uniform::seti (SWMsizei count, SWMsizei stride, SWMint   *data) {
                boost::lock_guard<boost::mutex> lock(_static_uniform_mutex_map[_name]);
                if(_has_data) clear(lock);
                type = I;
                this->data.i = {count, stride, new SWMint[count*stride]};
                for(int i = 0; i < count; i++) {
                    for(int j = 0; j < stride; j++) {
                        int index = i*stride + j;
                        this->data.i.data[index] = data[index];
                    }
                }
                _has_data = true;
            }

            void Uniform::setui(SWMsizei count, SWMsizei stride, SWMuint  *data) {
                boost::lock_guard<boost::mutex> lock(_static_uniform_mutex_map[_name]);
                if(_has_data) clear(lock);
                type = UI;
                this->data.ui = {count, stride, new SWMuint[count*stride]};
                for(int i = 0; i < count; i++) {
                    for(int j = 0; j < stride; j++) {
                        int index = i*stride + j;
                        this->data.ui.data[index] = data[index];
                    }
                }
                _has_data = true;
            }

            void Uniform::setm (SWMsizei count, SWMsizei width, SWMsizei height, SWMfloat *data) {
                boost::lock_guard<boost::mutex> lock(_static_uniform_mutex_map[_name]);
                if(_has_data) clear(lock);
                type = M;
                this->data.m = {count, width, height, new SWMfloat[count*width*height]};
                for(int i = 0; i < count; i++) {
                    for(int x = 0; x < width; x++) {
                        for(int y = 0; y < height; y++) {
                            int index = i*width*height + x*height + y;
                            this->data.m.data[index] = data[index];
                        }
                    }
                }
                _has_data = true;
            }
            
            void Uniform::bind(const Renderer &render) {
                if(!_has_data) return;
                boost::lock_guard<boost::mutex> lock(_static_uniform_mutex_map[_name]);
                Program* program = render.program();
                switch(type) {
                    case Uniform::F: {
                        switch(data.f.stride) {
                            case 1:  glUniform1fv(program->uniform(_name), data.f.count, data.f.data); break;
                            case 2:  glUniform2fv(program->uniform(_name), data.f.count, data.f.data); break;
                            case 3:  glUniform3fv(program->uniform(_name), data.f.count, data.f.data); break;
                            default: glUniform4fv(program->uniform(_name), data.f.count, data.f.data); break;
                        } } break;
                    case Uniform::I: {
                        switch(data.i.stride) {
                            case 1:  glUniform1iv(program->uniform(_name), data.i.count, data.i.data); break;
                            case 2:  glUniform2iv(program->uniform(_name), data.i.count, data.i.data); break;
                            case 3:  glUniform3iv(program->uniform(_name), data.i.count, data.i.data); break;
                            default: glUniform4iv(program->uniform(_name), data.i.count, data.i.data); break;
                        } } break;
                    case Uniform::UI: {
                        switch(data.ui.stride) {
                            case 1:  glUniform1uiv(program->uniform(_name), data.ui.count, data.ui.data); break;
                            case 2:  glUniform2uiv(program->uniform(_name), data.ui.count, data.ui.data); break;
                            case 3:  glUniform3uiv(program->uniform(_name), data.ui.count, data.ui.data); break;
                            default: glUniform4uiv(program->uniform(_name), data.ui.count, data.ui.data); break;
                        } } break;
                    case Uniform::M: {
                        switch(data.m.width) {
                            case 2: {
                                switch(data.m.height) {
                                    case 2:  glUniformMatrix2fv  (program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                    case 3:  glUniformMatrix2x3fv(program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                    default: glUniformMatrix2x4fv(program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                } } break;
                            case 3: {
                                switch(data.m.height) {
                                    case 2:  glUniformMatrix3x2fv(program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                    case 3:  glUniformMatrix3fv  (program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                    default: glUniformMatrix3x4fv(program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                } } break;
                            default: {
                                switch(data.m.height) {
                                    case 2:  glUniformMatrix4x2fv(program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                    case 3:  glUniformMatrix4x3fv(program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                    default: glUniformMatrix4fv  (program->uniform(_name), data.m.count, GL_FALSE, data.m.data); break;
                                } } break;
                        } } break;
                    default: break;
                }
            }

            void Uniform::clear() {
                boost::lock_guard<boost::mutex> lock(_static_uniform_mutex_map[_name]);
                clear(lock);
            }

        void Uniform::clear(boost::lock_guard<boost::mutex>&) {
            switch(type) {
                case F:  delete data.f.data;  data.f.data = nullptr;  break;
                case I:  delete data.i.data;  data.i.data = nullptr;  break;
                case UI: delete data.ui.data; data.ui.data = nullptr; break;
                case M:  delete data.m.data;  data.m.data = nullptr;  break;
                default: break;
            }
            _has_data = false;
        }
    }
}