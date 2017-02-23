#include "RenderInternal.h"

#include "api/Logging.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Render {

        std::set<RenderObjectStatic*> _static_all_static_render_objects;

        void RenderObjectStatic::cleanup() {
            for(RenderObjectStatic* ro : _static_all_static_render_objects) delete ro;
        }

        RenderObject* RenderObject::createStaticRenderObject(Model::Model &model, Texture::Texture &texture,
                                                             float position_x, float position_y, float position_z,
                                                             float scale_x,    float scale_y,    float scale_z,
                                                             float rotate_x,   float rotate_y,   float rotate_z) {
            RenderObjectStatic* ro = new RenderObjectStatic(model, texture,
                                                            glm::vec3(position_x, position_y, position_z),
                                                            glm::vec3(scale_x, scale_y, scale_z),
                                                            glm::vec3(rotate_x, rotate_y, rotate_z));
            _static_all_static_render_objects.insert(ro);
            return ro;
        }

        RenderObject* RenderObject::createStaticRenderObject(Model::Model &model, Texture::Texture &texture, glm::vec3 position, glm::vec3 scale, glm::vec3 rotate) {
            RenderObjectStatic* ro = new RenderObjectStatic(model, texture, position, scale, rotate);
            _static_all_static_render_objects.insert(ro);
            return ro;
        }

        RenderObjectStatic::RenderObjectStatic(Model::Model &model, Texture::Texture &texture, glm::vec3 position, glm::vec3 scale, glm::vec3 rotate)
                : _model(model), _texture(texture) {
            _matrix = glm::translate(position);
            if(rotate.x != 0.0f || rotate.y != 0.0f || rotate.z != 0.0f) _matrix = _matrix * glm::rotate(1.0f, rotate);
            _matrix = _matrix * glm::scale(scale);
        }

        void RenderObjectStatic::render(const Renderer &renderer) const {
            if(renderer.program() == nullptr) return;

            // Bind the Model Matrix
            glUniformMatrix4fv(renderer.program()->uniform(renderer.uniformName(Renderer::MODEL)), 1, GL_FALSE, &_matrix[0][0]);

            // Each Object has its own VAO
            SWMuint vao = _model.vao();
            glBindVertexArray(vao);

            // Draw the triangles
            glDrawElements(
                    GL_TRIANGLES,                   // mode
                    (GLuint)_model.elementCount(), // count
                    GL_UNSIGNED_INT,                // type
                    (void*)0                        // element array buffer offset
            );

            glBindVertexArray(0);
        }



        Util::UIDPool roc_uids;

        RenderObjectCollection::RenderObjectCollection() {
            _uid = roc_uids.next();
        }

        RenderObjectCollection::~RenderObjectCollection() {
            roc_uids.free(_uid);
        }

        std::unordered_map<size_t, boost::mutex> _static_roc_mutex_map;

        void RenderObjectCollection::insert(RenderObject* object) {
            if(object == nullptr) return;
            _queue_insert.insert(object);
            _queue_erase.erase(object);
        }

        void RenderObjectCollection::erase(RenderObject* object) {
            if(object == nullptr) return;
            _queue_insert.erase(object);
            _queue_erase.insert(object);
        }

        void RenderObjectCollection::flush() {

            // Lock
            boost::lock_guard<boost::mutex> lock(_static_roc_mutex_map[_uid]);

            // Insert
            for(RenderObject* object : _queue_insert) {
                _data[object->texture()].insert(object);
            }
            _queue_insert.clear();

            // Erase
            for(RenderObject* object : _queue_erase) {
                Texture::Texture &tex = object->texture();
                if(_data.count(tex) > 0) _data[tex].erase(object);
            }
            _queue_erase.clear();
        }

        void RenderObjectCollection::clear() {

            // Lock
            boost::lock_guard<boost::mutex> lock(_static_roc_mutex_map[_uid]);

            // Clear
            _data.clear();
            _queue_insert.clear();
            _queue_erase.clear();
        }

        void RenderObjectCollection::render(const Renderer &renderer) {

            // Lock
            boost::lock_guard<boost::mutex> lock(_static_roc_mutex_map[_uid]);

            // Render All
            for(auto && iter : _data) {

                iter.first.bind();
                for(RenderObject* object : iter.second)
                    if(object != nullptr) object->render(renderer);
            }
        }

    }
}