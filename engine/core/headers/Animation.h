#ifndef SWARMENGINE_ANIMATION_H
#define SWARMENGINE_ANIMATION_H

#include "../Definitions.h"

#include "ModelLoading.h"

using namespace ENGINE_NAMESPACE::Models;

namespace ENGINE_NAMESPACE {
    namespace Animation {

        class Rigging {
        public:
            Rigging(Model &input);
            virtual ~Rigging() {}

            virtual void apply() = 0;
            virtual void update(float delta) = 0;

        protected:
            Model* model = NULL;
        };

        class RiggingHumanoid: public Rigging {
        public:
            RiggingHumanoid(Model &input);

            void apply();
            void update(float delta);

        protected:
            Bone* leftKnee = NULL;
            Bone* rightKnee = NULL;
            Bone* leftFoot = NULL;
            Bone* rightFoot = NULL;
        };

    }
}

#endif //SWARMENGINE_ANIMATION_H
