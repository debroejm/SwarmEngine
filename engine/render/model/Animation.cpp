#include "../../Render.h"

namespace Swarm {
    namespace Anim {

        Rigging::Rigging(Model::Model &input) {
            model = &input;
        }


        /*
        RiggingHumanoid::RiggingHumanoid(Model::Model &input): Rigging(input) {
            leftKnee = input.getBone("KneeL");
            leftFoot = input.getBone("FootL");
            rightKnee = input.getBone("KneeR");
            rightFoot = input.getBone("FootR");
        }

        void RiggingHumanoid::apply() {
            if(leftKnee != NULL) leftKnee->rotatePosition(glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.2f));
            if(rightKnee != NULL) rightKnee->rotatePosition(glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, -0.2f));
            if(leftFoot != NULL) leftFoot->rotatePosition(glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            if(rightFoot != NULL) rightFoot->rotatePosition(glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        void RiggingHumanoid::update(float delta) {

        }
         */

    }
}