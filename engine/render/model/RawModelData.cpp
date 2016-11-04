#include "../../Render.h"

#include "../../Core.h"
#include "../../Util.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Model {

        namespace DataType {

            Type::Type(VecType type, VecVar defaultVal, GLuint attribID, float epsilon)
                    : type(type), defaultVal(defaultVal), attribID(attribID), epsilon(epsilon) {
            }

            Type::Type(const Type &other) : defaultVal(0.0f) {
                *this = other;
            }

            Type &Type::operator=(const Type &other) {
                type = other.type;
                defaultVal = other.defaultVal;
                epsilon = other.epsilon;
                attribID = other.attribID;
                return *this;
            }

            bool Type::operator==(const Type &other) const {
                return (attribID == other.attribID && type == other.type);
            }

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
                switch(iter.first.getType()) {
                    case ONE: {
                        dataMap[iter.first].v1 = new float[other.size];
                        float* data = dataMap[iter.first].v1;
                        for(int i = 0; i < other.size; i++) data[i] = iter.second.v1[i];
                    } break;
                    case TWO: {
                        dataMap[iter.first].v2 = new glm::vec2[other.size];
                        glm::vec2* data = dataMap[iter.first].v2;
                        for(int i = 0; i < other.size; i++) data[i] = iter.second.v2[i];
                    } break;
                    case THREE: {
                        dataMap[iter.first].v3 = new glm::vec3[other.size];
                        glm::vec3* data = dataMap[iter.first].v3;
                        for(int i = 0; i < other.size; i++) data[i] = iter.second.v3[i];
                    } break;
                    case FOUR: {
                        dataMap[iter.first].v4 = new glm::vec4[other.size];
                        glm::vec4* data = dataMap[iter.first].v4;
                        for(int i = 0; i < other.size; i++) data[i] = iter.second.v4[i];
                    } break;
                    default: break;
                }
            }
            return *this;
        }

        void RawModelData::upsize(unsigned int size) {
            if(size > this->size) {
                // Resize previous arrays to match new size and fill with empty (0.0f) data
                for(auto && iter : dataMap) {
                    switch(iter.first.getType()) {
                        case ONE: {
                            float* old = iter.second.v1;
                            iter.second.v1 = new float[size];;
                            for(int i = 0; i < this->size; i++) iter.second.v1[i] = old[i];
                            for(int i = this->size; i < size; i++) iter.second.v1[i] = iter.first.getDefault().val.v1;
                            delete [] old;
                        } break;
                        case TWO: {
                            glm::vec2* old = iter.second.v2;
                            iter.second.v2 = new glm::vec2[size];
                            for(int i = 0; i < this->size; i++) iter.second.v2[i] = old[i];
                            for(int i = this->size; i < size; i++) iter.second.v2[i] = iter.first.getDefault().val.v2;
                            delete [] old;
                        } break;
                        case THREE: {
                            glm::vec3* old = iter.second.v3;
                            iter.second.v3 = new glm::vec3[size];
                            for(int i = 0; i < this->size; i++) iter.second.v3[i] = old[i];
                            for(int i = this->size; i < size; i++) iter.second.v3[i] = iter.first.getDefault().val.v3;
                            delete [] old;
                        } break;
                        case FOUR: {
                            glm::vec4* old = iter.second.v4;
                            iter.second.v4 = new glm::vec4[size];
                            for(int i = 0; i < this->size; i++) iter.second.v4[i] = old[i];
                            for(int i = this->size; i < size; i++) iter.second.v4[i] = iter.first.getDefault().val.v4;
                            delete [] old;
                        } break;
                        default: break;
                    }
                }
            }
        }

        RawModelData &RawModelData::setData(const DataType::Type &type, float data[], unsigned int size) {
            upsize(size);
            switch(type.getType()) {
                case ONE: {
                    float *nData = new float[size];
                    for (int i = 0; i < size; i++) nData[i] = data[i];
                    if (hasData(type)) delete[] dataMap[type].v1;
                    dataMap[type].v1 = nData;
                } break;
                case TWO: {
                    glm::vec2 *nData = new glm::vec2[size];
                    for (int i = 0; i < size; i++) nData[i] = glm::vec2(data[i*2], data[i*2+1]);
                    if(hasData(type)) delete[] dataMap[type].v2;
                    dataMap[type].v2 = nData;
                } break;
                case THREE: {
                    glm::vec3 *nData = new glm::vec3[size];
                    for (int i = 0; i < size; i++) nData[i] = glm::vec3(data[i*3], data[i*3+1], data[i*3+2]);
                    if(hasData(type)) delete[] dataMap[type].v3;
                    dataMap[type].v3 = nData;
                } break;
                case FOUR: {
                    glm::vec4 *nData = new glm::vec4[size];
                    for (int i = 0; i < size; i++) nData[i] = glm::vec4(data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
                    if(hasData(type)) delete[] dataMap[type].v4;
                    dataMap[type].v4 = nData;
                } break;
                default: break;
            }

            this->size = size;
            return *this;
        }

        RawModelData &RawModelData::setData(const DataType::Type &type, VecArray data, unsigned int size) {
            upsize(size);
            this->size = size;
            dataMap[type] = data;
            return *this;
        }

        bool RawModelData::hasData(const DataType::Type &type) const {
            return dataMap.count(type) > 0;
        }

        VecArray RawModelData::getData(const DataType::Type &type) const {
            if(hasData(type))
                return dataMap.at(type);
            else return VecArray();
        }

        unsigned int RawModelData::getSize() const { return size; }

        void RawModelData::cleanup() {
            for(auto && iter : dataMap) {
                switch(iter.first.getType()) {
                    case ONE:
                        delete [] iter.second.v1; break;
                    case TWO:
                        delete [] iter.second.v2; break;
                    case THREE:
                        delete [] iter.second.v3; break;
                    case FOUR:
                        delete [] iter.second.v4; break;
                    default:
                        break;
                }
            }
        }

        struct PackedData {
            unordered_map<DataType::Type, VecVar> data;
            bool operator<(const PackedData &other) const {
                if(data.size() == other.data.size()) {
                    for(auto && iter : data) {
                        if(!other.data.count(iter.first)) goto uuid_count; // Need some way to distinguish them
                        VecVar data_this = iter.second;
                        VecVar data_other = other.data.at(iter.first);
                        if(data_this.type != data_other.type) goto uuid_count; // Shouldn't happen
                        switch(data_this.type) {
                            case ONE:
                                if(float_cmp(data_this.val.v1, data_other.val.v1, iter.first.getEpsilon()) < 0) return true;
                                break;
                            case TWO:
                                if(float_cmp(data_this.val.v2.x, data_other.val.v2.x, iter.first.getEpsilon()) < 0) return true;
                                if(float_cmp(data_this.val.v2.y, data_other.val.v2.y, iter.first.getEpsilon()) < 0) return true;
                                break;
                            case THREE:
                                if(float_cmp(data_this.val.v3.x, data_other.val.v3.x, iter.first.getEpsilon()) < 0) return true;
                                if(float_cmp(data_this.val.v3.y, data_other.val.v3.y, iter.first.getEpsilon()) < 0) return true;
                                if(float_cmp(data_this.val.v3.z, data_other.val.v3.z, iter.first.getEpsilon()) < 0) return true;
                                break;
                            case FOUR:
                                if(float_cmp(data_this.val.v4.x, data_other.val.v4.x, iter.first.getEpsilon()) < 0) return true;
                                if(float_cmp(data_this.val.v4.y, data_other.val.v4.y, iter.first.getEpsilon()) < 0) return true;
                                if(float_cmp(data_this.val.v4.z, data_other.val.v4.z, iter.first.getEpsilon()) < 0) return true;
                                if(float_cmp(data_this.val.v4.w, data_other.val.v4.w, iter.first.getEpsilon()) < 0) return true;
                                break;
                            default: break;
                        }
                    }
                    return false;
                    uuid_count:
                    unsigned long uuid_this = 0;
                    unsigned long uuid_other = 0;
                    std::hash<DataType::Type> hasher;
                    for(auto && iter : data) uuid_this += hasher(iter.first);
                    for(auto && iter : other.data) uuid_other += hasher(iter.first);
                    return uuid_this < uuid_other;
                } else return data.size() < other.data.size();
            }
        };

        RawModelDataIndexed* RawModelData::index() {

            map<PackedData, unsigned short> dataToIndexMap;
            unordered_map<DataType::Type, vector<VecVar>> out_data_vector;
            vector<unsigned short> out_indices_vector;

            unsigned short lastIndex = 0;
            for(int i = 0; i < size; i++) {
                PackedData pData;

                for(auto && iter : dataMap) {
                    switch(iter.first.getType()) {
                        case ONE:   pData.data[iter.first] = VecVar(iter.second.v1[i]); break;
                        case TWO:   pData.data[iter.first] = VecVar(iter.second.v2[i]); break;
                        case THREE: pData.data[iter.first] = VecVar(iter.second.v3[i]); break;
                        case FOUR:  pData.data[iter.first] = VecVar(iter.second.v4[i]); break;
                    }
                }

                if(dataToIndexMap.count(pData)) {
                    out_indices_vector.push_back(dataToIndexMap[pData]);
                } else {
                    unsigned short newIndex = lastIndex++;
                    for(auto && iter : pData.data)
                        out_data_vector[iter.first].push_back(iter.second);
                    out_indices_vector.push_back(newIndex);
                    dataToIndexMap[pData] = newIndex;
                }
            }

            RawModelDataIndexed* data_indexed = new RawModelDataIndexed;

            unsigned short* out_indices = new unsigned short[out_indices_vector.size()];
            for(int i = 0; i < out_indices_vector.size(); i++) out_indices[i] = out_indices_vector[i];
            data_indexed->setIndices(out_indices, out_indices_vector.size());

            for(auto && iter : out_data_vector) {
                VecArray out_data;
                switch(iter.first.getType()) {
                    case ONE:
                        out_data.v1 = new float[iter.second.size()];
                        for(int i = 0; i < iter.second.size(); i++) out_data.v1[i] = iter.second[i].val.v1;
                        break;
                    case TWO:
                        out_data.v2 = new glm::vec2[iter.second.size()];
                        for(int i = 0; i < iter.second.size(); i++) out_data.v2[i] = iter.second[i].val.v2;
                        break;
                    case THREE:
                        out_data.v3 = new glm::vec3[iter.second.size()];
                        for(int i = 0; i < iter.second.size(); i++) out_data.v3[i] = iter.second[i].val.v3;
                        break;
                    case FOUR:
                        out_data.v4 = new glm::vec4[iter.second.size()];
                        for(int i = 0; i < iter.second.size(); i++) out_data.v4[i] = iter.second[i].val.v4;
                        break;
                    default: break;
                }
                data_indexed->setData(iter.first, out_data, iter.second.size());
            }

            return data_indexed;
        }

        RawModelDataIndexed* RawModelDataIndexed::index() {
            RawModelDataIndexed* result = RawModelData::index();
            unsigned short* newIndices = new unsigned short[indexCount];
            unsigned short* shortIndices = result->getIndices();
            for(int i = 0; i < indexCount; i++) newIndices[i] = shortIndices[indices[i]];
            result->setIndices(newIndices, indexCount);
            return result;
        }

        RawModelDataIndexed::RawModelDataIndexed(const RawModelDataIndexed &other) {
            *this = other;
        }

        RawModelDataIndexed &RawModelDataIndexed::setIndices(unsigned short* indices, unsigned int size) {
            if(this->indices != NULL) delete [] this->indices;
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

        RawModelDataIndexed &RawModelDataIndexed::operator=(const RawModelDataIndexed &other) {
            cleanup(); // Make sure the indices are deleted
            RawModelData::operator=(other); // Might be an issue
            indexCount = other.indexCount;
            indices = new unsigned short[indexCount];
            for(int i = 0; i < indexCount; i++) indices[i] = other.indices[i];
            return *this;
        }

    }
}