#include "../headers/Animation.h"

namespace ENGINE_NAMESPACE {
    namespace Animation {

        Rigging::Rigging(Model &input) {
            model = &input;
        }


        RiggingHumanoid::RiggingHumanoid(Model &input): Rigging(input) {
            leftKnee = input.getBone("KneeL");
            leftFoot = input.getBone("FootL");
            rightKnee = input.getBone("KneeR");
            rightFoot = input.getBone("FootR");
        }

        void RiggingHumanoid::apply() {
            if(leftKnee != NULL) leftKnee->rotatePosition(radians(-35.0f), vec3(1.0f, 0.0f, 0.2f));
            if(rightKnee != NULL) rightKnee->rotatePosition(radians(-35.0f), vec3(1.0f, 0.0f, -0.2f));
            if(leftFoot != NULL) leftFoot->rotatePosition(radians(60.0f), vec3(1.0f, 0.0f, 0.0f));
            if(rightFoot != NULL) rightFoot->rotatePosition(radians(60.0f), vec3(1.0f, 0.0f, 0.0f));
        }

        void RiggingHumanoid::update(float delta) {

        }

    }
}