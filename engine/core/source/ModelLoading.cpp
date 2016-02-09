#include "../headers/ModelLoading.h"

namespace ENGINE_NAMESPACE {
    namespace Models {

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

        struct BoneLinkedVertex{
            vec3 vertex;
            Models::Bone *linkedBone;
        };

        Models::Bone *getClosestBone(std::vector<Models::Bone> & in_bones, glm::vec3 in_vertex)
        {
            int closestID = -1;
            double closeDist;
            for(int i = 0; i < in_bones.size(); i++)
            {
                vec3 bPos = in_bones[i].getPosition();
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

            return &in_bones[closestID];
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
                std::vector<BoneLinkedVertex> & in_vertices,
                std::vector<glm::vec2> & in_uvs,

                std::vector<unsigned short> & out_indices,
                std::vector<glm::vec2> & out_uvs,
                std::vector<glm::vec3> & out_normals,
                std::vector<glm::vec3> & out_bones
        ){
            std::map<PackedBone,unsigned short> BoneToOutIndex;

            std::vector<glm::vec3> in_normals;

            for( unsigned int i=0; i<in_vertices.size(); i++ )
            {
                glm::vec3 bonePos = (*in_vertices[i].linkedBone).getPosition();
                in_normals.push_back( vec3(in_vertices[i].vertex.x - bonePos.x,
                                           in_vertices[i].vertex.y - bonePos.y,
                                           in_vertices[i].vertex.z - bonePos.z) );
            }

            // For each input vertex
            for ( unsigned int i=0; i<in_vertices.size(); i++ ){

                PackedBone packed = {in_vertices[i].vertex, in_uvs[i], in_normals[i]};


                // Try to find a similar vertex in out_XXXX
                unsigned short index;
                bool found = getSimilarBoneIndex_fast( packed, BoneToOutIndex, index);

                if ( found ){ // A similar vertex is already in the VBO, use it instead !
                    out_indices.push_back( index );
                }else{ // If not, it needs to be added in the output data.
                    out_bones.push_back( (*in_vertices[i].linkedBone).getPosition() );
                    out_uvs     .push_back( in_uvs[i]);
                    out_normals .push_back( in_normals[i]);
                    (*in_vertices[i].linkedBone).addLinkedBonePos(out_bones.size()-1);
                    (*in_vertices[i].linkedBone).addLinkedNormalPos(out_normals.size()-1);
                    unsigned short newindex = (unsigned short)out_bones.size() - 1;
                    out_indices .push_back( newindex );
                    BoneToOutIndex[ packed ] = newindex;
                }
            }
        }

        Models::Bone::Bone(vec3 pos)
        {
            originalPos = pos;
            position = pos;
            parentID = -1;
        }

        Models::Bone::Bone(vec3 pos, int PID)
        {
            parentID = PID;
            originalPos = pos;
            position = pos;
        }

        Models::Bone::~Bone()
        {

        }

        bool Models::Bone::addChild(int childID)
        {
            children.push_back(childID);
            return true;
        }

        void Models::Bone::revertPosition()
        {
            position = originalPos;
        }

        void Models::Bone::changePosition(vec3 newPos)
        {
            position = position+newPos;
            for(int i = 0; i < boneIndices.size(); i++)
            {
                (*modelBones)[boneIndices[i]] = position;
            }
            for(int i = 0; i < children.size(); i++)
            {
                (*modelSkeleton)[children[i]].changePosition(newPos);
            }
        }

        bool Models::Bone::addLinkedBonePos(int BonePosIndex)
        {
            if(BonePosIndex < 0)
                return false;
            boneIndices.push_back(BonePosIndex);
            return true;
        }

        bool Models::Bone::addLinkedNormalPos(int NormalPosIndex)
        {
            if(NormalPosIndex < 0)
                return false;
            normalIndices.push_back(NormalPosIndex);
            return true;
        }

        void Models::Bone::setModelBonesPointer(vector<vec3> * mBones) { modelBones = mBones; }
        void Models::Bone::setModelNormalsPointer(vector<vec3> * mNormals) { modelNormals = mNormals; }
        void Models::Bone::setSkeletonPointer(vector<Bone> * mSkeleton) { modelSkeleton = mSkeleton; }

        glm::vec3 Models::Bone::getPosition()
        {
            return position;
        }

        Models::Model::Model()
        {
            loaded = false;
            minDim = vec3(0.0f, 0.0f, 0.0f);
            maxDim = vec3(0.0f, 0.0f, 0.0f);
        }

        Models::Model::~Model()
        {
            if(loaded)
            {
                glDeleteBuffers(1, &bonebuffer);
                glDeleteBuffers(1, &uvbuffer);
                glDeleteBuffers(1, &normalbuffer);
                glDeleteBuffers(1, &elementbuffer);
            }
        }

        void Models::Model::addTexture(const char *textureName) { texture.addTexture(textureName); }
        void Models::Model::addTexture(GLuint textureID) { texture.addTexture(textureID); }
        void Models::Model::setTexture(Textures::AnimatedTexture other) { texture = other; }

        bool intInVector(int item, vector<int> & list)
        {
            for(int i = 0; i < list.size(); i++)
            {
                if( list[i] == item )
                    return true;
            }
            return false;
        }

        bool Models::Model::loadMMD(const char *path)
        {
            if (loaded) return false;

            char fileLoadBuff[1000];
            sprintf(fileLoadBuff, "Loading MMD file: %s", path);
            Logging::Log(LOGGING_INFO, "Models", fileLoadBuff);

            std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
            std::vector<glm::vec3> temp_vertices;
            std::vector<glm::vec2> temp_uvs;
            std::vector<glm::vec3> temp_normals;
            std::vector<int> temp_boneIDs;

            std::vector<BoneLinkedVertex> out_vertices;
            std::vector<glm::vec2> out_uvs;
            std::vector<glm::vec3> out_normals;

            std::vector<Bone> out_bones;

            std::vector<int> usedBoneIDs;


            FILE * file = fopen(path, "r");
            if( file == NULL ){
                Logging::Log(LOGGING_ERROR, "Models", "Failed to open the file");
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
                    temp_boneIDs.push_back(-1);
                    if( vertex.x > maxDim.x ) maxDim.x = vertex.x;
                    if( vertex.y > maxDim.y ) maxDim.y = vertex.y;
                    if( vertex.z > maxDim.z ) maxDim.z = vertex.z;
                    if( vertex.x < minDim.x ) minDim.x = vertex.x;
                    if( vertex.y < minDim.y ) minDim.y = vertex.y;
                    if( vertex.z < minDim.z ) minDim.z = vertex.z;
                    // Vertex with Bone ID : [x y z ID]
                }else if (strcmp( lineHeader, "vb" ) == 0 ){
                    glm::vec3 vertex;
                    int BID;
                    fscanf(file, "%f %f %f %i\n", &vertex.x, &vertex.y, &vertex.z, &BID);
                    temp_vertices.push_back(vertex);
                    temp_boneIDs.push_back(BID);
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
                    // Bone : [x y z parent]
                }else if ( strcmp( lineHeader, "j" ) == 0 ){
                    vec3 bonePos;
                    int bIndex;
                    fscanf(file, "%f %f %f %i\n", &bonePos.x, &bonePos.y, &bonePos.z, &bIndex );
                    if(bIndex == 0)
                    {
                        Bone tBone(bonePos);
                        skeleton.push_back(tBone);
                    }
                    else if( (bIndex > 0) && (bIndex < skeleton.size()+1) )
                    {
                        Bone tBone(bonePos, bIndex-1);
                        skeleton.push_back(tBone);
                        skeleton[bIndex-1].addChild(skeleton.size()-1);
                    }
                    else
                    {
                        char logString[256];
                        sprintf(logString, "Error in reading bones; invalid index: %i", bIndex);
                        Logging::Log(LOGGING_WARNING, "ModelLoading", logString);
                        return false;
                    }
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

            // For each vertex of each triangle
            for( unsigned int i=0; i<vertexIndices.size(); i++ ){

                // Get the indices of its attributes
                unsigned int vertexIndex = vertexIndices[i];
                unsigned int uvIndex = uvIndices[i];
                unsigned int normalIndex = normalIndices[i];

                // Get the attributes thanks to the index
                glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
                int BID = temp_boneIDs[ vertexIndex-1 ];
                glm::vec2 uv = temp_uvs[ uvIndex-1 ];
                glm::vec3 normal = temp_normals[ normalIndex-1 ];

                // Put the attributes in buffers
                BoneLinkedVertex tVert; tVert.vertex = vertex;
                if(BID < 0)
                    tVert.linkedBone = getClosestBone(skeleton, vertex);
                else
                    tVert.linkedBone = &(skeleton[BID]);
                out_vertices.push_back(tVert);
                out_uvs     .push_back(uv);
                out_normals .push_back(normal);

            }

            std::vector<unsigned short> indices;
            std::vector<glm::vec3> indexed_bones;
            std::vector<glm::vec2> indexed_uvs;
            std::vector<glm::vec3> indexed_normals;

            indexVBO(out_vertices, out_uvs, indices, indexed_uvs, normals, bones);

            for(int i = 0; i < skeleton.size(); i++)
            {
                skeleton[i].setModelBonesPointer(&bones);
                skeleton[i].setModelNormalsPointer(&normals);
                skeleton[i].setSkeletonPointer(&skeleton);
            }

            glGenBuffers(1, &bonebuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
            glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(glm::vec3), &bones[0], GL_DYNAMIC_DRAW);

            glGenBuffers(1, &uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

            glGenBuffers(1, &normalbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);

            glGenBuffers(1, &elementbuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
            indexCount = indices.size();

            loaded = true;
            return true;
        }

        bool Models::Model::loadOBJ(const char *path)
        {
            if (loaded) return false;

            char fileLoadBuff[1000];
            sprintf(fileLoadBuff, "Loading OBJ file: %s", path);
            Logging::Log(LOGGING_INFO, "Models", fileLoadBuff);

            std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
            std::vector<glm::vec3> temp_vertices;
            std::vector<glm::vec2> temp_uvs;
            std::vector<glm::vec3> temp_normals;

            std::vector<BoneLinkedVertex> out_vertices;
            std::vector<glm::vec2> out_uvs;
            std::vector<glm::vec3> out_normals;

            FILE * file = fopen(path, "r");
            if( file == NULL ){
                Logging::Log(LOGGING_ERROR, "Models", "Failed to open the file");
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
            vec3 tBPos;
            tBPos.x = (maxDim.x + minDim.x) / 2;
            tBPos.y = (maxDim.y + minDim.y) / 2;
            tBPos.z = (maxDim.z + minDim.z) / 2;
            Bone newBone(tBPos);
            skeleton.clear();
            skeleton.push_back(newBone);

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
                BoneLinkedVertex tVert; tVert.vertex = vertex;
                tVert.linkedBone = &skeleton[0];
                out_vertices.push_back(tVert);
                out_uvs     .push_back(uv);
                out_normals .push_back(normal);

            }

            std::vector<unsigned short> indices;
            std::vector<glm::vec3> indexed_bones;
            std::vector<glm::vec2> indexed_uvs;
            std::vector<glm::vec3> indexed_normals;

            indexVBO(out_vertices, out_uvs, indices, indexed_uvs, normals, bones);

            for(int i = 0; i < skeleton.size(); i++)
            {
                skeleton[i].setModelBonesPointer(&bones);
                skeleton[i].setModelNormalsPointer(&normals);
                skeleton[i].setSkeletonPointer(&skeleton);
            }

            glGenBuffers(1, &bonebuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
            glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(glm::vec3), &bones[0], GL_DYNAMIC_DRAW);

            glGenBuffers(1, &uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

            glGenBuffers(1, &normalbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);

            glGenBuffers(1, &elementbuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
            indexCount = indices.size();

            loaded = true;
            return true;
        }

        void Models::Model::updateBoneBuffer()
        {
            glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, bones.size() * sizeof(glm::vec3), &bones[0]);
        }

        GLuint Models::Model::getBoneBuffer()	{ return bonebuffer; }
        GLuint Models::Model::getUVBuffer()		{ return uvbuffer; }
        GLuint Models::Model::getNormalBuffer() { return normalbuffer; }
        GLuint Models::Model::getElementBuffer(){ return elementbuffer; }
        int Models::Model::getElementCount()	{ return indexCount; }
        GLuint Models::Model::getTexture()		{ return texture.getTexture(); }

        bool Models::Model::isLoaded()			{ return loaded; }

        vec3 Models::Model::getMaxDimensions()	{ return maxDim; }
        vec3 Models::Model::getMinDimensions()	{ return minDim; }

        vec3 Models::Model::getBonePosition(int index)
        {
            if(index > -1 && index < skeleton.size())
            {
                return skeleton[index].getPosition();
            }
            return vec3(0.0f, 0.0f, 0.0f);
        }

        void Models::Model::changeBonePosition(int index, vec3 pos)
        {
            if(index > -1 && index < skeleton.size())
            {
                skeleton[index].changePosition(pos);
            }
        }

    }
}