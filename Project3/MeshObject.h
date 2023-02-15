#pragma once

#include <string>
#include <limits.h>
#include <fstream>
#include <map>
#include <algorithm>
#include <set>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <Common.h>
#include <ResourcePath.h>

#define BODYPARTSAMOUNT 33
#define HEAD 0
#define LEFTEAR 1
#define RIGHTEAR 2
#define LEFTEYE 3
#define RIGHTEYE 4
#define UPPERJAW 5
#define LOWERJAW 6
#define TEETH 7


#define UPPERBACK 8
#define MIDDLEBACK 9
#define LOWERBACK 10
#define TAILTOLOWERBUTT 11

#define LEFTTHIGHCAP 12
#define RIGHTTHIGHCAP 13

#define LEFTKNEECAP 14
#define RIGHTKNEECAP 15
#define LEFTKNEEAROUND 16
#define RIGHTKNEEAROUND 17

#define LEFTSHIN 18
#define RIGHTSHIN 19
#define LEFTMIDDLESHIN 20
#define RIGHTMIDDLESHIN 21
#define LEFTTOE 22
#define RIGHTTOE 23


#define RIGHTCHEST 24
#define LEFTCHEST 25
#define ABS 26

#define LEFTUPPERARMDOWN 27
#define LEFTLOWERARMTOP 28
#define LEFTLOWERARMDOWN 29

#define RIGHTUPPERARMDOWN 30
#define RIGHTLOWERARMTOP 31
#define RIGHTLOWERARMDOWN 32


enum WeightMode{
	UNIFORM,
	HARMONIC, 
	MEAN
};

enum BorderMode {
	QUAD,
	DISK
};

//enum WRAPMODE {
//	REPEAT,
//	MIRRORED_REPEAT,
//	CLAMP_EDGE,
//	CLAMP_BORDER
//};
//enum MIN_MAG_MODE {
//	NEAREST,
//	LINEAR
//};

struct UpdateData {
	//GUI need to have update data intact
	bool textureAttached[BODYPARTSAMOUNT];
	WeightMode currentUVWeightMode[BODYPARTSAMOUNT];
	BorderMode currentUVBorderMode[BODYPARTSAMOUNT];
	glm::vec2 UVOffset[BODYPARTSAMOUNT];
	float UVRotate[BODYPARTSAMOUNT];
	float UVScale[BODYPARTSAMOUNT];
	//regarding to texture every 
	std::string ccTextureIDMapping[BODYPARTSAMOUNT];


	/*WRAPMODE ccWrapMode[BODYPARTSAMOUNT];
	MIN_MAG_MODE ccMinMagMode[BODYPARTSAMOUNT];
	glm::vec4 ccBorderColor[BODYPARTSAMOUNT];*/
};


typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;
//directly from trimesh added by findvertex implementation
class MyMesh : public TriMesh
{
public:
	MyMesh();
	~MyMesh();

	int FindVertex(MyMesh::Point goalPoint);
	void ClearMesh();
};
//directly from MyMesh, but can also load texture
class GLMesh
{
public:
	GLMesh();
	~GLMesh();

	bool Init(std::string fileName);
	void Render();
	void LoadTexCoordToShader();

	MyMesh mesh;
	GLuint MeshVAO;
	GLuint MeshEBO;
	GLuint MeshVBOVertex, MeshVBONormal, MeshVBOTexCoord;


private:

	bool LoadModel(std::string fileName);
	void LoadToShader();
};

//the whole mesh, including all the required operations
class MeshObject
{
public:
	std::string meshNames[BODYPARTSAMOUNT] = {
		"HEAD",
		"LEFTEAR",
		"RIGHTEAR",
		"LEFTEYE",
		"RIGHTEYE",
		"UPPERJAW",
		"LOWERJAW",
		"TEETH",
		"UPPERBACK",
		"MIDDLEBACK",
		"LOWERBACK",
		"TAILTOLOWERBUTT",

		"LEFTTHIGHCAP",
		"RIGHTTHIGHCAP",

		"LEFTKNEECAP",
		"RIGHTKNEECAP",
		"LEFTKNEEAROUND",
		"RIGHTKNEEAROUND",

		"LEFTSHIN",
		"RIGHTSHIN",
		"LEFTMIDDLESHIN",
		"RIGHTMIDDLESHIN",
		"LEFTTOE",
		"RIGHTTOE",


		"RIGHTCHEST",
		"LEFTCHEST",
		"ABS",

		"LEFTUPPERARMDOWN",
		"LEFTLOWERARMTOP",
		"LEFTLOWERARMDOWN",

		"RIGHTUPPERARMDOWN",
		"RIGHTLOWERARMTOP",
		"RIGHTLOWERARMDOWN"
	};

	std::set<GLuint> choosenFaceForUniversal;

	bool universalTextureLoaded;
	GLMesh model;
	GLMesh submodel; //for partially chosen mesh
	MyMesh::HalfedgeHandle firstHeh; //for the first Heh (for parameterization iteration)
	WeightMode curWeightMode;
	BorderMode curBorderMode;

	//universal texture neccesities
	glm::vec4 univBgColor;
	std::string objName;
	GLuint submeshesAmount;

	/*void changeTextureWrap(GLuint currentTexID, std::map<GLuint, WRAPMODE>& texWrap, WRAPMODE curWrapMode, glm::vec4 texBorderWrapColor) {
		/// <summary>
		/// WARNING, NO TEXTURE BINDING
		/// </summary>
		texWrap[currentTexID] = curWrapMode;
		if (curWrapMode == REPEAT) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else if (curWrapMode == MIRRORED_REPEAT) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		}
		else if (curWrapMode == CLAMP_EDGE) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &texBorderWrapColor[0]);
		}
	}*/

	/*void changeTextureMinMag(GLuint currentTexID, std::map<GLuint, MIN_MAG_MODE>& texMinMagMode, MIN_MAG_MODE curMinMagMode) {
		/// <summary>
		/// WARNING, NO TEXTURE BINDING
		/// </summary>
		texMinMagMode[currentTexID] = curMinMagMode;
		if (curMinMagMode == LINEAR) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
		}
	}*/

	OpenMesh::FPropHandleT<int> faceCategorization;

	//related to submeshes structuring
	GLMesh submeshes[BODYPARTSAMOUNT];
	std::set<GLuint>submeshesFaces[BODYPARTSAMOUNT]; //including doubles
	//std::set<GLuint>noDoublesSubmeshesFaces[BODYPARTSAMOUNT]; //no double faces, for effective rendering
	OpenMesh::VPropHandleT<OpenMesh::VertexHandle> toModelSubmeshes[BODYPARTSAMOUNT];
	std::map<GLuint, MyMesh::TexCoord2D> submeshesVertexUVMappingMeanQuad[BODYPARTSAMOUNT];
	std::map<GLuint, MyMesh::TexCoord2D> submeshesVertexUVMappingMeanDisk[BODYPARTSAMOUNT];
	std::map<GLuint, MyMesh::TexCoord2D> submeshesVertexUVMappingHarmonicQuad[BODYPARTSAMOUNT];
	std::map<GLuint, MyMesh::TexCoord2D> submeshesVertexUVMappingHarmonicDisk[BODYPARTSAMOUNT];
	std::map<GLuint, MyMesh::TexCoord2D> submeshesVertexUVMappingUniformQuad[BODYPARTSAMOUNT];
	std::map<GLuint, MyMesh::TexCoord2D> submeshesVertexUVMappingUniformDisk[BODYPARTSAMOUNT];
	OpenMesh::HPropHandleT<OpenMesh::HalfedgeHandle> toModelHeSubmeshes[BODYPARTSAMOUNT];
	OpenMesh::HPropHandleT<bool> checkedBorderSubmeshes[BODYPARTSAMOUNT];
	int currentlySelectedComponent;
	
	//related to rendering
	GLuint submeshesVAO[BODYPARTSAMOUNT];
	GLuint submeshesVBO[BODYPARTSAMOUNT];
	GLuint submeshesNBO[BODYPARTSAMOUNT];
	GLuint submeshesTBO[BODYPARTSAMOUNT];

	MeshObject();
	~MeshObject();
	void bodyPartsWriter(int in) {
		std::fstream f;
		f.open(ResourcePath::universalTexPath+meshNames[in]+".mtc", std::ios::out);
		//std::sort(selectedFace.begin(), selectedFace.end());
		for (auto iter = selectedFace.begin(); iter != selectedFace.end(); iter++) {
			f << "f " << *iter << "\n";
		}
		f << "end\n";
		f.close();
	}

	void bodyPartsLoader(int in) {
		std::fstream f;
		f.open(ResourcePath::universalTexPath + meshNames[in] + ".mtc", std::ios::in);
		//selectedFace.clear();
		submeshesFaces[in].clear();
		std::string inS;
		while (f >> inS) {
			if (inS == "end") {
				break;
			}
			else if (inS == "f"){
				GLuint inU;
				f >> inU;
				submeshesFaces[in].insert(inU);
				model.mesh.property(faceCategorization, model.mesh.face_handle(inU)) = in;
			}
			else {
				std::cout << "Invalid format for body parts loader : " << inS << "\n";
				exit(EXIT_FAILURE);
			}
		}

		CopySelectFaceFromFile(in);

		//std::sort(selectedFace.begin(), selectedFace.end());
		f.close();
	}

	void bodyPartsLoaderDebug(int in) {
		std::fstream f;
		f.open(ResourcePath::universalTexPath + meshNames[in] + ".mtc", std::ios::in);
		//selectedFace.clear();
		selectedFace.clear();
		std::string inS;
		while (f >> inS) {
			if (inS == "end") {
				break;
			}
			else if (inS == "f") {
				GLuint inU;
				f >> inU;
				selectedFace.insert(inU);
			}
			else {
				std::cout << "Invalid format for body parts loader : " << inS << "\n";
				exit(EXIT_FAILURE);
			}
		}

		CopySelectFaceGL();

		//std::sort(selectedFace.begin(), selectedFace.end());
		f.close();
	}

	void clearSelectedFace() {
		selectedFace.clear();
	}

	int selectComponent() {
		GLuint countArr[BODYPARTSAMOUNT];
		for (int i = 0; i < BODYPARTSAMOUNT; i++) {
			countArr[i] = 0;
		}
		GLuint curMaxEnum = 0;
		int curMaxIdx = -1;
		for (auto iter = choosenFaceForUniversal.begin(); iter != choosenFaceForUniversal.end(); iter++) {
			int curCat = model.mesh.property(faceCategorization, model.mesh.face_handle(*iter));
			if (curCat != -1) {
				countArr[curCat]++;
				if (countArr[curCat] > curMaxEnum) {
					curMaxIdx = curCat;
				}
			}
		}

		if (curMaxIdx != -1) {
			//TransferSelectedFaceToMain(curMaxIdx);
			currentlySelectedComponent = curMaxIdx;
		}
		return curMaxIdx;
	}

	void loadAllUniversalFaceMappingDebug() {
		for (int i = 0; i < BODYPARTSAMOUNT; i++) {
			std::fstream f;
			f.open(ResourcePath::universalTexPath + meshNames[i] + ".mtc", std::ios::in);
			//selectedFace.clear();
			std::string inS;
			while (f >> inS) {
				if (inS == "end") {
					break;
				}
				else if (inS == "f") {
					GLuint inU;
					f >> inU;
					selectedFace.insert(inU);
				}
				else {
					std::cout << "Invalid format for body parts loader : " << inS << "\n";
					exit(EXIT_FAILURE);
				}
			}
			//std::sort(selectedFace.begin(), selectedFace.end());
			f.close();
		}
	}

	void loadParameterize(GLuint meshIndex, bool isQuad, WeightMode weightModeNow) {
		std::fstream f;
		if (meshIndex >= BODYPARTSAMOUNT) {
			std::cout << "Failed to load parameterization: Mesh Index overflow " << meshIndex << "\n";
			return;
		}

		std::string additionalString;

		if (weightModeNow == UNIFORM) {
			if (isQuad) {
				additionalString = "_UNI_QUAD.txc";
			}
			else {
				additionalString = "_UNI_DISK.txc";
			}
		}
		else if (weightModeNow == HARMONIC) {
			if (isQuad) {
				additionalString = "_HRM_QUAD.txc";
			}
			else {
				additionalString = "_HRM_DISK.txc";
			}
		}
		else {
			if (isQuad) {
				additionalString = "_MEAN_QUAD.txc";
			}
			else {
				additionalString = "_MEAN_DISK.txc";
			}
		}
		f.open(ResourcePath::universalTexPath + meshNames[meshIndex] + additionalString, std::ios::in);
		if (!f) {
			std::cout << "Failed to load parameterization: " << meshNames[meshIndex] + additionalString << ".txc not exist in universalTexPath.\n";
			return;
		}

		if (weightModeNow == UNIFORM) {
			if (isQuad) {
				submeshesVertexUVMappingUniformQuad[meshIndex].clear();
			}
			else {
				submeshesVertexUVMappingUniformDisk[meshIndex].clear();
			}
		}
		else if (weightModeNow == HARMONIC) {
			if (isQuad) {
				submeshesVertexUVMappingHarmonicQuad[meshIndex].clear();
			}
			else {
				submeshesVertexUVMappingHarmonicDisk[meshIndex].clear();
			}
		}
		else {
			if (isQuad) {
				submeshesVertexUVMappingMeanQuad[meshIndex].clear();
			}
			else {
				submeshesVertexUVMappingMeanDisk[meshIndex].clear();
			}
		}
		std::map<GLuint, MyMesh::TexCoord2D> inputSet;
		std::string inS;
		while (f >> inS) {
			if (inS == "end") {
				break;
			}
			else if (inS == "v") {
				GLuint id;
				float input1, input2;
				f >> id >> input1 >> input2;
				if (input1 != -10 && input2 != -10) {
					MyMesh::TexCoord2D curIn;
					curIn[0] = input1;
					curIn[1] = input2;
					inputSet[id] = curIn;
				}
			}
			else {
				std::cout << "Invalid format for body parts parameterize loader : " << inS << "\n";
				exit(EXIT_FAILURE);
			}
		}

		if (weightModeNow == UNIFORM) {
			if (isQuad) {
				submeshesVertexUVMappingUniformQuad[meshIndex]=inputSet;
			}
			else {
				submeshesVertexUVMappingUniformDisk[meshIndex] = inputSet;
			}
		}
		else if (weightModeNow == HARMONIC) {
			if (isQuad) {
				submeshesVertexUVMappingHarmonicQuad[meshIndex] = inputSet;
			}
			else {
				submeshesVertexUVMappingHarmonicDisk[meshIndex] = inputSet;
			}
		}
		else {
			if (isQuad) {
				submeshesVertexUVMappingMeanQuad[meshIndex] = inputSet;
			}
			else {
				submeshesVertexUVMappingMeanDisk[meshIndex] = inputSet;
			}
		}

		for (MyMesh::VertexIter vIt = submeshes[meshIndex].mesh.vertices_begin(); vIt != submeshes[meshIndex].mesh.vertices_end(); vIt++) {
			MyMesh::VertexHandle realVertices = submeshes[meshIndex].mesh.property(toModelSubmeshes[meshIndex], *vIt);
			if (inputSet.find(realVertices.idx()) == inputSet.end()) {
				std::cout << "Fault! No index of vertices found for : " << meshNames[meshIndex] << " ,vertice idx: " << realVertices.idx() << std::endl;
				exit(EXIT_FAILURE);
			}
			submeshes[meshIndex].mesh.set_texcoord2D(*vIt, inputSet[realVertices.idx()]);
		}
		//std::cout << "loadhere1\n";
		//for debug only
		//TransferSelectedFaceToMain(meshIndex);
		f.close();
	}

	void parameterizeAndSave(GLuint meshIndex, bool isQuad, WeightMode weightModeNow) {
		std::fstream f;
		if (meshIndex >= BODYPARTSAMOUNT) {
			std::cout << "Failed to parameterize body part: Mesh Index overflow " << meshIndex << "\n";
			return;
		}
		curWeightMode = weightModeNow;

		std::string additionalString;

		if (weightModeNow == UNIFORM) {
			if (isQuad) {
				additionalString = "_UNI_QUAD.txc";
				curBorderMode = QUAD;
				submeshesVertexUVMappingUniformQuad[meshIndex].clear();
			}
			else {
				additionalString = "_UNI_DISK.txc";
				curBorderMode = DISK;
				submeshesVertexUVMappingUniformDisk[meshIndex].clear();
			}
		}
		else if (weightModeNow == HARMONIC) {
			if (isQuad) {
				additionalString = "_HRM_QUAD.txc";
				curBorderMode = QUAD;
				submeshesVertexUVMappingHarmonicQuad[meshIndex].clear();
			}
			else {
				additionalString = "_HRM_DISK.txc";
				curBorderMode = DISK;
				submeshesVertexUVMappingHarmonicDisk[meshIndex].clear();
			}
		}
		else {
			if (isQuad) {
				additionalString = "_MEAN_QUAD.txc";
				curBorderMode = QUAD;
				submeshesVertexUVMappingMeanQuad[meshIndex].clear();
			}
			else {
				additionalString = "_MEAN_DISK.txc";
				curBorderMode = DISK; 
				submeshesVertexUVMappingMeanDisk[meshIndex].clear();
			}
		}
		f.open(ResourcePath::universalTexPath+ meshNames[meshIndex] + additionalString, std::ios::out);
		//parameterizing
		//std::cout << "arrive here1\n";
		customParameterization(meshIndex, 0, 0, 0, 0);
		//std::cout << "arrive here2\n";
		std::map<GLuint, MyMesh::TexCoord2D> resMap;
		for (MyMesh::VertexIter vIt = submeshes[meshIndex].mesh.vertices_begin(); vIt != submeshes[meshIndex].mesh.vertices_end(); vIt++) {
			f << "v ";
			GLuint curIdx = submeshes[meshIndex].mesh.property(toModelSubmeshes[meshIndex], *vIt).idx();
			f << curIdx << " ";
			MyMesh::TexCoord2D curTexCoord = submeshes[meshIndex].mesh.texcoord2D(*vIt);
			if (curTexCoord[0] < 0 || curTexCoord[0] > 1 || curTexCoord[1] < 0 || curTexCoord[1] > 1) {
				std::cout << "Abnormalities while parameterizing " << meshNames[meshIndex] << " index vIt = " << submeshes[meshIndex].mesh.property(toModelSubmeshes[meshIndex], *vIt) << "\n";
			}
			f << curTexCoord[0] << " " << curTexCoord[1] << "\n";
			resMap[curIdx] = curTexCoord;
		}

		f << "end\n";
		if (weightModeNow == UNIFORM) {
			if (isQuad) {
				submeshesVertexUVMappingUniformQuad[meshIndex] = resMap;
			}
			else {
				submeshesVertexUVMappingUniformDisk[meshIndex] = resMap;
			}
		}
		else if (weightModeNow == HARMONIC) {
			if (isQuad) {
				submeshesVertexUVMappingHarmonicQuad[meshIndex] = resMap;
			}
			else {
				submeshesVertexUVMappingHarmonicDisk[meshIndex] = resMap;
			}
		}
		else {
			if (isQuad) {
				submeshesVertexUVMappingMeanQuad[meshIndex] = resMap;
			}
			else {
				submeshesVertexUVMappingMeanDisk[meshIndex] = resMap;
			}
		}
		f.close();
	}
	
	void parameterizeAllBody(BorderMode bm, WeightMode wm){
		//curWeightMode = MEAN;
		//curBorderMode = QUAD; 
		std::cout << "Beginning ";

		if (wm == UNIFORM) {
			std::cout << "Uni ";
		}
		else if (wm == HARMONIC) {
			std::cout << "Harmonic ";
		}
		else {
			std::cout << "Mean ";
		}

		if (bm == QUAD) {
			std::cout << "Quad\n";
		}
		else {
			std::cout << "Disk\n";
		}

		bool isQuad = (bm == QUAD);

		for (int i = 0; i < BODYPARTSAMOUNT; i++) {
			parameterizeAndSave(i, isQuad, wm);
		}
	}

	void RenderComponent(unsigned int idx, GLuint curTexID, bool isLeftSubwindow, bool isTextureAttached) {
		//std::string texname = ccTextureIDMapping[idx];
		if (isLeftSubwindow && isTextureAttached) {
			glBindTexture(GL_TEXTURE_2D, curTexID);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		// for debug only
		/*if (idx == HEAD) {
			glBindTexture(GL_TEXTURE_2D, texIDMapping["checkerboard4.jpg"]);
		}*/
		glBindVertexArray(submeshesVAO[idx]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, submeshes[idx].mesh.n_faces() * 3);
	}
	void LoadVAOVBONBOBodyParts(unsigned int idx) {
		//not include texcoord
		if (idx >= BODYPARTSAMOUNT) {
			std::cout << "Failed loading VAO VBO NBO: Index Enum invalid\n";
		}
		std::vector<MyMesh::Point> vertices;
		std::vector<MyMesh::Normal> normals;
		//allocate space for faster vector insert
		vertices.reserve(submeshesFaces[idx].size() * 3);
		normals.reserve(submeshesFaces[idx].size() * 3);
		for (auto iter = submeshesFaces[idx].begin(); iter != submeshesFaces[idx].end(); iter++) {
			//read the faces data one by one and upload the data into the BO
			OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);


			//iterate vertex in face and add position, normal, texture
			for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
				//first, vertex position
				vertices.push_back(model.mesh.point(*fvIter));
				//then normals
				normals.push_back(model.mesh.normal(*fvIter));
			}
		}
		glBindVertexArray(submeshesVAO[idx]);
		//store vertices to vbo
		glBindBuffer(GL_ARRAY_BUFFER, submeshesVBO[idx]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
		glEnableVertexAttribArray(0);
		//store normal to nbo
		glBindBuffer(GL_ARRAY_BUFFER, submeshesNBO[idx]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Normal) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
		glEnableVertexAttribArray(1);
		//store texcoords to tbo
		//glBindBuffer(GL_ARRAY_BUFFER, submeshesTBO[i]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::TexCoord2D) * texcoords2D.size(), &texcoords2D[0], GL_STATIC_DRAW);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
		//glEnableVertexAttribArray(2);

		//unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);		
	}
	void LoadTBOBodyParts(unsigned int idx, BorderMode bm, WeightMode wm) {
		if (idx >= BODYPARTSAMOUNT) {
			std::cout << "Failed loading VAO VBO NBO: Index Enum invalid\n";
		}
		std::vector<MyMesh::TexCoord2D> texcoords2D;
		//allocate space for faster vector insert
		//texcoords2D.reserve(submeshesFaces[idx].size() * 3);

		if (wm == MEAN) {
			if (bm == QUAD) {
				for (auto iter = submeshesFaces[idx].begin(); iter != submeshesFaces[idx].end(); iter++) {
					//read the faces data one by one and upload the data into the BO
					OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);
					//iterate vertex in face and add position, normal, texture
					for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
						//first, texcoords2D
						texcoords2D.push_back(submeshesVertexUVMappingMeanQuad[idx][fvIter->idx()]);
					}
				}
			}
			else {
				//disk
				for (auto iter = submeshesFaces[idx].begin(); iter != submeshesFaces[idx].end(); iter++) {
					//read the faces data one by one and upload the data into the BO
					OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);
					//iterate vertex in face and add position, normal, texture
					for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
						//first, texcoords2D
						texcoords2D.push_back(submeshesVertexUVMappingMeanDisk[idx][fvIter->idx()]);
					}
				}
			}
		}
		else if (wm == UNIFORM) {
			if (bm == QUAD) {
				for (auto iter = submeshesFaces[idx].begin(); iter != submeshesFaces[idx].end(); iter++) {
					//read the faces data one by one and upload the data into the BO
					OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);
					//iterate vertex in face and add position, normal, texture
					for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
						//first, texcoords2D
						texcoords2D.push_back(submeshesVertexUVMappingUniformQuad[idx][fvIter->idx()]);
					}
				}
			}
			else {
				//disk
				for (auto iter = submeshesFaces[idx].begin(); iter != submeshesFaces[idx].end(); iter++) {
					//read the faces data one by one and upload the data into the BO
					OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);
					//iterate vertex in face and add position, normal, texture
					for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
						//first, texcoords2D
						texcoords2D.push_back(submeshesVertexUVMappingUniformDisk[idx][fvIter->idx()]);
					}
				}
			}
		}
		else {
			//harmonic
			if (bm == QUAD) {
				for (auto iter = submeshesFaces[idx].begin(); iter != submeshesFaces[idx].end(); iter++) {
					//read the faces data one by one and upload the data into the BO
					OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);
					//iterate vertex in face and add position, normal, texture
					for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
						//first, texcoords2D
						texcoords2D.push_back(submeshesVertexUVMappingHarmonicQuad[idx][fvIter->idx()]);
					}
				}
			}
			else {
				//disk
				for (auto iter = submeshesFaces[idx].begin(); iter != submeshesFaces[idx].end(); iter++) {
					//read the faces data one by one and upload the data into the BO
					OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);
					//iterate vertex in face and add position, normal, texture
					for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
						//first, texcoords2D
						texcoords2D.push_back(submeshesVertexUVMappingHarmonicDisk[idx][fvIter->idx()]);
					}
				}
			}
		}
		glBindVertexArray(submeshesVAO[idx]);
		//store texcoords to tbo
		glBindBuffer(GL_ARRAY_BUFFER, submeshesTBO[idx]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::TexCoord2D) * texcoords2D.size(), &texcoords2D[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
		glEnableVertexAttribArray(2);

		//unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	bool Init(std::string fileName);
	void Render();
	void RenderSelectedFace();
	void RenderSelectedFaceCC();
	bool AddSelectedFace(unsigned int faceID);

	bool AddSelectedFaceMulti(unsigned int faceID, int depth, int step = 0) {
		if (depth > step) {
			if (selectedFace.find(faceID) == selectedFace.end() &&
				faceID >= 0 && faceID < model.mesh.n_faces())
			{
				selectedFace.insert(faceID);
				return true;
			}
			MyMesh::FaceHandle fh = model.mesh.face_handle(faceID);
			for (MyMesh::FaceFaceIter ffi = model.mesh.ff_begin(fh); ffi != model.mesh.ff_end(fh); ffi++) {
				AddSelectedFaceMulti(ffi->idx(), depth, step + 1);
			}
		}
		return false;
	}

	bool AddSelectedFaceCCMulti(unsigned int faceID, int depth, int step = 0) {
		if (depth > step) {
			if (choosenFaceForUniversal.find(faceID) == choosenFaceForUniversal.end() &&
				faceID >= 0 && faceID < model.mesh.n_faces())
			{
				choosenFaceForUniversal.insert(faceID);
				return true;
			}
			MyMesh::FaceHandle fh = model.mesh.face_handle(faceID);
			for (MyMesh::FaceFaceIter ffi = model.mesh.ff_begin(fh); ffi != model.mesh.ff_end(fh); ffi++) {
				AddSelectedFaceCCMulti(ffi->idx(), depth, step + 1);
			}
		}
		return false;
	}

	void DeleteSelectedFace(unsigned int faceID) {
		selectedFace.erase(faceID);
		//selectedFace.erase(std::remove(selectedFace.begin(), selectedFace.end(), faceID), selectedFace.end());
	}

	void DeleteSelectedFaceMultiple(unsigned int faceID, int depth, int step) {
		if (depth > step) {
			if (selectedFace.find(faceID) != selectedFace.end() &&
				faceID >= 0 && faceID < model.mesh.n_faces())
			{
				selectedFace.erase(faceID);
				/*MyMesh::FaceHandle fh = model.mesh.face_handle(faceID);
				for (MyMesh::FaceFaceIter ffi = model.mesh.ff_begin(fh); ffi != model.mesh.ff_end(fh); ffi++) {
					DeleteSelectedFaceMultiple(ffi->idx(), depth, step + 1);
				}*/
				
			}
			MyMesh::FaceHandle fh = model.mesh.face_handle(faceID);
			for (MyMesh::FaceFaceIter ffi = model.mesh.ff_begin(fh); ffi != model.mesh.ff_end(fh); ffi++) {
				DeleteSelectedFaceMultiple(ffi->idx(), depth, step + 1);
			}
		}
		
	}

	bool borderCheck(glm::vec3 &closestPosClick) {
		CopySelectFaceGL();
		MyMesh::HalfedgeHandle heh;
		bool firstNotFound = true;

		for (MyMesh::EdgeIter e_it = submodel.mesh.edges_begin(); e_it != submodel.mesh.edges_end(); ++e_it) {
			if (submodel.mesh.is_boundary(*e_it)) {
				// we found the half edge boundary, only evaluate it when it is first found
				if (firstNotFound) {
					// init our half edge handle
					// get first half edge and check whether it is a boundary or not
					firstHeh = submodel.mesh.halfedge_handle(*e_it, 0);
					//go to opposite side of he that is boundary
					if (!submodel.mesh.is_boundary(firstHeh)) {
						firstHeh = submodel.mesh.halfedge_handle(*e_it, 1);
					}
					submodel.mesh.property(checkedBorder, firstHeh) = true;
					//traverse and mark each halfedge as boundary
					MyMesh::HalfedgeHandle he = firstHeh;
					//std::cout << "---c---\n";
					while ((he = submodel.mesh.next_halfedge_handle(he)) != firstHeh) {
						submodel.mesh.property(checkedBorder, he) = true;
					}
					//std::cout << "---d---\n";
					firstNotFound = false;
				}
				else {
					MyMesh::HalfedgeHandle borderHeh = submodel.mesh.halfedge_handle(*e_it, 0);
					if (!submodel.mesh.is_boundary(borderHeh)) {
						borderHeh = submodel.mesh.halfedge_handle(*e_it, 1);
					}

					//error detection, checking whether the Heh has been traversed before, if false, then stop parameterization, error
					if (!submodel.mesh.property(checkedBorder, borderHeh)) {
						std::cout << "================================\n";
						std::cout << "Stopping parameterization! Donut-shaped\n";
						std::cout << "================================\n";
						MyMesh::Point p = submodel.mesh.point(submodel.mesh.from_vertex_handle(borderHeh));
						closestPosClick[0] = p[0];
						closestPosClick[1] = p[1];
						closestPosClick[2] = p[2];
						return false;
					}
				}
			}
		}

		if (firstNotFound) {
			std::cout << "================================\n";
			std::cout << "Stopping parameterization! Couldn't find border!\n";
			std::cout << "================================\n";
			return false;
		}
		std::cout << "================================\n";
		std::cout << "Succeed!\n";
		std::cout << "================================\n";
		return true;
	}

	void TransferSelectedFaceToMain(unsigned int index) {
		if (index < 0 || index >= BODYPARTSAMOUNT) {
			std::cout << "Invalid index to transfer selected face\n";
			exit(EXIT_FAILURE);
		}
		selectedFace = submeshesFaces[index];
		CopySelectFaceGL();
		std::vector<GLuint> tempVec;
		for (auto iter = selectedFace.begin(); iter != selectedFace.end(); iter++) {
			tempVec.push_back(*iter);
		}
		//map back the tex coord from the submodel to the original mesh
		//all the unset tex coord has been set to -1 -1 previously during the beginning of the function
		int Vecindex = 0;
		MyMesh::TexCoord2D emptyTexCoord;
		emptyTexCoord[0] = -10;
		emptyTexCoord[1] = -10;
		for (MyMesh::VertexIter vIt = model.mesh.vertices_begin(); vIt != model.mesh.vertices_end(); vIt++) {
			model.mesh.set_texcoord2D(*vIt, emptyTexCoord);
		}
		for (MyMesh::FaceIter f_it = submeshes[index].mesh.faces_begin(); f_it != submeshes[index].mesh.faces_end(); ++f_it)
		{
			//std::cout << "a\n";
			MyMesh::FaceHandle realFace = model.mesh.face_handle(tempVec[Vecindex++]);
			MyMesh::FaceHandle subFace = *f_it;
			//std::cout << "b\n";
			MyMesh::FaceVertexIter real_fv_iter = model.mesh.fv_iter(realFace);
			//std::cout << "c\n";
			for (MyMesh::FaceVertexIter sub_fv_iter = submeshes[index].mesh.fv_iter(subFace); real_fv_iter.is_valid() && sub_fv_iter.is_valid(); ++real_fv_iter, ++sub_fv_iter)
			{
				MyMesh::TexCoord2D subTexCoord = submeshes[index].mesh.texcoord2D(*sub_fv_iter);
				//std::cout << "cur tex coord: " << subTexCoord[0] << " " << subTexCoord[1] << "\n";
				//take care of scaling
				/*glm::vec4 tempVec((subTexCoord[0] - 0.5) * scaler + 0.5, (subTexCoord[1] - 0.5) * scaler + 0.5, 0, 1);
				subTexCoord[0] = tempVec.x;
				subTexCoord[1] = tempVec.y;*/
				//submodel.mesh.set_texcoord2D(*sub_fv_iter, subTexCoord); //DANGEROUS, DO NOT OPEN
				model.mesh.set_texcoord2D(*real_fv_iter, subTexCoord);
				//std::cout << "cur tex coord: " << subTexCoord[0] << " " << subTexCoord[1] << "\n";
			}
			//std::cout << "d\n";
		}

		//update the data to VBO
		model.LoadTexCoordToShader();

		//For the multiDrawElements in render parameterized, update the face index and also the total vertices need to be drawn
		fvIDsPtr.clear();
		elemCount.clear();
		fvIDsPtr.reserve(tempVec.size());
		elemCount.reserve(tempVec.size());
		for (int i = 0; i < tempVec.size(); ++i)
		{
			fvIDsPtr.push_back((GLuint*)(tempVec[i] * 3 * sizeof(GLuint))); //multiply by 3 since 1 selectedFace = 3 vertices. So to point to first vertices, *3
			elemCount.push_back(3);
		}

		//std::vector<MyMesh::Point> vertices;
		//std::vector<MyMesh::Normal> normals;
		//std::vector<MyMesh::TexCoord2D> texcoords2D;
		////allocate space for faster vector insert
		//vertices.reserve(selectedFace.size() * 3);
		//normals.reserve(selectedFace.size() * 3);
		//texcoords2D.reserve(selectedFace.size() * 3); //vertex is less than nfaces*3 due to repeated vertices
		//	
		//for (auto iter = selectedFace.begin(); iter != selectedFace.end(); iter++) {
		//	//read the faces data one by one and upload the data into the BO
		//	OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);


		//	//iterate vertex in face and add position, normal, texture
		//	for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
		//		//first, vertex position
		//		vertices.push_back(model.mesh.point(*fvIter));
		//		normals.push_back(model.mesh.normal(*fvIter));
		//		texcoords2D.push_back(submeshesVertexUVMapping[index][fvIter->idx()]);
		//	}
		//}
		//glBindVertexArray(selectedLayerVAO);
		////store vertices to vbo
		//glBindBuffer(GL_ARRAY_BUFFER, selectedLayerVBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
		//glEnableVertexAttribArray(0);
		////store normal to nbo
		//glBindBuffer(GL_ARRAY_BUFFER, selectedLayerNBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Normal) * normals.size(), &normals[0], GL_STATIC_DRAW);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
		//glEnableVertexAttribArray(1);
		////store texcoords to tbo
		//glBindBuffer(GL_ARRAY_BUFFER, selectedLayerTBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::TexCoord2D) * texcoords2D.size(), &texcoords2D[0], GL_STATIC_DRAW);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
		//glEnableVertexAttribArray(2);

		////unbind
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	}
	bool FindClosestPoint(unsigned int faceID, glm::vec3 inClickedPoint, glm::vec3& res) {
		double minLength = DBL_MAX;
		OpenMesh::FaceHandle curFace = model.mesh.face_handle(faceID);
		if (!curFace.is_valid()) {
			return false;
		}

		MyMesh::Point clickedPoint(inClickedPoint.x, inClickedPoint.y, inClickedPoint.z);

		
		
		MyMesh::VertexHandle resVertex;
		for (MyMesh::FVIter vertexAround = model.mesh.fv_iter(curFace); vertexAround.is_valid(); vertexAround++)
		{
			MyMesh::Point vertexPoint= model.mesh.point(*vertexAround);
			/*glm::vec3 distanceVec(clickedPoint[0] - vertexPoint[0], clickedPoint[1] - vertexPoint[1], clickedPoint[2] - vertexPoint[2]);
			double curDis = abs(distanceVec.length());*/
			double curDis = (clickedPoint - vertexPoint).norm();
			if (curDis < minLength)
			{
				minLength = curDis;
				resVertex = *vertexAround;
			}
		}

		//update result
		MyMesh::Point resPoint = model.mesh.point(resVertex);
		res.x = resPoint[0];
		res.y = resPoint[1];
		res.z = resPoint[2];
		return true;
	}
	void customParameterization(GLuint index, float uvRotateAngle = 0, float scaler = 1, float offSetX = 0, float offSetY = 0);

	void Parameterization(float uvRotateAngle = 0,float scaler = 1, float offSetX = 0, float offSetY = 0);
	void RenderParameterized();

	//convertor
	// converting vertex identity from submodel to model
	OpenMesh::VPropHandleT<OpenMesh::VertexHandle> toModelV;
	OpenMesh::HPropHandleT<OpenMesh::HalfedgeHandle> toModelHe;
	//debugger
	//to check whether a border that is not initial, has been traversed before, to detect the mistake of 'donut' like shape in parameterization
	OpenMesh::HPropHandleT<bool> checkedBorder;

	//store weight for every halfedge (computed directly)
	OpenMesh::HPropHandleT<double> halfEdgeWeight;

	//store weight for every halfedge (computed beforehand)
	OpenMesh::HPropHandleT<double> uniOrigheWeight;

	//store weight for every halfedge (computed beforehand)
	OpenMesh::HPropHandleT<double> cotOrigheWeight;

	//store weight for every halfedge (computed beforehand)
	OpenMesh::HPropHandleT<double> tanOrigheWeight;

	void debugObjDebugger() {
		std::fstream debugObj;
		debugObj.open("../Proj3Assets/debug.obj", std::ios::in);

		if (!debugObj) {
			std::cout << "Fail to open debug obj, make sure path is correct\n";
		}
		else {
			std::cout << "success, now debugging\n";
			std::string in;
			while (debugObj >> in) {
				if (in == "vt") {
					std::string vt0, vt1;
					debugObj >> vt0 >> vt1;
					if(vt0 != "nan" && vt0 != "-nan(ind)" && vt1 != "nan" && vt1 != "-nan(ind)")
					std::cout << vt0 << " " << vt1 << std::endl;
				}
			}
		}
		debugObj.close();
	}

	//void loadUniversalMappingToBuffer() {
	//	for (int i = 0; i < submeshes.size(); i++) {
	//		if (i >= submeshesVAO.size()) {
	//			submeshesVAO.push_back(0);
	//			glGenVertexArrays(1, &submeshesVAO[i]);
	//		}
	//		if (i >= submeshesVBO.size()) {
	//			submeshesVBO.push_back(0);
	//			glGenBuffers(1, &submeshesVBO[i]);
	//		}
	//		if (i >= submeshesNBO.size()) {
	//			submeshesNBO.push_back(0);
	//			glGenBuffers(1, &submeshesNBO[i]);
	//		}
	//		if (i >= submeshesTBO.size()) {
	//			submeshesTBO.push_back(0);
	//			glGenBuffers(1, &submeshesTBO[i]);
	//		}

	//		std::vector<MyMesh::Point> vertices;
	//		std::vector<MyMesh::Normal> normals;
	//		std::vector<MyMesh::TexCoord2D> texcoords2D;
	//		//allocate space for faster vector insert
	//		vertices.reserve(noDoublesSubmeshesFaces[i].size() * 3);
	//		normals.reserve(noDoublesSubmeshesFaces[i].size() * 3);
	//		texcoords2D.reserve(noDoublesSubmeshesFaces[i].size() * 3); //vertex is less than nfaces*3 due to repeated vertices
	//		
	//		for (auto iter = noDoublesSubmeshesFaces[i].begin(); iter != noDoublesSubmeshesFaces[i].end(); iter++) {
	//			//read the faces data one by one and upload the data into the BO
	//			OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);


	//			//iterate vertex in face and add position, normal, texture
	//			for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter) {
	//				//first, vertex position
	//				vertices.push_back(model.mesh.point(*fvIter));
	//				normals.push_back(model.mesh.normal(*fvIter));
	//				texcoords2D.push_back(submeshesVertexUVMapping[i][fvIter->idx()]);
	//			}
	//		}
	//		glBindVertexArray(submeshesVAO[i]);
	//		//store vertices to vbo
	//		glBindBuffer(GL_ARRAY_BUFFER, submeshesVBO[i]);
	//		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
	//		glEnableVertexAttribArray(0);
	//		//store normal to nbo
	//		glBindBuffer(GL_ARRAY_BUFFER, submeshesNBO[i]);
	//		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Normal) * normals.size(), &normals[0], GL_STATIC_DRAW);
	//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
	//		glEnableVertexAttribArray(1);
	//		//store texcoords to tbo
	//		glBindBuffer(GL_ARRAY_BUFFER, submeshesTBO[i]);
	//		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::TexCoord2D) * texcoords2D.size(), &texcoords2D[0], GL_STATIC_DRAW);
	//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); //in vec3
	//		glEnableVertexAttribArray(2);

	//		//unbind
	//		glBindBuffer(GL_ARRAY_BUFFER, 0);
	//		glBindVertexArray(0);
	//		
	//	}
	//}

	//void uploadUniversalTexture(GLuint series, std::map<std::string,GLuint> texIDMapping, glm::vec4 &externalMeshFaceColor) {
	//	std::fstream univFile;
	//	univFile.open(ResourcePath::universalTexPath + objName + "-" + std::to_string(series)+".ctm");
	//	if (!univFile) {
	//		std::cout << "Error! Universal Texture Path not found! Make sure to check path is correct\n";
	//		exit(EXIT_FAILURE);
	//	}
	//	else {
	//		resetAllUniversalState();
	//		std::string in;
	//		bool textureListFound = false;
	//		int idxTracking = 0;
	//		while (univFile >> in) {
	//			if (textureListFound) {
	//				if (in == "fullend") {
	//					std::cout << "Parse Success, EOF reach of universal texture\n";
	//					break;
	//				}
	//				else if (in == "In") {
	//					GLuint currentIdx;
	//					univFile >> currentIdx;
	//					if (idxTracking != currentIdx) {
	//						std::cout << "Error: ctm file index enumeration error\n";
	//						exit(EXIT_FAILURE);
	//					}
	//					else {
	//						idxTracking++;
	//						if (idxTracking > submeshesAmount) {
	//							std::cout << "Error: ctm enumeration index overflow\n";
	//							exit(EXIT_FAILURE);
	//						}
	//						GLuint curTexIdx;
	//						univFile >> curTexIdx;

	//						if (curTexIdx + 1> texIndexToID.size()) {
	//							std::cout << "Error: texture enumeration index overflow\n";
	//							exit(EXIT_FAILURE);
	//						}
	//						else {
	//							submeshesToTexIDMap[currentIdx] = texIndexToID[curTexIdx];
	//						}

	//						univFile >> in;
	//						if (in == "WRAPPINGMODE") {
	//							univFile >> in;
	//							if (in == "R") {
	//								submeshTexWrapMode.push_back(REPEAT);
	//							}
	//							else if (in == "MR") {
	//								submeshTexWrapMode.push_back(MIRRORED_REPEAT);
	//							}
	//							else if (in == "CE") {
	//								submeshTexWrapMode.push_back(CLAMP_EDGE);
	//							}
	//							else if (in == "CB") {
	//								submeshTexWrapMode.push_back(CLAMP_BORDER);
	//							}
	//							else {
	//								std::cout << "Error! Unrecognized tex wrap mode: " << in << "\n";
	//								exit(EXIT_FAILURE);
	//							}
	//						}
	//						else {
	//							std::cout << "Error6: Corrupt Universal Texture format, no WRAPPINGMODE\n";
	//							exit(EXIT_FAILURE);
	//						}
	//						univFile >> in;
	//						if (in == "BORDERCOLOR") {
	//							float i1, i2, i3, i4;
	//							univFile >> i1>>i2>>i3>>i4;
	//							submeshesTexBorderColor.push_back(glm::vec4(i1, i2, i3, i4));
	//						}
	//						else {
	//							std::cout << "Error7: Corrupt Universal Texture format, no BORDERCOLOR\n";
	//							exit(EXIT_FAILURE);
	//						}
	//						univFile >> in;
	//						if (in == "MIN_MAG") {
	//							univFile >> in;
	//							if (in == "LIN") {
	//								submeshTexMinMagMode.push_back(LINEAR);
	//							}
	//							else if (in == "NEAR") {
	//								submeshTexMinMagMode.push_back(NEAREST);
	//							}
	//							else {
	//								std::cout << "Error! Unrecognized tex min mag mode: " << in << "\n";
	//								exit(EXIT_FAILURE);
	//							}
	//						}
	//						else {
	//							std::cout << "Error8: Corrupt Universal Texture format, no MIN_MAG\n";
	//							exit(EXIT_FAILURE);
	//						}

	//						std::set<GLuint> tempFaceIdxList;
	//						std::map<GLuint, MyMesh::TexCoord2D> tempCoordList;
	//						while (univFile >> in) {
	//							if (in == "end") {
	//								break;
	//							}
	//							else if (in == "f") {
	//								GLuint inIdx;
	//								univFile >> inIdx;
	//								if (!AddSelectedFaceCustom(inIdx, tempFaceIdxList)) {
	//									std::cout << "Error: input face index is invalid: " << inIdx << std::endl;
	//									exit(EXIT_FAILURE);
	//								}
	//							}
	//							else if (in == "v") {
	//								GLuint id;
	//								float input1, input2;
	//								univFile >> id >> input1 >> input2;
	//								if (input1 != -10 && input2 != -10) {
	//									MyMesh::TexCoord2D curIn;
	//									curIn[0] = input1;
	//									curIn[1] = input2;
	//									tempCoordList[id] = curIn;
	//								}
	//							}
	//							else {
	//								std::cout << "Error: Undetected string while parsing ctm: " << in<<"\n";
	//								exit(EXIT_FAILURE);
	//							}
	//						}
	//						//std::sort(tempFaceIdxList.begin(), tempFaceIdxList.end());
	//						OpenMesh::VPropHandleT<OpenMesh::VertexHandle>* newProp = new OpenMesh::VPropHandleT<OpenMesh::VertexHandle>;
	//						toModelSubmeshes.push_back(newProp);
	//						GLMesh *newMesh = new GLMesh;
	//						submeshes.push_back(newMesh);
	//						CopySelectFaceFromFile(currentIdx, tempFaceIdxList);
	//						submeshesFaces.push_back(tempFaceIdxList);
	//					}
	//				}
	//				else {
	//					std::cout << "Error5: No In or fullend detected in parsing ctm file\n";
	//					exit(EXIT_FAILURE);
	//				}
	//			}
	//			else {
	//				if (in == "OBJname") {
	//					std::string targetObjName;
	//					univFile >> targetObjName;
	//					if (targetObjName == objName) {
	//						univFile >> in;
	//						if (in == "UnivBgColor") {
	//							float i1, i2, i3, i4;
	//							univFile >> i1 >> i2 >> i3 >> i4;
	//							univBgColor = glm::vec4(i1, i2, i3, i4);
	//							externalMeshFaceColor = univBgColor;
	//							univFile >> in;
	//							if (in != "end") {
	//								std::cout << "Error4: Corrupt universal texture format, no end after UnivBgColor\n";
	//								exit(EXIT_FAILURE);
	//							}
	//						}
	//						else {
	//							std::cout << "Error3: Corrupt universal texture format, no UnivBgColor\n";
	//							exit(EXIT_FAILURE);
	//						}

	//						if (in == "TextureList") {
	//							GLuint texEnum = 0; //background not included
	//							while (univFile >> in) {
	//								if (in != "end") {
	//									if (texIDMapping.find(in) == texIDMapping.end()) {
	//										std::cout << "Error: Texture in TextureList not found in the list of available texture\n";
	//									}
	//									else {
	//										texIndexToID[texEnum++] = texIDMapping[in];
	//									}
	//								}
	//								else {
	//									break;
	//								}
	//							}

	//							univFile >> in;
	//							if (in == "CCAmount") {
	//								GLuint amount;
	//								univFile >> amount;
	//								submeshesAmount = amount;
	//							}
	//							else {
	//								std::cout << "Error5: Corrupt universal texture format, no CCAmount\n";
	//								exit(EXIT_FAILURE);
	//							}

	//							textureListFound = true;
	//						}
	//						else {
	//							std::cout << "Error2: Corrupt Universal Texture Format, no TextureList\n";
	//							exit(EXIT_FAILURE);
	//						}
	//					}
	//					else {
	//						std::cout << "Error: Different OBJ Target inside Universal Texture\n";
	//						exit(EXIT_FAILURE);
	//					}
	//				}
	//				else {
	//					std::cout << "Error1: Corrupt Universal Texture Format, no OBJname\n";
	//					exit(EXIT_FAILURE);
	//				}
	//			}
	//		}
	//	}
	//	//if this point is reached, then this means that the whole parsing has finished

	//	//finalize sizes
	//	if (submeshesAmount != submeshes.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshes size are not the same: " << submeshesAmount << " and " << submeshes.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != submeshesToTexIDMap.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshesToTexIDMap size are not the same: " << submeshesAmount << " and " << submeshesToTexIDMap.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != toModelSubmeshes.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and toModelSubmeshes size are not the same: " << submeshesAmount << " and " << toModelSubmeshes.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != submeshTexWrapMode.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshTexWrapMode size are not the same: " << submeshesAmount << " and " << submeshTexWrapMode.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != submeshTexMinMagMode.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshTexMinMagMode size are not the same: " << submeshesAmount << " and " << submeshTexMinMagMode.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != submeshesTexBorderColor.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshesTexBorderColor size are not the same: " << submeshesAmount << " and " << submeshesTexBorderColor.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != submeshesVertexUVMapping.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshesVertexUVMapping size are not the same: " << submeshesAmount << " and " << submeshesToTexIDMap.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != submeshesFaces.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshesFaces size are not the same: " << submeshesAmount << " and " << submeshesFaces.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	checkSubMeshes();
	//	constructNoDoubles();
	//	if(submeshesAmount != noDoublesSubmeshesFaces.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and noDoublesSubmeshesFaces size are not the same: " << submeshesAmount << " and " << noDoublesSubmeshesFaces.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}
	//	loadUniversalMappingToBuffer();


	//	if (submeshesAmount != submeshesVBO.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshesVBO size are not the same: " << submeshesAmount << " and " << submeshesVBO.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != submeshesNBO.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshesNBO size are not the same: " << submeshesAmount << " and " << submeshesNBO.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}

	//	if (submeshesAmount != submeshesTBO.size()) {
	//		std::cout << "Error: Amount of submesh inside the CCAmount and submeshesFaces size are not the same: " << submeshesAmount << " and " << submeshesTBO.size() << "\n";
	//		exit(EXIT_FAILURE);
	//	}
	//	std::cout << "Universal Texture Loading is complete\n";
	//	universalTextureLoaded = true;
	//}

	//bool renderSelectedUniversalTextureLayer(GLuint layerIndex, std::map<GLuint, WRAPMODE>& texWrapMode, std::map<GLuint, MIN_MAG_MODE>& texMinMagMode) {
	//	if (layerIndex >= submeshesAmount) {
	//		std::cout << "renderSelectedUniversalTexture failure: layerIndex invalid\n";
	//		exit(EXIT_FAILURE);
	//	}
	//	glBindVertexArray(selectedLayerVAO);
	//	glActiveTexture(GL_TEXTURE0 + 0);
	//	GLuint curTexID = submeshesToTexIDMap[layerIndex];
	//	glBindTexture(GL_TEXTURE_2D, curTexID);
	//	if (texWrapMode[curTexID] != submeshTexWrapMode[layerIndex]) {
	//		changeTextureWrap(curTexID, texWrapMode, submeshTexWrapMode[layerIndex], submeshesTexBorderColor[layerIndex]);
	//	}
	//	if (texMinMagMode[curTexID] != submeshTexMinMagMode[layerIndex]) {
	//		changeTextureMinMag(curTexID, texMinMagMode, submeshTexMinMagMode[layerIndex]);
	//	}
	//	glDrawArrays(GL_TRIANGLES, 0, submodel.mesh.n_faces() * 3);
	//	glBindVertexArray(0);
	//}

	//void renderCurrentUniversalTexture(std::map<GLuint, WRAPMODE>& texWrapMode, std::map<GLuint, MIN_MAG_MODE>& texMinMagMode) {
	//	if (!universalTextureLoaded) {
	//		std::cout << "Universal Texture not loaded! Render universal failure\n";
	//		exit(EXIT_FAILURE);
	//	}
	//	for (int i = 0; i < submeshesAmount; i++) {
	//		glBindVertexArray(submeshesVAO[i]);
	//		glActiveTexture(GL_TEXTURE0 + 0);
	//		GLuint curTexID = submeshesToTexIDMap[i];
	//		glBindTexture(GL_TEXTURE_2D, curTexID);
	//		if (texWrapMode[curTexID] != submeshTexWrapMode[i]) {
	//			changeTextureWrap(curTexID, texWrapMode, submeshTexWrapMode[i], submeshesTexBorderColor[i]);
	//		}
	//		if (texMinMagMode[curTexID] != submeshTexMinMagMode[i]) {
	//			changeTextureMinMag(curTexID, texMinMagMode, submeshTexMinMagMode[i]);
	//		}
	//		glDrawArrays(GL_TRIANGLES, 0, submeshes[i]->mesh.n_faces() * 3);
	//		glBindVertexArray(0);
	//	}
	//}

	//bool saveUniversalTexture(bool newFile, bool isAppend, GLuint series, std::map<std::string, GLuint> texIDMapping, glm::vec4 singleBackgroundColor, std::string singleTexName, WRAPMODE singleWrapMode, MIN_MAG_MODE singleMinMagMode, glm::vec4 externalMeshFaceColor, glm::vec4 singleBorderColor) {
	//	//if create new, just save the current face
	//	//is append = add new entity into the 
	//	if (!newFile) {
	//		if (!universalTextureLoaded) {
	//			glm::vec4 temp;
	//			uploadUniversalTexture(series, texIDMapping, temp);
	//		}
	//		univBgColor = externalMeshFaceColor;
	//		if (isAppend) {

	//		}
	//		else {

	//		}
	//	}
	//	else {
	//		//erase old, create new
	//		std::fstream univOut;
	//		univOut.open(ResourcePath::universalTexPath + objName + "-" + std::to_string(series) + ".ctm", std::ios::out);
	//		univOut << "OBJname " << objName;
	//		univOut << "\n\nUnivBgColor " << singleBackgroundColor[0] << " " << singleBackgroundColor[1] << " " << singleBackgroundColor[2] << " " << singleBackgroundColor[3] << " end";
	//		univOut << "\n\nTextureList\n" << singleTexName << "\nend\n\n";
	//		univOut << "CCAmount 1\n\n";
	//		univOut << "In 0 0\n";
	//		univOut << "WRAPPINGMODE ";
	//		if (singleWrapMode == REPEAT) {
	//			univOut << "R\n";
	//		}
	//		else if (singleWrapMode == MIRRORED_REPEAT) {
	//			univOut << "MR\n";
	//		}
	//		else if (singleWrapMode == CLAMP_EDGE) {
	//			univOut << "CE\n";
	//		}
	//		else {
	//			univOut << "CB\n";
	//		}
	//		univOut << "BORDERCOLOR " << singleBorderColor[0] << " " << singleBorderColor[1] << " " << singleBorderColor[2] << " " << singleBorderColor[3] << "\n";
	//		univOut << "MIN_MAG";
	//		if (singleMinMagMode == LINEAR) {
	//			univOut << "LIN\n";
	//		}
	//		else {
	//			univOut << "NEAR\n";
	//		}

	//		std::map<GLuint, MyMesh::TexCoord2D> newFaceTexCoord;

	//	}
	//}

std::vector<int> elemCount;

std::vector<unsigned int*> fvIDsPtr;
private:
	//std::vector<unsigned int> selectedFace;
	std::set<unsigned int> selectedFace;


	void CopySelectFaceFromFile(int targetIdx) {
		submeshes[targetIdx].mesh.clear();
		submeshes[targetIdx].mesh.request_vertex_normals();
		submeshes[targetIdx].mesh.request_face_normals();
		submeshes[targetIdx].mesh.add_property(toModelSubmeshes[targetIdx], "toModelSubmeshes"+std::to_string(targetIdx)); //bridging sub model vertex to main model
		submeshes[targetIdx].mesh.request_vertex_texcoords2D();
		submeshes[targetIdx].mesh.add_property(toModelHeSubmeshes[targetIdx], "toModelHeSubmeshes" + std::to_string(targetIdx)); //bridging sub model vertex to main model
		submeshes[targetIdx].mesh.add_property(checkedBorderSubmeshes[targetIdx], "checkedBorderSubmeshes" + std::to_string(targetIdx)); //bridging sub model vertex to main model
		std::map<int, int> usedVertex; // linking orig model to submodel
		
		
		// get the mesh of selected face
		for (auto iter = submeshesFaces[targetIdx].begin(); iter != submeshesFaces[targetIdx].end(); iter++) {
			// get face handler by faceID from original mesh
			OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);

			// face vhandles
			std::vector<MyMesh::VertexHandle> face_vhandles;

			//iterate vertex in face
			//order of adding: 
			//1. add vertex(v)
			//2. from 1's vh, put it inside array/vector
			//3. push the result of 2 to add_face
			int j = 0;
			for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter, j++) {
				// get each vertex handler
				MyMesh::Point p = model.mesh.point(*fvIter);
				MyMesh::VertexHandle vh;

				auto usedVertexIter = usedVertex.find(fvIter->idx()); //possibly found, or return the end iterator of the map

				// the vertex didn't exist yet, append to submodel
				//actually, if we iterate one by one, we should make it counter clockwise
				//but, here, since we use fv_iter of an already existing mesh, we don't need to worry whether it is not clockwise
				if (usedVertexIter == usedVertex.end()) {
					vh = submeshes[targetIdx].mesh.add_vertex(p);
					usedVertex[fvIter->idx()] = vh.idx();

					// set the "pointer" to have a link between new vertex
					// old vertex linked to new vertex
					submeshes[targetIdx].mesh.property(toModelSubmeshes[targetIdx], vh) = *fvIter;
				}
				else {
					vh = submeshes[targetIdx].mesh.vertex_handle(usedVertexIter->second);
				}
				// add the vertex handle for the new mesh(as a new face, collects it inside a handle, needed for add_face later)
				face_vhandles.push_back(vh);
			}

			// add face to the boundary model's mesh
			submeshes[targetIdx].mesh.add_face(face_vhandles);
		}

		//handle the He convertion
		for (MyMesh::HalfedgeIter heIter = submeshes[targetIdx].mesh.halfedges_begin(); heIter != submeshes[targetIdx].mesh.halfedges_end(); heIter++) {
			MyMesh::HalfedgeHandle curHe = *heIter;
			MyMesh::VertexHandle subFrom = submeshes[targetIdx].mesh.from_vertex_handle(curHe);
			MyMesh::VertexHandle subTo = submeshes[targetIdx].mesh.to_vertex_handle(curHe);
			MyMesh::VertexHandle realFrom = submeshes[targetIdx].mesh.property(toModelSubmeshes[targetIdx], subFrom);
			MyMesh::VertexHandle realTo = submeshes[targetIdx].mesh.property(toModelSubmeshes[targetIdx], subTo);

			MyMesh::HalfedgeHandle realHe = model.mesh.find_halfedge(realFrom, realTo);
			submeshes[targetIdx].mesh.property(toModelHeSubmeshes[targetIdx], curHe) = realHe;
		}

		//initialized checked border all to false, to indicate that it has not previously checked
		for (MyMesh::HalfedgeIter heIter = submeshes[targetIdx].mesh.halfedges_begin(); heIter != submeshes[targetIdx].mesh.halfedges_end(); heIter++) {
			submeshes[targetIdx].mesh.property(checkedBorderSubmeshes[targetIdx], *heIter) = false;
		}

	}

	void CopySelectFaceGL() {
		//reset submodel of chosen vertices
		submodel.mesh.clear();
		submodel.mesh.request_vertex_normals();
		submodel.mesh.request_face_normals();
		submodel.mesh.add_property(toModelV, "toModelV"); //bridging sub model vertex to main model
		submodel.mesh.add_property(toModelHe, "toModelHe"); //bridging sub model vertex to main model
		submodel.mesh.request_vertex_texcoords2D();
		submodel.mesh.add_property(halfEdgeWeight, "halfEdgeWeight");
		submodel.mesh.add_property(checkedBorder, "checkedBorder");

		//std::cout << "---a---\n";
		//initialized checked border all to false, to indicate that it has not previously checked
		
		//std::cout << "---b---\n";

		std::map<int, int> usedVertex; // linking orig model to submodel
		// get the mesh of selected face
		for (auto iter = selectedFace.begin(); iter != selectedFace.end(); iter++) {
			// get face handler by faceID from original mesh
			OpenMesh::FaceHandle fh = model.mesh.face_handle(*iter);

			// face vhandles
			std::vector<MyMesh::VertexHandle> face_vhandles;

			//iterate vertex in face
			//order of adding: 
			//1. add vertex(v)
			//2. from 1's vh, put it inside array/vector
			//3. push the result of 2 to add_face
			int j = 0;
			for (MyMesh::FaceVertexIter fvIter = model.mesh.fv_iter(fh); fvIter.is_valid(); ++fvIter, j++) {
				// get each vertex handler
				MyMesh::Point p = model.mesh.point(*fvIter);
				MyMesh::VertexHandle vh;

				auto usedVertexIter = usedVertex.find(fvIter->idx()); //possibly found, or return the end iterator of the map

				// the vertex didn't exist yet, append to submodel
				//actually, if we iterate one by one, we should make it counter clockwise
				//but, here, since we use fv_iter of an already existing mesh, we don't need to worry whether it is not clockwise
				if (usedVertexIter == usedVertex.end()) {
					vh = submodel.mesh.add_vertex(p);
					usedVertex[fvIter->idx()] = vh.idx();

					// set the "pointer" to have a link between new vertex
					// old vertex linked to new vertex
					submodel.mesh.property(toModelV, vh) = *fvIter;
				}
				else {
					vh = submodel.mesh.vertex_handle(usedVertexIter->second);
				}
				// add the vertex handle for the new mesh(as a new face, collects it inside a handle, needed for add_face later)
				face_vhandles.push_back(vh);
			}

			// add face to the boundary model's mesh
			submodel.mesh.add_face(face_vhandles);
		}

		//handle the He convertion
		for (MyMesh::HalfedgeIter heIter = submodel.mesh.halfedges_begin(); heIter != submodel.mesh.halfedges_end(); heIter++) {
			MyMesh::HalfedgeHandle curHe = *heIter;
			MyMesh::VertexHandle subFrom = submodel.mesh.from_vertex_handle(curHe);
			MyMesh::VertexHandle subTo = submodel.mesh.to_vertex_handle(curHe);
			MyMesh::VertexHandle realFrom = submodel.mesh.property(toModelV, subFrom);
			MyMesh::VertexHandle realTo = submodel.mesh.property(toModelV, subTo);

			MyMesh::HalfedgeHandle realHe = model.mesh.find_halfedge(realFrom, realTo);
			submodel.mesh.property(toModelHe, curHe) = realHe;
		}

		for (MyMesh::HalfedgeIter heIter = submodel.mesh.halfedges_begin(); heIter != submodel.mesh.halfedges_end(); heIter++) {
			submodel.mesh.property(checkedBorder, *heIter) = false;
			//std::cout << "In copyselectface: " << submodel.mesh.property(checkedBorder, *heIter) << "\n";
		}
	}
};

