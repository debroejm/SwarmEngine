#include "RenderInternal.h"

#include "api/Logging.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Render {

        std::set<CameraInternal*> registered_cameras;
        
        Util::ReadWriteBuffer<CameraPosition> _static_camera_position_buffer;

        void CameraInternal::cleanup() {
            for(CameraInternal* camera : registered_cameras) delete camera;
        }

        Camera* Camera::create(float speed, MoveType type) {
            CameraInternal* camera = new CameraInternal(CameraPosition(glm::vec3(0), glm::vec3(0,0,1), glm::vec3(0,1,0)), speed, type);
            registered_cameras.insert(camera);
            return (Camera*)camera;
        }

        Camera* Camera::create(const CameraPosition &pos, float speed, MoveType type) {
            CameraInternal* camera = new CameraInternal(pos, speed, type);
            registered_cameras.insert(camera);
            return (Camera*)camera;
        }

        void CameraInternal::updateAll(double delta_time) {
            for(CameraInternal* camera : registered_cameras) camera->update(delta_time);
            _static_camera_position_buffer.flush();
        }

        CameraInternal::CameraInternal(const CameraPosition &pos, float speed, MoveType type)
                : _position_current(_static_camera_position_buffer, pos),
                  _position_target (_static_camera_position_buffer, pos),
                  _move_speed(speed), _move_type(type) {}

        void CameraInternal::update(double delta_time) {

            switch(_move_type) {
                case INSTANT:
                    _position_current = _position_target;
                    break;
                case SMOOTH:
                case LERP: {
                    // TODO: Introduce Actual Lerp
                    boost::lock_guard<boost::mutex> write_lock(_static_camera_position_buffer.write_mutex());
                    CameraPosition &current = _position_current.getModified(write_lock);
                    CameraPosition &target  = _position_target.getModified(write_lock);
                    if (current.position != target.position)
                        current.position = Util::movePoint(current.position.vec(),
                                                                      target.position.vec(),
                                                                      (float) (_move_speed * delta_time));
                    if (current.look_at != target.look_at)
                        current.look_at = Util::movePoint(current.look_at.vec(),
                                                                     target.look_at.vec(),
                                                                     (float) (_move_speed * delta_time));
                    if (current.up != target.up)
                        current.up = Util::movePoint(current.up.vec(), target.up.vec(),
                                                                (float) (_move_speed * delta_time));
                } break;
            }
        }

        glm::mat4 CameraInternal::viewMatrix() const {
            const CameraPosition current = _position_current.get();
            return glm::lookAt(current.position.vec(), current.look_at.vec(), current.up.vec());
        }

        glm::mat4 CameraInternal::projectionMatrix(int width, int height) const {
            return glm::perspective(_fov, (float)width / (float)height, 0.1f, _view_distance);
        }

    }
}