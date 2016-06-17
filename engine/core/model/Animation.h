#ifndef SWARMENGINE_ANIMATION_H
#define SWARMENGINE_ANIMATION_H

#include "Definitions.h"

#include "ModelLoading.h"

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_ANIM {

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_ANIM, -5)
        class Rigging {
        public:
            Rigging(ENGINE_NAMESPACE_MODEL::Model &input);
            virtual ~Rigging() {}

            virtual void apply() = 0;
            virtual void update(float delta) = 0;

        protected:
            ENGINE_NAMESPACE_MODEL::Model* model = NULL;
        };

        class RiggingHumanoid: public Rigging {
        public:
            RiggingHumanoid(ENGINE_NAMESPACE_MODEL::Model &input);

            void apply();
            void update(float delta);

        protected:
            ENGINE_NAMESPACE_MODEL::Bone* leftKnee = NULL;
            ENGINE_NAMESPACE_MODEL::Bone* rightKnee = NULL;
            ENGINE_NAMESPACE_MODEL::Bone* leftFoot = NULL;
            ENGINE_NAMESPACE_MODEL::Bone* rightFoot = NULL;
        };
        #pragma lhgMultiOff()

    }
}

#endif //SWARMENGINE_ANIMATION_H
