#define SWARM_INCLUDE_GLM
#define SWARM_INCLUDE_GLEW
#include "api/Render.h"

#include "api/Logging.h"

#include <set>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Model {

        VecArray::VecArray() : _type(ONE), _size(0) {
            data = new VecVar[0];
        }

        VecArray::VecArray(VecType type, size_t size) : _type(type), _size(size) {
            data = new VecVar[_size];
        }

        VecArray::~VecArray() {
            if(data != nullptr) delete [] data;
        }

        VecArray &VecArray::operator=(const VecArray &other) {
            _size = other._size;
            _type = other._type;
            if(data != nullptr) delete [] data;
            data = new VecVar[_size];
            for(size_t i = 0; i < _size; i++) data[i] = other.at(i);
            return *this;
        }

        const VecVar &VecArray::at(size_t index) const {
            return data[index];
        }

        VecVar &VecArray::operator[](size_t index) {
            return data[index];
        }



        namespace Type {

            DataType::DataType(VecType type, SWMuint attribID, VecVar defaultVal)
                    : _type(type), _attrib_id(attribID), _default_val(defaultVal) {
            }

            DataType::DataType(const DataType &other) {
                *this = other;
            }

            DataType &DataType::operator=(const DataType &other) {
                _type = other._type;
                _default_val = other._default_val;
                _attrib_id = other._attrib_id;
                return *this;
            }

            bool DataType::operator==(const DataType &other) const {
                return (_attrib_id == other._attrib_id && _type == other._type);
            }

            bool DataType::operator<(const DataType &other) const {
                if(_attrib_id == other._attrib_id) return _type < other._type;
                else return _attrib_id < other._attrib_id;
            }

        }



        RawModelData::RawModelData(const RawModelData &other) {
            *this = other;
        }

        RawModelData &RawModelData::operator=(const RawModelData &other) {
            data_map = other.data_map;
            _size = other._size;
            return *this;
        }

        void RawModelData::resize(size_t size) {
            if(size > _size) {
                for (auto &&iter : data_map) {
                    VecArray va(iter.second.type(), size);
                    for (int i = 0; i < size; i++) {
                        if(i < _size) va[i] = iter.second[i];
                        else va[i] = iter.first.defaultValue();
                    }
                    iter.second = va;
                }
            }
            _size = size;
        }

        RawModelData &RawModelData::put(const Type::DataType &type, float *data, size_t size) {
            resize(size);
            _size = size;

            VecArray va(type.type(), size);
            for(int i = 0; i < size; i++) {
                switch(type.type()) {
                    case ONE:
                        va[i] = VecVar( data[i] );
                        break;
                    case TWO:
                        va[i] = VecVar( data[i*2], data[i*2+1] );
                        break;
                    case THREE:
                        va[i] = VecVar( data[i*3], data[i*3+1], data[i*3+2] );
                        break;
                    case FOUR:
                        va[i] = VecVar( data[i*4], data[i*4+1], data[i*4+2], data[i*4+3] );
                        break;
                    default: break;
                }
            }

            data_map[type] = va;
            return *this;
        }

        RawModelData &RawModelData::put(const Type::DataType &type, VecArray data) {
            resize(data.size());
            _size = data.size();
            data_map[type] = data;
            return *this;
        }

        bool RawModelData::exists(const Type::DataType &type) const {
            return data_map.count(type) > 0;
        }

        const VecArray &RawModelData::at(const Type::DataType &type) const {
            if(data_map.count(type)) return data_map.at(type);
            else throw std::out_of_range("RawModelData::at");
        }

        VecArray &RawModelData::operator[](const Type::DataType &type) {
            return data_map[type];
        }



        bool computeTangents(RawModelData &raw_model_data,
                             const Type::DataType &vertex_type, const Type::DataType &uv_type, const Type::DataType &normal_type,
                             const Type::DataType &tangent_type, const Type::DataType &bitangent_type) {

            // Are the DataTypes compatible?
            if( vertex_type.type() < 3) return false;
            if( uv_type.type() < 2) return false;
            if( normal_type.type() < 3) return false;
            if( tangent_type.type() != 3) return false;
            if( bitangent_type.type() != 3) return false;

            // Does this RawModelData have everything we need?
            if( !(raw_model_data.exists(vertex_type) && raw_model_data.exists(uv_type) && raw_model_data.exists(normal_type)) ) return false;

            size_t size = raw_model_data.size();

            // Get the input data arrays
            VecArray &vertex_data = raw_model_data[vertex_type];
            VecArray &uv_data     = raw_model_data[uv_type];
            VecArray &normal_data = raw_model_data[normal_type];

            // Create the output data arrays
            VecArray tangent_data  (THREE, size);
            VecArray bitangent_data(THREE, size);

            for(size_t i = 0; i < size; i+=3) {

                // Must do calculations in pairs, but have to accomodate for sizes that aren't multiples of 3
                if(i >= size-2) i = size-3;

                // Retrieve the needed data
                glm::vec3 v0 (vertex_data[i+0].val.v3.x, vertex_data[i+0].val.v3.y, vertex_data[i+0].val.v3.z);
                glm::vec3 v1 (vertex_data[i+1].val.v3.x, vertex_data[i+1].val.v3.y, vertex_data[i+1].val.v3.z);
                glm::vec3 v2 (vertex_data[i+2].val.v3.x, vertex_data[i+2].val.v3.y, vertex_data[i+2].val.v3.z);
                glm::vec2 uv0(uv_data    [i+0].val.v2.x, uv_data    [i+0].val.v2.y);
                glm::vec2 uv1(uv_data    [i+1].val.v2.x, uv_data    [i+1].val.v2.y);
                glm::vec2 uv2(uv_data    [i+2].val.v2.x, uv_data    [i+2].val.v2.y);
                glm::vec3 n0 (normal_data[i+0].val.v3.x, normal_data[i+0].val.v3.y, normal_data[i+0].val.v3.z);
                glm::vec3 n1 (normal_data[i+1].val.v3.x, normal_data[i+1].val.v3.y, normal_data[i+1].val.v3.z);
                glm::vec3 n2 (normal_data[i+2].val.v3.x, normal_data[i+2].val.v3.y, normal_data[i+2].val.v3.z);

                // Calculate Deltas
                glm::vec3 dV1  = v1-v0;
                glm::vec3 dV2  = v2-v0;
                glm::vec2 dUV1 = uv1-uv0;
                glm::vec2 dUV2 = uv2-uv0;

                // Calculate Tangent and Bitangent values
                float r = 1.0f / ( dUV1.x * dUV2.y - dUV1.y * dUV2.x );
                glm::vec3 tangent =   r * ( dV1 * dUV2.y - dV2 * dUV1.y );
                glm::vec3 bitangent = r * ( dV2 * dUV1.x - dV1 * dUV2.x );
                glm::vec3 t0 = glm::normalize(tangent - n0 * glm::dot(n0, tangent));
                glm::vec3 t1 = glm::normalize(tangent - n1 * glm::dot(n1, tangent));
                glm::vec3 t2 = glm::normalize(tangent - n2 * glm::dot(n2, tangent));

                // Calculate final Tangents and Bitangents
                // CLion (and possibly other IDEs) will see an error when comparing a glm::dot result to a float,
                //   but it compiles just fine (it can't read the GLM template)
                tangent_data[i+0] = VecVar( t0 * ( glm::dot(glm::cross(n0, t0), bitangent) < 0.0f ? -1.0f : 1.0f) );
                tangent_data[i+1] = VecVar( t1 * ( glm::dot(glm::cross(n1, t1), bitangent) < 0.0f ? -1.0f : 1.0f) );
                tangent_data[i+2] = VecVar( t2 * ( glm::dot(glm::cross(n2, t2), bitangent) < 0.0f ? -1.0f : 1.0f) );
                VecVar btvv( bitangent );
                bitangent_data[i+0] = btvv;
                bitangent_data[i+1] = btvv;
                bitangent_data[i+2] = btvv;

            }

            // Store the resulting tangents in the RawModelData
            raw_model_data.put(tangent_type,   tangent_data);
            raw_model_data.put(bitangent_type, bitangent_data);

            return true;
        }

        struct IndexDatum {
            struct SubDatum {
                int cmp_1, cmp_2, cmp_3, cmp_4;
                VecVar val;
            };
            std::map<Type::DataType, SubDatum> data;
            unsigned int index;
            IndexDatum(unsigned int index) {
                this->index = index;
            }
            void insert(const Type::DataType &type, VecVar &val, short epsilon_factor) {
                float mult_factor = std::pow(10.0f, epsilon_factor );
                switch(val.type) {
                    case ONE:
                        data[type] = SubDatum{
                                (int)(val.val.v1.x * mult_factor),
                                0, 0, 0,
                                val
                        }; break;
                    case TWO:
                        data[type] = SubDatum{
                                (int)(val.val.v2.x * mult_factor),
                                (int)(val.val.v2.y * mult_factor),
                                0, 0,
                                val
                        }; break;
                    case THREE:
                        data[type] = SubDatum{
                                (int)(val.val.v3.x * mult_factor),
                                (int)(val.val.v3.y * mult_factor),
                                (int)(val.val.v3.z * mult_factor),
                                0,
                                val
                        }; break;
                    case FOUR:
                        data[type] = SubDatum{
                                (int)(val.val.v4.x * mult_factor),
                                (int)(val.val.v4.y * mult_factor),
                                (int)(val.val.v4.z * mult_factor),
                                (int)(val.val.v4.w * mult_factor),
                                val
                        }; break;
                }
            }
            bool operator<(const IndexDatum &rhs) const {
                const IndexDatum &lhs = *this;
                std::map<Type::DataType, SubDatum>::const_iterator it_lhs = lhs.data.begin();
                std::map<Type::DataType, SubDatum>::const_iterator it_rhs = rhs.data.begin();
                while(it_lhs != lhs.data.end() && it_rhs != rhs.data.end()) {
                    if(it_lhs->first != it_rhs->first) return it_lhs->first < it_rhs->first;
                    if(it_lhs->second.cmp_1 != it_rhs->second.cmp_1) return it_lhs->second.cmp_1 < it_rhs->second.cmp_1;
                    if(it_lhs->second.cmp_2 != it_rhs->second.cmp_2) return it_lhs->second.cmp_2 < it_rhs->second.cmp_2;
                    if(it_lhs->second.cmp_3 != it_rhs->second.cmp_3) return it_lhs->second.cmp_3 < it_rhs->second.cmp_3;
                    if(it_lhs->second.cmp_4 != it_rhs->second.cmp_4) return it_lhs->second.cmp_4 < it_rhs->second.cmp_4;
                    it_lhs++;
                    it_rhs++;
                }
                return lhs.data.size() < rhs.data.size();
            }
        };

        RawModelDataIndexed RawModelData::index(short epsilon_factor) {

            unsigned int last_index = 0;
            std::set<IndexDatum> nocopy_pool;
            std::vector<unsigned int> out_indices;
            std::map<Type::DataType, std::vector<VecVar>> out_data;

            for(int i = 0; i < _size; i++) {
                IndexDatum datum(last_index);
                for(auto && iter : data_map) datum.insert(iter.first, iter.second[i], epsilon_factor);

                std::set<IndexDatum>::iterator it_datum = nocopy_pool.find(datum);
                if(it_datum != nocopy_pool.end()) out_indices.push_back(it_datum->index);
                else {
                    nocopy_pool.insert(datum);
                    out_indices.push_back(last_index);
                    for(auto && iter : datum.data) out_data[iter.first].push_back(iter.second.val);
                    last_index++;
                }
            }

            RawModelDataIndexed output;

            for(auto && iter : out_data) {
                VecArray array(iter.first.type(), iter.second.size());
                for(unsigned int i = 0; i < iter.second.size(); i++) array[i] = iter.second[i];
                output.put(iter.first, array);
            }

            output.putIndices(out_indices.data(), out_indices.size());

            return output;
        }

        RawModelDataIndexed RawModelDataIndexed::index(short epsilon_factor) {
            RawModelDataIndexed output = RawModelData::index(epsilon_factor);
            unsigned int* newIndices = new unsigned int[_index_size];
            unsigned int* shortIndices = output.indices();
            for(int i = 0; i < _index_size; i++) newIndices[i] = shortIndices[_indices[i]];
            output.putIndices(newIndices, _index_size);
            return output;
        }

        RawModelDataIndexed::~RawModelDataIndexed() {
            if(_indices != nullptr) delete [] _indices;
        }

        RawModelDataIndexed::RawModelDataIndexed(const RawModelDataIndexed &other) {
            *this = other;
        }

        RawModelDataIndexed &RawModelDataIndexed::putIndices(unsigned int *indices, size_t size) {
            if(this->_indices != nullptr) delete [] this->_indices;
            _indices = new unsigned int[size];
            for(int i = 0; i < size; i++) _indices[i] = indices[i];
            this->_index_size = size;
            return *this;
        }

        RawModelDataIndexed &RawModelDataIndexed::operator=(const RawModelDataIndexed &other) {
            RawModelData::operator=(other); // Might be an issue
            _index_size = other._index_size;
            _indices = new unsigned int[_index_size];
            for(int i = 0; i < _index_size; i++) _indices[i] = other._indices[i];
            return *this;
        }

    }
}