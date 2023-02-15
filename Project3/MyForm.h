#pragma once

#include <Common.h>
#include <ViewManager.h>

#include "DotNetUtilities.h"

#include <ResourcePath.h>

#include "OpenMesh.h"
#pragma unmanaged
#include "MeshObject.h"
#pragma managed
#include "MainShader.h"
#include "PickingTexture.h"

#pragma unmanaged
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Include/STB/stb_image_write.h"
#pragma managed

#define TEXTURE_AMOUNT 2
#define MODEL_AMOUNT 2

int currentIndex = 0;

std::string textureList[TEXTURE_AMOUNT] = {
	"checkerboard4.jpg",
	"marble.jpg"
};

std::string currentTextureName = textureList[0];
std::string prevTextureName = currentTextureName;
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

std::map<std::string, GLuint> textureID; //list of texture attachment ID's
//map<GLuint, WRAPMODE> texIDToWrapMapping; 
//map<GLuint, MIN_MAG_MODE> texIDToMinMagMapping;
//map<GLuint, glm::vec4> texBorderColor;
ViewManager		meshWindowCam, texCoordWindowCam;

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

// vbo and vao for drawing point when needed
GLuint PVBO;
GLuint PVAO;
glm::vec3 closestPosClick;
float pointSize = 10.f;

//colors
glm::vec4 meshFaceColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 texFaceColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
glm::vec4 meshWireColor = glm::vec4(0.f, 0.f, 0.f, 1.0f);
glm::vec4 choosenWireColor = glm::vec4(0.f, 0.5f, 0.5f, 0.8f);
glm::vec4 texWireColor = glm::vec4(1.0f, 1.0f, 0.f, 1.f);
glm::vec4 pointColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
glm::vec4 texBgColor = glm::vec4(0.3f, 0.3f, 0.3f, 0.3f);
glm::vec4 texBorderWrapColor = meshFaceColor;

std::string saveLoadFilename = "";

//to track changes, only for now
std::string prevCCTextureIDMapping;

//to track when there is achange of chosen component
int prevSelectedComponent = -1;

UpdateData dataNow;

//cc buttons and gui
bool ccTextureAttached;
WeightMode ccCurrentUVWeightMode;
BorderMode ccCurrentUVBorderMode;
glm::vec2 ccUVOffset;
float ccUVRotate;
float ccUVScale;
std::string ccTextureIDMapping;
// functions here
//.....


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

namespace OpenMesh_EX {

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

	/// <summary>
	/// MyForm 的摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		SelectionMode prevSelectionMode;
		SelectionMode selectionMode;

		MyForm(void)
		{
			prevSelectionMode = ADD_FACE;
			selectionMode = ADD_FACE;
			InitializeComponent();
			//
			//TODO:  在此加入建構函式程式碼
			//
		}

	protected:
		/// <summary>
		/// 清除任何使用中的資源。
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::MenuStrip^ menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ loadModelToolStripMenuItem;
	private: System::Windows::Forms::OpenFileDialog^ openModelDialog;
	private: System::Windows::Forms::SaveFileDialog^ saveModelDialog;
	private: System::Windows::Forms::ToolStripMenuItem^ saveModelToolStripMenuItem;
	private: HKOGLPanel::HKOGLPanelControl^ hkoglPanelControl1;
	protected:

	private:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
		/// 這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			HKOGLPanel::HKCOGLPanelCameraSetting^ hkcoglPanelCameraSetting1 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
			HKOGLPanel::HKCOGLPanelPixelFormat^ hkcoglPanelPixelFormat1 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openModelDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveModelDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->hkoglPanelControl1 = (gcnew HKOGLPanel::HKOGLPanelControl());
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(613, 24);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->loadModelToolStripMenuItem,
					this->saveModelToolStripMenuItem
			});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(38, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// loadModelToolStripMenuItem
			// 
			this->loadModelToolStripMenuItem->Name = L"loadModelToolStripMenuItem";
			this->loadModelToolStripMenuItem->Size = System::Drawing::Size(144, 22);
			this->loadModelToolStripMenuItem->Text = L"Load Model";
			this->loadModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::loadModelToolStripMenuItem_Click);
			// 
			// saveModelToolStripMenuItem
			// 
			this->saveModelToolStripMenuItem->Name = L"saveModelToolStripMenuItem";
			this->saveModelToolStripMenuItem->Size = System::Drawing::Size(144, 22);
			this->saveModelToolStripMenuItem->Text = L"Save Model";
			this->saveModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::saveModelToolStripMenuItem_Click);
			// 
			// openModelDialog
			// 
			this->openModelDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::openModelDialog_FileOk);
			// 
			// saveModelDialog
			// 
			this->saveModelDialog->DefaultExt = L"obj";
			this->saveModelDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::saveModelDialog_FileOk);
			// 
			// hkoglPanelControl1
			// 
			hkcoglPanelCameraSetting1->Far = 1000;
			hkcoglPanelCameraSetting1->Fov = 45;
			hkcoglPanelCameraSetting1->Near = -1000;
			hkcoglPanelCameraSetting1->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
			this->hkoglPanelControl1->Camera_Setting = hkcoglPanelCameraSetting1;
			this->hkoglPanelControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->hkoglPanelControl1->Location = System::Drawing::Point(0, 24);
			this->hkoglPanelControl1->Name = L"hkoglPanelControl1";
			hkcoglPanelPixelFormat1->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat1->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			hkcoglPanelPixelFormat1->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
			this->hkoglPanelControl1->Pixel_Format = hkcoglPanelPixelFormat1;
			this->hkoglPanelControl1->Size = System::Drawing::Size(613, 430);
			this->hkoglPanelControl1->TabIndex = 2;
			this->hkoglPanelControl1->Load += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl1_Load);
			this->hkoglPanelControl1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::hkoglPanelControl1_Paint);
			this->hkoglPanelControl1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseDown);
			this->hkoglPanelControl1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseMove);
			this->hkoglPanelControl1->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseWheel);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(613, 454);
			this->Controls->Add(this->hkoglPanelControl1);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MyForm";
			this->Text = L"OpenMesh_EX";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void hkoglPanelControl1_Load(System::Object^ sender, System::EventArgs^ e)
	{

	}
	private: System::Void hkoglPanelControl1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e)
	{
		
	}
	private: System::Void hkoglPanelControl1_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		
	}
	private: System::Void hkoglPanelControl1_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		
	}
	private: System::Void hkoglPanelControl1_MouseWheel(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		
	}
	private: System::Void loadModelToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
	{
		openModelDialog->Filter = "Model(*.obj)|*obj";
		openModelDialog->Multiselect = false;
		openModelDialog->ShowDialog();
	}
	private: System::Void openModelDialog_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e)
	{
		
	}
	private: System::Void saveModelToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
	{
		saveModelDialog->Filter = "Model(*.obj)|*obj";
		saveModelDialog->ShowDialog();
	}
	private: System::Void saveModelDialog_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e)
	{
		std::string filename;
		MarshalString(saveModelDialog->FileName, filename);

		std::cout << filename << "\n";
	}
	};
}
