#include "api/Util.h"

#include <stdio.h>

namespace Swarm {
    namespace Util {

        size_t UIDPool::next() {
            if(_free_ids.empty()) return _next++;
            else {
                size_t id = *_free_ids.begin();
                if(id < _next) {
                    _free_ids.erase(_free_ids.begin());
                    return id;
                } else if(id == _next) {
                    _free_ids.erase(_free_ids.begin());
                    return _next++;
                } else return _next++;
            }
        }

        void UIDPool::free(size_t id) {
            _free_ids.insert(id);
        }
    }
}