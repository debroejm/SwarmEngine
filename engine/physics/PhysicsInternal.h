/*
 * Copyright 2017 James De Broeck
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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