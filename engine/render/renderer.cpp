#include "RenderInternal.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Render {

        std::set<RendererInternal*> registered_renderers;

        void RendererInternal::cleanup() {
            for(RendererInternal* renderer : registered_renderers) delete renderer;
        }

        Renderer* Renderer::create(Program* program) {
            if(program == nullptr) return nullptr;
            RendererInternal* renderer = new RendererInternal(program);
            registered_renderers.insert(renderer);
            return renderer;
        }

        void defaultRenderCycle_start(const Renderer &renderer, const Window &window) {

            // Bind Program
            glUseProgram(renderer.program()->ID());
            renderer.program()->clearUniformCache();

            // Set Uniforms
            renderer.bindUniformsMatrix(window);
            renderer.bindUniformsTexture();
            renderer.bindUniformsCustom();

        }

        void defaultRenderCycle_end(const Renderer &renderer, const Window &window) {

            // Unbind Program
            glUseProgram(0);
        }

        RendererInternal::RendererInternal(Program* program) : _program(program) {

            // Set Default Matrix Uniforms
            _uniform_map_matrix[MODEL] = "_m";
            _uniform_map_matrix[VIEW] = "_v";
            _uniform_map_matrix[PROJECTION] = "_p";

            // Set Default Cycle Phase Functions
            _cycle_func_map[START] = defaultRenderCycle_start;
            _cycle_func_map[END] = defaultRenderCycle_end;
        }

        void RendererInternal::doPhase(RenderCyclePhase phase, WindowInternal &window) const {
            if(_cycle_func_map.count(phase) < 1) return;
            if(_cycle_func_map.at(phase) == nullptr) return;
            _cycle_func_map.at(phase)(*this, Window(&window));
        }

        void RendererInternal::bindUniformsMatrix(const Window &window) const {
            if(window.camera() == nullptr) return;
            if(_uniform_map_matrix.count(VIEW))       glUniformMatrix4fv(_program->uniform(_uniform_map_matrix.at(VIEW)),       1, GL_FALSE, &window.camera()->viewMatrix()      [0][0]);
            if(_uniform_map_matrix.count(PROJECTION)) glUniformMatrix4fv(_program->uniform(_uniform_map_matrix.at(PROJECTION)), 1, GL_FALSE, &window.camera()->projectionMatrix(window.width(), window.height())[0][0]);
        }

        void RendererInternal::bindUniformsTexture() const {
            for(auto && iter : _uniform_map_texture)
                glUniform1i(_program->uniform(iter.second), iter.first);
        }

        void RendererInternal::bindUniformsCustom() const {
            for(Uniform* uniform : _uniform_set)
                uniform->bind(*this);
        }

        void RendererInternal::setRenderCycleFunction(RenderCyclePhase phase, RenderCycleFunc function) {
            _cycle_func_map[phase] = function;
        }

        void RendererInternal::setUniformName(MatrixUniformType type, const std::string &name) {
            _uniform_map_matrix[type] = name;
        }

        std::string RendererInternal::uniformName(MatrixUniformType type) const {
            if(_uniform_map_matrix.count(type)) return _uniform_map_matrix.at(type);
            else return "";
        }

        void RendererInternal::setTextureName(SWMuint active, const std::string &name) {
            _uniform_map_texture[active] = name;
        }

        std::string RendererInternal::textureName(SWMuint active) const {
            if(_uniform_map_texture.count(active)) return _uniform_map_texture.at(active);
            else return "";
        }

        void RendererInternal::insertUniform(Uniform &uniform) {
            _uniform_set.insert(&uniform);
        }

    }
}