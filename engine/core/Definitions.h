#ifndef SWARMENGINE_DEFINITIONS_H
#define SWARMENGINE_DEFINITIONS_H

// ***********************
//  Namespace Definitions
// ***********************

#ifndef SWARMENGINE_DEFINITIONS_H
#pragma lhgMultiOn(SwarmEngine, LHG_NONE, 101)
#if defined(_WIN32) || defined(__WIN32__)
#if defined(SwarmEngine_EXPORTS)
#define DLLFUNC __declspec(dllexport)
#else
#define DLLFUNC __declspec(dllimport)
#endif
#else
#define DLLFUNC
#endif

// External Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
#include <map>
#include <unordered_map>
using namespace std;

#include <stdlib.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#pragma lhgMultiOff()
#endif

#pragma lhgMultiOn(SwarmEngine, LHG_NONE, 100)
#pragma lhgFlagOn(PROCESS_DEFINES)
#define ENGINE_NAMESPACE Swarm

#define ENGINE_NAMESPACE_ANIM Anim
#define ENGINE_NAMESPACE_CONFIG Config
#define ENGINE_NAMESPACE_GLFW GLFW
#define ENGINE_NAMESPACE_INIT Init
#define ENGINE_NAMESPACE_INPUT Input
#define ENGINE_NAMESPACE_LOG Logging
#define ENGINE_NAMESPACE_MODEL Model
#define ENGINE_NAMESPACE_RENDER Render
#define ENGINE_NAMESPACE_SHADER Shader
#define ENGINE_NAMESPACE_TEXTURE Texture
#define ENGINE_NAMESPACE_WORLD World
#pragma lhgFlagOff(PROCESS_DEFINES)
#pragma lhgMultiOff()

#endif //SWARMENGINE_DEFINITIONS_H
