#include "../../Render.h"

#include "../../Core.h"

#include <regex>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Model {

        std::regex pattern_v("v\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s*");
        std::regex pattern_vt("vt\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s*");
        std::regex pattern_vn("vn\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s*");
        std::regex pattern_f("f\\s+(\\d+)\\/(\\d*)\\/(\\d+)\\s+(\\d+)\\/(\\d*)\\/(\\d+)\\s+(\\d+)\\/(\\d*)\\/(\\d+)(?:\\s+(\\d+)\\/(\\d*)\\/(\\d+))?\\s*");

        RawModelDataIndexed* loadFromOBJ(const char * path) {

            Log::log_render(INFO) << "Loading OBJ File: " << path;

            FILE * file = fopen(path, "r");
            if( file == NULL ) {
                Log::log_render(ERR) << "Failed to open file [" << path << "]"; // TODO: Add file read error report
                getchar();
                return NULL;
            }

            // TODO: Modify Models for unsigned ints instead of shorts if need be
            std::vector<unsigned short> indices_vertex, indices_uv, indices_normal;
            std::vector<glm::vec3> obj_vertices;
            std::vector<glm::vec2> obj_uvs;
            std::vector<glm::vec3> obj_normals;

            bool fail = false;
            bool noUV = false;
            int lineNum = 0;
            char line_c[1024];
            while( 1 ) {

                const char* nullCheck = fgets(line_c, 1024, file);
                lineNum++;
                if( ferror(file) ) {
                    Log::log_render(ERR) << "File read error [" << path << "][" << lineNum << "]"; // TODO: Add file read error report
                    return NULL;
                } else if( feof(file) && nullCheck == NULL ) break;

                string line(line_c);
                if(line.size() < 1) continue;

                char lineHeader[1024];
                if(sscanf(line_c, "%s", lineHeader) < 1) continue;

                // Vertex : [x y z]
                if( strcmp( lineHeader, "v"  ) == 0 ) {
                    std::smatch m;
                    if(std::regex_match(line, m, pattern_v)) obj_vertices.push_back(glm::vec3(stof(m[1]), stof(m[2]), stof(m[3])));
                    else {
                        Log::log_render(ERR) << "OBJ Read Error [" << lineNum << "]: Incorrect Vertex Argument Count; " << m.size()-1 << " instead of 3";
                        fail = true;
                    }
                } else
                // UV : [u v]
                if( strcmp( lineHeader, "vt" ) == 0 ) {
                    std::smatch m;
                    if(std::regex_match(line, m, pattern_vt)) obj_uvs.push_back(glm::vec2(stof(m[1]), stof(m[2])));
                    else {
                        Log::log_render(ERR) << "OBJ Read Error [" << lineNum << "]: Incorrect UV Argument Count; " << m.size()-1 << " instead of 2";
                        fail = true;
                    }
                } else
                // Normal : [x y z]
                if( strcmp( lineHeader, "vn" ) == 0 ) {
                    std::smatch m;
                    if(std::regex_match(line, m, pattern_vn)) obj_normals.push_back(glm::vec3(stof(m[1]), stof(m[2]), stof(m[3])));
                    else {
                        Log::log_render(ERR) << "OBJ Read Error [" << lineNum << "]: Incorrect Normal Argument Count; " << m.size()-1 << " instead of 3";
                        fail = true;
                    }
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
                        if(uv_mismatch) {
                            Log::log_render(ERR) << "OBJ Read Error [" << lineNum << "]: Found UV Index when previous UVs had no Index";
                            fail = true;
                        }
                        indices_normal.push_back((unsigned short)stoi(m[3]));
                        indices_normal.push_back((unsigned short)stoi(m[6]));
                        indices_normal.push_back((unsigned short)stoi(m[9]));
                        if(m.size() == 13) {
                            // Must Triangulate a Square Face
                            indices_normal.push_back((unsigned short)stoi(m[3]));
                            indices_normal.push_back((unsigned short)stoi(m[9]));
                            indices_normal.push_back((unsigned short)stoi(m[12]));
                        }
                    } else {
                        Log::log_render(ERR) << "OBJ Read Error [" << lineNum << "]: Incorrect Face Argument Count; " << m.size()-1 << " instead of 9/12 (3/4 triplets)";
                        fail = true;
                    }
                }
                // TODO: Add other OBJ format spec support


                if(feof(file)) break;
            }

            fclose(file);

            if(fail) return NULL;

            unsigned int index_count = indices_vertex.size();
            glm::vec3 *array_vertices = new glm::vec3[index_count];
            glm::vec2 *array_uvs      = new glm::vec2[index_count];
            glm::vec3 *array_normals  = new glm::vec3[index_count];

            for(unsigned int i = 0; i < index_count; i++) {

                unsigned short index_vertex = indices_vertex[i];
                unsigned short index_uv = 0; if(!noUV) index_uv = indices_uv[i];
                unsigned short index_normal = indices_normal[i];

                if(index_vertex > obj_vertices.size()) {
                    Log::log_render(ERR) << "OBJ Load Error: Vertex Index Out of Bounds; " << index_vertex << " > maximum of " << obj_vertices.size();
                    return NULL;
                }
                if(!noUV && index_uv > obj_uvs.size()) {
                    Log::log_render(ERR) << "OBJ Load Error: UV Index Out of Bounds; " << index_uv << " > maximum of " << obj_uvs.size();
                    return NULL;
                }
                if(index_normal > obj_normals.size()) {
                    Log::log_render(ERR) << "OBJ Load Error: Normal Index Out of Bounds; " << index_normal << " > maximum of " << obj_normals.size();
                    return NULL;
                }

                array_vertices     [i] = obj_vertices [index_vertex-1];
                if(!noUV) array_uvs[i] = obj_uvs      [index_uv-1];
                array_normals      [i] = obj_normals  [index_normal-1];
            }

            RawModelData data;
            data.setData          (DataType::VERTEX, VecArray(array_vertices), index_count);
            if(!noUV) data.setData(DataType::UV,     VecArray(array_uvs),      index_count);
            data.setData          (DataType::NORMAL, VecArray(array_normals),  index_count);
            return data.index();
        }

    }
}