#pragma once

#define SWARM_INCLUDE_GLM
#define SWARM_BOOST_AVAILABLE
#define SWARM_REQUEST_BOOST_MUTEX
#include "api/Physics.h"

#include "api/Util.h"


// ************
//  Code Begin
// ************

namespace Swarm {
    namespace Physics {

        struct RigidBodyProperties {
            float mass;
        };

        struct RigidBodyMotion {
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 velocity_linear;
            glm::vec3 velocity_angular;
        };

        struct RigidBodyForces {
            glm::vec3 force;
            glm::vec3 torque;
        };

        class PhysicsObjectInternal {
        protected:
            Util::BufferData<RigidBodyProperties> _properties;
            Util::BufferData<RigidBodyMotion> _motion;
            Util::BufferData<RigidBodyForces> _forces;
        public:
            PhysicsObjectInternal();
        };

        void startPhysicsThread();
        void stopPhysicsThread();

    }
}