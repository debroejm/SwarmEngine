#include "../Render.h"

namespace Swarm {
    namespace Render {

        RenderObjectMultiPart::RenderObjectMultiPart(RenderObjectMulti &parent, Model::Model &model, Texture::Texture &texture, glm::mat4 matrix)
                : parent(parent), RenderObjectSimple(model, texture, matrix) {
            parent.add(*this);
        }

        void RenderObjectMulti::add(RenderObjectMultiPart &part) {
            parts.push_back(part);
        }
    }
}