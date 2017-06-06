#define SWARM_INCLUDE_GLM
#include "api/Core.h"
#include "api/Logging.h"
#include "api/Render.h"

#include <iostream>

using namespace Swarm;

using namespace Swarm::Logging;

// Logger
Log log_test("Test");

int main() {

    try {
        if (!Core::init(SWM_INIT_MINIMAL)) {
            return -1;
        }
        
        log_test.initFile();
        
        Model::RawModelData data = Model::loadFromOBJ("resources/Cube.obj");

        
        Model::VecArray vec_vertex = data[Model::Type::VERTEX];
        Model::VecArray vec_uv     = data[Model::Type::UV];
        Model::VecArray vec_normal = data[Model::Type::NORMAL];
        log_test(INFO) << "Data Points:";
        for(int i = 0; i < data.size(); i++) {
            log_test(INFO) << "[" << i << "] vertex" << vec_vertex[i].val.v3.vec()
                                                << ", uv" << vec_uv[i].val.v2.vec()
                                            << ", normal" << vec_normal[i].val.v3.vec();
        }
        log_test.flush();
        
        Model::RawModelDataIndexed data_index = data.index();

        unsigned int* indices = data_index.indices();
        log_test(INFO) << "Indexed Data Indices: ";
        for(int i = 0; i < data_index.indexSize(); i++) log_test << indices[i] << ", ";
        log_test.flush();
        
        Model::VecArray vec_index_vertex = data_index[Model::Type::VERTEX];
        Model::VecArray vec_index_uv     = data_index[Model::Type::UV];
        Model::VecArray vec_index_normal = data_index[Model::Type::NORMAL];
        log_test(INFO) << "Indexed Data Points:";
        for(int i = 0; i < data_index.size(); i++) {
            log_test(INFO) << "[" << i << "] vertex" << vec_index_vertex[i].val.v3.vec()
                                << ", uv" << vec_index_uv[i].val.v2.vec()
                                << ", normal" << vec_index_normal[i].val.v3.vec();
        }
        log_test.flush();

        Core::cleanup();
        
    } catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}