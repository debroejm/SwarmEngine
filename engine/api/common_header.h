
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

}

#endif