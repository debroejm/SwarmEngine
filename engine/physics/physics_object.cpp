#include "PhysicsInternal.h"

#include "api/Logging.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Physics {

        Util::ReadWriteBuffer<RigidBodyProperties> _static_buffer_rigidbody_properties;
        Util::ReadWriteBuffer<RigidBodyForces> _static_buffer_rigidbody_forces;
        Util::ReadWriteBuffer<RigidBodyMotion> _static_buffer_rigidbody_motion;

        PhysicsObjectInternal::PhysicsObjectInternal()
                : _properties(_static_buffer_rigidbody_properties),
                  _forces(_static_buffer_rigidbody_forces),
                  _motion(_static_buffer_rigidbody_motion)
        {

        }

    }
}