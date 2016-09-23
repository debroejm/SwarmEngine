#include "../../Render.h"

#include "../../Core.h"

#include <map>

using namespace Swarm::Logging;


namespace Swarm {
    namespace Model {

        vector<GLuint> registeredBuffers;
        vector<GLuint> registeredVAOs;
        vector<glm::vec3*> registeredNormals;

        void registerStaticNormalArray(glm::vec3 normals[]) {
            for(int i = 0; i < registeredNormals.size(); i++) {
                if(registeredNormals[i] == normals) break;
            }
            registeredNormals.push_back(normals);
        }

        void cleanupBuffers() {
            for(int i = 0; i < registeredBuffers.size(); i++) {
                Log::log_render(INFO) << "Deleting Buffer [" << registeredBuffers[i] << "]";
                glDeleteBuffers(1, &registeredBuffers[i]);
            }

            for(int i = 0; i < registeredVAOs.size(); i++) {
                Log::log_render(INFO) << "Deleting VAO [" << registeredVAOs[i] << "]";
                glDeleteVertexArrays(1, &registeredVAOs[i]);
            }

            for(int i = 0; i < registeredNormals.size(); i++) {
                delete [] registeredNormals[i];
            }
        }

        struct PackedVertex{
            glm::vec3 position;
            glm::vec2 uv;
            glm::vec3 normal;
            bool operator<(const PackedVertex that) const{
                return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
            };
        };

        struct PackedBone{
            glm::vec3 bone;
            glm::vec2 uv;
            glm::vec3 normal;
            bool operator<(const PackedBone that) const{
                return memcmp((void*)this, (void*)&that, sizeof(PackedBone))>0;
            };
        };

        struct PreBoneData {
            glm::vec3 position;
            int parentID;
        };

        struct PreVertexData {
            glm::vec3 position;
            int boneID;
        };

        int getClosestBoneID(glm::vec3 in_vertex, vector<PreBoneData> &bones)
        {
            int closestID = -1;
            double closeDist = 0.0;
            for(int i = 0; i < bones.size(); i++)
            {
                glm::vec3 bPos = bones[i].position;
                double newDist = sqrt( pow(bPos.x - in_vertex.x, 2) + pow(bPos.y - in_vertex.y, 2) + pow(bPos.z - in_vertex.z, 2) );
                if(closestID == -1)
                {
                    closestID = i;
                    closeDist = newDist;
                }
                else if( newDist < closeDist )
                {
                    closestID = i;
                    closeDist = newDist;
                }
            }

            return closestID;
        }

        bool getSimilarVertexIndex_fast(
                PackedVertex & packed,
                std::map<PackedVertex,unsigned short> & VertexToOutIndex,
                unsigned short & result
        ){
            std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
            if ( it == VertexToOutIndex.end() ){
                return false;
            }else{
                result = it->second;
                return true;
            }
        }

        bool getSimilarBoneIndex_fast(
                PackedBone & packed,
                std::map<PackedBone,unsigned short> & BoneToOutIndex,
                unsigned short & result
        )
        {
            std::map<PackedBone,unsigned short>::iterator it = BoneToOutIndex.find(packed);
            if ( it == BoneToOutIndex.end() ){
                return false;
            }else{
                result = it->second;
                return true;
            }
        }

        void indexVBO(
                std::vector<glm::vec3> & in_vertices,
                std::vector<glm::vec2> & in_uvs,
                std::vector<glm::vec3> & in_normals,

                std::vector<unsigned short> & out_indices,
                std::vector<glm::vec3> & out_vertices,
                std::vector<glm::vec2> & out_uvs,
                std::vector<glm::vec3> & out_normals
        ){
            std::map<PackedVertex,unsigned short> VertexToOutIndex;

            // For each input vertex
            for ( unsigned int i=0; i<in_vertices.size(); i++ ){

                PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};


                // Try to find a similar vertex in out_XXXX
                unsigned short index;
                bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

                if ( found ){ // A similar vertex is already in the VBO, use it instead !
                    out_indices.push_back( index );
                }else{ // If not, it needs to be added in the output data.
                    out_vertices.push_back( in_vertices[i]);
                    out_uvs     .push_back( in_uvs[i]);
                    out_normals .push_back( in_normals[i]);
                    unsigned short newindex = (unsigned short)out_vertices.size() - 1;
                    out_indices .push_back( newindex );
                    VertexToOutIndex[ packed ] = newindex;
                }
            }
        }

        void indexVBO(
                vector<PreVertexData> &in_vertices,
                vector<PreBoneData> &in_bones,
                vector<glm::vec2> &in_uvs,

                vector<unsigned short> &out_indices,
                vector<PreBoneData> &out_bones,
                vector<glm::vec2> &out_uvs,
                vector<PreVertexData> &out_normals
        ){
            std::map<PackedVertex,unsigned short> BoneToOutIndex;

            std::vector<glm::vec3> in_normals;

            for( unsigned int i=0; i<in_vertices.size(); i++ )
            {
                glm::vec3 bonePos = in_bones[in_vertices[i].boneID].position;
                in_normals.push_back( glm::vec3(in_vertices[i].position.x - bonePos.x,
                                                in_vertices[i].position.y - bonePos.y,
                                                in_vertices[i].position.z - bonePos.z) );
            }

            // For each input vertex
            for ( unsigned int i=0; i<in_vertices.size(); i++ ){

                PackedVertex packed = {in_bones[in_vertices[i].boneID].position, in_uvs[i], in_normals[i]};

                // Try to find a similar vertex in out_XXXX
                unsigned short index;
                bool found = getSimilarVertexIndex_fast( packed, BoneToOutIndex, index);

                if ( found ){ // A similar vertex is already in the VBO, use it instead !
                    out_indices.push_back( index );
                }else{ // If not, it needs to be added in the output data.
                    out_bones.push_back( in_bones[in_vertices[i].boneID] );
                    out_uvs     .push_back( in_uvs[i]);
                    out_normals .push_back({ in_normals[i], in_vertices[i].boneID });
                    unsigned short newindex = (unsigned short)out_bones.size() - 1;
                    out_indices .push_back( newindex );
                    BoneToOutIndex[ packed ] = newindex;
                }
            }
        }

        Bone* constructNewSkeleton(Bone* oldSkeleton, int boneCount, glm::vec3* oldBones, glm::vec3* oldNormals, glm::vec3* newBones, glm::vec3* newNormals, int elementCount) {
            Bone* newSkeleton = new Bone[boneCount];
            for(int i = 0; i < boneCount; i++) {
                //newSkeleton[i] = Bone(oldSkeleton[i], oldSkeleton[i].getName()+"_2");
                newSkeleton[i] = oldSkeleton[i];

                // Scan and find this bones' parent
                for(int j = 0; j < boneCount; j++) {
                    if(&oldSkeleton[j] == oldSkeleton[i].getParent()) {
                        newSkeleton[i].setParent(newSkeleton[j]);
                        break;
                    }
                }

                // Scan and find this bones' children
                vector<Bone*> children = oldSkeleton[i].getChildren();
                for(int j = 0; j < children.size(); j++) {
                    for(int k = 0; k < boneCount; k++) {
                        if(&oldSkeleton[k] == children[j]) {
                            newSkeleton[i].addChild(newSkeleton[k]);
                            break;
                        }
                    }
                }

                // Scan and find this bones' positions
                vector<glm::vec3*> positions = oldSkeleton[i].getBonePositions();
                for(int j = 0; j < positions.size(); j++) {
                    for(int k = 0; k < elementCount; k++) {
                        if(&oldBones[k] == positions[j]) {
                            newSkeleton[i].addBonePosition(newBones[k]);
                            break;
                        }
                    }
                }

                // Scan and find this bones' normals
                vector<glm::vec3*> normals = oldSkeleton[i].getNormals();
                for(int j = 0; j < normals.size(); j++) {
                    for(int k = 0; k < elementCount; k++) {
                        if(&oldNormals[k] == normals[j]) {
                            newSkeleton[i].addNormal(newNormals[k]);
                            break;
                        }
                    }
                }
            }

            return newSkeleton;
        }

        Model::Model()
        {
            loaded = false;
            minDim = glm::vec3(0.0f, 0.0f, 0.0f);
            maxDim = glm::vec3(0.0f, 0.0f, 0.0f);
            name = "Unnamned";
        }

        Model::Model(string name) {
        loaded = false;
        minDim = glm::vec3(0.0f, 0.0f, 0.0f);
        maxDim = glm::vec3(0.0f, 0.0f, 0.0f);
        this->name = name;
    }

    Model::Model(Model &other) {
    operator=(other);
}

Model::~Model()
{
    delete [] skeleton;
    delete [] bonePositions;
    delete [] uvs;
    delete [] normals;
    delete [] indexArray;
}

void Model::operator=(Model &rhs) {
    delete [] skeleton;
    delete [] bonePositions;
    delete [] uvs;
    delete [] normals;
    delete [] indexArray;

    name = rhs.name;
    loaded = rhs.loaded;

    if(loaded) {
        maxDim = rhs.maxDim;
        minDim = rhs.minDim;
        texture = rhs.texture;
        boneCount = rhs.boneCount;

        glm::vec3* bonePositions = new glm::vec3[rhs.elementCount];
        glm::vec2* uvs = new glm::vec2[rhs.elementCount];
        glm::vec3* normals = new glm::vec3[rhs.elementCount];
        for(int i = 0; i < rhs.elementCount; i++) {
            bonePositions[i] = glm::vec3(rhs.bonePositions[i]);
            uvs[i] = glm::vec2(rhs.uvs[i]);
            normals[i] = glm::vec3(rhs.normals[i]);
        }
        unsigned short* indexArray = new unsigned short[rhs.indexCount];
        for(int i = 0; i < rhs.indexCount; i++) indexArray[i] = rhs.indexArray[i];
        Bone* skeleton = constructNewSkeleton(rhs.skeleton, rhs.boneCount, rhs.bonePositions, rhs.normals, bonePositions, normals, rhs.elementCount);

        loadData(skeleton, bonePositions, uvs, normals, rhs.normalsStatic, rhs.elementCount, indexArray, rhs.indexCount);
    }
}

void Model::addTexture(const char *textureName) { texture.addTexture(textureName); }
void Model::addTexture(GLuint textureID) { texture.addTexture(textureID); }
void Model::setTexture(Texture::AnimatedTexture other) { texture = other; }

bool Model::loadMMD(const char *path)
{
    if (loaded) return false;

    Log::log_render(INFO) << "Loading MMD file: " << path;

    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;
    vector<int> temp_boneIDs;

    vector<PreBoneData> readBones;
    vector<string> readBoneNames;

    //vector<int> usedBoneIDs;


    FILE * file = fopen(path, "r");
    if( file == NULL ){
        Log::log_render(ERR) << "Failed to open file: " << path;
        getchar();
        return false;
    }

    int lineNum = -1;

    while( 1 ){

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        lineNum++;

        // else : parse lineHeader

        // Vertex : [x y z]
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            int matches = fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            if(matches != 3) {
                Log::log_render(ERR) << "[" << lineNum << "] Incorrect Argument Count";
                return false;
            }
            temp_vertices.push_back(vertex);
            temp_boneIDs.push_back(-1);
            if( vertex.x > maxDim.x ) maxDim.x = vertex.x;
            if( vertex.y > maxDim.y ) maxDim.y = vertex.y;
            if( vertex.z > maxDim.z ) maxDim.z = vertex.z;
            if( vertex.x < minDim.x ) minDim.x = vertex.x;
            if( vertex.y < minDim.y ) minDim.y = vertex.y;
            if( vertex.z < minDim.z ) minDim.z = vertex.z;
        }
            // Vertex with Bone ID : [x y z ID]
        else if (strcmp( lineHeader, "vb" ) == 0 ){
            glm::vec3 vertex;
            int BID;
            int matches = fscanf(file, "%f %f %f %i\n", &vertex.x, &vertex.y, &vertex.z, &BID);
            if(matches != 4) {
                Log::log_render(ERR) << "[" << lineNum << "] Incorrect Argument Count";
                return false;
            }
            temp_vertices.push_back(vertex);
            temp_boneIDs.push_back(BID);
            if( vertex.x > maxDim.x ) maxDim.x = vertex.x;
            if( vertex.y > maxDim.y ) maxDim.y = vertex.y;
            if( vertex.z > maxDim.z ) maxDim.z = vertex.z;
            if( vertex.x < minDim.x ) minDim.x = vertex.x;
            if( vertex.y < minDim.y ) minDim.y = vertex.y;
            if( vertex.z < minDim.z ) minDim.z = vertex.z;
        }
            // UV : [u v]
        else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            int matches = fscanf(file, "%f %f\n", &uv.x, &uv.y );
            if(matches != 2) {
                Log::log_render(ERR) << "[" << lineNum << "] Incorrect Argument Count";
                return false;
            }
            temp_uvs.push_back(uv);
        }
            // Normal : [x y z]
        else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            int matches = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            if(matches != 3) {
                Log::log_render(ERR) << "[" << lineNum << "] Incorrect Argument Count";
                return false;
            }
            temp_normals.push_back(normal);
        }
            // Bone : [x y z parent]
        else if ( strcmp( lineHeader, "j" ) == 0 ){
            glm::vec3 bonePos;
            int bIndex;
            //int matches = fscanf(file, "%f %f %f %i %32s\n", &bonePos.x, &bonePos.y, &bonePos.z, &bIndex, boneName );
            int matches = fscanf(file, "%f %f %f %i\n", &bonePos.x, &bonePos.y, &bonePos.z, &bIndex);
            if(matches != 4) {
                Log::log_render(ERR) << "[" << lineNum << "] Incorrect Argument Count";
                return false;
            }
            if(bIndex == 0)
            {
                readBones.push_back({ bonePos, -1 });
            }
            else if( (bIndex > 0) && (bIndex < readBones.size() + 1) )
            {
                readBones.push_back({ bonePos, bIndex-1 });
            }
            else
            {
                Log::log_render(ERR) << "[" << lineNum << "] Invalid Bone Index: " << bIndex;
                return false;
            }
            readBoneNames.push_back("Bone " + readBoneNames.size());
        }
            // Bone, Named : [x y z parent name]
        else if ( strcmp( lineHeader, "jn" ) == 0 ) {
            glm::vec3 bonePos;
            int bIndex;
            char boneName[55];
            int matches = fscanf(file, "%f %f %f %i %50s\n", &bonePos.x, &bonePos.y, &bonePos.z, &bIndex, boneName );
            if(matches != 5) {
                Log::log_render(ERR) << "[" << lineNum << "] Incorrect Argument Count";
                return false;
            }
            if(bIndex == 0)
            {
                readBones.push_back({ bonePos, -1 });
            }
            else if( (bIndex > 0) && (bIndex < readBones.size() + 1) )
            {
                readBones.push_back({ bonePos, bIndex-1 });
            }
            else
            {
                Log::log_render(ERR) << "[" << lineNum << "] Invalid Bone Index: " << bIndex;
                return false;
            }
            readBoneNames.push_back(string(boneName));
        }
            // Face : [v0 u0 n0 v1 u1 n1 v2 u2 n2]
        else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if(matches != 9) {
                Log::log_render(ERR) << "[" << lineNum << "] Incorrect Argument Count";
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }

    vector<PreVertexData> out_vertices;
    vector<glm::vec2> out_uvs;

    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        int BID = temp_boneIDs[ vertexIndex-1 ];
        glm::vec2 uv = temp_uvs[ uvIndex-1 ];

        // Put the attributes in buffers
        PreVertexData tVert; tVert.position = vertex;
        if(BID < 0)
            tVert.boneID = getClosestBoneID(vertex, readBones);
        else
            tVert.boneID = BID-1;
        out_vertices.push_back(tVert);
        out_uvs     .push_back(uv);
    }

    vector<unsigned short> indices;
    vector<PreBoneData> indexed_bones;
    vector<glm::vec2> indexed_uvs;
    vector<PreVertexData> indexed_normals;

    indexVBO(out_vertices, readBones, out_uvs, indices, indexed_bones, indexed_uvs, indexed_normals);

    // Compile Final Skeleton Array
    boneCount = readBones.size();
    Bone *skeleton = new Bone[boneCount];
    for(int i = 0; i < boneCount; i++) {
        if(readBones[i].parentID > -1) skeleton[i] = Bone(readBones[i].position - readBones[readBones[i].parentID].position, readBoneNames[i] );
        else skeleton[i] = Bone(readBones[i].position, readBoneNames[i] );
    }
    for(int i = 0; i < boneCount; i++) {
        if(readBones[i].parentID > -1) {
            skeleton[i].setParent(skeleton[readBones[i].parentID]);
            skeleton[readBones[i].parentID].addChild(skeleton[i]);
        }
    }

    // Compile Element Arrays
    unsigned int elementCount = indexed_bones.size();
    glm::vec3 *bones = new glm::vec3[elementCount];
    glm::vec2 *uvs = new glm::vec2[elementCount];
    glm::vec3 *normals = new glm::vec3[elementCount];
    glm::vec3 *normalsStatic = new glm::vec3[elementCount];
    for(int i = 0; i < elementCount; i++) {
        bones[i] = indexed_bones[i].position;
        uvs[i] = indexed_uvs[i];
        normals[i] = indexed_normals[i].position;
        normalsStatic[i] = indexed_normals[i].position;
        skeleton[indexed_normals[i].boneID].addBonePosition(bones[i]);
        skeleton[indexed_normals[i].boneID].addNormal(normals[i], normalsStatic[i]);
    }

    loadData(skeleton, bones, uvs, normals, normalsStatic, elementCount, indices.data(), indices.size());
}

bool Model::loadOBJ(const char *path)
{
    if (loaded) return false;

    Log::log_render(INFO) << "Loading OBJ file: " << path;

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    std::vector<PreVertexData> out_vertices;
    std::vector<glm::vec2> out_uvs;
    std::vector<glm::vec3> out_normals;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        Log::log_render(ERR) << "Failed to open file: " << path;
        getchar();
        return false;
    }

    while( 1 ){

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        // Vertex : [x y z]
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
            if( vertex.x > maxDim.x ) maxDim.x = vertex.x;
            if( vertex.y > maxDim.y ) maxDim.y = vertex.y;
            if( vertex.z > maxDim.z ) maxDim.z = vertex.z;
            if( vertex.x < minDim.x ) minDim.x = vertex.x;
            if( vertex.y < minDim.y ) minDim.y = vertex.y;
            if( vertex.z < minDim.z ) minDim.z = vertex.z;
            // UV : [u v]
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
            // Normal : [x y z]
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
            // Face : [v0 u0 n0 v1 u1 n1 v2 u2 n2]
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }

    // Create the one Bone that a loaded OBJ will use
    glm::vec3 tBPos;
    tBPos.x = (maxDim.x + minDim.x) / 2;
    tBPos.y = (maxDim.y + minDim.y) / 2;
    tBPos.z = (maxDim.z + minDim.z) / 2;
    Bone newBone(tBPos);

    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        glm::vec2 uv = temp_uvs[ uvIndex-1 ];
        glm::vec3 normal = temp_normals[ normalIndex-1 ];

        // Put the attributes in buffers
        PreVertexData tVert; tVert.position = vertex;
        tVert.boneID = 0;
        out_vertices.push_back(tVert);
        out_uvs     .push_back(uv);
        out_normals .push_back(normal);

    }

    PreBoneData bData = { newBone.getPosition(), -1 };
    vector<PreBoneData> out_bones; out_bones.push_back(bData);
    vector<unsigned short> indices;
    vector<PreBoneData> indexed_bones;
    vector<glm::vec2> indexed_uvs;
    vector<PreVertexData> indexed_normals;

    indexVBO(out_vertices, out_bones, out_uvs, indices, indexed_bones, indexed_uvs, indexed_normals);

    // Compile Final Skeleton Array
    boneCount = out_bones.size();
    Bone *skeleton = new Bone[boneCount];
    for(int i = 0; i < boneCount; i++) {
        skeleton[i] = Bone(out_bones[i].position);
    }
    for(int i = 0; i < boneCount; i++) {
        if(out_bones[i].parentID > -1) {
            skeleton[i].setParent(skeleton[out_bones[i].parentID]);
            skeleton[out_bones[i].parentID].addChild(skeleton[i]);
        }
    }

    // Compile Element Arrays
    unsigned int elementCount = indexed_bones.size();
    glm::vec3 *bones = new glm::vec3[elementCount];
    glm::vec2 *uvs = new glm::vec2[elementCount];
    glm::vec3 *normals = new glm::vec3[elementCount];
    glm::vec3 *normalsStatic = new glm::vec3[elementCount];
    for(int i = 0; i < elementCount; i++) {
        bones[i] = indexed_bones[i].position;
        uvs[i] = indexed_uvs[i];
        normals[i] = indexed_normals[i].position;
        normalsStatic[i] = indexed_normals[i].position;
        skeleton[indexed_normals[i].boneID].addBonePosition(bones[i]);
        skeleton[indexed_normals[i].boneID].addNormal(normals[i], normalsStatic[i]);
    }

    loadData(skeleton, bones, uvs, normals, normalsStatic, elementCount, indices.data(), indices.size());
}

bool Model::loadData(Bone skeleton[], glm::vec3 bones[], glm::vec2 uvs[], glm::vec3 normals[], glm::vec3 normalsStatic[], unsigned int elementCount, unsigned short indices[], unsigned int indexCount) {
    this->elementCount = elementCount;
    this->skeleton = skeleton;
    this->bonePositions = bones;
    this->uvs = uvs;
    this->normals = normals;
    this->normalsStatic = normalsStatic;
    registerStaticNormalArray(normalsStatic);
    this->indexCount = indexCount;
    this->indexArray = indices;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &bonebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
    glBufferData(GL_ARRAY_BUFFER, this->elementCount * sizeof(glm::vec3), &this->bonePositions[0], GL_DYNAMIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, this->elementCount * sizeof(glm::vec2), &this->uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, this->elementCount * sizeof(glm::vec3), &this->normals[0], GL_DYNAMIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexCount * sizeof(unsigned short), &this->indexArray[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(SHADER_ATTRIB_ID_VERTEX);
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
    glVertexAttribPointer(
            SHADER_ATTRIB_ID_VERTEX,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void *) 0            // array buffer offset
    );

    glEnableVertexAttribArray(SHADER_ATTRIB_ID_UV);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            SHADER_ATTRIB_ID_UV,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void *) 0                          // array buffer offset
    );

    glEnableVertexAttribArray(SHADER_ATTRIB_ID_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            SHADER_ATTRIB_ID_NORMAL,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void *) 0                          // array buffer offset
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    registeredBuffers.push_back(bonebuffer);
    registeredBuffers.push_back(uvbuffer);
    registeredBuffers.push_back(normalbuffer);
    registeredBuffers.push_back(elementbuffer);

    registeredVAOs.push_back(vao);

    glBindVertexArray(0);

    Log::log_render(INFO) << "Model Loaded: '" << name << "' [B-" << bonebuffer << "|U-" << uvbuffer << "|N-" << normalbuffer << "|E-" << elementbuffer << "]";
    loaded = true;
    return true;
}

void Model::updateBoneBuffer()
{
    for(int i = 0; i < boneCount; i++) {
        skeleton[i].updateBoneBuffer();
    }
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, elementCount * sizeof(glm::vec3), &bonePositions[0]);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, elementCount * sizeof(glm::vec3), &normals[0]);
}

Bone* Model::getBone(int index) {
    if(index > -1 && index < boneCount) {
        return &skeleton[index];
    } return NULL;
}

Bone* Model::getBone(string tag) {
    // TODO: Make Case-Insensitive
    for(int i = 0; i < boneCount; i++) {
        if(skeleton[i].getName().find(tag) != string::npos) return &skeleton[i];
    } return NULL;
}

//GLuint Model::getBoneBuffer()	{ return bonebuffer; }
//GLuint Model::getUVBuffer()		{ return uvbuffer; }
//GLuint Model::getNormalBuffer() { return normalbuffer; }
//GLuint Model::getElementBuffer(){ return elementbuffer; }
GLuint Model::getVaoID()       { return vao; }
int Model::getElementCount()	{ return indexCount; }
GLuint Model::getTexture()		{ return texture.getTexture(); }

bool Model::isLoaded()			{ return loaded; }

glm::vec3 Model::getMaxDimensions()	{ return maxDim; }
glm::vec3 Model::getMinDimensions()	{ return minDim; }

glm::vec3 Model::getBonePosition(int index)
{
    if(index > -1 && index < boneCount)
    {
        return skeleton[index].getPosition();
    }
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

void Model::addBonePosition(int index, glm::vec3 pos) {
    if(index > -1 && index < boneCount)
    {
        skeleton[index].addPosition(pos);
    }
}

void Model::setBonePosition(int index, glm::vec3 pos)
{
    if(index > -1 && index < boneCount)
    {
        skeleton[index].setPosition(pos);
    }
}

void Model::rotateBonePosition(int index, float angle, glm::vec3 amount) {
    if(index > -1 && index < boneCount) {
        skeleton[index].rotatePosition(angle, amount);
    }
}
    }
}