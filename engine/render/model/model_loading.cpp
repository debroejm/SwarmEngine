#define SWARM_INCLUDE_GLM
#include "api/Render.h"

#include "api/Logging.h"
#include "api/Exception.h"

#include <regex>

using namespace Swarm::Logging;
using namespace Swarm::Exception;

namespace Swarm {
    namespace Model {

        std::regex pattern_v("v\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s*");
        std::regex pattern_vt("vt\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s*");
        std::regex pattern_vn("vn\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s*");
        std::regex pattern_f("f\\s+(\\d+)\\/(\\d*)\\/(\\d+)\\s+(\\d+)\\/(\\d*)\\/(\\d+)\\s+(\\d+)\\/(\\d*)\\/(\\d+)(?:\\s+(\\d+)\\/(\\d*)\\/(\\d+))?\\s*");

        RawModelData loadFromOBJ(const char * path, const Type::DataType &vertex_type, const Type::DataType &uv_type, const Type::DataType &normal_type) {

            Log::log_render(INFO) << "Loading OBJ File: " << path;

            FILE * file = fopen(path, "r");
            if( file == NULL ) {
                getchar();
                throw ModelLoadingException::FileLoad(std::string(path));
            }

            std::vector<unsigned int> indices_vertex, indices_uv, indices_normal;
            std::vector<glm::vec3> obj_vertices;
            std::vector<glm::vec2> obj_uvs;
            std::vector<glm::vec3> obj_normals;

            bool noUV = false;
            unsigned int lineNum = 0;
            char line_c[1024];
            std::vector<OBJParseException> exceptions;
            while( 1 ) {

                const char* nullCheck = fgets(line_c, 1024, file);
                lineNum++;
                if( ferror(file) )
                    throw FileParseException::STDError(std::string(path), lineNum, errno);
                else if( feof(file) && nullCheck == NULL ) break;

                std::string line(line_c);
                if(line.size() < 1) continue;

                char lineHeader[1024];
                if(sscanf(line_c, "%s", lineHeader) < 1) continue;

                // Vertex : [x y z]
                if( strcmp( lineHeader, "v"  ) == 0 ) {
                    std::smatch m;
                    if(std::regex_match(line, m, pattern_v)) obj_vertices.push_back(glm::vec3(stof(m[1]), stof(m[2]), stof(m[3])));
                    else exceptions.push_back(OBJParseException::ArgumentCount(std::string(path), lineNum, "Vertex", m.size()-1, 3));
                } else
                // UV : [u v]
                if( strcmp( lineHeader, "vt" ) == 0 ) {
                    std::smatch m;
                    if(std::regex_match(line, m, pattern_vt)) obj_uvs.push_back(glm::vec2(stof(m[1]), stof(m[2])));
                    else exceptions.push_back(OBJParseException::ArgumentCount(std::string(path), lineNum, "UV", m.size()-1, 2));
                } else
                // Normal : [x y z]
                if( strcmp( lineHeader, "vn" ) == 0 ) {
                    std::smatch m;
                    if(std::regex_match(line, m, pattern_vn)) obj_normals.push_back(glm::vec3(stof(m[1]), stof(m[2]), stof(m[3])));
                    else exceptions.push_back(OBJParseException::ArgumentCount(std::string(path), lineNum, "Normal", m.size()-1, 3));
                } else
                // Face : [v0/(u0)/n0 v1/(u1)/n1 v2/(u2)/n2 (v3/(u3)/n2)]
                if( strcmp( lineHeader, "f" ) == 0 ) {
                    std::smatch m;
                    std::regex_match(line, m, pattern_f);
                    if(m.size() == 10 || m.size() == 13) { // Triangles
                        indices_vertex.push_back((unsigned short)stoi(m[1]));
                        indices_vertex.push_back((unsigned short)stoi(m[4]));
                        indices_vertex.push_back((unsigned short)stoi(m[7]));
                        if(m.size() == 13) {
                            // Must Triangulate a Square Face
                            indices_vertex.push_back((unsigned short)stoi(m[1]));
                            indices_vertex.push_back((unsigned short)stoi(m[7]));
                            indices_vertex.push_back((unsigned short)stoi(m[10]));
                        }
                        bool uv_mismatch = false;
                        if(m[2].length() > 0) {
                            if (noUV) uv_mismatch = true;
                            else indices_uv.push_back((unsigned short) stoi(m[2]));
                        } else noUV = true;
                        if(m[5].length() > 0) {
                            if (noUV) uv_mismatch = true;
                            else indices_uv.push_back((unsigned short) stoi(m[5]));
                        } else noUV = true;
                        if(m[8].length() > 0) {
                            if (noUV) uv_mismatch = true;
                            else indices_uv.push_back((unsigned short) stoi(m[8]));
                        } else noUV = true;
                        if(m.size() == 13) {
                            // Must Triangulate a Square Face
                            if(m[2].length() > 0) {
                                if (noUV) uv_mismatch = true;
                                else indices_uv.push_back((unsigned short) stoi(m[2]));
                            } else noUV = true;
                            if(m[8].length() > 0) {
                                if (noUV) uv_mismatch = true;
                                else indices_uv.push_back((unsigned short) stoi(m[8]));
                            } else noUV = true;
                            if (m[11].length() > 0) {
                                if (noUV) uv_mismatch = true;
                                else indices_uv.push_back((unsigned short) stoi(m[11]));
                            } else noUV = true;
                        }
                        if(uv_mismatch) exceptions.push_back(OBJParseException::UVFound(std::string(path), lineNum));
                        indices_normal.push_back((unsigned short)stoi(m[3]));
                        indices_normal.push_back((unsigned short)stoi(m[6]));
                        indices_normal.push_back((unsigned short)stoi(m[9]));
                        if(m.size() == 13) {
                            // Must Triangulate a Square Face
                            indices_normal.push_back((unsigned short)stoi(m[3]));
                            indices_normal.push_back((unsigned short)stoi(m[9]));
                            indices_normal.push_back((unsigned short)stoi(m[12]));
                        }
                    } else exceptions.push_back(OBJParseException::FaceCount(std::string(path), lineNum, m.size()-1));
                }
                // TODO: Add other OBJ format spec support


                if(feof(file)) break;
            }

            fclose(file);

            if(!exceptions.empty()) throw ModelLoadingException::ParseFail(exceptions.data(), exceptions.size());

            size_t index_count = indices_vertex.size();
            VecArray array_vertices(THREE, index_count);
            VecArray array_uvs(TWO, index_count);
            VecArray array_normals(THREE, index_count);

            for(unsigned int i = 0; i < index_count; i++) {

                unsigned int index_vertex = indices_vertex[i];
                unsigned int index_uv = 0; if(!noUV) index_uv = indices_uv[i];
                unsigned int index_normal = indices_normal[i];

                if(index_vertex > obj_vertices.size()) throw ModelLoadingException::IndexOutOfBounds("Vertex", index_vertex, obj_vertices.size());
                if(!noUV && index_uv > obj_uvs.size()) throw ModelLoadingException::IndexOutOfBounds("UV",     index_uv,     obj_uvs.size());
                if(index_normal > obj_normals.size())  throw ModelLoadingException::IndexOutOfBounds("Normal", index_normal, obj_normals.size());

                array_vertices     [i] = VecVar(obj_vertices [index_vertex-1]);
                if(!noUV) array_uvs[i] = VecVar(obj_uvs      [index_uv-1]);
                array_normals      [i] = VecVar(obj_normals  [index_normal-1]);
            }

            RawModelData data;
            data.put(vertex_type, VecArray(array_vertices));
            if(!noUV) data.put(uv_type, VecArray(array_uvs));
            data.put(normal_type, VecArray(array_normals));
            return data;
        }

    }
}