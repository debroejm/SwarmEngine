#include "api/Exception.h"

namespace Swarm {
    namespace Exception {

        TextureException::TextureException(Type type, const std::string &message)
                : runtime_error(message), _type(type) {};

        TextureException TextureException::FileLoadPNG(const std::string &path, const std::string &err) {
            return TextureException(FILE_LOAD_PNG,
                                    "Failed to load PNG file '"+ path +"':\n" + err);
        }

    }
}