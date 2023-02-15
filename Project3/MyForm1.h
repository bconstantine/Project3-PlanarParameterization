#pragma once

#include <Common.h>
#include <ViewManager.h>
#include <ResourcePath.h>

#include "DotNetUtilities.h"

#include "OpenMesh.h"
#include "MeshObject.h"
#include "MainShader.h"
#include "PickingTexture.h"

#pragma unmanaged
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Include/STB/stb_image_write.h"
#pragma managed

#define TEXTURE_AMOUNT 2
#define MODEL_AMOUNT 2

int currentIndex = 0;

UpdateData dataNow;

bool MyCheckGLError()
{
	GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR)
	{
		const GLubyte* errString = gluErrorString(errCode);
		printf("%d %s\n", errCode, errString);

		return false;
	}
	return true;
}

std::string textureList[TEXTURE_AMOUNT] = {
	"checkerboard4.jpg",
	"marble.jpg"
};

//std::string currentTextureName = textureList[0];
//std::string prevTextureName = currentTextureName;
std::string modelList[MODEL_AMOUNT] = {
	"armadillo.obj",
	"UnionSphere.obj"
};
int chosenModelIndex = 0;
bool updateChosenVertex = false;
bool updateParameterized = false;

int brushSize = 1;

GLuint			program;			//shader program
glm::mat4			proj_matrix;		//projection matrix
float			aspect;

GLuint textureID[BODYPARTSAMOUNT];
//map<GLuint, WRAPMODE> texIDToWrapMapping; 
//map<GLuint, MIN_MAG_MODE> texIDToMinMagMapping;
//map<GLuint, glm::vec4> texBorderColor;
ViewManager		meshWindowCam, texCoordWindowCam;

GLuint individualTextureID;

bool isRightButtonPress = false;
GLuint currentFaceID = 0;
int currentMouseX = 0;
int currentMouseY = 0;
bool drawTexture = false;
float uvRotateAngle = 0.0f;
float prevRotate = uvRotateAngle;
float scaler = 1.0f; //scale the parameterization (between 0 - 1)
float prevScaler = scaler;
float offsetX = 0.0f;
float offsetY = 0.0f;
float prevOffsetX = offsetX;
float prevOffsetY = offsetY;
bool showOutline = true;
MeshObject* model = NULL;

TextureBackgroundDisplay tBgDisplay;
DrawPickingFaceShader drawPickingFaceShader;
PickingShader pickingShader;
PickingTexture pickingTexture;
ChoosenVertexShader choosenVertexShader;
MainShader mainShader;

//vbo and vao for drawing point when needed
GLuint PVBO;
GLuint PVAO;
glm::vec3 closestPosClick;
float pointSize = 10.f;

//colors
glm::vec4 meshFaceColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 texFaceColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
glm::vec4 meshWireColor = glm::vec4(0.f, 0.f, 0.f, 1.0f);
glm::vec4 choosenWireColor = glm::vec4(1.0f, 1.0f, 0.0f, 0.3f);
glm::vec4 texWireColor = glm::vec4(1.0f, 1.0f, 0.f, 1.f);
glm::vec4 pointColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
glm::vec4 texBgColor = glm::vec4(0.3f, 0.3f, 0.3f, 0.3f);
glm::vec4 texBorderWrapColor = meshFaceColor;

std::string saveLoadFilename = "";

//cc buttons and gui
bool ccTextureAttached;
WeightMode ccCurrentUVWeightMode;
BorderMode ccCurrentUVBorderMode;
glm::vec2 ccUVOffset;
float ccUVRotate;
float ccUVScale;
std::string ccTextureIDMapping;

//to track changes, only for now
std::string prevCCTextureIDMapping;

//to track when there is achange of chosen component
int prevSelectedComponent = -1;

void resetCCDataGui() {
	ccTextureAttached = false;
	ccCurrentUVWeightMode = MEAN;
	ccCurrentUVBorderMode = DISK;
	ccUVOffset = glm::vec2(0, 0);
	ccUVRotate = 0;
	ccUVScale = 1;
	//regarding to texture every 
	ccTextureIDMapping = "";
	/*ccWrapMode = MIRRORED_REPEAT;
	ccMinMagMode = LINEAR;
	ccBorderColor = meshFaceColor;*/

	//only for tracking
	prevCCTextureIDMapping = ccTextureIDMapping;
}

void resetDataNow() {
	for (int i = 0; i < BODYPARTSAMOUNT; i++) {
		dataNow.textureAttached[i] = false;
		dataNow.currentUVWeightMode[i] = MEAN;
		dataNow.currentUVBorderMode[i] = DISK;
		dataNow.UVOffset[i] = glm::vec2(0, 0);
		dataNow.UVRotate[i] = 0;
		dataNow.UVScale[i] = 1;
		//regarding to texture every 
		dataNow.ccTextureIDMapping[i] = "";
		glBindTexture(GL_TEXTURE_2D, textureID[i]);
		TextureData tdata = Common::Load_png("");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
		glBindTexture(GL_TEXTURE_2D, 0);
		/*dataNow.ccWrapMode[i] = MIRRORED_REPEAT;
		dataNow.ccMinMagMode[i] = LINEAR;
		dataNow.ccBorderColor[i] = meshFaceColor;*/
	}
}

void resetDataNowAndGUI() {
	resetCCDataGui();
	resetDataNow();
}

void removeTexture(int idx) {
	if (idx < 0 || idx >= BODYPARTSAMOUNT) {
		//std::cout << "Invalid idx when update data now\n";
	}
	else {
		dataNow.ccTextureIDMapping[idx] = "";
		ccTextureIDMapping = "";
	}
}

std::string changeToSingleSlash(std::string filename)
{
	std::string slashResult = "";
	for (int i = 0; i < filename.size(); i++)
	{
		if (filename[i] == '\\')
		{
			slashResult += '/';
		}
		else
		{
			slashResult += filename[i];
		}
	}

	return slashResult;
}

void updaterDataNow(int idx) {
	if (idx < 0 || idx >= BODYPARTSAMOUNT) {
		std::cout << "Invalid idx when update data now\n";
	}
	else {
		dataNow.textureAttached[idx] = ccTextureAttached;
		if (dataNow.currentUVWeightMode[idx] != ccCurrentUVWeightMode || dataNow.currentUVBorderMode[idx] != ccCurrentUVBorderMode) {
			dataNow.currentUVWeightMode[idx] = ccCurrentUVWeightMode;
			dataNow.currentUVBorderMode[idx] = ccCurrentUVBorderMode;
			model->LoadTBOBodyParts(idx, ccCurrentUVBorderMode, ccCurrentUVWeightMode);
		}
		dataNow.UVOffset[idx] = ccUVOffset;
		dataNow.UVRotate[idx] = ccUVRotate;
		dataNow.UVScale[idx] = ccUVScale;
		//regarding to texture every 
		if (dataNow.ccTextureIDMapping[idx] != ccTextureIDMapping) {
			dataNow.ccTextureIDMapping[idx] = ccTextureIDMapping;
		}
		/*dataNow.ccWrapMode[idx] = ccWrapMode;
		dataNow.ccMinMagMode[idx] = ccMinMagMode;
		dataNow.ccBorderColor[idx] = ccBorderColor;*/
	}
}

void passDataNowToVariable(int idx) {
	if (idx < 0 || idx >= BODYPARTSAMOUNT) {
		std::cout << "Invalid idx when update data now\n";
	}
	else {
		prevSelectedComponent = model->currentlySelectedComponent;
		ccTextureAttached = dataNow.textureAttached[idx];
		ccCurrentUVWeightMode = dataNow.currentUVWeightMode[idx];
		ccCurrentUVBorderMode = dataNow.currentUVBorderMode[idx];
		ccUVOffset = dataNow.UVOffset[idx];
		ccUVRotate = dataNow.UVRotate[idx];
		ccUVScale = dataNow.UVScale[idx];
		ccTextureIDMapping = dataNow.ccTextureIDMapping[idx];
		/*ccWrapMode = dataNow.ccWrapMode[idx];
		ccMinMagMode = dataNow.ccMinMagMode[idx];
		ccBorderColor = dataNow.ccBorderColor[idx];*/
	}
}

void chosenVertexUpdater(int windowHeight, glm::mat4 modelView, glm::mat4 proj) {
	//from the example of 
	updateChosenVertex = false;
	float depth = 0;
	int windowX = currentMouseX;
	int windowY = windowHeight - currentMouseY;
	glReadPixels(windowX, windowY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	GLint curViewport[4];
	glGetIntegerv(GL_VIEWPORT, curViewport);
	glm::vec4 viewportVec(curViewport[0], curViewport[1], curViewport[2], curViewport[3]);
	glm::vec3 windowPos(windowX, windowY, depth);
	glm::vec3 worldClickPos = glm::unProject(windowPos, modelView, proj, viewportVec);
	model->FindClosestPoint(currentFaceID - 1, worldClickPos, closestPosClick);

	//update vbo data
	glBindBuffer(GL_ARRAY_BUFFER, PVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), glm::value_ptr(closestPosClick), GL_DYNAMIC_DRAW);
	glBindVertexArray(PVAO);
}

float clampMinMax(float in, float min, float max) {
	if (in < min) {
		return min;
	}
	else if (in > max) {
		return max;
	}
	else {
		return in;
	}
}

void loadModel()
{
	if (model != NULL) {
		delete model;
	}
	model = new MeshObject;
	if (model->Init(ResourcePath::modelPath))
	{
		std::cout << "Initial load model successful\n";
	}
	else
	{
		std::cout << "Failed loading initial model : " << ResourcePath::modelPath << "\n";
		exit(EXIT_FAILURE);

	}
}

void reloadObj(int index) {
	if (model != NULL) {
		delete model;
	}
	model = new MeshObject;
	chosenModelIndex = index;
	ResourcePath::modelPath = "../Proj3Assets/Model/" + modelList[chosenModelIndex];
	if (model->Init(ResourcePath::modelPath))
	{
		std::cout << "reload model successful\n";
	}
	else
	{
		std::cout << "Failed reloading model: " << ResourcePath::modelPath << "\n";
		exit(EXIT_FAILURE);
	}
}

void initTextures()
{

	glGenTextures(1, &individualTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &meshFaceColor[0]);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*texIDToWrapMapping[textureID[textureList[i]]] = MIRRORED_REPEAT;
	texIDToMinMagMapping[textureID[textureList[i]]] = LINEAR;
	texBorderColor[textureID[textureList[i]]] = meshFaceColor;*/
	glBindTexture(GL_TEXTURE_2D, 0);
	for (int i = 0; i < BODYPARTSAMOUNT; i++) {
		textureID[i] = 0;
		glGenTextures(1, &textureID[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &meshFaceColor[0]);
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		/*texIDToWrapMapping[textureID[textureList[i]]] = MIRRORED_REPEAT;
		texIDToMinMagMapping[textureID[textureList[i]]] = LINEAR;
		texBorderColor[textureID[textureList[i]]] = meshFaceColor;*/
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

bool addIndividualTexture(std::string filenameNoPath, int curIdx) {
	//remember, do not include imagePath
	if (curIdx == -10) {
		std::fstream test;
		test.open((ResourcePath::imagePath + filenameNoPath), std::ios::in);
		std::cout << "currentTexture new: " << filenameNoPath << "\n";
		if (!test) {
			std::cout << ResourcePath::imagePath + filenameNoPath << " textureName doesn't exist\n";
			return false;
		}
		test.close();
		//Load texture given filenamein list of string
		std::cout << "Adding texture new: " << filenameNoPath << "\n";
		TextureData tdata = Common::Load_png((ResourcePath::imagePath + filenameNoPath).c_str());
		GLuint newTex = individualTextureID;
		//dataNow.ccTextureIDMapping[curIdx] = filenameNoPath;
		//Generate texture
		//glGenTextures(1, &newTex);
		glBindTexture(GL_TEXTURE_2D, newTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &meshFaceColor[0]);
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		/*texIDToWrapMapping[textureID[filenameNoPath]] = MIRRORED_REPEAT;
		texIDToMinMagMapping[textureID[filenameNoPath]] = LINEAR;
		texBorderColor[textureID[filenameNoPath]] = meshFaceColor;*/
		glBindTexture(GL_TEXTURE_2D, 0);

		//textureID[filenameNoPath] = newTex;

		return true;
	}

	if (curIdx < 0 || curIdx >= BODYPARTSAMOUNT) {
		std::cout << "Stopping adding texture: Invalid Index\n";
		return false;
	}
	std::fstream test;
	test.open((ResourcePath::imagePath + filenameNoPath), std::ios::in);
	std::cout << "currentTexture: " << filenameNoPath << "\n";
	if (!test) {
		std::cout << ResourcePath::imagePath + filenameNoPath << " textureName doesn't exist\n";
		return false;
	}
	test.close();
	//Load texture given filenamein list of string
	std::cout << "Adding texture: " << filenameNoPath << "\n";
	TextureData tdata = Common::Load_png((ResourcePath::imagePath + filenameNoPath).c_str());
	GLuint newTex = textureID[curIdx];
	dataNow.ccTextureIDMapping[curIdx] = filenameNoPath;
	//Generate texture
	//glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_2D, newTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	std::cout << "Successfully add texture " << filenameNoPath << "\n";
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &meshFaceColor[0]);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*texIDToWrapMapping[textureID[filenameNoPath]] = MIRRORED_REPEAT;
	texIDToMinMagMapping[textureID[filenameNoPath]] = LINEAR;
	texBorderColor[textureID[filenameNoPath]] = meshFaceColor;*/
	glBindTexture(GL_TEXTURE_2D, 0);

	//textureID[filenameNoPath] = newTex;

	return true;
}

void InitOpenGL()
{
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*glEnable(GL_CULL_FACE);
	glEnable(GL_BACK);*/

	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); //just in case we draw on same depth but in different order
}

glm::mat4 initialWindowCam;
void InitData(int windowWidth, int windowHeight, bool isSubHKO)
{
	resetDataNowAndGUI();

	initTextures();
	resetDataNowAndGUI();
	ResourcePath::imagePath = "../Proj3Assets/Textures/";
	ResourcePath::modelPath = "../Proj3Assets/Model/" + modelList[chosenModelIndex];
	ResourcePath::universalTexPath = "../Proj3Assets/CustomMetadata/" + modelList[chosenModelIndex];;
	ResourcePath::fullTexturePath = "../Proj3Assets/FullTex/";

	initialWindowCam[0][0] = 1;
	initialWindowCam[0][1] = 0;
	initialWindowCam[0][2] = -0;
	initialWindowCam[0][3] = 0;
	initialWindowCam[1][0] = -0;
	initialWindowCam[1][1] = 1;
	initialWindowCam[1][2] = 0;
	initialWindowCam[1][3] = 0;
	initialWindowCam[2][0] = 0;
	initialWindowCam[2][1] = 0;
	initialWindowCam[2][2] = 1;
	initialWindowCam[2][3] = -0;
	initialWindowCam[3][0] = 0;
	initialWindowCam[3][1] = 0;
	initialWindowCam[3][2] = -10;
	initialWindowCam[3][3] = 1;


	//shaders init
	//cout << "tass0\n";
	tBgDisplay.Init();
	//cout << "tass-1\n";
	mainShader.Init();
	//cout << "tass\n";
	pickingShader.Init();
	//cout << "tass2\n";
	pickingTexture.Init(windowWidth, windowHeight);
	drawPickingFaceShader.Init();
	//cout << "tass3\n";
	choosenVertexShader.Init();

	//for point selection
	glGenBuffers(1, &PVBO);
	glGenVertexArrays(1, &PVAO);


	//load one model only, reload everytime model is changed
	loadModel();
}

void ReshapeforMainHKO(int width, int height)
{
	int meshWindowWidth = width;
	int meshWindowHeight = height;
	glViewport(0, 0, meshWindowWidth, meshWindowHeight);

	aspect = static_cast<float> (meshWindowWidth) / meshWindowHeight;
	meshWindowCam.SetWindowSize(meshWindowWidth, meshWindowHeight);
	//reset picking texture size
	pickingTexture.Init(meshWindowWidth, meshWindowHeight);
}

void ReshapeforSubHKO(int width, int height)
{
	int texCoordWindowWidth = width;
	int texCoordWindowHeight = height;
	//set window to display texture coordinate
	glViewport(0, 0, texCoordWindowWidth, texCoordWindowHeight);

	texCoordWindowCam.SetWindowSize(texCoordWindowWidth, texCoordWindowHeight);
	texCoordWindowCam.ToggleOrtho(); //set as orthogonal
}

std::string cleanPathNameTex(std::string path) {
	int slash = -1;
	for (int i = 0; i < path.size(); i++) {
		if (path[i] == '/') {
			slash = i;
		}
	}
	std::string newString;
	if (slash == -1) {
		std::cout << "no slash found!\n";
		newString = path;
	}
	else {
		for (int i = slash; i < path.size(); i++) {
			newString.push_back(path[i]);
		}
	}

	//clean path name to follow the correct format
	//newString = ResourcePath::imagePath + newString;
	return newString;
}

std::string cleanPathNameSavingUniversalTex(std::string path) {
	int idx = path.size() - 1;
	for (; idx > -1; idx--) {
		if (path[idx] == '.') {
			break;
		}
	}

	//take the filename of all in front, but leave the file extension
	std::string returnString;
	if (idx == -1) {
		std::cout << "Warning: No file extensions found, return entire path instead\n";
		returnString = path;
	}
	else {
		for (int i = 0; i < idx; i++) {
			returnString.push_back(path[i]);
		}
		returnString += ".ctm";
	}
	return returnString;
}

void univTexSaving(std::string name) {
	//std::string trueName = cleanPathName(name);
	std::string trueName = cleanPathNameSavingUniversalTex(name);

	std::fstream f;
	std::cout << "entering here, truename " << trueName << "\n";
	f.open(trueName, std::ios::out);

	for (int i = 0; i < BODYPARTSAMOUNT; i++) {
		//parts enumeration
		f << "PARTS " << i << "\n";

		//Weight Mode
		f << "WEIGHTMODE ";
		if (dataNow.currentUVWeightMode[i] == MEAN) {
			f << "MEAN\n";
		}
		else if (dataNow.currentUVWeightMode[i] == HARMONIC) {
			f << "HARMONIC\n";
		}
		else {
			f << "UNIFORM\n";
		}

		//Border Mode
		f << "BORDERMODE ";
		if (dataNow.currentUVBorderMode[i] == DISK) {
			f << "DISK\n";
		}
		else {
			f << "QUAD\n";
		}

		//UV Offset
		f << "UVOFFSET " << dataNow.UVOffset[i][0] << " " << dataNow.UVOffset[i][1] << "\n";

		//UV Rotate
		f << "UVROTATE " << dataNow.UVRotate[i] << "\n";

		//UV Scale
		f << "UVSCALE " << dataNow.UVScale[i] << "\n";

		//TextureName
		f << "CCTEXTUREIDMAPPING ";
		if (dataNow.ccTextureIDMapping[i] == "") {
			f << "NULL\n";
		}
		else {
			std::cout << "Writing textuer name: " << dataNow.ccTextureIDMapping[i];
			f << dataNow.ccTextureIDMapping[i] << "\n";
		}
		f << "END\n\n";
	}
	f << "FULLEND\n";
	f.close();
}

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	public enum SelectionMode
	{
		ADD_FACE,
		DEL_FACE,
		SELECT_POINT,
		UNIVERSAL_TEX
	};

	void SelectionHandler(SelectionMode selectionMode, int windowHeight, int x, int y) {
		GLuint faceID = pickingTexture.ReadTexture(x, windowHeight - y - 1);
		if (faceID != 0)
		{
			currentFaceID = faceID;
		}

		if (selectionMode == ADD_FACE)
		{
			if (faceID != 0)
			{
				//model->AddSelectedFace(faceID - 1);
				model->AddSelectedFaceMulti(faceID - 1, brushSize, 0);
			}
		}
		else if (selectionMode == DEL_FACE)
		{
			if (faceID != 0)
			{
				//model->DeleteSelectedFace(faceID - 1);
				model->DeleteSelectedFaceMultiple(faceID - 1, brushSize, 0);
			}
		}
		else if (selectionMode == SELECT_POINT)
		{
			currentMouseX = x;
			currentMouseY = y;
			updateChosenVertex = true;
		}
		else if (selectionMode == UNIVERSAL_TEX) {
			if (faceID != 0)
			{
				//model->DeleteSelectedFace(faceID - 1);
				model->AddSelectedFaceCCMulti(faceID - 1, brushSize, 0);
			}
		}
	}

	void updateAllVariable()
	{
		if (model != NULL && model->currentlySelectedComponent != -1) {
			updaterDataNow(model->currentlySelectedComponent);
		}
	}

	void univTexLoading(std::string name) {
		int t = name.size();
		if (t < 3 || name[t - 3] != 'c' || name[t - 2] != 't' || name[t - 1] != 'm') {
			std::cout << "Wrong format for universal texture loading! Stopping\n";
		}
		std::string trueName = name;
		std::fstream f;
		f.open(trueName, std::ios::in);

		if (!f) {
			std::cout << "Failed Loading Universal Texture: " << trueName << "file doesn't exist\n";
			return;
		}
		model->currentlySelectedComponent = -1;
		resetCCDataGui();
		std::string in;
		int idxEnum = 0;
		while (f >> in) {
			//INDEX CHECKING
			if (in == "PARTS") {
				int inInt;
				f >> inInt;
				if (inInt != idxEnum) {
					std::cout << "Fault in loading universal texture: index PARTS " << idxEnum << " is missing!\n";
				}
			}
			//WEIGHT MODE
			else if (in == "WEIGHTMODE") {
				f >> in;
				if (in == "MEAN") {
					dataNow.currentUVWeightMode[idxEnum] = MEAN;
				}
				else if (in == "HARMONIC") {
					dataNow.currentUVWeightMode[idxEnum] = HARMONIC;
				}
				else {
					dataNow.currentUVWeightMode[idxEnum] = UNIFORM;
				}
			}
			//BORDERMODE
			else if (in == "BORDERMODE") {
				f >> in;
				if (in == "DISK") {
					dataNow.currentUVBorderMode[idxEnum] = DISK;
				}
				else {
					dataNow.currentUVBorderMode[idxEnum] = QUAD;
				}
				if (model != NULL) {
					model->LoadTBOBodyParts(idxEnum, dataNow.currentUVBorderMode[idxEnum], dataNow.currentUVWeightMode[idxEnum]);
				}
			}
			//UV OFFSET
			else if (in == "UVOFFSET") {
				float x;
				float y;
				f >> x;
				f >> y;
				glm::vec2 resOffset(x, y);
				dataNow.UVOffset[idxEnum] = resOffset;
			}
			//UVRotate
			else if (in == "UVROTATE") {
				float rot;
				f >> rot;
				dataNow.UVRotate[idxEnum] = rot;
			}
			//UV Scale
			else if (in == "UVSCALE") {
				float scaler;
				f >> scaler;
				dataNow.UVScale[idxEnum] = scaler;
			}
			//TextureName
			else if (in == "CCTEXTUREIDMAPPING") {
				f >> in;
				if (in == "NULL") {
					dataNow.ccTextureIDMapping[idxEnum] = "";
				}
				else {
					//check texture duplicity
					std::string finalTexName = cleanPathNameTex(in);
					if (addIndividualTexture(finalTexName, idxEnum)) {
						//update name
						dataNow.ccTextureIDMapping[idxEnum] = finalTexName;
						dataNow.textureAttached[idxEnum] = true;
					}
					else {
						std::cout << "Tex " << finalTexName << " for universal texture " << trueName << " is not found in " << ResourcePath::imagePath << "\n";
						exit(EXIT_FAILURE);
					}

				}
			}
			else if (in == "END") {
				idxEnum++;
			}
			else if (in == "FULLEND") {
				std::cout << "EOF of LOADING TEXTURE IS REACHED!\n";
				if (idxEnum != BODYPARTSAMOUNT) {
					std::cout << "Fault in loading universal texture: index enumeration not match by EOF!\n";
				}
				break;
			}
			else {
				std::cout << "UNRECOGNIZED COMMAND WHILE LOADING UNIVERSAL TEXTURE: " << in << "\n";
			}
		}
		f.close();
	}

	SelectionMode prevSelectionMode;
	SelectionMode selectionMode;
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		void resetMyButton()
		{
			ccSelectedVal->Text = "-1";
			ccShowTexToggle->Checked = false;
			ccWeightBox->SelectedIndex = MEAN;
			ccBorderBox->SelectedIndex = DISK;
			ccUVOffsetX->Value = 0;
			ccUVOffsetY->Value = 0;
			ccUVRotateBar->Value = 0;
			ccUVScaleBar->Value = 1;
			// ccTextureName->Text = ccTextureIDMapping;
		}

		void setMyButton(int curNum, UpdateData dataNow)
		{
			ccSelectedVal->Text = curNum.ToString();
			ccShowTexToggle->Checked = dataNow.textureAttached[curNum];
			ccWeightBox->SelectedIndex = dataNow.currentUVWeightMode[curNum];
			ccBorderBox->SelectedIndex = dataNow.currentUVBorderMode[curNum];
			ccUVOffsetX->Value = dataNow.UVOffset[curNum][0] * 100;
			ccUVOffsetY->Value = dataNow.UVOffset[curNum][1] * 100;
			ccUVRotateBar->Value = dataNow.UVRotate[curNum];
			ccUVScaleBar->Value = dataNow.UVScale[curNum] * 100;
		}
		Form1(void)
		{
			prevSelectionMode = ADD_FACE;
			selectionMode = ADD_FACE;

			InitializeComponent();
			//
			//TODO: Konstruktorcode hier hinzuf�gen.
			//
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	protected:




	private: HKOGLPanel::HKOGLPanelControl^ hkoglPanelControl1;










	private: System::Windows::Forms::Timer^ timer1;



















	private: System::Windows::Forms::Label^ brushLabel;








	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::Panel^ panel2;
	private: System::Windows::Forms::Label^ ccTextureLabel;
	private: System::Windows::Forms::TextBox^ ccSelectedVal;
	private: System::Windows::Forms::Label^ ccSelectedLabel;
	private: System::Windows::Forms::ComboBox^ ccBorderBox;

	private: System::Windows::Forms::ComboBox^ ccWeightBox;

	private: System::Windows::Forms::Label^ uvEditorLabel;
	private: System::Windows::Forms::Label^ ccUVScaleLabel;
	private: System::Windows::Forms::Label^ ccUVRotateLabel;
	private: System::Windows::Forms::Label^ ccUVOffsetYLabel;
	private: System::Windows::Forms::Label^ ccUVOffsetXLabel;


	private: System::Windows::Forms::TrackBar^ ccUVOffsetY;
	private: System::Windows::Forms::TrackBar^ ccUVOffsetX;
	private: System::Windows::Forms::CheckBox^ ccShowTexToggle;


	private: System::Windows::Forms::Button^ ccAddTexBtn;
	private: System::Windows::Forms::Label^ paramSettingsLabel;



	private: System::Windows::Forms::NumericUpDown^ numericUpDown1;


	private: System::Windows::Forms::TrackBar^ ccUVRotateBar;
	private: System::Windows::Forms::TrackBar^ ccUVScaleBar;

private: System::Windows::Forms::ColorDialog^ colorDialog1;
private: System::Windows::Forms::SaveFileDialog^ saveFileDialog1;
private: System::Windows::Forms::Button^ ccSaveTexBtn;
private: System::Windows::Forms::Button^ ccLoadTexBtn;
private: System::Windows::Forms::OpenFileDialog^ openFileDialog2;
private: System::Windows::Forms::OpenFileDialog^ openFileDialog3;

private: System::Windows::Forms::Label^ label1;
private: System::Windows::Forms::Button^ unselectComponentBtn;
private: System::Windows::Forms::Button^ cleanCanvasBtn;

private: System::Windows::Forms::ComboBox^ selectionBox;
private: System::Windows::Forms::CheckBox^ showOutlineToggle;
private: System::Windows::Forms::CheckBox^ showTexToggle;
private: System::Windows::Forms::Button^ paramBtn;
private: System::Windows::Forms::Button^ checkBorderBtn;
private: System::Windows::Forms::Panel^ panel3;
private: System::Windows::Forms::Button^ clearFaceBtn;
private: System::Windows::Forms::TrackBar^ uvScaleBar;


private: System::Windows::Forms::TrackBar^ uvRotateBar;


private: System::Windows::Forms::Label^ label8;
private: System::Windows::Forms::Label^ label3;
private: System::Windows::Forms::Label^ label4;
private: System::Windows::Forms::Label^ label5;
private: System::Windows::Forms::Label^ label6;
private: System::Windows::Forms::Label^ label7;
private: System::Windows::Forms::TrackBar^ uvOffsetYBar;
private: System::Windows::Forms::TrackBar^ uvOffsetXBar;



private: System::Windows::Forms::Label^ label2;
private: System::Windows::Forms::ComboBox^ weightBox;
private: System::Windows::Forms::ComboBox^ borderBox;
private: System::Windows::Forms::Button^ addTextureBtn;
private: System::Windows::Forms::OpenFileDialog^ openFileDialog1;
private: System::ComponentModel::BackgroundWorker^ backgroundWorker1;








	private: System::ComponentModel::IContainer^ components;

	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode f�r die Designerunterst�tzung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor ge�ndert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			HKOGLPanel::HKCOGLPanelCameraSetting^ hkcoglPanelCameraSetting1 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
			HKOGLPanel::HKCOGLPanelPixelFormat^ hkcoglPanelPixelFormat1 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
			this->hkoglPanelControl1 = (gcnew HKOGLPanel::HKOGLPanelControl());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->brushLabel = (gcnew System::Windows::Forms::Label());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->unselectComponentBtn = (gcnew System::Windows::Forms::Button());
			this->ccLoadTexBtn = (gcnew System::Windows::Forms::Button());
			this->ccSaveTexBtn = (gcnew System::Windows::Forms::Button());
			this->ccUVScaleBar = (gcnew System::Windows::Forms::TrackBar());
			this->ccUVRotateBar = (gcnew System::Windows::Forms::TrackBar());
			this->paramSettingsLabel = (gcnew System::Windows::Forms::Label());
			this->cleanCanvasBtn = (gcnew System::Windows::Forms::Button());
			this->ccBorderBox = (gcnew System::Windows::Forms::ComboBox());
			this->ccWeightBox = (gcnew System::Windows::Forms::ComboBox());
			this->uvEditorLabel = (gcnew System::Windows::Forms::Label());
			this->ccUVScaleLabel = (gcnew System::Windows::Forms::Label());
			this->ccUVRotateLabel = (gcnew System::Windows::Forms::Label());
			this->ccUVOffsetYLabel = (gcnew System::Windows::Forms::Label());
			this->ccUVOffsetXLabel = (gcnew System::Windows::Forms::Label());
			this->ccUVOffsetY = (gcnew System::Windows::Forms::TrackBar());
			this->ccUVOffsetX = (gcnew System::Windows::Forms::TrackBar());
			this->ccShowTexToggle = (gcnew System::Windows::Forms::CheckBox());
			this->ccAddTexBtn = (gcnew System::Windows::Forms::Button());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->ccTextureLabel = (gcnew System::Windows::Forms::Label());
			this->ccSelectedVal = (gcnew System::Windows::Forms::TextBox());
			this->ccSelectedLabel = (gcnew System::Windows::Forms::Label());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->addTextureBtn = (gcnew System::Windows::Forms::Button());
			this->clearFaceBtn = (gcnew System::Windows::Forms::Button());
			this->checkBorderBtn = (gcnew System::Windows::Forms::Button());
			this->paramBtn = (gcnew System::Windows::Forms::Button());
			this->uvScaleBar = (gcnew System::Windows::Forms::TrackBar());
			this->showOutlineToggle = (gcnew System::Windows::Forms::CheckBox());
			this->showTexToggle = (gcnew System::Windows::Forms::CheckBox());
			this->weightBox = (gcnew System::Windows::Forms::ComboBox());
			this->uvRotateBar = (gcnew System::Windows::Forms::TrackBar());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->uvOffsetYBar = (gcnew System::Windows::Forms::TrackBar());
			this->uvOffsetXBar = (gcnew System::Windows::Forms::TrackBar());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->borderBox = (gcnew System::Windows::Forms::ComboBox());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->colorDialog1 = (gcnew System::Windows::Forms::ColorDialog());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->openFileDialog2 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->openFileDialog3 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->selectionBox = (gcnew System::Windows::Forms::ComboBox());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ccUVScaleBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ccUVRotateBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ccUVOffsetY))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ccUVOffsetX))->BeginInit();
			this->panel3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->uvScaleBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->uvRotateBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->uvOffsetYBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->uvOffsetXBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->BeginInit();
			this->SuspendLayout();
			// 
			// hkoglPanelControl1
			// 
			hkcoglPanelCameraSetting1->Far = 1000;
			hkcoglPanelCameraSetting1->Fov = 45;
			hkcoglPanelCameraSetting1->Near = -1000;
			hkcoglPanelCameraSetting1->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
			this->hkoglPanelControl1->Camera_Setting = hkcoglPanelCameraSetting1;
			this->hkoglPanelControl1->Location = System::Drawing::Point(1, 0);
			this->hkoglPanelControl1->Name = L"hkoglPanelControl1";
			hkcoglPanelPixelFormat1->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat1->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat1->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			this->hkoglPanelControl1->Pixel_Format = hkcoglPanelPixelFormat1;
			this->hkoglPanelControl1->Size = System::Drawing::Size(833, 872);
			this->hkoglPanelControl1->TabIndex = 0;
			this->hkoglPanelControl1->Load += gcnew System::EventHandler(this, &Form1::hkoglPanelControl1_Load);
			this->hkoglPanelControl1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::hkoglPanelControl1_Paint);
			this->hkoglPanelControl1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::hkoglPanelControl1_MouseDown);
			this->hkoglPanelControl1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::hkoglPanelControl1_MouseMove);
			this->hkoglPanelControl1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::hkoglPanelControl1_MouseUp);
			this->hkoglPanelControl1->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::hkoglPanelControl1_MouseWheel);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// brushLabel
			// 
			this->brushLabel->AutoSize = true;
			this->brushLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->brushLabel->Location = System::Drawing::Point(220, 881);
			this->brushLabel->Name = L"brushLabel";
			this->brushLabel->Size = System::Drawing::Size(113, 25);
			this->brushLabel->TabIndex = 20;
			this->brushLabel->Text = L"Brush Size:";
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->unselectComponentBtn);
			this->panel1->Controls->Add(this->ccLoadTexBtn);
			this->panel1->Controls->Add(this->ccSaveTexBtn);
			this->panel1->Controls->Add(this->ccUVScaleBar);
			this->panel1->Controls->Add(this->ccUVRotateBar);
			this->panel1->Controls->Add(this->paramSettingsLabel);
			this->panel1->Controls->Add(this->cleanCanvasBtn);
			this->panel1->Controls->Add(this->ccBorderBox);
			this->panel1->Controls->Add(this->ccWeightBox);
			this->panel1->Controls->Add(this->uvEditorLabel);
			this->panel1->Controls->Add(this->ccUVScaleLabel);
			this->panel1->Controls->Add(this->ccUVRotateLabel);
			this->panel1->Controls->Add(this->ccUVOffsetYLabel);
			this->panel1->Controls->Add(this->ccUVOffsetXLabel);
			this->panel1->Controls->Add(this->ccUVOffsetY);
			this->panel1->Controls->Add(this->ccUVOffsetX);
			this->panel1->Controls->Add(this->ccShowTexToggle);
			this->panel1->Controls->Add(this->ccAddTexBtn);
			this->panel1->Controls->Add(this->panel2);
			this->panel1->Controls->Add(this->ccTextureLabel);
			this->panel1->Controls->Add(this->ccSelectedVal);
			this->panel1->Controls->Add(this->ccSelectedLabel);
			this->panel1->Location = System::Drawing::Point(849, 12);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(423, 868);
			this->panel1->TabIndex = 26;
			this->panel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::panel1_Paint);
			// 
			// unselectComponentBtn
			// 
			this->unselectComponentBtn->Location = System::Drawing::Point(296, 62);
			this->unselectComponentBtn->Name = L"unselectComponentBtn";
			this->unselectComponentBtn->Size = System::Drawing::Size(100, 33);
			this->unselectComponentBtn->TabIndex = 46;
			this->unselectComponentBtn->Text = L"Unselect";
			this->unselectComponentBtn->UseVisualStyleBackColor = true;
			this->unselectComponentBtn->Click += gcnew System::EventHandler(this, &Form1::unselectComponentBtn_Click);
			// 
			// ccLoadTexBtn
			// 
			this->ccLoadTexBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->ccLoadTexBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccLoadTexBtn->Location = System::Drawing::Point(228, 360);
			this->ccLoadTexBtn->Name = L"ccLoadTexBtn";
			this->ccLoadTexBtn->Size = System::Drawing::Size(179, 57);
			this->ccLoadTexBtn->TabIndex = 46;
			this->ccLoadTexBtn->Text = L"Load Texture";
			this->ccLoadTexBtn->UseVisualStyleBackColor = false;
			this->ccLoadTexBtn->Click += gcnew System::EventHandler(this, &Form1::ccLoadTexBtn_Click);
			// 
			// ccSaveTexBtn
			// 
			this->ccSaveTexBtn->BackColor = System::Drawing::Color::Lime;
			this->ccSaveTexBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccSaveTexBtn->Location = System::Drawing::Point(21, 360);
			this->ccSaveTexBtn->Name = L"ccSaveTexBtn";
			this->ccSaveTexBtn->Size = System::Drawing::Size(201, 57);
			this->ccSaveTexBtn->TabIndex = 45;
			this->ccSaveTexBtn->Text = L"Save Texture";
			this->ccSaveTexBtn->UseVisualStyleBackColor = false;
			this->ccSaveTexBtn->Click += gcnew System::EventHandler(this, &Form1::ccSaveTexBtn_Click);
			// 
			// ccUVScaleBar
			// 
			this->ccUVScaleBar->Location = System::Drawing::Point(208, 685);
			this->ccUVScaleBar->Maximum = 100;
			this->ccUVScaleBar->Name = L"ccUVScaleBar";
			this->ccUVScaleBar->Size = System::Drawing::Size(159, 69);
			this->ccUVScaleBar->TabIndex = 44;
			this->ccUVScaleBar->Scroll += gcnew System::EventHandler(this, &Form1::ccUVScaleBar_Scroll);
			// 
			// ccUVRotateBar
			// 
			this->ccUVRotateBar->Location = System::Drawing::Point(208, 610);
			this->ccUVRotateBar->Maximum = 360;
			this->ccUVRotateBar->Name = L"ccUVRotateBar";
			this->ccUVRotateBar->Size = System::Drawing::Size(165, 69);
			this->ccUVRotateBar->TabIndex = 43;
			this->ccUVRotateBar->Scroll += gcnew System::EventHandler(this, &Form1::ccUVRotateBar_Scroll);
			// 
			// paramSettingsLabel
			// 
			this->paramSettingsLabel->AutoSize = true;
			this->paramSettingsLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->paramSettingsLabel->Location = System::Drawing::Point(40, 128);
			this->paramSettingsLabel->Name = L"paramSettingsLabel";
			this->paramSettingsLabel->Size = System::Drawing::Size(216, 36);
			this->paramSettingsLabel->TabIndex = 40;
			this->paramSettingsLabel->Text = L"Param Settings";
			// 
			// cleanCanvasBtn
			// 
			this->cleanCanvasBtn->BackColor = System::Drawing::Color::Red;
			this->cleanCanvasBtn->ForeColor = System::Drawing::Color::White;
			this->cleanCanvasBtn->Location = System::Drawing::Point(247, 797);
			this->cleanCanvasBtn->Name = L"cleanCanvasBtn";
			this->cleanCanvasBtn->Size = System::Drawing::Size(160, 50);
			this->cleanCanvasBtn->TabIndex = 46;
			this->cleanCanvasBtn->Text = L"Clean Canvas";
			this->cleanCanvasBtn->UseVisualStyleBackColor = false;
			this->cleanCanvasBtn->Click += gcnew System::EventHandler(this, &Form1::cleanCanvasBtn_Click);
			// 
			// ccBorderBox
			// 
			this->ccBorderBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccBorderBox->FormattingEnabled = true;
			this->ccBorderBox->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"QUAD Mode", L"DISK Mode" });
			this->ccBorderBox->Location = System::Drawing::Point(219, 184);
			this->ccBorderBox->Name = L"ccBorderBox";
			this->ccBorderBox->Size = System::Drawing::Size(188, 33);
			this->ccBorderBox->TabIndex = 39;
			this->ccBorderBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::ccBorderBox_SelectedIndexChanged);
			// 
			// ccWeightBox
			// 
			this->ccWeightBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccWeightBox->FormattingEnabled = true;
			this->ccWeightBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Uniform Map", L"Harmonic Map", L"Mean Value Map" });
			this->ccWeightBox->Location = System::Drawing::Point(17, 184);
			this->ccWeightBox->Name = L"ccWeightBox";
			this->ccWeightBox->Size = System::Drawing::Size(188, 33);
			this->ccWeightBox->TabIndex = 38;
			this->ccWeightBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::ccWeightBox_SelectedIndexChanged);
			// 
			// uvEditorLabel
			// 
			this->uvEditorLabel->AutoSize = true;
			this->uvEditorLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->uvEditorLabel->Location = System::Drawing::Point(40, 459);
			this->uvEditorLabel->Name = L"uvEditorLabel";
			this->uvEditorLabel->Size = System::Drawing::Size(145, 36);
			this->uvEditorLabel->TabIndex = 37;
			this->uvEditorLabel->Text = L"UV Editor";
			// 
			// ccUVScaleLabel
			// 
			this->ccUVScaleLabel->AutoSize = true;
			this->ccUVScaleLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccUVScaleLabel->Location = System::Drawing::Point(47, 702);
			this->ccUVScaleLabel->Name = L"ccUVScaleLabel";
			this->ccUVScaleLabel->Size = System::Drawing::Size(95, 25);
			this->ccUVScaleLabel->TabIndex = 36;
			this->ccUVScaleLabel->Text = L"UV Scale";
			// 
			// ccUVRotateLabel
			// 
			this->ccUVRotateLabel->AutoSize = true;
			this->ccUVRotateLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccUVRotateLabel->Location = System::Drawing::Point(41, 623);
			this->ccUVRotateLabel->Name = L"ccUVRotateLabel";
			this->ccUVRotateLabel->Size = System::Drawing::Size(101, 25);
			this->ccUVRotateLabel->TabIndex = 35;
			this->ccUVRotateLabel->Text = L"UV Rotate";
			// 
			// ccUVOffsetYLabel
			// 
			this->ccUVOffsetYLabel->AutoSize = true;
			this->ccUVOffsetYLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccUVOffsetYLabel->Location = System::Drawing::Point(239, 508);
			this->ccUVOffsetYLabel->Name = L"ccUVOffsetYLabel";
			this->ccUVOffsetYLabel->Size = System::Drawing::Size(115, 25);
			this->ccUVOffsetYLabel->TabIndex = 34;
			this->ccUVOffsetYLabel->Text = L"UV Offset Y";
			// 
			// ccUVOffsetXLabel
			// 
			this->ccUVOffsetXLabel->AutoSize = true;
			this->ccUVOffsetXLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccUVOffsetXLabel->Location = System::Drawing::Point(41, 508);
			this->ccUVOffsetXLabel->Name = L"ccUVOffsetXLabel";
			this->ccUVOffsetXLabel->Size = System::Drawing::Size(116, 25);
			this->ccUVOffsetXLabel->TabIndex = 33;
			this->ccUVOffsetXLabel->Text = L"UV Offset X";
			// 
			// ccUVOffsetY
			// 
			this->ccUVOffsetY->Location = System::Drawing::Point(244, 536);
			this->ccUVOffsetY->Maximum = 100;
			this->ccUVOffsetY->Minimum = -100;
			this->ccUVOffsetY->Name = L"ccUVOffsetY";
			this->ccUVOffsetY->Size = System::Drawing::Size(152, 69);
			this->ccUVOffsetY->TabIndex = 30;
			this->ccUVOffsetY->Scroll += gcnew System::EventHandler(this, &Form1::ccUVOffsetY_Scroll);
			// 
			// ccUVOffsetX
			// 
			this->ccUVOffsetX->Location = System::Drawing::Point(53, 536);
			this->ccUVOffsetX->Maximum = 100;
			this->ccUVOffsetX->Minimum = -100;
			this->ccUVOffsetX->Name = L"ccUVOffsetX";
			this->ccUVOffsetX->Size = System::Drawing::Size(152, 69);
			this->ccUVOffsetX->TabIndex = 29;
			this->ccUVOffsetX->Scroll += gcnew System::EventHandler(this, &Form1::ccUVOffsetX_Scroll);
			// 
			// ccShowTexToggle
			// 
			this->ccShowTexToggle->AutoSize = true;
			this->ccShowTexToggle->Checked = true;
			this->ccShowTexToggle->CheckState = System::Windows::Forms::CheckState::Checked;
			this->ccShowTexToggle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccShowTexToggle->Location = System::Drawing::Point(247, 254);
			this->ccShowTexToggle->Name = L"ccShowTexToggle";
			this->ccShowTexToggle->Size = System::Drawing::Size(160, 29);
			this->ccShowTexToggle->TabIndex = 27;
			this->ccShowTexToggle->Text = L"Show Texture";
			this->ccShowTexToggle->UseVisualStyleBackColor = true;
			this->ccShowTexToggle->CheckedChanged += gcnew System::EventHandler(this, &Form1::ccShowTexToggle_CheckedChanged);
			// 
			// ccAddTexBtn
			// 
			this->ccAddTexBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccAddTexBtn->Location = System::Drawing::Point(21, 297);
			this->ccAddTexBtn->Name = L"ccAddTexBtn";
			this->ccAddTexBtn->Size = System::Drawing::Size(386, 57);
			this->ccAddTexBtn->TabIndex = 27;
			this->ccAddTexBtn->Text = L"Add Texture";
			this->ccAddTexBtn->UseVisualStyleBackColor = true;
			this->ccAddTexBtn->Click += gcnew System::EventHandler(this, &Form1::ccAddTexBtn_Click);
			// 
			// panel2
			// 
			this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel2->Location = System::Drawing::Point(17, 101);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(390, 11);
			this->panel2->TabIndex = 3;
			// 
			// ccTextureLabel
			// 
			this->ccTextureLabel->AutoSize = true;
			this->ccTextureLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccTextureLabel->Location = System::Drawing::Point(40, 254);
			this->ccTextureLabel->Name = L"ccTextureLabel";
			this->ccTextureLabel->Size = System::Drawing::Size(165, 36);
			this->ccTextureLabel->TabIndex = 2;
			this->ccTextureLabel->Text = L"CC Texture";
			// 
			// ccSelectedVal
			// 
			this->ccSelectedVal->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccSelectedVal->Location = System::Drawing::Point(296, 19);
			this->ccSelectedVal->Name = L"ccSelectedVal";
			this->ccSelectedVal->Size = System::Drawing::Size(100, 41);
			this->ccSelectedVal->TabIndex = 1;
			this->ccSelectedVal->TextChanged += gcnew System::EventHandler(this, &Form1::ccSelectedVal_TextChanged);
			// 
			// ccSelectedLabel
			// 
			this->ccSelectedLabel->AutoSize = true;
			this->ccSelectedLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ccSelectedLabel->Location = System::Drawing::Point(15, 24);
			this->ccSelectedLabel->Name = L"ccSelectedLabel";
			this->ccSelectedLabel->Size = System::Drawing::Size(180, 36);
			this->ccSelectedLabel->TabIndex = 0;
			this->ccSelectedLabel->Text = L"Selected CC";
			// 
			// panel3
			// 
			this->panel3->Controls->Add(this->addTextureBtn);
			this->panel3->Controls->Add(this->clearFaceBtn);
			this->panel3->Controls->Add(this->checkBorderBtn);
			this->panel3->Controls->Add(this->paramBtn);
			this->panel3->Controls->Add(this->uvScaleBar);
			this->panel3->Controls->Add(this->showOutlineToggle);
			this->panel3->Controls->Add(this->showTexToggle);
			this->panel3->Controls->Add(this->weightBox);
			this->panel3->Controls->Add(this->uvRotateBar);
			this->panel3->Controls->Add(this->label8);
			this->panel3->Controls->Add(this->label3);
			this->panel3->Controls->Add(this->label4);
			this->panel3->Controls->Add(this->label5);
			this->panel3->Controls->Add(this->label6);
			this->panel3->Controls->Add(this->label7);
			this->panel3->Controls->Add(this->uvOffsetYBar);
			this->panel3->Controls->Add(this->uvOffsetXBar);
			this->panel3->Controls->Add(this->label2);
			this->panel3->Controls->Add(this->borderBox);
			this->panel3->Location = System::Drawing::Point(846, 12);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(426, 860);
			this->panel3->TabIndex = 54;
			// 
			// addTextureBtn
			// 
			this->addTextureBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->addTextureBtn->Location = System::Drawing::Point(23, 384);
			this->addTextureBtn->Name = L"addTextureBtn";
			this->addTextureBtn->Size = System::Drawing::Size(386, 57);
			this->addTextureBtn->TabIndex = 56;
			this->addTextureBtn->Text = L"Add Texture";
			this->addTextureBtn->UseVisualStyleBackColor = true;
			this->addTextureBtn->Click += gcnew System::EventHandler(this, &Form1::addTextureBtn_Click_1);
			// 
			// clearFaceBtn
			// 
			this->clearFaceBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(128)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->clearFaceBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->clearFaceBtn->ForeColor = System::Drawing::Color::White;
			this->clearFaceBtn->Location = System::Drawing::Point(33, 221);
			this->clearFaceBtn->Name = L"clearFaceBtn";
			this->clearFaceBtn->Size = System::Drawing::Size(186, 55);
			this->clearFaceBtn->TabIndex = 55;
			this->clearFaceBtn->Text = L"Clear Faces";
			this->clearFaceBtn->UseVisualStyleBackColor = false;
			this->clearFaceBtn->Click += gcnew System::EventHandler(this, &Form1::clearFaceBtn_Click);
			// 
			// checkBorderBtn
			// 
			this->checkBorderBtn->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->checkBorderBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->checkBorderBtn->Location = System::Drawing::Point(33, 160);
			this->checkBorderBtn->Name = L"checkBorderBtn";
			this->checkBorderBtn->Size = System::Drawing::Size(180, 55);
			this->checkBorderBtn->TabIndex = 52;
			this->checkBorderBtn->Text = L"Check Border";
			this->checkBorderBtn->UseVisualStyleBackColor = false;
			this->checkBorderBtn->Click += gcnew System::EventHandler(this, &Form1::checkBorderBtn_Click_1);
			// 
			// paramBtn
			// 
			this->paramBtn->BackColor = System::Drawing::Color::Lime;
			this->paramBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->paramBtn->ForeColor = System::Drawing::Color::Black;
			this->paramBtn->Location = System::Drawing::Point(227, 161);
			this->paramBtn->Name = L"paramBtn";
			this->paramBtn->Size = System::Drawing::Size(186, 115);
			this->paramBtn->TabIndex = 51;
			this->paramBtn->Text = L"Parameterize!";
			this->paramBtn->UseVisualStyleBackColor = false;
			this->paramBtn->Click += gcnew System::EventHandler(this, &Form1::paramBtn_Click_1);
			// 
			// uvScaleBar
			// 
			this->uvScaleBar->Location = System::Drawing::Point(198, 734);
			this->uvScaleBar->Maximum = 100;
			this->uvScaleBar->Name = L"uvScaleBar";
			this->uvScaleBar->Size = System::Drawing::Size(159, 69);
			this->uvScaleBar->TabIndex = 53;
			this->uvScaleBar->Value = 100;
			this->uvScaleBar->Scroll += gcnew System::EventHandler(this, &Form1::uvScaleBar_Scroll);
			// 
			// showOutlineToggle
			// 
			this->showOutlineToggle->AutoSize = true;
			this->showOutlineToggle->Checked = true;
			this->showOutlineToggle->CheckState = System::Windows::Forms::CheckState::Checked;
			this->showOutlineToggle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->showOutlineToggle->Location = System::Drawing::Point(247, 63);
			this->showOutlineToggle->Name = L"showOutlineToggle";
			this->showOutlineToggle->Size = System::Drawing::Size(155, 29);
			this->showOutlineToggle->TabIndex = 49;
			this->showOutlineToggle->Text = L"Show Outline";
			this->showOutlineToggle->UseVisualStyleBackColor = true;
			this->showOutlineToggle->CheckedChanged += gcnew System::EventHandler(this, &Form1::showOutlineToggle_CheckedChanged);
			// 
			// showTexToggle
			// 
			this->showTexToggle->AutoSize = true;
			this->showTexToggle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->showTexToggle->Location = System::Drawing::Point(247, 344);
			this->showTexToggle->Name = L"showTexToggle";
			this->showTexToggle->Size = System::Drawing::Size(160, 29);
			this->showTexToggle->TabIndex = 50;
			this->showTexToggle->Text = L"Show Texture";
			this->showTexToggle->UseVisualStyleBackColor = true;
			this->showTexToggle->CheckedChanged += gcnew System::EventHandler(this, &Form1::showTexToggle_CheckedChanged);
			// 
			// weightBox
			// 
			this->weightBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->weightBox->FormattingEnabled = true;
			this->weightBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Uniform Map", L"Harmonic Map", L"Mean Value Map" });
			this->weightBox->Location = System::Drawing::Point(33, 104);
			this->weightBox->Name = L"weightBox";
			this->weightBox->Size = System::Drawing::Size(186, 33);
			this->weightBox->TabIndex = 4;
			this->weightBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::weightBox_SelectedIndexChanged_1);
			// 
			// uvRotateBar
			// 
			this->uvRotateBar->Location = System::Drawing::Point(198, 659);
			this->uvRotateBar->Maximum = 360;
			this->uvRotateBar->Name = L"uvRotateBar";
			this->uvRotateBar->Size = System::Drawing::Size(165, 69);
			this->uvRotateBar->TabIndex = 52;
			this->uvRotateBar->Scroll += gcnew System::EventHandler(this, &Form1::uvRotateBar_Scroll);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(30, 336);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(115, 36);
			this->label8->TabIndex = 47;
			this->label8->Text = L"Texture";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(30, 508);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(145, 36);
			this->label3->TabIndex = 51;
			this->label3->Text = L"UV Editor";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(48, 746);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(95, 25);
			this->label4->TabIndex = 50;
			this->label4->Text = L"UV Scale";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(42, 667);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(101, 25);
			this->label5->TabIndex = 49;
			this->label5->Text = L"UV Rotate";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(230, 556);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(115, 25);
			this->label6->TabIndex = 48;
			this->label6->Text = L"UV Offset Y";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label7->Location = System::Drawing::Point(31, 557);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(116, 25);
			this->label7->TabIndex = 47;
			this->label7->Text = L"UV Offset X";
			// 
			// uvOffsetYBar
			// 
			this->uvOffsetYBar->Location = System::Drawing::Point(230, 584);
			this->uvOffsetYBar->Maximum = 100;
			this->uvOffsetYBar->Minimum = -100;
			this->uvOffsetYBar->Name = L"uvOffsetYBar";
			this->uvOffsetYBar->Size = System::Drawing::Size(152, 69);
			this->uvOffsetYBar->TabIndex = 46;
			this->uvOffsetYBar->Scroll += gcnew System::EventHandler(this, &Form1::uvOffsetYBar_Scroll);
			// 
			// uvOffsetXBar
			// 
			this->uvOffsetXBar->Location = System::Drawing::Point(47, 585);
			this->uvOffsetXBar->Maximum = 100;
			this->uvOffsetXBar->Minimum = -100;
			this->uvOffsetXBar->Name = L"uvOffsetXBar";
			this->uvOffsetXBar->Size = System::Drawing::Size(158, 69);
			this->uvOffsetXBar->TabIndex = 45;
			this->uvOffsetXBar->Scroll += gcnew System::EventHandler(this, &Form1::uvOffsetXBar_Scroll);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(18, 55);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(216, 36);
			this->label2->TabIndex = 43;
			this->label2->Text = L"Param Settings";
			// 
			// borderBox
			// 
			this->borderBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->borderBox->FormattingEnabled = true;
			this->borderBox->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"QUAD Mode", L"DISK Mode" });
			this->borderBox->Location = System::Drawing::Point(225, 104);
			this->borderBox->Name = L"borderBox";
			this->borderBox->Size = System::Drawing::Size(186, 33);
			this->borderBox->TabIndex = 5;
			this->borderBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::borderBox_SelectedIndexChanged);
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->numericUpDown1->Location = System::Drawing::Point(348, 878);
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(120, 30);
			this->numericUpDown1->TabIndex = 44;
			this->numericUpDown1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numericUpDown1->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown1_ValueChanged);
			// 
			// saveFileDialog1
			// 
			this->saveFileDialog1->InitialDirectory = L"../Proj3Assets/FullTex/";
			this->saveFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::saveFileDialog1_FileOk);
			// 
			// openFileDialog2
			// 
			this->openFileDialog2->FileName = L"openFileDialog2";
			this->openFileDialog2->InitialDirectory = L"../Proj3Assets/FullTex/";
			this->openFileDialog2->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::openFileDialog2_FileOk);
			// 
			// openFileDialog3
			// 
			this->openFileDialog3->FileName = L"openFileDialog3";
			this->openFileDialog3->InitialDirectory = L"../Proj3Assets/Textures/";
			this->openFileDialog3->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::openFileDialog3_FileOk);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Blue;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::White;
			this->label1->Location = System::Drawing::Point(907, 959);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(365, 25);
			this->label1->TabIndex = 45;
			this->label1->Text = L"Place all necessary files in ../Proj3Assets";
			this->label1->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// selectionBox
			// 
			this->selectionBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->selectionBox->FormattingEnabled = true;
			this->selectionBox->Items->AddRange(gcnew cli::array< System::Object^  >(4) {
				L"Add Face", L"Delete Face", L"Select Point",
					L"Universal Texture"
			});
			this->selectionBox->Location = System::Drawing::Point(12, 878);
			this->selectionBox->Name = L"selectionBox";
			this->selectionBox->Size = System::Drawing::Size(202, 33);
			this->selectionBox->TabIndex = 48;
			this->selectionBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::selectionBox_SelectedIndexChanged);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog3";
			this->openFileDialog1->InitialDirectory = L"../Proj3Assets/Textures/";
			this->openFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::openFileDialog1_FileOk);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->ClientSize = System::Drawing::Size(1274, 1000);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->selectionBox);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->numericUpDown1);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->brushLabel);
			this->Controls->Add(this->hkoglPanelControl1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ccUVScaleBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ccUVRotateBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ccUVOffsetY))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ccUVOffsetX))->EndInit();
			this->panel3->ResumeLayout(false);
			this->panel3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->uvScaleBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->uvRotateBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->uvOffsetYBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->uvOffsetXBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e) {
		//AllocConsole();
		selectionBox->SelectedIndex = SelectionMode::ADD_FACE;
		weightBox->SelectedIndex = WeightMode::MEAN;
		borderBox->SelectedIndex = BorderMode::QUAD;
		ccWeightBox->SelectedIndex = WeightMode::MEAN;
		ccBorderBox->SelectedIndex = BorderMode::DISK;
	}
	private: System::Void hkoglPanelControl1_Load(System::Object^ sender, System::EventArgs^ e)
	{
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::cout << "GLEW 1 is not initialized!\n";
		}

		InitOpenGL();
		InitData(this->hkoglPanelControl1->Width, this->hkoglPanelControl1->Height, false);
	}
	private: System::Void hkoglPanelControl1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e)
	{
		ReshapeforMainHKO(this->hkoglPanelControl1->Width, this->hkoglPanelControl1->Height);

		//Update shaders' input variable
	///////////////////////////	

		glm::mat4 modelView = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
		glm::mat4 proj = meshWindowCam.GetProjectionMatrix(aspect);

		//redraw picking texture
		pickingTexture.Enable();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pickingShader.Enable();
		pickingShader.SetMVMat(glm::value_ptr(modelView));
		pickingShader.SetPMat(glm::value_ptr(proj));

		model->Render();

		pickingShader.Disable();
		pickingTexture.Disable();
		///////////////////////////	

		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mainShader.Enable();
		//glm::mat4 rotateTex(1.0f);

		if (selectionMode != UNIVERSAL_TEX) {
			float deltaAngle = uvRotateAngle - prevRotate;
			float deltaScaler = scaler / prevScaler;
			float deltaXOffset = offsetX - prevOffsetX;
			float deltaYOffset = offsetY - prevOffsetY;
			float radian = deltaAngle * M_PI / 180.0f;
			glm::mat4 rotateScaleTrans = glm::translate(glm::mat4(1.0), glm::vec3(deltaXOffset, deltaYOffset, 0));
			rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(0.5, 0.5, 0));
			//rotateScaleTrans = glm::rotate(rotateScaleTrans, radian, glm::vec3(0.0, 0.0, 1.0));
			rotateScaleTrans = glm::rotate(rotateScaleTrans, deltaAngle, glm::vec3(0.0, 0.0, 1.0));
			rotateScaleTrans = glm::scale(rotateScaleTrans, glm::vec3(deltaScaler, deltaScaler, 1.0f));
			rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(-0.5, -0.5, 0));
			bool texDrawn = drawTexture;
			//matrices

			mainShader.SetModelView(modelView);
			mainShader.SetProj(proj);
			mainShader.SetRotateTex(rotateScaleTrans);
			mainShader.setOffsetX(0.0);
			mainShader.setOffsetY(0.0);
			mainShader.setScalerXY(1.0);
			/*glm::mat4 temp(1.0);
			mainShader.SetModelView(temp);
			mainShader.SetProj(temp);
			mainShader.SetRotateTex(temp);*/
			//flags
			mainShader.SetLightingACK(true);
			mainShader.SetDrawTexCoordACK(false);
			mainShader.SetDrawWireACK(!texDrawn && showOutline);
			mainShader.SetDrawTextureFragACK(false);
			//color vectors
			mainShader.SetWireColor(meshWireColor);
			mainShader.SetFaceColor(meshFaceColor);
			//cout << "enter1\n";
			//mainShader.bindTexture(textureID[currentTextureName]);
			//cout << "enter2\n";
			model->Render();
			//cout << "enter3\n";
			if (!texDrawn) {
				mainShader.Disable();
				// render selected face
				if (selectionMode == SelectionMode::ADD_FACE || selectionMode == SelectionMode::DEL_FACE)
				{
					drawPickingFaceShader.Enable();
					drawPickingFaceShader.SetMVMat(glm::value_ptr(modelView));
					drawPickingFaceShader.SetPMat(glm:: value_ptr(proj));
					model->RenderSelectedFace();
					drawPickingFaceShader.Disable();
				}
				else if (selectionMode == SelectionMode::SELECT_POINT) {
					if (updateChosenVertex)
					{
						chosenVertexUpdater(this->hkoglPanelControl1->Height, modelView, proj);
					}

					//draw point
					glBindVertexArray(PVAO);
					glBindBuffer(GL_ARRAY_BUFFER, PVBO);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(0);

					choosenVertexShader.Enable();
					choosenVertexShader.SetModelView(modelView);
					choosenVertexShader.SetProj(proj);
					choosenVertexShader.SetColor(pointColor);
					choosenVertexShader.SetSize(pointSize);

					glDrawArrays(GL_POINTS, 0, 1);

					choosenVertexShader.Disable();
					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
			}
			else {
				//we need to draw separately, to prevent the texture get distorted with other kind of colors
				mainShader.SetDrawTextureFragACK(true);
				/*if (curWrapMode != texIDToWrapMapping[textureID[currentTextureName]] || texBorderColor[textureID[currentTextureName]] != texBorderWrapColor) {
					changeTextureWrapNow(textureID[currentTextureName], texIDToWrapMapping, texBorderWrapColor);
				}
				if (curMinMagMode != texIDToMinMagMapping[textureID[currentTextureName]]) {
					changeTextureMinMagNow(textureID[currentTextureName], texIDToMinMagMapping);
				}*/
				mainShader.bindTexture(individualTextureID);
				model->RenderParameterized();
			}


			/////only her
			//std::cout << "cur idx: " << idx << std::endl;
			mainShader.Enable();
			float deltaAngleLit = uvRotateAngle - prevRotate;
			float deltaScalerLit = scaler / prevScaler;
			float deltaXOffsetLit = offsetX - prevOffsetX;
			//cout << "delta x offset lit: " << deltaXOffsetLit << std::endl;
			float deltaYOffsetLit = offsetY - prevOffsetY;
			float radianLit = deltaAngleLit * M_PI / 180.0f;

			glm::mat4 rotateScaleTransLit = glm::mat4(1.0);
			//glm::mat4 rotateScaleTrans = glm::translate(glm::mat4(1.0), glm::vec3(deltaXOffset, deltaYOffset, 0));
			//rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(deltaXOffset, deltaYOffset, 0));
			//rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(5, 5, 0));
			rotateScaleTransLit = glm::translate(rotateScaleTransLit, glm::vec3(0.5, 0.5, 0));
			//rotateScaleTransLit = glm::rotate(rotateScaleTransLit, radianLit, glm::vec3(0.0, 0.0, 1.0));
			rotateScaleTransLit = glm::rotate(rotateScaleTransLit, deltaAngleLit, glm::vec3(0.0, 0.0, 1.0));
			//rotateScaleTrans = glm::scale(rotateScaleTrans, glm::vec3(1, 1, 1)); //here
			rotateScaleTransLit = glm::scale(rotateScaleTransLit, glm::vec3(deltaScalerLit, deltaScalerLit, 1.0f));
			rotateScaleTransLit = glm::translate(rotateScaleTransLit, glm::vec3(-0.5, -0.5, 0));

			float curScalingLit = 0.5;

			//glm::mat4 rotateScaleTrans = glm::mat4(1.0);
			glm::mat4 newModelViewLit = glm::mat4(1.0);
			newModelViewLit = glm::translate(newModelViewLit, glm::vec3(0.75, -0.75, 0));


			newModelViewLit = glm::scale(newModelViewLit, glm::vec3(curScalingLit, curScalingLit, 1));

			glm::mat4 projLit = meshWindowCam.GetProjectionMatrix(aspect);
			glm::mat4 newProjLit = glm::mat4(1.0);
			bool texDrawnLit = drawTexture;
			if (texDrawnLit)
			{
				tBgDisplay.Enable();
				//matrices
				tBgDisplay.SetModelView(newModelViewLit);
				tBgDisplay.SetProj(newProjLit);
				//texture
				///from here
				GLuint curTexID = individualTextureID;
				///to here
				//color vector
				tBgDisplay.SetBackgroundColor(texBgColor);
				tBgDisplay.SetTex(curTexID);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

				tBgDisplay.Disable();
			}
			mainShader.Enable();
			///////////////////////////////////
			mainShader.setOffsetX(deltaXOffsetLit);
			mainShader.setOffsetY(deltaYOffsetLit);
			mainShader.setScalerXY(curScalingLit);
			//mainShader.Enable();
			//vec colors
			mainShader.SetFaceColor(texFaceColor); ///mark
			mainShader.SetWireColor(texWireColor);
			//bool flags
			mainShader.SetLightingACK(false);
			mainShader.SetDrawWireACK(true);
			mainShader.SetDrawTexCoordACK(true);
			mainShader.SetDrawTextureFragACK(false);
			//matrices
			mainShader.SetModelView(newModelViewLit);
			mainShader.SetProj(projLit);
			mainShader.SetRotateTex(rotateScaleTransLit);
			model->RenderParameterized();
		}
		else {
			//universalTextureMode
			mainShader.SetModelView(modelView);
			mainShader.SetProj(proj);
			mainShader.SetRotateTex(glm::mat4(1.0));
			//flags
			mainShader.SetLightingACK(true);
			mainShader.SetDrawTexCoordACK(false);
			mainShader.SetDrawWireACK(false);
			mainShader.SetDrawTextureFragACK(false);
			mainShader.setOffsetX(0.0);
			mainShader.setOffsetY(0.0);
			mainShader.setScalerXY(1.0);
			//color vectors
			mainShader.SetWireColor(meshWireColor);
			mainShader.SetFaceColor(meshFaceColor);
			//cout << "enter1\n";
			//mainShader.bindTexture(textureID[currentTextureName]);
			//cout << "enter2\n";
			model->Render();
			mainShader.SetWireColor(choosenWireColor);
			for (int i = 0; i < BODYPARTSAMOUNT; i++) {
				if (dataNow.textureAttached[i] || model->currentlySelectedComponent == i) {
					glm::mat4 rotateScaleTrans(1.0);
					if (dataNow.textureAttached[i] && dataNow.ccTextureIDMapping[i] != "") {
						float deltaAngle = dataNow.UVRotate[i];
						float deltaScaler = dataNow.UVScale[i];
						float deltaXOffset = dataNow.UVOffset[i][0];
						float deltaYOffset = dataNow.UVOffset[i][1];
						float radian = deltaAngle * M_PI / 180.0f;
						rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(deltaXOffset, deltaYOffset, 0));
						rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(0.5, 0.5, 0));
						//rotateScaleTrans = glm::rotate(rotateScaleTrans, radian, glm::vec3(0.0, 0.0, 1.0));
						rotateScaleTrans = glm::rotate(rotateScaleTrans, deltaAngle, glm::vec3(0.0, 0.0, 1.0));
						rotateScaleTrans = glm::scale(rotateScaleTrans, glm::vec3(deltaScaler, deltaScaler, 1.0f));
						rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(-0.5, -0.5, 0));
						mainShader.SetDrawTextureFragACK(true);
						mainShader.SetRotateTex(rotateScaleTrans);
						GLuint curTexID = textureID[i];
						/*if (dataNow.ccWrapMode[i] != texIDToWrapMapping[curTexID] || texBorderColor[curTexID] != dataNow.ccBorderColor[i]) {
							cout << "Enteringg\n";
							changeTextureWrapNow(curTexID, texIDToWrapMapping, dataNow.ccBorderColor[i]);
						}
						if (dataNow.ccMinMagMode[i] != texIDToMinMagMapping[curTexID]) {
							changeTextureMinMagNow(curTexID, texIDToMinMagMapping);
						}*/
					}
					else {
						mainShader.SetDrawTextureFragACK(false);
					}
					if (model->currentlySelectedComponent == i) {
						mainShader.SetDrawWireACK(true);
					}
					else {
						mainShader.SetDrawWireACK(false);
					}
					model->RenderComponent(i, textureID[i], true, dataNow.textureAttached[i]);
				}
			}
			if (model->choosenFaceForUniversal.size() > 0) {

				drawPickingFaceShader.Enable();
				drawPickingFaceShader.SetMVMat(glm::value_ptr(modelView));
				drawPickingFaceShader.SetPMat(glm::value_ptr(proj));
				model->RenderSelectedFaceCC();
				drawPickingFaceShader.Disable();
			}


			//handle the sub texture
			if (model->currentlySelectedComponent != -1) {
				int idx = model->currentlySelectedComponent;
				//std::cout << "cur idx: " << idx << std::endl;
				float deltaAngle = dataNow.UVRotate[idx];
				float deltaScaler = dataNow.UVScale[idx];
				float deltaXOffset = dataNow.UVOffset[idx][0];
				float deltaYOffset = dataNow.UVOffset[idx][1];
				float radian = deltaAngle * M_PI / 180.0f;

				glm::mat4 rotateScaleTrans = glm::mat4(1.0);
				//glm::mat4 rotateScaleTrans = glm::translate(glm::mat4(1.0), glm::vec3(deltaXOffset, deltaYOffset, 0));
				//rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(deltaXOffset, deltaYOffset, 0));
				//rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(5, 5, 0));
				rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(0.5, 0.5, 0));
				//rotateScaleTrans = glm::rotate(rotateScaleTrans, radian, glm::vec3(0.0, 0.0, 1.0));
				rotateScaleTrans = glm::rotate(rotateScaleTrans, deltaAngle, glm::vec3(0.0, 0.0, 1.0));
				//rotateScaleTrans = glm::scale(rotateScaleTrans, glm::vec3(1, 1, 1)); //here
				rotateScaleTrans = glm::scale(rotateScaleTrans, glm::vec3(deltaScaler, deltaScaler, 1.0f));
				rotateScaleTrans = glm::translate(rotateScaleTrans, glm::vec3(-0.5, -0.5, 0));

				float curScaling = 0.5;

				//glm::mat4 rotateScaleTrans = glm::mat4(1.0);
				glm::mat4 newModelView = glm::mat4(1.0);
				newModelView = glm::translate(newModelView, glm::vec3(0.75, -0.75, 0));


				newModelView = glm::scale(newModelView, glm::vec3(curScaling, curScaling, 1));



				glm::mat4 proj = meshWindowCam.GetProjectionMatrix(aspect);
				glm::mat4 newProj = glm::mat4(1.0);
				bool texDrawn = dataNow.textureAttached[idx];
				std::string texString = dataNow.ccTextureIDMapping[idx];
				if (texDrawn && texString != "")
				{
					tBgDisplay.Enable();
					//matrices
					tBgDisplay.SetModelView(newModelView);
					tBgDisplay.SetProj(newProj);
					//texture
					///from here
					/*if (textureID.find(texString) == textureID.end()) {
						std::cout << "Flow Fault! Texture ID Mapping not found! Stopping!\n";
						exit(EXIT_FAILURE);
					}*/
					GLuint curTexID = textureID[idx];
					/*if (dataNow.ccWrapMode[idx] != texIDToWrapMapping[curTexID] || texBorderColor[curTexID] != dataNow.ccBorderColor[idx]) {
						changeTextureWrapNow(curTexID, texIDToWrapMapping, dataNow.ccBorderColor[idx]);
					}
					if (dataNow.ccMinMagMode[idx] != texIDToMinMagMapping[curTexID]) {
						changeTextureMinMagNow(curTexID, texIDToMinMagMapping);
					}*/
					///to here
					//color vector
					tBgDisplay.SetBackgroundColor(texBgColor);
					tBgDisplay.SetTex(curTexID);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

					tBgDisplay.Disable();
				}

				///////////////////////////////////

				mainShader.Enable();
				mainShader.setOffsetX(deltaXOffset);
				mainShader.setOffsetY(deltaYOffset);
				mainShader.setScalerXY(curScaling);
				//vec colors
				mainShader.SetFaceColor(texFaceColor); ///mark
				mainShader.SetWireColor(texWireColor);
				//bool flags
				mainShader.SetLightingACK(false);
				mainShader.SetDrawWireACK(true);
				mainShader.SetDrawTexCoordACK(true);
				mainShader.SetDrawTextureFragACK(false);
				//matrices
				mainShader.SetModelView(newModelView);
				mainShader.SetProj(proj);
				mainShader.SetRotateTex(rotateScaleTrans);
				model->RenderComponent(idx, textureID[idx], false, dataNow.textureAttached[idx]);
			}
		}
		/*float x[4] = { 0,0,0,0 };
		glBindBuffer(GL_ARRAY_BUFFER, model->submeshesTBO[ABS]);
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4, &x[0]);
		for (int i = 0; i < 4; i++) {
			cout << "res: " << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << "\n";
		}
		GLint size = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		cout << "size: " << size << "\n";*/
		mainShader.Disable();


		glUseProgram(0);
	}
	private: System::Void hkoglPanelControl1_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			meshWindowCam.mousePressEvent(90, e->X, e->Y);
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			meshWindowCam.mousePressEvent(92, e->X, e->Y);
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Right)
		{
			if (selectionMode == SelectionMode::UNIVERSAL_TEX)
			{
				isRightButtonPress = true;
			}
			SelectionHandler(selectionMode, this->hkoglPanelControl1->Height, e->X, e->Y);
			hkoglPanelControl1->Invalidate();
		}
		else
		{
			meshWindowCam.mouseReleaseEvent(90, e->X, e->Y);
			meshWindowCam.mouseReleaseEvent(92, e->X, e->Y);
			std::cout << "--->" << isRightButtonPress << "\n";
			if (isRightButtonPress)
			{
				isRightButtonPress = false;
				//std::cout << "Choosen face for universal size: " << model->choosenFaceForUniversal.size() << "\n";
				if (selectionMode == UNIVERSAL_TEX && model->choosenFaceForUniversal.size() > 0) {
					model->selectComponent();
				}
				model->choosenFaceForUniversal.clear();
			}
			hkoglPanelControl1->Invalidate();
		}

	}
	private: System::Void hkoglPanelControl1_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		if (e->Button == System::Windows::Forms::MouseButtons::Left || e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			meshWindowCam.mouseReleaseEvent(90, e->X, e->Y);
			hkoglPanelControl1->Invalidate();
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			meshWindowCam.mouseReleaseEvent(92, e->X, e->Y);
			hkoglPanelControl1->Invalidate();
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Right)
		{
			if (selectionMode == SelectionMode::UNIVERSAL_TEX)
			{
				isRightButtonPress = false;
				std::cout << "Choosen face for universal size: " << model->choosenFaceForUniversal.size() << "\n";
				if (selectionMode == UNIVERSAL_TEX && model->choosenFaceForUniversal.size() > 0) {
					model->selectComponent();
				}
				model->choosenFaceForUniversal.clear();
			}
			hkoglPanelControl1->Invalidate();
		}
	}
	private: System::Void hkoglPanelControl1_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			meshWindowCam.mouseMoveEvent(90, e->X, e->Y);
			hkoglPanelControl1->Invalidate();
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			meshWindowCam.mouseMoveEvent(92, e->X, e->Y);
			hkoglPanelControl1->Invalidate();
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Right)
		{
			SelectionHandler(selectionMode, this->hkoglPanelControl1->Height, e->X, e->Y);
			hkoglPanelControl1->Invalidate();
		}
	}
	private: System::Void hkoglPanelControl1_MouseWheel(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		meshWindowCam.wheelEvent(-(e->Delta));
		hkoglPanelControl1->Invalidate();
	}
	private: System::Void addFaceBtn_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		selectionMode = SelectionMode::ADD_FACE;
	}
	private: System::Void delFaceBtn_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		selectionMode = SelectionMode::DEL_FACE;

	}
	private: System::Void selPointBtn_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		selectionMode = SelectionMode::SELECT_POINT;

	}
	private: System::Void uniTexRadio_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		selectionMode = SelectionMode::UNIVERSAL_TEX;
	}
	private: System::Void saveParamBtn_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (model != NULL) {
			scaler = clampMinMax(scaler, 0, 1);
			uvRotateAngle = clampMinMax(uvRotateAngle, 0, 360);
			scaler = clampMinMax(scaler, 0, 1);
			offsetX = clampMinMax(offsetX, 0, 1);
			offsetY = clampMinMax(offsetY, 0, 1);
			model->Parameterization(uvRotateAngle, scaler, offsetX, offsetY);
			prevRotate = uvRotateAngle;
			prevOffsetX = offsetX;
			prevOffsetY = offsetY;
			prevScaler = scaler;
			//model->debugObjDebugger();
		}
	}

	private: System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e)
	{
		if (selectionMode == UNIVERSAL_TEX) {
			//cout << "prevSelectedComponent : " << prevSelectedComponent << std::endl;
			if (prevSelectionMode != selectionMode) {
				resetCCDataGui();
				resetMyButton();
				prevSelectedComponent = -1;
				if (model != NULL) model->currentlySelectedComponent = -1;
			}
			else if (model != NULL) {
				int curNum = model->currentlySelectedComponent;
				if (prevSelectedComponent != curNum) {
					if (curNum == -1) {
						resetCCDataGui();
						resetMyButton();
						// ccTextureName->Text = ccTextureIDMapping;
					}
					else {
						passDataNowToVariable(curNum);
						setMyButton(curNum, dataNow);
					}
				}
				prevSelectedComponent = model->currentlySelectedComponent;
				//if(prev)
			}
		}
		prevSelectionMode = selectionMode;

		hkoglPanelControl1->Invalidate();
		// hkoglPanelControl2->Invalidate();
		//std::cout << "update error: \n";
		//GLenum err;
		//MyCheckGLError();
		//while ((err = glGetError()) != GL_NO_ERROR)
		//{
		//	// Process/log the error.
		//}
	}
	/*private: System::Void weightBox_SelectedIndexChanged_1(System::Object^ sender, System::EventArgs^ e) 
	{
		switch (weightBox->SelectedIndex)
		{
		case 0:
			model->curWeightMode = WeightMode::UNIFORM;
			break;
		case 1:
			model->curWeightMode = WeightMode::HARMONIC;
			break;
		case 2:
			model->curWeightMode = WeightMode::MEAN;
			break;
		default:
			model->curWeightMode = WeightMode::UNIFORM;
			break;
		}
	}
private: System::Void borderBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
{
	switch (borderBox->SelectedIndex)
	{
	case 0:
		model->curBorderMode = BorderMode::QUAD;
		break;
	case 1:
		model->curBorderMode = BorderMode::DISK;
		break;
	default:
		model->curBorderMode = BorderMode::QUAD;
		break;
	}
}*/
private: System::Void panel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) 
{
	
}
private: System::Void paramBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	if (model != NULL) {
		scaler = clampMinMax(scaler, 0, 1);
		uvRotateAngle = clampMinMax(uvRotateAngle, 0, 360);
		scaler = clampMinMax(scaler, 0, 1);
		offsetX = clampMinMax(offsetX, 0, 1);
		offsetY = clampMinMax(offsetY, 0, 1);
		model->Parameterization(uvRotateAngle, scaler, offsetX, offsetY);
		prevRotate = uvRotateAngle;
		prevOffsetX = offsetX;
		prevOffsetY = offsetY;
		prevScaler = scaler;
		//model->debugObjDebugger();
	}
}
private: System::Void clearSelectFaceBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	if (model != NULL) {
		model->clearSelectedFace();
	}
}

private: System::Void numericUpDown1_ValueChanged(System::Object^ sender, System::EventArgs^ e) 
{
	if (numericUpDown1->Value > 7)
	{
		numericUpDown1->Value = 7;
		brushSize = 7;
	}
	else if (numericUpDown1->Value < 1)
	{
		numericUpDown1->Value = 1;
		brushSize = 1;
	}
	else
	{
		brushSize = int(numericUpDown1->Value);
	}
}
private: System::Void checkBorderBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	if (model != NULL) {
		model->borderCheck(closestPosClick);
		glBindBuffer(GL_ARRAY_BUFFER, PVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), glm::value_ptr(closestPosClick), GL_DYNAMIC_DRAW);
		glBindVertexArray(PVAO);
	}
}
//private: System::Void showOutlineToggle_CheckedChanged(System::Object^ sender, System::EventArgs^ e) 
//{
//	showOutline = showOutlineToggle->Checked;
//}
//private: System::Void showTexToggle_CheckedChanged(System::Object^ sender, System::EventArgs^ e) 
//{
//	drawTexture = showTexToggle->Checked;
//}
private: System::Void panel3_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) 
{
	// for left side settings
}
//private: System::Void uvOffsetX_Scroll(System::Object^ sender, System::EventArgs^ e) 
//{
//	offsetX = uvOffsetX->Value / 100;
//}
//private: System::Void uvOffsetY_Scroll(System::Object^ sender, System::EventArgs^ e) 
//{
//	offsetY = uvOffsetY->Value / 100;
//}
//private: System::Void uvRotate_Scroll(System::Object^ sender, System::EventArgs^ e) 
//{
//	uvRotateAngle = uvRotate->Value;
//}
//
//private: System::Void uvScale_Scroll(System::Object^ sender, System::EventArgs^ e) 
//{
//	scaler = uvScale->Value / 100;
//}
private: System::Void ccSelectedVal_TextChanged(System::Object^ sender, System::EventArgs^ e) 
{
	
}
private: System::Void label3_Click(System::Object^ sender, System::EventArgs^ e) 
{
	// label3->Text = prevSelectedComponent;
}
private: System::Void addTexBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	
}
private: System::Void ccWeightBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
{
	switch (ccWeightBox->SelectedIndex)
	{
	case 0:
		ccCurrentUVWeightMode = WeightMode::UNIFORM;
		break;
	case 1:
		ccCurrentUVWeightMode = WeightMode::HARMONIC;
		break;
	case 2:
		ccCurrentUVWeightMode = WeightMode::MEAN;
		break;
	default:
		ccCurrentUVWeightMode = WeightMode::MEAN;
		break;
	}
	updateAllVariable();
}
private: System::Void ccBorderBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
{
	switch (ccBorderBox->SelectedIndex)
	{
	case 0:
		ccCurrentUVBorderMode = BorderMode::QUAD;
		break;
	case 1:
		ccCurrentUVBorderMode = BorderMode::DISK;
		break;
	default:
		ccCurrentUVBorderMode = BorderMode::DISK;
		break;
	}
	updateAllVariable();
}
private: System::Void ccUVOffsetX_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
	ccUVOffset[0] = ccUVOffsetX->Value / 100.0;
	updateAllVariable();
}
private: System::Void ccUVOffsetY_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
	ccUVOffset[1] = ccUVOffsetY->Value / 100.0;
	updateAllVariable();
}
private: System::Void ccUVRotateBar_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
	ccUVRotate = ccUVRotateBar->Value;
	updateAllVariable();
}
private: System::Void ccUVScaleBar_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
	ccUVScale = ccUVScaleBar->Value / 100.0;
	updateAllVariable();
}
private: System::Void ccApplyBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	if (model != NULL && model->currentlySelectedComponent != -1) {
		updaterDataNow(model->currentlySelectedComponent);
	}
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
private: System::Void ccShowTexToggle_CheckedChanged(System::Object^ sender, System::EventArgs^ e) 
{
	ccTextureAttached = ccShowTexToggle->Checked;
	updateAllVariable();
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
private: System::Void ccAddTexBtn_Click(System::Object^ sender, System::EventArgs^ e)
{
	// openFileDialog1->InitialDirectory = ResourcePath::imagePath;
	openFileDialog3->Filter = "Image Files|*.jpg;*.jpeg;";
	openFileDialog3->Multiselect = false;
	openFileDialog3->ShowDialog();
}
private: System::Void openFileDialog3_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) 
{
	MarshalString(openFileDialog3->FileName, ccTextureIDMapping);
	ccTextureIDMapping = changeToSingleSlash(ccTextureIDMapping);
	
	std::cout << "Filename: " << ccTextureIDMapping << "\n";
	if (ccTextureIDMapping != "") {
		ccTextureIDMapping = cleanPathNameTex(ccTextureIDMapping);
		if (addIndividualTexture(ccTextureIDMapping, model->currentlySelectedComponent)) {
			//update name
			prevCCTextureIDMapping = ccTextureIDMapping;
			std::cout << "File present: " << prevCCTextureIDMapping << "\n";
			ccTextureAttached = true;
			ccShowTexToggle->Checked = true;
			updateAllVariable();
		}
		else {
			std::cout << "File invalid: " << ccTextureIDMapping << "\n";
			ccTextureIDMapping = prevCCTextureIDMapping;
		}
	}
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
private: System::Void saveFileDialog1_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) 
{
	MarshalString(saveFileDialog1->FileName, saveLoadFilename);

	saveLoadFilename = changeToSingleSlash(saveLoadFilename);
	std::cout << "Filename: " << saveLoadFilename << "\n";
	if (saveLoadFilename.size() != 0) {
		univTexSaving(saveLoadFilename);
		std::cout << "File present: " << saveLoadFilename << "\n";
		updateAllVariable();
	}
	else {
		std::cout << "cancel saving: filename is empty\n";
	}
}
private: System::Void ccSaveTexBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	saveFileDialog1->Filter = "Tex Files|*.ctm;";
	saveFileDialog1->ShowDialog();
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
private: System::Void openFileDialog2_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) 
{
	MarshalString(openFileDialog2->FileName, saveLoadFilename);

	saveLoadFilename = changeToSingleSlash(saveLoadFilename);
	std::cout << saveLoadFilename << "\n";
	if (saveLoadFilename.size() != 0) {
		univTexLoading(saveLoadFilename);
		resetMyButton();
		std::cout << "File present: " << saveLoadFilename << "\n";
		updateAllVariable();
	}
	else {
		std::cout << "cancel loading: filename is empty\n";
	}	
}
private: System::Void ccLoadTexBtn_Click(System::Object^ sender, System::EventArgs^ e)
{
	openFileDialog2->Filter = "Tex Files|*.ctm;";
	openFileDialog2->Multiselect = false;
	openFileDialog2->ShowDialog();
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
private: System::Void ccTextureName_TextChanged(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void unselectComponentBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	model->currentlySelectedComponent = -1;
}
private: System::Void cleanCanvasBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	model->currentlySelectedComponent = -1;
	resetDataNow();
	resetMyButton();
}
private: System::Void ssBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	
}
private: System::Void selectionBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
{
	switch (selectionBox->SelectedIndex)
	{
	case 0:
		selectionMode = SelectionMode::ADD_FACE;
		panel1->Visible = false;
		panel3->Visible = true;
		break;
	case 1:
		selectionMode = SelectionMode::DEL_FACE;
		panel1->Visible = false;
		panel3->Visible = true;
		break;
	case 2:
		selectionMode = SelectionMode::SELECT_POINT;
		panel1->Visible = false;
		panel3->Visible = true;
		break;
	case 3:
		selectionMode = SelectionMode::UNIVERSAL_TEX;
		panel1->Visible = true;
		panel3->Visible = false;
		break;
	default:
		selectionMode = SelectionMode::ADD_FACE;
		panel1->Visible = false;
		panel3->Visible = true;
		break;
	}
}
private: System::Void paramBtn_Click_1(System::Object^ sender, System::EventArgs^ e) 
{
	if (model != NULL) {
		scaler = clampMinMax(scaler, 0, 1);
		uvRotateAngle = clampMinMax(uvRotateAngle, 0, 360);
		scaler = clampMinMax(scaler, 0, 1);
		offsetX = clampMinMax(offsetX, 0, 1);
		offsetY = clampMinMax(offsetY, 0, 1);
		model->Parameterization(uvRotateAngle, scaler, offsetX, offsetY);
		prevRotate = uvRotateAngle;
		prevOffsetX = offsetX;
		prevOffsetY = offsetY;
		prevScaler = scaler;
		//model->debugObjDebugger();
	}
}
private: System::Void showOutlineToggle_CheckedChanged(System::Object^ sender, System::EventArgs^ e) 
{
	showOutline = showOutlineToggle->Checked;
}

private: System::Void showTexToggle_CheckedChanged(System::Object^ sender, System::EventArgs^ e) 
{
	drawTexture = showTexToggle->Checked;
}
private: System::Void checkBorderBtn_Click_1(System::Object^ sender, System::EventArgs^ e) 
{
	if (model != NULL) {
		model->borderCheck(closestPosClick);
		glBindBuffer(GL_ARRAY_BUFFER, PVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), glm::value_ptr(closestPosClick), GL_DYNAMIC_DRAW);
		glBindVertexArray(PVAO);
	}
}
private: System::Void borderBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
{
	switch (borderBox->SelectedIndex)
	{
	case 0:
		model->curBorderMode = BorderMode::QUAD;
		break;
	case 1:
		model->curBorderMode = BorderMode::DISK;
		break;
	default:
		model->curBorderMode = BorderMode::QUAD;
		break;
	}
}
private: System::Void weightBox_SelectedIndexChanged_1(System::Object^ sender, System::EventArgs^ e) 
{
	switch (weightBox->SelectedIndex)
	{
	case 0:
		model->curWeightMode = WeightMode::UNIFORM;
		break;
	case 1:
		model->curWeightMode = WeightMode::HARMONIC;
		break;
	case 2:
		model->curWeightMode = WeightMode::MEAN;
		break;
	default:
		model->curWeightMode = WeightMode::UNIFORM;
		break;
	}
}
private: System::Void clearFaceBtn_Click(System::Object^ sender, System::EventArgs^ e) 
{
	if (model != NULL) {
		model->clearSelectedFace();
	}
}
private: System::Void uvOffsetXBar_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
	offsetX = uvOffsetXBar->Value / 100.0;
}
private: System::Void uvOffsetYBar_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
	offsetY = uvOffsetYBar->Value / 100.0;
}
private: System::Void uvRotateBar_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
	uvRotateAngle = uvRotateBar->Value;
}

private: System::Void uvScaleBar_Scroll(System::Object^ sender, System::EventArgs^ e) 
{
	scaler = uvScaleBar->Value / 100.0;
}
private: System::Void addTextureBtn_Click_1(System::Object^ sender, System::EventArgs^ e)
{
	openFileDialog1->Filter = "Image Files | *.jpg; *.jpeg;";
	openFileDialog1->Multiselect = false;
	openFileDialog1->ShowDialog();
}
private: System::Void openFileDialog1_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) 
{
	std::string inputFile;
	MarshalString(openFileDialog1->FileName, inputFile);
	inputFile = changeToSingleSlash(inputFile);

	std::cout << "Filename: " << inputFile << "\n";
	if (inputFile != "") {
		inputFile = cleanPathNameTex(inputFile);
		if (addIndividualTexture(inputFile, -10)) {
			//update name
			std::cout << "Upload Regular Texture SUCCESS!\n";
			std::cout << inputFile << "\n";
		}
		else {
			std::cout << "Upload Regular Texture FAILED!\n";
			std::cout << inputFile << "\n";
		}
	}
}
};
}
