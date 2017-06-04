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