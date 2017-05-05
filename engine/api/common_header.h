
// *********************
//  3rd Party Libraries
// *********************
// Only include these if the developer wants to

#if defined(SWARM_INCLUDE_GLEW) || defined(SWARM_INCLUDE_GLFW)
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#if defined(SWARM_INCLUDE_GLFW)
#include <GLFW/glfw3.h>
#endif

#if defined(SWARM_INCLUDE_GLM)
#include <glm/glm.hpp>
#endif

#if defined(SWARM_INCLUDE_CL)
#include "CL/cl.h"
#endif



// *****************
//  Boost Libraries
// *****************
#if defined(SWARM_BOOST_AVAILABLE)

#if defined(SWARM_REQUEST_BOOST_MUTEX)
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#endif

#endif


// Include Guard is used instead of pragma once because the above includes
// can very depending on where this file is included from
#ifndef SWARM_TYPEDEF_INCLUDE_GUARD
#define SWARM_TYPEDEF_INCLUDE_GUARD

#include <string>

// *****************
//  Engine Typedefs
// *****************

namespace Swarm {

    #if defined(SWARM_INCLUDE_GLEW) || defined(SWARM_INCLUDE_GLFW)
    typedef     GLenum          SWMenum;
    typedef     GLuint          SWMuint;
    typedef     GLint           SWMint;
    typedef     GLsizei         SWMsizei;
    typedef     GLboolean       SWMboolean;
    typedef     GLfloat         SWMfloat;
    typedef     GLdouble        SWMdouble;
    #else
    typedef     unsigned int    SWMenum;
    typedef     unsigned int    SWMuint;
    typedef     int             SWMint;
    typedef     int             SWMsizei;
    typedef     unsigned char   SWMboolean;
    typedef     float           SWMfloat;
    typedef     double          SWMdouble;
    #endif

    enum MemoryPrefix {
        MEM_BYTE = 1,
        MEM_KB = MEM_BYTE*1024,
        MEM_MB = MEM_KB*1024,
        MEM_GB = MEM_MB*1024
    };

    enum BitWidth {
        BIT_8 = 1,
        BIT_16 = 2,
        BIT_32 = 4,
        BIT_64 = 8
    };

}

namespace std {
    inline string to_string(Swarm::MemoryPrefix pre) {
        switch(pre) {
            case Swarm::MEM_BYTE: return "B";
            case Swarm::MEM_KB: return "KB";
            case Swarm::MEM_MB: return "MB";
            case Swarm::MEM_GB: return "GB";
            default: return "";
        }
    }
    inline string to_string(Swarm::BitWidth width) {
        switch(width) {
            case Swarm::BIT_8: return "8-Bit";
            case Swarm::BIT_16: return "16-Bit";
            case Swarm::BIT_32: return "32-Bit";
            case Swarm::BIT_64: return "64-Bit";
            default: return "";
        }
    }
}

#endif