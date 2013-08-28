#pragma once

#include "MainApp/Application.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/SimpleGeometry.h"
#include "Graphics/CameraControler.h"

using namespace RcEngine;

class TestApp : public RcEngine::Application
{
public:
	TestApp(const String& config);
	~TestApp(void);

protected:
	void Initialize();
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);

	void CalculateFrameRate();

	void InitGUI();
	void DrawUI();

protected:
	void SliderValueChange(int32_t value);
	void ButtonClicked();
	void CheckBoxToggle(bool checked);

private:
	shared_ptr<Effect> mEffect;
	EffectTechnique* mTechnique;
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
	shared_ptr<Texture> mTexture;
	shared_ptr<SimpleBox> mBox;
	shared_ptr<SimpleTexturedQuad> mQuad;
	shared_ptr<Mesh> mDwarf;
	shared_ptr<Material> mDwarfMaterial;
	shared_ptr<SpriteBatch> mSpriteBatch;
	FPSCameraControler* mCameraControler;
	int mFramePerSecond;

	//ModelViewerCameraControler* mCameraControler;


	Label* mFPSLabel;
	Slider* mSlider;
	Button* mButton;
	CheckBox* mCheckBox;
	ScrollBar* mScrollBar;
	ListBox* mListBox;
	TextEdit* mTextEdit;
	TextEdit* mLineEdit;
	ComboBox* mComboBox;
};

