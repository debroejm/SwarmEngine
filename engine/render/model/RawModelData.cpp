#include "../../Render.h"

#include "../../Core.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Model {

        unsigned int ModelDataType::nextID = 0;

        ModelDataType::ModelDataType(int size) : size(size) {
            uuid = nextID++;
        }

        ModelDataType::ModelDataType(const ModelDataType &other) {
            *this = other;
        }

        ModelDataType &ModelDataType::operator=(const ModelDataType &other) {
            size = other.size;
            uuid = other.uuid;
            return *this;
        }

        bool ModelDataType::operator==(const ModelDataType &other) const {
            return uuid == other.uuid;
        }

        RawModelData::RawModelData(RawModelData &other) {
            *this = other;
        }

        RawModelData::~RawModelData() {
            cleanup();
        }

        RawModelData &RawModelData::operator=(const RawModelData &other) {
            cleanup();
            for(auto && iter : other.dataMap) {
                float *newData = new float[other.size * iter.first.getSize()];
                for(int i = 0; i < other.size * iter.first.getSize(); i++) newData[i] = iter.second[i];
                dataMap[iter.first] = newData;
            }
            return *this;
        }

        RawModelData &RawModelData::setData(ModelDataType type, float data[], unsigned int size) {
            if(size > this->size) {
                for(auto && iter : dataMap) {
                    float *newData = new float[size * iter.first.getSize()];
                    for(int i = 0; i < this->size * iter.first.getSize(); i++) newData[i] = iter.second[i];
                    for(int i = this->size * iter.first.getSize(); i < size; i++) newData[i] = 0.0f;
                    delete [] iter.second;
                    iter.second = newData;
                }
            } // If size is less than the current, simply lower the current and 'strand' extra data.
            if(hasData(type)) delete [] dataMap[type];
            dataMap[type] = data;
            this->size = size;
            return *this;
        }

        bool RawModelData::hasData(ModelDataType type) const {
            return dataMap.count(type) > 0;
        }

        float* RawModelData::getData(ModelDataType type) const {
            if(hasData(type))
                return dataMap.at(type);
            else return NULL;
        }

        unsigned int RawModelData::getSize() const { return size; }

        void RawModelData::cleanup() {
            for(auto && iter : dataMap) {
                delete [] iter.second;
            }
        }

        struct PackedData {
            int size;
            float* data;
            bool operator<(const PackedData &that) const {
                int size = (that.size > this->size ? that.size : this->size);
                return memcmp(data, that.data, sizeof(float)*size)>0;
            };
            ~PackedData() {
                delete [] data;
            }
        };

        RawModelDataIndexed* RawModelData::index() {

            // TODO: Turn unordered maps to normal maps

            unordered_map<ModelDataType, map<PackedData, unsigned short>> dataToIndexMap;
            //for(auto && iter : dataMap) dataToIndexMap[iter.first] = map<PackedData, unsigned short>();

            unordered_map<ModelDataType, vector<float>> out_data_vector;
            vector<unsigned short> out_indices_vector;
            //for(auto && iter : dataMap) out_data_vector[iter.first] = vector<float>();

            unsigned short lastIndex = 0;

            for(int i = 0; i < size; i++) {
                unordered_map<ModelDataType, PackedData> tempDataMap;
                bool found = true;
                for(auto && iter : dataMap) {
                    unsigned int floatCount = iter.first.getSize();
                    tempDataMap[iter.first] = PackedData{floatCount, new float[floatCount]};
                    PackedData &data = tempDataMap[iter.first];
                    for(int j = 0; j < floatCount; j++) data.data[j] = iter.second[i*floatCount+j];
                    if(dataToIndexMap[iter.first].find(data) == dataToIndexMap[iter.first].end()) found = false;
                }
                if(found) {
                    for(auto && iter : tempDataMap) {
                        out_indices_vector.push_back(dataToIndexMap[iter.first][iter.second]);
                        break;
                    }
                } else {
                    unsigned short newIndex = lastIndex++;
                    for(auto && iter : tempDataMap) {
                        vector<float> &vect = out_data_vector[iter.first];
                        PackedData &data = iter.second;
                        for(int j = 0; j < data.size; j++)
                            vect.push_back(data.data[j]);
                        dataToIndexMap[iter.first][iter.second] = newIndex;
                    }
                    out_indices_vector.push_back(newIndex);
                }
            }

            RawModelDataIndexed* data_indexed = new RawModelDataIndexed;

            unsigned short* out_indices = new unsigned short[out_indices_vector.size()];
            for(int i = 0; i < out_indices_vector.size(); i++) out_indices[i] = out_indices_vector[i];
            data_indexed->setIndices(out_indices, out_indices_vector.size());

            for(auto && iter : out_data_vector) {
                float * out_data = new float[iter.second.size()];
                for(int i = 0; i < iter.second.size(); i++) out_data[i] = iter.second[i];
                data_indexed->setData(iter.first, out_data, iter.second.size() / iter.first.getSize());
            }

            return data_indexed;
        }

        RawModelDataIndexed &RawModelDataIndexed::setIndices(unsigned short* indices, unsigned int size) {
            this->indices = indices;
            this->indexCount = size;
            return *this;
        }

        void RawModelDataIndexed::cleanup() {
            RawModelData::cleanup();
            delete [] indices;
        }

        unsigned short* RawModelDataIndexed::getIndices() const { return indices; }
        unsigned int RawModelDataIndexed::getIndexCount() const { return indexCount; }

    }
}