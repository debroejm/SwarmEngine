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



// ***************
//  STD Libraries
// ***************



// ***************
//  Common Header
// ***************

#include "common_header.h"



// ***********
//  API Begin
// ***********

namespace Swarm {

    namespace Render {
        class RenderObject;
    }
    namespace Physics {
        class PhysicsObject;
    }

    namespace Game {

        class GameObject
                : public Render::RenderObject,
                  public Physics::PhysicsObject {

        };
    }
}