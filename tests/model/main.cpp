#define SWARM_INCLUDE_GLM
#include "api/Core.h"
#include "api/Logging.h"
#include "api/Render.h"

#include <iostream>

using namespace Swarm;

using namespace Swarm::Logging;

int main() {

    try {
        if (!Core::init(SWM_INIT_MODEL)) {
            return -1;
        }
        
        Model::RawModelData data = Model::loadFromOBJ("resources/Cube.obj");

        
        Model::VecArray vec_vertex = data[Model::Type::VERTEX];
        Model::VecArray vec_uv     = data[Model::Type::UV];
        Model::VecArray vec_normal = data[Model::Type::NORMAL];
        Log::log_core(INFO) << "Data Points:";
        for(int i = 0; i < data.size(); i++) {
            Log::log_core(INFO) << "[" << i << "] vertex" << vec_vertex[i].val.v3.vec()
                                                << ", uv" << vec_uv[i].val.v2.vec()
                                            << ", normal" << vec_normal[i].val.v3.vec();
        }
        Log::log_core.newline();
        
        Model::RawModelDataIndexed data_index = data.index();

        unsigned int* indices = data_index.indices();
        Log::log_core(INFO) << "Indexed Data Indices: ";
        for(int i = 0; i < data_index.indexSize(); i++) Log::log_core << indices[i] << ", ";
        
        Model::VecArray vec_index_vertex = data_index[Model::Type::VERTEX];
        Model::VecArray vec_index_uv     = data_index[Model::Type::UV];
        Model::VecArray vec_index_normal = data_index[Model::Type::NORMAL];
        Log::log_core(INFO) << "Indexed Data Points:";
        for(int i = 0; i < data_index.size(); i++) {
            Log::log_core(INFO) << "[" << i << "] vertex" << vec_index_vertex[i].val.v3.vec()
                                << ", uv" << vec_index_uv[i].val.v2.vec()
                                << ", normal" << vec_index_normal[i].val.v3.vec();
        }

        Core::cleanup();
        
    } catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}