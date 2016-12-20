#include "../Render.h"

#include <algorithm>

namespace Swarm {
    namespace Render {

        RenderObjectMultiPart::RenderObjectMultiPart(RenderObjectMulti &parent, Model::Model &model, Texture::Texture &texture, glm::mat4 matrix)
                : parent(parent), RenderObject(model, texture, matrix) {
            parent.add(*this);
        }

        void RenderObjectMulti::add(RenderObjectMultiPart &part) {
            parts.push_back(part);
        }

        void RenderObjectCollection::add(RenderObject &object) {
            std::vector<RenderObject*> &list = data[object.getTexture()];
            list.push_back(&object);
        }

        void RenderObjectCollection::remove(RenderObject &object) {
            Texture::Texture &tex = object.getTexture();
            if(data.count(tex) > 0) {
                std::vector<RenderObject*> &list = data[tex];
                for(int i = list.size()-1; i >= 0; i--) {
                    // Should not be NULL in the first place, so no need for nullptr check
                    if(*list[i] == object) list.erase(list.begin()+i);
                }
            }
        }

        void RenderObjectCollection::clear() {
            data.clear();
        }

        std::vector<RenderObject*> RenderObjectCollection::getList(const Texture::Texture &tex) {
            if(data.count(tex) > 0) return data[tex];
            else return std::vector<RenderObject*>();
        }
    }
}