#pragma once



// ***************
//  STD Libraries
// ***************

#include <map>
#include <set>
#include <unordered_map>
#include <vector>


// ***************
//  Common Header
// ***************

#define SWARM_REQUEST_BOOST_MUTEX
#include "common_header.h"
#undef SWARM_REQUEST_BOOST_MUTEX



// ***********
//  API Begin
// ***********

namespace Swarm {
    namespace Texture {
        namespace Type {

            //! Class-Enum used to represent GLenum Texture Target types.
            /*!
             * The TargetType class is used as a manual enumeration to represent different Texture Target types in
             * OpenGL. The TargetType constructor is private, and it cannot be created other than by the static Types
             * defined in the TargetType namespace. This Class-Enum is essentially used to represent an OpenGL type
             * w/o using 3rd party libraries/loaders (e.g. GLEW), but will return a GLenum if \ref SWARM_INCLUDE_GLEW
             * is defined.
             */
            class TargetType {
            private:
                TargetType(SWMenum type) : _type(type) {};
                SWMenum _type;
            public:

                //! TEX_1D maps to the GLenum GL_TEXTURE_1D
                static TargetType TEX_1D;
                //! TEX_1D_ARRAY maps to the GLenum GL_TEXTURE_1D_ARRAY
                static TargetType TEX_1D_ARRAY;
                //! TEX_2D maps to the GLenum GL_TEXTURE_2D
                static TargetType TEX_2D;
                //! TEX_2D_ARRAY maps to the GLenum GL_TEXTURE_2D_ARRAY
                static TargetType TEX_2D_ARRAY;
                //! TEX_3D maps to the GLenum GL_TEXTURE_3D
                static TargetType TEX_3D;
                //! TEX_RECTANGLE maps to the GLenum GL_TEXTURE_RECTANGLE
                static TargetType TEX_RECTANGLE;
                //! TEX_CUBE_MAP maps to the GLenum GL_TEXTURE_CUBE_MAP
                static TargetType TEX_CUBE_MAP;
                //! TEX_CUBE_MAP_ARRAY maps to the GLenum GL_TEXTURE_CUBE_MAP_ARRAY
                static TargetType TEX_CUBE_MAP_ARRAY;
                //! TEX_BUFFER maps to the GLenum GL_TEXTURE_BUFFER
                static TargetType TEX_BUFFER;
                //! TEX_2D_MULTISAMPLE maps to the GLenum GL_TEXTURE_2D_MULTISAMPLE
                static TargetType TEX_2D_MULTISAMPLE;
                //! TEX_2D_MULTISAMPLE_ARRAY maps to the GLenum GL_TEXTURE_2D_MULTISAMPLE_ARRAY
                static TargetType TEX_2D_MULTISAMPLE_ARRAY;

                SWMenum type() const     { return _type; }
                operator SWMenum() const { return _type; }

                bool operator==(const TargetType &rhs) const { return _type == rhs._type; }
                bool operator!=(const TargetType &rhs) const { return !this->operator==(rhs); }

                bool operator< (const TargetType &rhs) const { return _type < rhs._type; }
                bool operator> (const TargetType &rhs) const { return _type > rhs._type; }
                bool operator<=(const TargetType &rhs) const { return !this->operator>(rhs); }
                bool operator>=(const TargetType &rhs) const { return !this->operator<(rhs); }

            };

            //! Class used to represent a Texture Map type.
            /*!
             * The MapType class is used to represent different types of Texture Maps, as well as store data such as
             * what kind of OpenGL Texture Target to use and what Active Texture ID to use for the Texture Map. The Engine
             * provides definitions for these static MapTypes by default:
             *
             *  \ref DIFFUSE
             *  \ref SPECULAR
             *  \ref NORMAL
             *  \ref EMISSIVE
             */
            class MapType {
            public:

                //! MapType main Constructor
                /*!
                 * Creates a custom MapType using the given TargetType and Active Texture ID.
                 *
                 * \param active a \ref SWMuint describing the texture unit to bind to
                 * \sa TargetType
                 */
                MapType(SWMuint active) : _active(active) {}

                //! MapType Copy Constructor
                /*!
                 * Creates a copy of a MapType.
                 *
                 * \param other MapType to copy
                 */
                MapType(const MapType &other) { *this = other; }

                //! MapType assignment operator
                /*!
                 * Assigns this MapType to be equivalent to another MapType. All equivalent MapType objects are
                 * considered equivalent for identification (i.e. has the same hash).
                 *
                 * \param other MapType to copy
                 * \return a reference to the current Maptype
                 */
                MapType &operator=(const MapType &other);

                // Comparison Operators
                bool operator==(const MapType &other) const { return hash() == other.hash(); }
                bool operator!=(const MapType &other) const { return hash() != other.hash(); }
                bool operator< (const MapType &other) const { return hash() < other.hash(); }
                bool operator<=(const MapType &other) const { return hash() <= other.hash(); }
                bool operator> (const MapType &other) const { return hash() > other.hash(); }
                bool operator>=(const MapType &other) const { return hash() >= other.hash(); }

                SWMuint active() const   { return _active; }
                operator SWMuint() const { return _active; }

                //! Hashing function
                /*!
                 * Returns a hash representation of a MapType. In general, the hash will be equivalent to a hash
                 * of the MapType's Active Texture ID.
                 *
                 * \return size_t hash
                 */
                size_t hash() const { return hasher(_active); }

            protected:
                SWMuint _active;

                static std::hash<SWMuint> hasher;
            };

            //! Standard Diffuse Texture Map that describes the RGBA look of a texture.
            static const MapType DIFFUSE   (0);
            //! Specular Texture Map that describes the 'shininess' of a texture.
            static const MapType SPECULAR  (1);
            //! Normal Texture Map that describes the physical direction parts of a texture face.
            static const MapType NORMAL    (2);
            //! Emissive Texture Map that describes any light that is emitted from the texture.
            static const MapType EMISSIVE  (3);
        }
    }
}

namespace std {
    // Hashing Structure so that MapTypes can be used in an std::unordered_map
    template<> struct hash<Swarm::Texture::Type::MapType> {
        size_t operator() (const Swarm::Texture::Type::MapType &var) const {
            return var.hash();
        }
    };
}

namespace Swarm {

    namespace Texture {

        void cleanup();

        struct TexMap {
            TexMap() : _target(Type::TargetType::TEX_2D), _id(0) {}
            TexMap(const Type::TargetType target, SWMuint id) : _target(target), _id(id) {}
            const Type::TargetType target() const { return _target; }
            SWMuint id() const { return _id; }
            void bind(SWMuint active) const;
            bool operator==(const TexMap &rhs) const;
            bool operator!=(const TexMap &rhs) const { return !operator==(rhs); }
            bool operator< (const TexMap &rhs) const;
            bool operator<=(const TexMap &rhs) const { return operator<(rhs) || operator==(rhs); }
            bool operator> (const TexMap &rhs) const { return !operator<=(rhs); }
            bool operator>=(const TexMap &rhs) const { return !operator<(rhs); }
        protected:
            Type::TargetType _target;
            SWMuint _id;
        };

        enum FileType {
            PNG
        };

        TexMap loadTexFromFile(const std::string &path, FileType type);

        class Texture {
        public:
            void bind() const;

            void put(const Type::MapType &type, const TexMap &tex) { put(type.active(), tex); }
            void put(SWMuint active, const TexMap &tex);

            TexMap at(const Type::MapType &type) const { return at(type.active()); }
            TexMap at(SWMuint active) const;
            TexMap &operator[](const Type::MapType &type) { return operator[](type.active()); };
            TexMap &operator[](SWMuint active);

            // Comparison Operators
            bool operator==(const Texture &rhs) const;
            bool operator!=(const Texture &rhs) const { return !operator==(rhs); }
            bool operator< (const Texture &rhs) const;
            bool operator<=(const Texture &rhs) const { return operator<(rhs) || operator==(rhs); }
            bool operator> (const Texture &rhs) const { return !operator<=(rhs); }
            bool operator>=(const Texture &rhs) const { return !operator<(rhs); }

        protected:
            std::map<SWMuint, TexMap> _data;
        };

    }



    namespace Model {

        //! Enum representation of vector dimensionality
        /*!
         * This enumeration represents how many dimensions a vector can have, from 1 to 4.
         * Used in raw Model data loading.
         */
        enum VecType {
            ONE     = 1,
            TWO     = 2,
            THREE   = 3,
            FOUR    = 4
        };

        //! Structure representing a single vector point.
        /*!
         * This structure contains float data for a single vector point, of dimension between 1 and 4. Internally,
         * it is represented as a union of types v1 through v4, each representing the dimensionality. Ideally, an
         * instance of this structure should be accessed as the same type it was created with - which can be accessed
         * with the identifier \ref type - or possibly with a lower dimensionality. Undefined behaviour can result
         * when attempting to access a higher dimensionality than a VecVar has been created with.
         */
        struct VecVar {
            union Val {
                struct {
                    float x;
                    #if defined(SWARM_INCLUDE_GLM)
                    float vec() { return x; }
                    #endif
                } v1;
                struct {
                    float x;
                    float y;
                    #if defined(SWARM_INCLUDE_GLM)
                    glm::vec2 vec() { return glm::vec2(x, y); }
                    #endif
                } v2;
                struct {
                    float x;
                    float y;
                    float z;
                    #if defined(SWARM_INCLUDE_GLM)
                    glm::vec3 vec() { return glm::vec3(x, y, z); }
                    #endif
                } v3;
                struct {
                    float x;
                    float y;
                    float z;
                    float w;
                    #if defined(SWARM_INCLUDE_GLM)
                    glm::vec4 vec() { return glm::vec4(x, y, z, w); }
                    #endif
                } v4;

                explicit Val(float x)                            { v1.x = x; }
                explicit Val(float x, float y)                   { v2.x = x; v2.y = y; }
                explicit Val(float x, float y, float z)          { v3.x = x; v3.y = y; v3.z = z; }
                explicit Val(float x, float y, float z, float w) { v4.x = x; v4.y = y; v4.z = z; v4.w = w; }
            } val;
            VecType type;
            explicit VecVar()                                   : type(ONE),   val(0.0f)       {}
            explicit VecVar(float x)                            : type(ONE),   val(x)          {}
            explicit VecVar(float x, float y)                   : type(TWO),   val(x, y)       {}
            explicit VecVar(float x, float y, float z)          : type(THREE), val(x, y, z)    {}
            explicit VecVar(float x, float y, float z, float w) : type(FOUR),  val(x, y, z, w) {}

            #if defined(SWARM_INCLUDE_GLM)
            explicit VecVar(glm::vec2 in) : type(TWO),   val(in.x, in.y)             {}
            explicit VecVar(glm::vec3 in) : type(THREE), val(in.x, in.y, in.z)       {}
            explicit VecVar(glm::vec4 in) : type(FOUR),  val(in.x, in.y, in.z, in.w) {}
            #endif

        };

        //! An array-based implementation of the \ref VecVar data type.
        /*!
         * This class is a collection of VecVar data points in a single array. The size of the array is set at object
         * creation, and cannot be changed later. The object can, however, be set equal to another VecArray with a
         * different size.
         */
        class VecArray {
        public:

            //! VecArray Default Constructor
            /*!
             * Creates a VecArray with a size of '0'; effectively empty.
             */
            VecArray();

            //! VecArray Main Constructor
            /*!
             * Creates a VecArray of a specific size and allocates space for its contents.
             *
             * \param type a \ref VecType of data to store
             * \param size the size of this VecArray
             */
            VecArray(VecType type, size_t size);

            ~VecArray();

            VecArray(const VecArray &other) { *this = other; }
            VecArray &operator=(const VecArray &other);

            const VecVar &at(size_t index) const;
            VecVar &operator[](size_t index);

            size_t size() { return _size; }
            VecType type() { return _type; }
        protected:
            VecVar* data = nullptr;
            size_t _size;
            VecType _type = ONE;
        };

        namespace Type {

            //! Class used to represent a model Data Point type
            /*!
             * The DataType class is used to represent different types of Data Points, as well as store default values.
             * All DataType objects with the same Attribute ID are considered equivalent for the purposes of hashing
             * and identification. The Engine provides definitions for these static DataTypes by default:
             *
             *  \ref VERTEX
             *  \ref UV
             *  \ref NORMAL
             *  \ref COLOR
             *  \ref TANGENT
             *  \ref BITANGENT
             */
            class DataType {
            public:

                //! DataType main Constructor
                /*!
                 * \param type a \ref VecType to use
                 * \param attribID a GLuint (or unsigned int, if not using GL headers) describing the attribute to bind to
                 * \param defaultVal a \ref VecVar value to use as a default value for this DataType
                 * \sa VecType, VecVar
                 */
                DataType(VecType type, SWMuint attribID, VecVar defaultVal);

                //! DataType Copy Constructor
                /*!
                 * Creates a copy of a DataType. Sets the copy equivalent to the original.
                 *
                 * \param other DataType to copy
                 */
                DataType(const DataType &other);

                //! DataType assignment operator
                /*!
                 * Assigns this DataType to be equivalent to another DataType.
                 *
                 * \param other DataType to copy
                 * \return a reference to the current DataType
                 */
                DataType &operator=(const DataType &rhs);

                bool operator==(const DataType &rhs) const;
                bool operator!=(const DataType &rhs) const { return !operator==(rhs); }
                bool operator< (const DataType &rhs) const;
                bool operator<=(const DataType &rhs) const { return operator<(rhs) || operator==(rhs); }
                bool operator> (const DataType &rhs) const { return !operator<=(rhs); }
                bool operator>=(const DataType &rhs) const { return !operator<(rhs); }

                VecType type() const { return _type; }
                VecVar defaultValue() const { return _default_val; }
                SWMuint attribID() const { return _attrib_id; }
                void setAttribID(unsigned int attrib_id) { _attrib_id = attrib_id; }

            private:
                VecType _type;
                VecVar _default_val;
                SWMuint _attrib_id;
            };

            //! Standard Vertex positional data.
            static const DataType VERTEX    ( THREE, 1, VecVar(0.0f, 0.0f, 0.0f));
            //! Texture coordinate data.
            static const DataType UV        ( TWO,   2, VecVar(0.0f, 0.0f));
            //! Physical directional data.
            static const DataType NORMAL    ( THREE, 3, VecVar(0.0f, 1.0f, 0.0f));
            //! Color description data.
            static const DataType COLOR     ( FOUR,  4, VecVar(1.0f, 1.0f, 1.0f, 1.0f));
            //! Tangential directional data.
            static const DataType TANGENT   ( THREE, 5, VecVar(0.0f, 1.0f, 0.0f));
            //! Bi-Tangential directional data.
            static const DataType BITANGENT ( THREE, 6, VecVar(0.0f, 1.0f, 0.0f));

        }

    }
}

namespace std {
    template<> struct hash<Swarm::Model::Type::DataType> {
        size_t operator() (const Swarm::Model::Type::DataType &var) const {
            return hash<unsigned int>()((var.type()-1) + 4*var.attribID());
        }
    };
}

namespace Swarm {
    namespace Model {

        void cleanup();

        class RawModelDataIndexed;

        //! Storage object for a raw collection of model data points
        /*!
         * A RawModelData object is a raw collection of data points that make up a model. The exact type and
         * count of data points is dynamic and depends on the RawModelData object instance, but in general
         * should contain at least a basic positional vertex format.
         */
        class RawModelData {
        public:

            //! Default RawModelData Constructor
            /*!
             * Constructs an empty RawModelData collection.
             */
            RawModelData() {}

            //! RawModelData Copy Constructor
            /*!
             * Constructs a new RawModelData collection and gives it equivalent data to another.
             *
             * \param other RawModelData collection to copy
             */
            RawModelData(const RawModelData &other);

            //! RawModelData Assignment Operator
            /*!
             * Assigns this RawModelData collection as equivalent to another.
             *
             * \param other RawModelData collection to copy
             * \return a reference to this RawModelData collection
             */
            RawModelData &operator=(const RawModelData &other);

            //! Set the raw point data associated with a \ref DataType
            /*!
             * Adds a selection of raw point data to this RawModelData collection under the \ref DataType given.
             * If the new data's size is different than the current collection's size, the current collection is
             * first resized to match.
             *
             * \param type a \ref DataType to store the given point data under
             * \param data a \ref VecArray of point data to store
             * \return a reference to this RawModelData collection, for chaining put() commands
             * \sa DataType, VecArray, resize()
             */
            RawModelData &put(const Type::DataType &type, VecArray data);

            //! Set the raw point data associated with a \ref DataType
            /*!
             * Adds a selection of raw point data to this RawModelData collection under the \ref DataType given, after
             * compacting the given data into a \ref VecArray object. If the new data's size is different than the
             * current collection's size, the current collection is first resized to match.
             *
             * \param type a \ref DataType to store the given point data under
             * \param data an array of float values; the stride of said values is determined by the given \ref DataType
             * \param size the count of data points given
             * \return a reference to this RawModelData collection, for chaining put() commands
             * \sa DataType, VecArray, resize()
             */
            RawModelData &put(const Type::DataType &type, float *data, size_t size);

            //! Does this RawModelData collection have data points associated with the given \ref DataType
            bool exists(const Type::DataType &type) const;

            //! Get the data points associated with the given \ref DataType
            /*!
             * Attempts to retrieve the data point group that is associated with the given \ref DataType. If this
             * RawModelData collection does not have a data point group associated with the given \ref DataType,
             * returns a new \ref VecArray of size '0'.
             *
             * \param type a \ref DataType associated with a data point group
             * \return the \ref VecArray associated with the given \ref DataType, or a new \ref VecArray of size '0'
             * \sa DataType, VecArray
             */
            const VecArray &at(const Type::DataType &type) const;

            //! Get the data points associated with the given \ref DataType
            /*!
             * Attempts to retrieve the data point group that is associated with the given \ref DataType. If this
             * RawModelData collection does not have a data point group associated with the given \ref DataType,
             * constructs a new \ref VecArray of size '0' with the given \ref DataType and returns it.
             *
             * \param type a \ref DataType associated with a data point group
             * \return the \ref VecArray associated with the given \ref DataType
             * \sa DataType, VecArray
             */
            VecArray &operator[](const Type::DataType &type);


            std::unordered_map<Type::DataType, VecArray>::iterator begin() { return data_map.begin(); }
            std::unordered_map<Type::DataType, VecArray>::const_iterator begin() const { return data_map.begin(); }
            std::unordered_map<Type::DataType, VecArray>::iterator end() { return data_map.end(); }
            std::unordered_map<Type::DataType, VecArray>::const_iterator end() const { return data_map.end(); }

            size_t size() const { return _size; };

            //! Resize this RawModelData collection
            /*!
             * Resizes this RawModelData collection to the given size. If this RawModelData collection's current size
             * is larger than the given size, this collection's size is truncated to match, clipping off excess data.
             * If this RawModelData collection's current size is smaller than the given size, this collection's size
             * is extended to match, filling in the new space with default values determined by the stored data's
             * \ref DataType. If the size given matches this RawModelData collection's size, nothing happens.
             *
             * \param size a new size
             * \sa DataType
             */
            void resize(size_t size);

            //! Indexes this RawModelData collection into a \ref RawModelDataIndexed collection
            /*!
             * Runs through the stored data point groups, and indexes them so that lateral copies of data points
             * (i.e. data points at index 3 and 8 have the same vertex, uv, AND normal, in a RawModelData collection
             * that contains data points for vertices, uvs, and normals) are removed, resulting in only one copy a
             * point for each set of duplicates. The \a epsilon_factor determines how exact the floating point
             * comparison is when determining equality. Floating point comparison happens on the scale of
             * \f$10^{-1*epsilonfactor}\f$, and defaults to an \a epsilon_factor of '5', or on the scale of 0.00001f.
             *
             * \param epsilon_factor The scale of floating point comparison
             * \return a \ref RawModelDataIndexed collection that stores the indexed data points
             * \sa RawModelDataIndexed
             */
            virtual RawModelDataIndexed index(short epsilon_factor = 5);

        protected:
            std::unordered_map <Type::DataType, VecArray> data_map;

            size_t _size = 0;
        };

        //! Computes Tangent data for a \ref RawModelData collection
        /*!
         * Computes the Tangent data for a given \ref RawModelData collection, using a given selection of \ref DataType
         * values. If the given \ref DataType values for vertex, uv, and normal types do not exist for the given
         * \ref RawModelData collection, or if the given \ref DataType values don't have the right dimensionality (see
         * parameters), this function does nothing and returns false. For a function that performs the
         * same functionality, but with default \ref DataType values, see \ref computeTangents(RawModelData&).
         *
         * \param raw_model_data \ref RawModelData collection to compute Tangents for
         * \param vertex_type \ref DataType that represents vertex positions; must have at least 3 dimensions
         * \param uv_type \ref DataType that represents texture coordinates; must have at least 2 dimensions
         * \param normal_type \ref DataType that represents normal directions; must have at least 3 dimensions
         * \param tangent_type \ref DataType that represents the produced tangents; must have exactly 3 dimensions
         * \param bitangent_type \ref DataType that represents the produced bitangents; must have exactly 3 dimensions
         * \return True upon a successful computation, false otherwise
         * \sa RawModelData, DataType, computeTangents(RawModelData&)
         */
        bool computeTangents(RawModelData &raw_model_data,
                             const Type::DataType &vertex_type, const Type::DataType &uv_type,
                             const Type::DataType &normal_type,
                             const Type::DataType &tangent_type, const Type::DataType &bitangent_type);

        //! Computes Tangent data for a \ref RawModelData collection
        /*!
         * Performs the same functionality as \ref computeTangents(RawModelData&, Type::DataType&, Type::DataType&,
         * Type::DataType&, Type::DataType&, Type::DataType&), but with the standard defaults for the available
         * \ref DataType values as follows:
         *
         *  vertex_type:    \ref Type::VERTEX
         *  uv_type:        \ref Type::UV
         *  normal_type:    \ref Type::NORMAL
         *  tangent_type:   \ref Type::TANGENT
         *  bitangent_type: \ref Type::BITANGENT
         *
         * \param raw_model_data \ref RawModelData collection to compute Tangents for
         * \return True upon a successful computation, false otherwise
         * \sa RawModelData, DataType, computeTangents(RawModelData&, Type::DataType&, Type::DataType&, Type::DataType&, Type::DataType&, Type::DataType&)
         */
        static inline bool computeTangents(RawModelData &raw_model_data) {
            return computeTangents(raw_model_data, Type::VERTEX, Type::UV, Type::NORMAL, Type::TANGENT,
                                   Type::BITANGENT);
        }

        //! Storage object for a raw collection of indexed model data points
        /*!
         * A RawModelDataIndexed object is a raw collection of data points that make up a model. Unlike a \ref RawModelData
         * object - which a RawModelDataIndexed object inherits from - the stored points are indexed such that
         * duplicate sets of lateral point data are removed (i.e. when index 3 and 8 have the same vertex, uv, AND
         * normal point data for an object that has data for vertices, uvs, and normals) and indices are added to the
         * data collection in replacement.
         */
        class RawModelDataIndexed : public RawModelData {
        public:

            //! Default RawModelDataIndexed Constructor
            /*!
             * Constructs an empty RawModelDataIndexed collection.
             */
            RawModelDataIndexed() {}

            ~RawModelDataIndexed();

            //! RawModelDataIndexed Copy Constructor
            /*!
             * Constructs a new RawModelDataIndexed collection and gives it equivalent data to another.
             *
             * \param other RawModelDataIndexed collection to copy
             */
            RawModelDataIndexed(const RawModelDataIndexed &other);

            //! RawModelDataIndexed Assignment Operator
            /*!
             * Assigns this RawModelDataIndexed collection as equivalent to another.
             *
             * \param other RawModelDataIndexed collection to copy
             * \return a reference to this RawModelDataIndexed collection
             */
            RawModelDataIndexed &operator=(const RawModelDataIndexed &other);

            //! Set the index data
            /*!
             * Sets this RawModelDataIndexed collection's indices to the given indices. Index values must fall within
             * the collection's data size, otherwise undefined behaviour occurs.
             *
             * \param indices an array of short values representing indices
             * \param size the count of indices given
             * \return a reference to this RawModelDataIndexed collection, for chaining put() commands
             */
            RawModelDataIndexed &putIndices(unsigned int *indices, size_t size);

            //! Get the indices for this RawModelDataIndexed
            /*!
             * Retrieves the collection of indices stored for this RawModelDataIndexed collection.
             *
             * \return the stored array of indices, or \a nullptr if no indices are stored
             */
            unsigned int *indices() const { return _indices; };

            size_t indexSize() const { return _index_size; };

            //! Further indexes this RawModelDataIndexed collection into another RawModelDataIndexed collection
            /*!
             * Attempts to further refine and index this RawModelDataIndexed collection using the same method of
             * indexing as \ref RawModelData::index(). Unless data has been manually altered, usually results in no
             * change between the old and new RawModelDataIndexed collections.
             *
             * \param epsilon_factor The scale of floating point comparison
             * \return a new RawModelDataIndexed collection
             * \sa RawModelData::index()
             */
            virtual RawModelDataIndexed index(short epsilon_factor = 5);

        protected:
            unsigned int *_indices = nullptr;
            size_t _index_size;
        };

        //! Loads an OBJ file as a \ref RawModelData collection
        /*!
         * Attempts to open a given OBJ file and parse its contents. Contents are parsed into a \ref RawModelData
         * collection, with the vertices, uvs, and normals of the OBJ file stored under the given \ref DataType values.
         * For a function that does the same thing, but with default \ref DataType values, see \ref loadFromOBJ(const char*).
         * OBJ file loading currently supports the following features:
         *
         *  - Parsing Vertices, UVs, Normals, and Faces
         *    - UVs may be omitted from Face declarations, but if omitted once they must be omitted for the entirety of the object
         *    - Faces may be Triangles or Quads
         *
         * \param path location of the OBJ file to be read; path may be relative or absolute
         * \param vertex_type \ref DataType that represents the vertex positions; must have exactly 3 dimensions
         * \param uv_type \ref DataType that represents the texture coordinates; must have exactly 2 dimensions
         * \param normal_type \ref DataType that represents the normal directions; must have exactly 3 dimensions
         * \return \ref RawModelData collection that stores the read data
         * \throw ModelLoadingException Throws a \ref ModelLoadingException when there is a problem opening the given OBJ file,
         * or when there is a structural problem with the given OBJ file, causing an error in parsing.
         * \throw FileParseException Throws a \ref FileParseException when there is a read error in the file beyond a
         * structural problem.
         * \sa RawModelData, DataType, loadFromOBJ(const char*)
         */
        RawModelData loadFromOBJ(const char *path, const Type::DataType &vertex_type, const Type::DataType &uv_type,
                                 const Type::DataType &normal_type);

        //! Loads an OBJ file as a \ref RawModelData collection
        /*!
         * Performs the same functionality as \ref loadFromOBJ(const char*, Type::DataType&, Type::DataType&, Type::DataType&),
         * but with the standard defaults for the available \ref DataType values as follows:
         *
         *  vertex_type:    \ref Type::VERTEX
         *  uv_type:        \ref Type::UV
         *  normal_type:    \ref Type::NORMAL
         *
         * \param path location of the OBJ file to be read; path may be relative or absolute
         * \return \ref RawModelData collection that stores the read data
         * \throw ModelLoadingException Throws a \ref ModelLoadingException when there is a problem opening the given OBJ file,
         * or when there is a structural problem with the given OBJ file, causing an error in parsing.
         * \throw FileParseException Throws a \ref FileParseException when there is a read error in the file beyond a
         * structural problem.
         * \sa RawModelData, DataType, loadFromOBJ(const char*, Type::DataType&, Type::DataType&, Type::DataType&)
         */
        static inline RawModelData loadFromOBJ(const char *path) {
            return loadFromOBJ(path, Type::VERTEX, Type::UV, Type::NORMAL);
        }

        //! Storage object for OpenGL Buffer and VAO IDs
        /*!
         * A Model object stores a collection of Buffer IDs representing OpenGL Data Buffers. The IDs are created with
         * data from a \ref RawModelDataIndexed collection; only one Model needs to be created for each unique
         * \ref RawModelDataIndexed collection. Models can be rendered by binding their Vertex Array Object (VAO) IDs,
         * which can be retrieved with \ref ::vao(), and are generated on an as needed basis for each OpenGL context,
         * depending on which one is active in the current thread.
         */
        class Model {
        public:

            //! Default Model Constructor
            /*!
             * Creates a default empty Model object; cannot be used for rendering.
             */
            Model() {};

            //! Model Creation Constructor
            /*!
             * Creates a loaded Model object with Buffer IDs dependent on the passed \ref RawModelDataIndexed collection.
             *
             * \param data \ref RawModelDataIndexed collection to use
             */
            Model(const RawModelDataIndexed &data);

            //! Model Copy Constructor
            /*!
             * Copies another Model object, making both Model objects have the same Buffer and VAO IDs.
             *
             * \param other Model to copy
             */
            Model(const Model &other);

            //! Assignment Operator
            /*!
             * Assigns this Model object equal to another, making both Model objects have the same Buffer and VAO IDs.
             *
             * \param other Model to copy
             * \return reference to the current Model, for chaining statements
             */
            Model &operator=(const Model &other);

            //! Get this Model's VAO ID
            /*!
             * Retrieves a VAO ID for the current model, based on which OpenGL context is active in the current thread.
             * If a VAO has not yet been generated for this Model and OpenGL context combination, generate a new VAO
             * before retrieving.
             *
             * \return VAO ID for this Model
             */
            SWMuint vao();

            //! Get this Model's number of Elements (Indices)
            size_t elementCount() const { return _element_count; }

            //! Has this Model been properly loaded yet? (Either from Creation or Copy/Assignment)
            bool loaded() const { return _loaded; }

            bool operator==(const Model &rhs) const;
            bool operator!=(const Model &rhs) const { return !operator==(rhs); }
            bool operator< (const Model &rhs) const;
            bool operator<=(const Model &rhs) const { return operator<(rhs) || operator==(rhs); }
            bool operator> (const Model &rhs) const { return !operator<=(rhs); }
            bool operator>=(const Model &rhs) const { return !operator<(rhs); }

            //! Cleanup all Model Objects
            /*!
             * Releases all Buffers and VAOs created by the construction of new Models. Generally called by
             * \ref Swarm::Model::cleanup().
             */
            static void cleanup();

        protected:
            void genBuffers(const RawModelDataIndexed &data);
            void genVAO();

            struct BufferEntry {
                SWMuint buffer;
                SWMuint attrib;
                VecType type;
                friend bool operator<(const BufferEntry &lhs, const BufferEntry &rhs) { return lhs.buffer < rhs.buffer; }
            };

            bool _loaded = false;
            std::set<BufferEntry> _data_buffers;
            SWMuint _element_buffer = 0;
            size_t _element_count = 0;

        };
    }

    namespace Render {

        void init();
        void cleanup();

        class Renderer;

        //! Representation of a GL Uniform
        /*!
         * The Uniform object is a representation of a GL Uniform. Changing a Uniform object will not change the Uniform
         * data bound to the GPU until \ref ::bind() is called, which is usually done internally by a \ref Renderer
         * object.
         * \sa ::bind()
         */
        class Uniform {
        public:
            friend class Renderer;

            //! Uniform Constructor
            /*!
             * Create a new Uniform object with a specified name ID.
             *
             * \param name name used to represent the Uniform in a GLSL shader
             * \sa ::name, ::setName()
             */
            Uniform(const std::string &name) : _name(name) {}

            //! Gets the Uniform's name
            /*!
             * Gets the name ID associated with this Uniform.
             *
             * \return name name used to represent the Uniform in a GLSL shader
             * \sa ::setName()
             */
            std::string name() { return _name; }

            void setf (SWMsizei count, SWMsizei stride, SWMfloat *data);
            void seti (SWMsizei count, SWMsizei stride, SWMint   *data);
            void setui(SWMsizei count, SWMsizei stride, SWMuint  *data);
            void setm (SWMsizei count, SWMsizei width,  SWMsizei height, SWMfloat *data);

            //! Binds the Uniform
            /*!
             * Binds the current data associated with this Uniform to the \ref Program used by the specified
             * \ref Renderer. Usually only called internally by a \ref Renderer object. Empty Uniform objects are not
             * valid to bind with.
             *
             * \param render \ref Renderer object to bind to
             * \sa ::empty(), ::clear()
             */
            void bind(const Renderer &render);

            //! Clear this Uniform
            /*!
             * Clears the data that this Uniform object has stored and sets it to empty.
             * \sa ::bind(), ::empty()
             */
            void clear();

            //! Is this Uniform empty?
            /*!
             * Determines if this Uniform object is 'empty' and has no data. Empty Uniform objects are not valid to
             * bind with.
             *
             * \return true if empty, false otherwise
             * \sa ::bind(), ::clear()
             */
            bool empty() { return _has_data; }

        protected:
            const std::string _name;
            bool _has_data = false;

            enum {
                F, I, UI, M
            } type;
            union {
                struct { SWMsizei count; SWMsizei stride; SWMfloat* data; } f;
                struct { SWMsizei count; SWMsizei stride; SWMint*   data; } i;
                struct { SWMsizei count; SWMsizei stride; SWMuint*  data; } ui;
                struct { SWMsizei count; SWMsizei width;  SWMsizei height; SWMfloat* data; } m;
            } data;

            #if defined(SWARM_BOOST_AVAILABLE)
            void clear(boost::lock_guard<boost::mutex>&);
            #endif
        };

        //! Object representing a collection of Render data
        /*!
         * A RenderObject object is a pure virtual class interface used for rendering. Each virtual method has certain
         * constraints that must be followed when they are implemented, as describes in the respective method
         * documentation. In addition, a simple 'static' position RenderObject can be created with the
         * \ref createStaticRenderObject() static method.
         *
         * \sa ::createStaticRenderObject()
         */
        class RenderObject {
        public:
            virtual ~RenderObject() {}

            //! Get the \ref Model::Model associated with this RenderObject
            virtual Model::Model &model() const = 0;

            //! Get the \ref Texture::Texture associated with this RenderObject
            virtual Texture::Texture &texture() const = 0;

            //! Render this render object using the given \ref Renderer
            /*!
             * This method uses the given \ref Renderer to render this RenderObject.
             *
             * Implementation Constraints: Assumed to bind a \ref Model's VAO and use a glDraw call (most likely
             * glDrawElements), though could do nothing. Also a good idea to bind a model matrix.
             *
             * Multithreading: Rendering runs in its own thread, so any data that a RenderObject uses to render must
             * be sufficiently mutually excluded. Renderers are not accessed simultaneously by the rendering thread,
             * and neither are individual RenderObjects, so constant data is fine.
             *
             * \param renderer \ref Renderer object to render with
             */
            virtual void render(const Renderer &renderer) const = 0;

            //! Create a new statically positioned RenderObject
            /*!
             * Constructs and returns a new statically positioned RenderObject. The RenderObject has a position, scale,
             * and rotation, but they cannot be changed after creation. The engine owns the pointer that is returned.
             *
             * \param model \ref Model::Model to use
             * \param texture \ref Texture::Texture to use
             * \param position_x Float representing X-Position
             * \param position_y Float representing Y-Position
             * \param position_z Float representing Z-Position
             * \param scale_x Float representing X-Scale
             * \param scale_y Float representing Y-Scale
             * \param scale_z Float representing Z-Scale
             * \param rotate_x Float representing X-Rotate
             * \param rotate_y Float representing Y-Rotate
             * \param rotate_z Float representing Z-Rotate
             * \return pointer to new RenderObject
             */
            static RenderObject* createStaticRenderObject(Model::Model &model, Texture::Texture &texture,
                                                          float position_x, float position_y, float position_z,
                                                          float scale_x,    float scale_y,    float scale_z,
                                                          float rotate_x,   float rotate_y,   float rotate_z);

            #if defined(SWARM_INCLUDE_GLM)
            static RenderObject* createStaticRenderObject(Model::Model &model, Texture::Texture &texture, glm::vec3 position, glm::vec3 scale, glm::vec3 rotate);
            #endif

        protected:
            RenderObject() {}
        };

        //! A Collection of \ref RenderObject pointers
        /*!
         * A RenderObjectCollection is a complex, thread-safe collection of \ref RenderObject pointers.
         * \ref RenderObject objects that have the same \ref Texture::Texture are mapped together under that
         * \ref Texture::Texture. Internally, \ref RenderObject pointers are stored in a set, so only one pointer to
         * each actual \ref RenderObject can be stored. In addition, any inserts or erases of RenderObjects in the
         * collection are only applied after \ref ::flush() is called. Normally, this is called automatically by the
         * Window system.
         */
        class RenderObjectCollection {
        public:

            RenderObjectCollection();
            ~RenderObjectCollection();

            //! Insert a \ref RenderObject to this collection
            /*!
             * Queues an insert on the next \ref ::flush() for the given \ref RenderObject pointer into this
             * collection, if it hasn't already been inserted. If a nullptr is given, does nothing.
             *
             * \param object \ref RenderObject pointer to insert
             */
            void insert(RenderObject* object);

            //! Erase a \ref RenderObject from this collection
            /*!
             * Queues an erase on the next \ref ::flush() for the given \ref RenderObject pointer from this collection
             * if it exists. If the given pointer is null, or it does not exist in this collection, does nothing.
             * Erasing a \ref RenderObject pointer does delete the object, only remove the pointer from this collection.
             *
             * \param object \ref RenderObject pointer to erase
             */
            void erase(RenderObject* object);

            //! Flushes all changes
            /*!
             * Applies any \ref ::insert() and \ref ::erase() changes that have been queued. This is done automatically
             * by the Window system during the Render cycle, so there is no need to manually call it in most cases.
             */
            void flush();

            //! Clears this collection
            /*!
             * Clears this collection, and erases all \ref RenderObject pointers.
             *
             * \sa erase()
             */
            void clear();

            void render(const Renderer &renderer);

            //! Gets a set of \ref RenderObject pointers
            /*!
             * Retrieves the set of \ref RenderObject pointers that are mapped to a specific \ref Texture::Texture, or
             * an empty set if there are none for the specified \ref Texture::Texture.
             *
             * \param tex \ref Texture::Texture to use as a key
             * \return a set of \ref RenderObject pointers
             */
            std::set<RenderObject*> &get(const Texture::Texture &tex) { return _data[tex]; }

            //! Gets the set of \ref RenderObject pointers
            /*!
             * Retrieves the set of \ref RenderObject pointers that are mapped to a specific \ref Texture::Texture, or
             * an empty set if there are none for the specified \ref Texture::Texture.
             * Alias for \ref ::get().
             *
             * \param tex \ref Texture::Texture to use as a key
             * \return a set of \ref RenderObject pointers
             * \sa get()
             */
            std::set<RenderObject*> &operator[](const Texture::Texture &tex) { return _data[tex]; }

            std::map<Texture::Texture, std::set<RenderObject*>>::iterator       begin()       { return _data.begin(); }
            std::map<Texture::Texture, std::set<RenderObject*>>::const_iterator begin() const { return _data.begin(); }
            std::map<Texture::Texture, std::set<RenderObject*>>::iterator       end()       { return _data.end(); }
            std::map<Texture::Texture, std::set<RenderObject*>>::const_iterator end() const { return _data.end(); }

        protected:
            std::map<Texture::Texture, std::set<RenderObject*>> _data;
            std::set<RenderObject*> _queue_insert;
            std::set<RenderObject*> _queue_erase;
            size_t _uid;
        };

        //! Class-Enum used to represent GLenum Shader types.
        /*!
         * The ShaderType class is used as a manual enumeration to represent different Shader types in OpenGL. The
         * ShaderType constructor is private, and it cannot be created other than by the static Types defined in the
         * ShaderType namespace. This Class-Enum is essentially used to represent an OpenGL type
         * w/o using 3rd party libraries/loaders (e.g. GLEW), but will return a GLenum if \ref SWARM_INCLUDE_GLEW
         * is defined.
         */
        class ShaderType {
        private:
            ShaderType(SWMenum type, const std::string &name) : _type(type), _name(name) {};
            SWMenum _type;
            std::string _name;
        public:

            //! VERTEX maps to the GLenum GL_VERTEX_SHADER
            static ShaderType VERTEX;
            //! GEOMETRY maps to the GLenum GL_GEOMETRY_SHADER
            static ShaderType GEOMETRY;
            //! FRAGMENT maps to the GLenum GL_FRAGMENT_SHADER
            static ShaderType FRAGMENT;

            SWMenum type()     const { return _type; }
            operator SWMenum() const { return _type; }
            std::string name() const { return _name; }

            ShaderType(const ShaderType &other) { operator=(other); }
            ShaderType &operator=(const ShaderType &rhs);

        };

        class Shader {
        public:
            virtual ~Shader() {}

            virtual SWMuint ID()             const = 0;
            virtual operator SWMuint()       const = 0;
            virtual const ShaderType &type() const = 0;

            static Shader* compileFromSource(const std::string &src, const ShaderType &type);
            static Shader* compileFromFile(const std::string &path, const ShaderType &type);

        private:
            Shader() {}
            friend class ShaderInternal;
        };

        class Program {
        public:
            virtual ~Program() {}

            //! Get the GLuint ID representing this linked shader Program.
            virtual SWMuint ID() const = 0;

            //! Get the ID for a specified shader Uniform variable.
            /*!
             * Scans the linked Program for the specified shader Uniform and returns its ID.
             * If the Uniform is found, it is cached in an internal map for fast future lookup.
             */
            virtual SWMint uniform(const std::string &name) = 0;

            //! Clears the Uniform Cache.
            /*!
             * Clears the internal map used for caching Uniforms. Should be done once in the beginning of each
             * render cycle. Usually done in the start phase of a \ref Renderer;
             */
            virtual void clearUniformCache() = 0;

            static Program* compile(Shader* shaders[], size_t count);

        private:
            Program() {}
            friend class ProgramInternal;
        };

        struct CameraPosition {
            struct CPos {
                float x;
                float y;
                float z;
                #if defined(SWARM_INCLUDE_GLM)
                CPos &operator=(const glm::vec3 &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; }
                CPos &operator=(const CPos &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; }
                operator glm::vec3() const { return glm::vec3(x, y, z); }
                glm::vec3 vec() const { return glm::vec3(x, y, z); }
                #endif
                bool operator==(const CPos &rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
                bool operator!=(const CPos &rhs) { return !operator==(rhs); }
            } position, look_at, up;
            CameraPosition() :
                    position(CPos{0.0f, 0.0f, 0.0f}),
                    look_at(CPos{0.0f, 0.0f, 1.0f}),
                    up(CPos{0.0f, 1.0f, 0.0f}) {}
            CameraPosition(float pos_x, float pos_y, float pos_z, float look_x, float look_y, float look_z, float up_x, float up_y, float up_z) :
                    position(CPos{pos_x, pos_y, pos_z}),
                    look_at(CPos{look_x, look_y, look_z}),
                    up(CPos{up_x, up_y, up_z}) {}
            #if defined(SWARM_INCLUDE_GLM)
            CameraPosition(glm::vec3 pos, glm::vec3 look, glm::vec3 up) :
                    position(CPos{pos.x, pos.y, pos.z}),
                    look_at(CPos{look.x, look.y, look.z}),
                    up(CPos{up.x, up.y, up.z}) {}
            #endif
            bool operator==(const CameraPosition &rhs) { return position == rhs.position && look_at == rhs.look_at && up == rhs.up; }
            bool operator!=(const CameraPosition &rhs) { return !operator==(rhs); }
        };

        class Camera {
        public:

            enum MoveType {
                INSTANT,
                SMOOTH,
                LERP
            };

            virtual MoveType moveType() const = 0;
            virtual void setMoveType(MoveType type) = 0;

            virtual float speed() const = 0;
            virtual void setSpeed(float speed) = 0;

            virtual float fov() const = 0;
            virtual void setFOV(float fov) = 0;

            virtual float viewDistance() const = 0;
            virtual void setViewDistance(float distance) = 0;

            virtual void setPosition(const CameraPosition &pos, bool instant = false) = 0;
            virtual CameraPosition position(bool instant = false) = 0;

            virtual void update(double delta_time) = 0;

            static Camera* create(float speed = 1.0f, MoveType type = INSTANT);
            static Camera* create(const CameraPosition &pos, float speed = 1.0f, MoveType type = INSTANT);

            #if defined(SWARM_INCLUDE_GLM)
            virtual glm::mat4 viewMatrix() const = 0;
            virtual glm::mat4 projectionMatrix(int width, int height) const = 0;
            #endif

        private:
            Camera() {}
            friend class CameraInternal;
        };

        #if defined(SWARM_BOOST_AVAILABLE)
        boost::mutex &getWindowContextMutex();
        #endif

        class WindowInternal;
        class RendererInternal;

        class Window {
        public:
            Window();
            Window(unsigned int width, unsigned int height, const std::string &name = "Swarm Engine Instance");
            Window(const Window &rhs);

            Window &operator=(const Window &rhs);

            bool operator==(const Window &rhs) const;
            bool operator!=(const Window &rhs) const { return !operator==(rhs); }

            int width()  const;
            int height() const;
            int posX() const;
            int posY() const;
            std::string name() const;

            bool focused() const;

            void setVisible(bool visible);
            bool visible() const;

            void setCamera(Camera* camera);
            Camera* camera() const;

            void cursorPos(double &pos_x, double &pos_y);

            void attachRenderer(const Renderer* renderer, RenderObjectCollection &collection, bool reverse = false);
            void setClearColor(float r, float g, float b, float a = 1.0f);

            #if defined(SWARM_INCLUDE_GLFW)
            GLFWwindow* window() const;
            operator GLFWwindow*() const;
            #endif

        protected:
            friend class WindowInternal;
            friend class RendererInternal;
            Window(WindowInternal* window);
            WindowInternal* _window;

        };

        class Renderer {
        public:
            typedef void (*RenderCycleFunc)(const Renderer&, const Window&);

            virtual ~Renderer() {}

            virtual Program* program() const = 0;

            enum RenderCyclePhase {
                START,
                END
            };

            virtual void setRenderCycleFunction(RenderCyclePhase phase, RenderCycleFunc function) = 0;

            enum MatrixUniformType {
                MODEL,
                VIEW,
                PROJECTION
            };

            virtual void setUniformName(MatrixUniformType type, const std::string &name) = 0;
            virtual std::string uniformName(MatrixUniformType type) const = 0;

            virtual void setTextureName(SWMuint active, const std::string &name) = 0;
            virtual void setTextureName(const Texture::Type::MapType &type, const std::string &name) = 0;
            virtual std::string textureName(SWMuint active) const = 0;
            virtual std::string textureName(const Texture::Type::MapType &type) const = 0;

            virtual void insertUniform(Uniform &uniform) = 0;

            virtual void bindUniformsMatrix(const Window &window) const = 0;
            virtual void bindUniformsTexture() const = 0;
            virtual void bindUniformsCustom() const = 0;

            static Renderer* create(Program* program);

        private:
            Renderer() {}
            friend class RendererInternal;
        };

    }
}