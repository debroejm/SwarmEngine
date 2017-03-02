#pragma once



// ***************
//  STD Libraries
// ***************

#include <stdexcept>
#include <string>



// ***************
//  Common Header
// ***************

#include "common_header.h"



// ***********
//  API Begin
// ***********

namespace Swarm {
    namespace Exception {

        // TODO: Document Swarm Engine Exceptions

        class FileParseException : public std::runtime_error {
        public:
            std::string filepath() { return _filepath; }
            size_t linenum() { return _linenum; }

            static FileParseException Generic(const std::string &filepath, size_t linenum, const std::string &message);
            static FileParseException STDError(const std::string &filepath, size_t linenum, int errnum);

        protected:
            FileParseException(const std::string &filepath, size_t linenum, const std::string &message);
            std::string _filepath;
            size_t _linenum;
        };

        class OBJParseException : public FileParseException {
        public:
            enum Type {
                ARGUMENT_COUNT,
                UV_FOUND,
                FACE_COUNT
            };

            Type type() { return _type; }

            static OBJParseException ArgumentCount(const std::string &filepath, size_t linenum, const std::string &value_type, size_t found_count, size_t correct_count);
            static OBJParseException UVFound(const std::string &filepath, size_t linenum);
            static OBJParseException FaceCount(const std::string &filepath, size_t linenum, size_t found_count);

        protected:
            OBJParseException(Type _type, const std::string &filepath, size_t linenum, const std::string &message);
            Type _type;
        };

        class ModelLoadingException : public std::runtime_error {
        public:
            enum Type {
                FILE_LOAD,
                INDEX_OUT_OF_BOUNDS,
                PARSE_FAIL
            };

            Type type() { return _type; }

            static ModelLoadingException FileLoad(const std::string& path);
            static ModelLoadingException IndexOutOfBounds(const std::string& type, size_t inv_index, size_t max_index);
            static ModelLoadingException ParseFail(std::exception ex_list[], size_t count);

        protected:
            ModelLoadingException(Type type, const std::string &message);
            Type _type;
        };

        class RenderProgramException : public std::runtime_error {
        public:
            enum Type {
                LINK
            };

            Type type() { return _type; }

            static RenderProgramException Link(SWMuint ID, const std::string &err);

        protected:
            RenderProgramException(Type type, const std::string &message);
            Type _type;
        };

        class RenderShaderException : public std::runtime_error {
        public:
            enum Type {
                FILE_LOAD,
                COMPILE
            };

            Type type() { return _type; }

            static RenderShaderException FileLoad(const std::string &path);
            static RenderShaderException Compile(SWMuint ID, const std::string &err);

        protected:
            RenderShaderException(Type type, const std::string &message);
            Type _type;
        };

        class TextureException : public std::runtime_error {
        public:
            enum Type {
                FILE_LOAD_PNG
            };

            Type type() { return _type; }

            static TextureException FileLoadPNG(const std::string &path, const std::string &err);

        protected:
            TextureException(Type type, const std::string &message);
            Type _type;
        };

        class WindowException : public std::runtime_error {
        public:
            enum Type {
                GLEW_INIT
            };

            Type type() { return _type; }

            static WindowException GlewInit(const std::string &window_name, const std::string &err);

        protected:
            WindowException(Type type, const std::string &message);
            Type _type;
        };

        class CLObjectCreationException : public std::runtime_error {
        public:
            enum Type {
                BUFFER,
                KERNEL,
                PROGRAM,
                PROGRAM_BUILD,
                COMMAND_QUEUE
            };

            Type type() { return _type; }

            // TODO: Convert all CL Object Creation errors to exceptions
            static CLObjectCreationException Buffer(int err_code);
            static CLObjectCreationException Kernel(int err_code, const std::string &kernel_name);
            static CLObjectCreationException Program(int err_code);
            static CLObjectCreationException ProgramBuild(int err_code);
            static CLObjectCreationException CommandQueue(int err_code);

        protected:
            CLObjectCreationException(Type type, const std::string &message);
            Type _type;
        };

    }
}