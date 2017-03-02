#include "api/Exception.h"

#include <cstring>

namespace Swarm {
    namespace Exception {

        FileParseException::FileParseException(const std::string &filepath, size_t linenum, const std::string &message)
                : runtime_error("["+std::to_string(linenum)+"] "+message), _filepath(filepath), _linenum(linenum) {}

        FileParseException FileParseException::Generic(const std::string &filepath, size_t linenum, const std::string &message) {
            return FileParseException(filepath, linenum, message);
        }

        FileParseException FileParseException::STDError(const std::string &filepath, size_t linenum, int errnum) {
            return FileParseException(filepath, linenum,
                                      "Problem reading file: " + std::string(std::strerror(errnum)));
        }

        OBJParseException::OBJParseException(Type type, const std::string &filepath, size_t linenum, const std::string &message)
                : FileParseException(filepath, linenum, message), _type(type) {}

        OBJParseException OBJParseException::ArgumentCount(const std::string &filepath, size_t linenum, const std::string &value_type, size_t found_count, size_t correct_count) {
            return OBJParseException(ARGUMENT_COUNT, filepath, linenum,
                                     "Incorrect OBJ " + value_type + " Argument Count: " + std::to_string(found_count)
                                     + " instead of " + std::to_string(correct_count));
        }

        OBJParseException OBJParseException::UVFound(const std::string &filepath, size_t linenum) {
            return OBJParseException(UV_FOUND, filepath, linenum,
                                     "Found OBJ UV Index when previous Faces had no UVs");
        }

        OBJParseException OBJParseException::FaceCount(const std::string &filepath, size_t linenum, size_t found_count) {
            return OBJParseException(FACE_COUNT, filepath, linenum,
                                     "Incorrect OBJ Face Argument Count: " + std::to_string(found_count)
                                     + " instead of 9 or 12 (3 or 4 triplets)");
        }

    }
}