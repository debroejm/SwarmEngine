#include "Core.h"
#include "Render.h"

using namespace Swarm;

using namespace Swarm::Logging;

int main() {

    if (!Init::init(SWM_INIT_MODEL)) {
        return -1;
    }


    Model::RawModelDataIndexed *data = Model::loadFromOBJ("resources/Cube.obj");

    if(data != NULL) {
        unsigned short *data_array_index = data->getIndices();
        Log::log_core(DEBUG) << data->getIndexCount() << ": Indices[";
        for (int i = 0; i < data->getIndexCount(); i++) Log::log_core << data_array_index[i] << ", ";
        Log::log_core << "]";
        const Model::DataType::Type &tempType = Model::DataType::VERTEX;
        glm::vec3 *data_array_vertex = data->getData(Model::DataType::VERTEX).v3;
        glm::vec2 *data_array_uv = data->getData(Model::DataType::UV).v2;
        glm::vec3 *data_array_normal = data->getData(Model::DataType::NORMAL).v3;
        for (int i = 0; i < data->getSize(); i++) {
            Log::log_core(DEBUG) << "[" << i << "] ( " << data_array_vertex[i].x << ", " << data_array_vertex[i].y << ", " << data_array_vertex[i].z
                    << " ),  ( " << data_array_uv[i].x << ", " << data_array_uv[i].y
                    << " ),  ( " << data_array_normal[i].x << ", " << data_array_normal[i].y << ", " << data_array_normal[i].z << " )";
        }
    }


    Init::cleanup();
    return 0;
}