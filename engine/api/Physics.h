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

        void init();
        void cleanup();

        class PhysicsObjectInternal;
        //class PhysicsObject : public PhysicsObjectInternal {
        //public:

        //};
    }
}