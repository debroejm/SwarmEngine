#include "../headers/ModelLoading.h"

namespace ENGINE_NAMESPACE {
    namespace Models {

        vector<GLuint> registeredBuffers;

        void cleanupBuffers() {
            for(int i = 0; i < registeredBuffers.size(); i++) {
                char infoMsg[256];
                sprintf(infoMsg, "Deleting Buffer [%i]", registeredBuffers[i]);
                Logging::Log(LOGGING_INFO, "Cleanup", infoMsg);
                glDeleteBuffers(1, &registeredBuffers[i]);
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
            vec3 position;
            int parentID;
        };

        struct PreVertexData {
            vec3 position;
            int boneID;
        };

        int getClosestBoneID(vec3 in_vertex, vector<PreBoneData> &bones)
        {
            int closestID = -1;
            double closeDist = 0.0;
            for(int i = 0; i < bones.size(); i++)
            {
                vec3 bPos = bones[i].position;
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
                vector<vec2> &in_uvs,

                vector<unsigned short> &out_indices,
                vector<PreBoneData> &out_bones,
                vector<vec2> &out_uvs,
                vector<vec3> &out_normals
        ){
            std::map<PackedVertex,unsigned short> BoneToOutIndex;

            std::vector<glm::vec3> in_normals;

            for( unsigned int i=0; i<in_vertices.size(); i++ )
            {
                glm::vec3 bonePos = in_bones[in_vertices[i].boneID].position;
                in_normals.push_back( vec3(in_vertices[i].position.x - bonePos.x,
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
                    out_normals .push_back( in_normals[i]);
                    unsigned short newindex = (unsigned short)out_bones.size() - 1;
                    out_indices .push_back( newindex );
                    BoneToOutIndex[ packed ] = newindex;
                }
            }
        }

        Bone::Bone() {
            parent = NULL;
        }

        Bone::Bone(vec3 pos)
        {
            originalPos = pos;
            position = pos;
            parent = NULL;
        }

        Bone::Bone(vec3 pos, Bone &parent)
        {
            originalPos = pos;
            position = pos;
            this->parent = &parent;
        }

        Bone::~Bone()
        {

        }

        void Bone::revertPosition()
        {
            position = originalPos;
        }

        void Bone::addPosition(vec3 newPos) { setPosition(position+newPos); }
        void Bone::setPosition(vec3 newPos)
        {
            position = newPos;
            for(int i = 0; i < bonePositions.size(); i++)
            {
                bonePositions[i]->operator=(position);
            }
            for(int i = 0; i < children.size(); i++)
            {
                children[i]->setPosition(newPos);
            }
        }

        void Bone::addChild(Bone &child) {
            children.push_back(&child);
        }

        void Bone::addBonePosition(vec3 &bonePos) {
            bonePositions.push_back(&bonePos);
        }

        void Bone::addNormal(vec3 &normal) {
            normals.push_back(&normal);
        }

        glm::vec3 Bone::getPosition()
        {
            return position;
        }

        void Bone::operator =(const Bone &rhs)
        {
            position = rhs.position;
            originalPos = position;
            parent = rhs.parent;
            children = rhs.children;
            bonePositions = rhs.bonePositions;
            normals = rhs.normals;
        }

        Model::Model()
        {
            loaded = false;
            minDim = vec3(0.0f, 0.0f, 0.0f);
            maxDim = vec3(0.0f, 0.0f, 0.0f);
            name = "Unnamned";
        }

        Model::Model(string name) {
            loaded = false;
            minDim = vec3(0.0f, 0.0f, 0.0f);
            maxDim = vec3(0.0f, 0.0f, 0.0f);
            this->name = name;
        }

        Model::~Model()
        {
            delete [] skeleton;
            delete [] bonePositions;
            delete [] normals;
        }

        void Model::addTexture(const char *textureName) { texture.addTexture(textureName); }
        void Model::addTexture(GLuint textureID) { texture.addTexture(textureID); }
        void Model::setTexture(Textures::AnimatedTexture other) { texture = other; }

        bool Model::loadMMD(const char *path)
        {
            if (loaded) return false;

            char fileLoadBuff[1000];
            sprintf(fileLoadBuff, "Loading MMD file: %s", path);
            Logging::Log(LOGGING_INFO, "Models", fileLoadBuff);

            vector<unsigned int> vertexIndices, uvIndices, normalIndices;
            vector<glm::vec3> temp_vertices;
            vector<glm::vec2> temp_uvs;
            vector<glm::vec3> temp_normals;
            vector<int> temp_boneIDs;

            vector<PreBoneData> readBones;

            //vector<int> usedBoneIDs;


            FILE * file = fopen(path, "r");
            if( file == NULL ){
                Logging::Log(LOGGING_ERROR, "Models", "Failed to open the file");
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
                    fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
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
                    fscanf(file, "%f %f %f %i\n", &vertex.x, &vertex.y, &vertex.z, &BID);
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
                    fscanf(file, "%f %f\n", &uv.x, &uv.y );
                    temp_uvs.push_back(uv);
                }
                // Normal : [x y z]
                else if ( strcmp( lineHeader, "vn" ) == 0 ){
                    glm::vec3 normal;
                    fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
                    temp_normals.push_back(normal);
                }
                // Bone : [x y z parent]
                else if ( strcmp( lineHeader, "j" ) == 0 ){
                    vec3 bonePos;
                    int bIndex;
                    fscanf(file, "%f %f %f %i\n", &bonePos.x, &bonePos.y, &bonePos.z, &bIndex );
                    if(bIndex == 0)
                    {
                        readBones.push_back({ bonePos, -1 });
                    }
                    else if( (bIndex > 0) && (bIndex < readBones.size() + 1) )
                    {
                        readBones.push_back({ bonePos, bIndex-1});
                    }
                    else
                    {
                        char logString[256];
                        sprintf(logString, "[%i] Invalid Bone index: %i", lineNum, bIndex);
                        Logging::Log(LOGGING_ERROR, "MMDLoading", logString);
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
                    tVert.boneID = BID;
                out_vertices.push_back(tVert);
                out_uvs     .push_back(uv);
            }

            vector<unsigned short> indices;
            vector<PreBoneData> indexed_bones;
            vector<glm::vec2> indexed_uvs;
            vector<glm::vec3> indexed_normals;

            indexVBO(out_vertices, readBones, out_uvs, indices, indexed_bones, indexed_uvs, indexed_normals);

            // Compile Final Skeleton Array
            boneCount = readBones.size();
            skeleton = new Bone[boneCount];
            for(int i = 0; i < boneCount; i++) {
                skeleton[i] = Bone(readBones[i].position);
            }
            for(int i = 0; i < boneCount; i++) {
                if(readBones[i].parentID > -1) skeleton[i].setParent(skeleton[readBones[i].parentID]);
            }

            // Compile Element Arrays
            elementCount = indexed_bones.size();
            bonePositions = new vec3[elementCount];
            normals = new vec3[elementCount];
            for(int i = 0; i < elementCount; i++) {
                bonePositions[i] = indexed_bones[i].position;
                normals[i] = indexed_normals[i];
            }

            indexCount = indices.size();

            glGenBuffers(1, &bonebuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
            glBufferData(GL_ARRAY_BUFFER, elementCount * sizeof(glm::vec3), &bonePositions[0], GL_DYNAMIC_DRAW);

            glGenBuffers(1, &uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, elementCount * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

            glGenBuffers(1, &normalbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glBufferData(GL_ARRAY_BUFFER, elementCount * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);

            glGenBuffers(1, &elementbuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

            registeredBuffers.push_back(bonebuffer);
            registeredBuffers.push_back(uvbuffer);
            registeredBuffers.push_back(normalbuffer);
            registeredBuffers.push_back(elementbuffer);

            char successMsg[256];
            sprintf(successMsg, "Model Loaded: '%s' [B-%i|U-%i|N-%i|E-%i]", name.c_str(), bonebuffer, uvbuffer, normalbuffer, elementbuffer);
            Logging::Log(LOGGING_INFO, "Models", successMsg);

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

            std::vector<PreVertexData> out_vertices;
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
            vector<glm::vec3> indexed_normals;

            indexVBO(out_vertices, out_bones, out_uvs, indices, indexed_bones, indexed_uvs, indexed_normals);

            // Compile Final Skeleton Array
            boneCount = out_bones.size();
            skeleton = new Bone[boneCount];
            for(int i = 0; i < boneCount; i++) {
                skeleton[i] = Bone(out_bones[i].position);
            }
            for(int i = 0; i < boneCount; i++) {
                if(out_bones[i].parentID > -1) skeleton[i].setParent(skeleton[out_bones[i].parentID]);
            }

            // Compile Element Arrays
            elementCount = indexed_bones.size();
            bonePositions = new vec3[elementCount];
            normals = new vec3[elementCount];
            for(int i = 0; i < elementCount; i++) {
                bonePositions[i] = indexed_bones[i].position;
                normals[i] = indexed_normals[i];
            }

            indexCount = indices.size();


            glGenBuffers(1, &bonebuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
            glBufferData(GL_ARRAY_BUFFER, elementCount * sizeof(glm::vec3), &bonePositions[0], GL_DYNAMIC_DRAW);

            glGenBuffers(1, &uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, elementCount * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

            glGenBuffers(1, &normalbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glBufferData(GL_ARRAY_BUFFER, elementCount * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);

            glGenBuffers(1, &elementbuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

            registeredBuffers.push_back(bonebuffer);
            registeredBuffers.push_back(uvbuffer);
            registeredBuffers.push_back(normalbuffer);
            registeredBuffers.push_back(elementbuffer);

            char successMsg[256];
            sprintf(successMsg, "Model Loaded: '%s' [B-%i|U-%i|N-%i|E-%i]", name.c_str(), bonebuffer, uvbuffer, normalbuffer, elementbuffer);
            Logging::Log(LOGGING_INFO, "Models", successMsg);

            loaded = true;
            return true;
        }

        void Model::updateBoneBuffer()
        {
            glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, elementCount * sizeof(glm::vec3), &bonePositions[0]);
        }

        GLuint Model::getBoneBuffer()	{ return bonebuffer; }
        GLuint Model::getUVBuffer()		{ return uvbuffer; }
        GLuint Model::getNormalBuffer() { return normalbuffer; }
        GLuint Model::getElementBuffer(){ return elementbuffer; }
        int Model::getElementCount()	{ return indexCount; }
        GLuint Model::getTexture()		{ return texture.getTexture(); }

        bool Model::isLoaded()			{ return loaded; }

        vec3 Model::getMaxDimensions()	{ return maxDim; }
        vec3 Model::getMinDimensions()	{ return minDim; }

        vec3 Model::getBonePosition(int index)
        {
            if(index > -1 && index < boneCount)
            {
                return skeleton[index].getPosition();
            }
            return vec3(0.0f, 0.0f, 0.0f);
        }

        void Model::addBonePosition(int index, vec3 pos) {
            if(index > -1 && index < boneCount)
            {
                skeleton[index].addPosition(pos);
            }
        }

        void Model::setBonePosition(int index, vec3 pos)
        {
            if(index > -1 && index < boneCount)
            {
                skeleton[index].setPosition(pos);
            }
        }

        string Model::getStringData() {
            for(int i = 0; i < elementCount; i++) {
                char infoMsg[256];
                sprintf(infoMsg, "(%f, %f, %f)", normals[i].x, normals[i].y, normals[i].z);
                Logging::Log(LOGGING_INFO, "Models", infoMsg);
            }
            return "";
        }

    }
}