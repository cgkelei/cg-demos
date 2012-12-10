#include "Common.h"
#include "ShadowMap.h"
#include "RenderTextureFBO.h"
#include "Camera.h"
#include "Utility.h"
#include <nvMath.h>
#include <nvModel.h>
#include <nvImage.h>
#include <nvShaderUtils.h>
#include <nvGlutWidgets.h>
#include <nvSDKPath.h>
#include <nvGlutManipulators.h>

using namespace std;

static const char StretchMapVS[] = /*MEDIA_SHADER_PATH*/ "Stretch.vert";
static const char StretchMapFS[] = /*MEDIA_SHADER_PATH*/ "Stretch.frag";
static const char ConvolutionStretchVS[] = /*MEDIA_SHADER_PATH*/ "ConvolutionStretch.vert";
static const char ConvolutionStretchUFS[] = /*MEDIA_SHADER_PATH*/ "ConvolutionStretchU.frag";
static const char ConvolutionStretchVFS[] = /*MEDIA_SHADER_PATH*/ "ConvolutionStretchV.frag";

const char* HeadFile = "Infinite-Level_02.OBJ";
const char* HeadAlbedo =  "Map-COL.dds";
const char* HeadNormal =  "Infinite-Level_02_World_SmoothUV.dds";
const char* EnvMap = "Env.dds";
const char* RhodMap = "rho_d.png";

#define LIGHT_COUNT 3
#define BLOOM_BUFFER_COUNT 2
#define TEXTURE_BUFFER_COUNT 6
#define BUFFER_SIZE 1024
#define SHADOW_MAP_SIZE 1024
#define CAMERA_FOV 20.0f
#define MATH_PI float(3.14159265358979323846264338327950288f)

struct Light
{
	glm::vec3 Color;
	float Amount;
	Camera Camera;
	ShadowMap* ShadowMap;
};

Light gLights[LIGHT_COUNT];


int gWindowWidth = 1024;
int gWindowHeight = 768;

nv::SDKPath gAppPath;
nv::GlutUIContext gHub;

nv::Model gLightModel;
GLuint gLightTexID, gLightModelVBO, gLightModelIBO;

nv::Model gModel;
float gModelDiameter = 0.0f; //diameter of the model
glm::vec3 gCenter;	// center of the model
glm::mat4 gModelWorld;

GLuint gModelVBO, gModelIBO;
GLuint gAlbedoTexID, gNormalTexID, gRhodTexID;
GLuint gEnvTexID;

bool gStrechMapDirty = true;		

Camera gCamera, gCameraTemp;

RenderTexture* gTempBuffer, *gFinalBuffer;
RenderTexture* gStretchBuffer[TEXTURE_BUFFER_COUNT];	// stretch texture blur storage
RenderTexture* gIrradianceBuffer[LIGHT_COUNT][TEXTURE_BUFFER_COUNT];

float gConvolutionScale[TEXTURE_BUFFER_COUNT-1] = { 0.0484f, 0.187f, 0.567f, 1.99f, 7.41f};


enum Object { OBJECT_CAMERA = 0, OBJECT_LIGHT1, OBJECT_LIGHT2, OBJECT_LIGHT3 };
int gManipulator = OBJECT_CAMERA;

enum UIOption {
	OPTION_WIREFRAME = 0,
	OPTION_DRAW_SKYBOX,
	OPTION_SHOW_LIGHTS,
	OPTION_MANIPULATE_CAMERA,
	OPTION_MANIPULATE_LIGHT0,
	OPTION_ANIMATE = OPTION_MANIPULATE_LIGHT0 + LIGHT_COUNT,
	OPTION_COUNT,
};

bool gOptions[OPTION_COUNT];
float gLightColor[LIGHT_COUNT];

enum RenderMode
{
	RM_NORMAL = 0,
	RM_SHADOW_0,
	RM_Convolution_0 = RM_SHADOW_0 + LIGHT_COUNT,
};
int gMode = RM_NORMAL;

struct PipelineEffect
{
	GLuint ProgramID;
	GLint WorldParam, ViewParam, ProjectionParam, AlbedoTexParam;
} gPipelineEffect;

struct StretchEffect
{
	GLuint ProgramID;
	GLint WorldParam, ScaleParam;

} gStretchEffect;

struct ConvolutionStretchEffect
{
	GLuint ProgramID;
	GLint WorldViewProjParam, GaussWidthParam, StretchTexParam, InputTexParam;

} gConvStretchUEffect, gConvStretchVEffect;

struct TextureSpaceLightEffect
{
	GLuint ProgramID;
	GLint WorldParam, ShadowMatrixParam, AlbedoTexParam, SpecTexParam,
		  NormalTexParam, Rho_d_TexParam, EnvCubeParam, ShadowTexParam;
	GLint LightsPosParam, LightsColorParam, LightsAmountParam;

} gTexSpaceLightEffect;

void BuildModel(nv::Model* model, GLuint* vbo, GLuint* ibo)
{
	glGenBuffers(1, vbo);
	glGenBuffers(1, ibo);

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(model->getCompiledVertexCount() * model->getCompiledVertexSize() * sizeof(float)),
		model->getCompiledVertices(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(model->getCompiledIndexCount( nv::Model::eptTriangles) * sizeof(GLuint)),
		model->getCompiledIndices( nv::Model::eptTriangles), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawModel(nv::Model* model, GLuint vbo, GLuint ibo)
{
	#define BUFFER_OFFSET(i) ((char*)NULL + (i))

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	int vertexSize = model->getCompiledVertexSize() * sizeof(float);
	int offset = 0;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, model->getPositionSize(), GL_FLOAT, false, vertexSize, BUFFER_OFFSET(model->getCompiledPositionOffset() * sizeof(float)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, model->getTexCoordSize(),  GL_FLOAT, false, vertexSize, BUFFER_OFFSET(model->getCompiledTexCoordOffset() * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,3, GL_FLOAT, false, vertexSize, BUFFER_OFFSET(model->getCompiledNormalOffset() * sizeof(float)));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, model->getCompiledIndexCount( nv::Model::eptTriangles), GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint Create2DTextureFromFile(const char* fileName, GLenum wrapType = GL_REPEAT)
{
	GLuint tex = 0;

	std::string resolvedPath;
	if (gAppPath.getFilePath( fileName, resolvedPath))
	{
		nv::Image image;
		if ( !image.loadImageFromFile(resolvedPath.c_str()) )
		{
			fprintf(stderr, "Error: Failed to load texture: %s\n", fileName);
			exit(-1);
		}

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapType);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapType);
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

		glTexImage2D( GL_TEXTURE_2D, 0, image.getInternalFormat(), image.getWidth(), image.getHeight(), 0,
			image.getFormat(), image.getType(), image.getLevel(0));
	}

	return tex;
}

GLuint CreateCubemapTextureFromFile(const char* fileName)
{
	GLuint tex = 0;

	std::string resolvedPath;
	if (gAppPath.getFilePath( fileName, resolvedPath))
	{
		nv::Image image;
		if ( !image.loadImageFromFile(resolvedPath.c_str()) )
		{
			fprintf(stderr, "Error: Failed to load texture: %s\n", fileName);
			exit(-1);
		}
		assert( image.isCubeMap() == true);

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

		// load face data
		for(int i=0; i<6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				image.getInternalFormat(), image.getWidth(), image.getHeight(), 0, 
				GL_RGB, GL_FLOAT, image.getLevel(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i));
		}
	}

	return tex;
}

void CreateBuffers(GLenum format)
{
	GLenum target = GL_TEXTURE_2D;

	for (int i = 0; i < TEXTURE_BUFFER_COUNT; i++)
	{
		for (int j = 0; j < LIGHT_COUNT; ++j)
		{
			gIrradianceBuffer[j][i] = new RenderTexture(BUFFER_SIZE, BUFFER_SIZE, target); 
			gIrradianceBuffer[j][i]->InitColor_Tex(0, format);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			if (i == 0)
			{
				gIrradianceBuffer[j][i]->InitDepth_Tex();
			}
		}
	}

	for (int i = 0; i < LIGHT_COUNT; i++) 
	{
		//gLights->Camera.SetDistance(2.0);
		gLights[i].Camera.SetProjection(CAMERA_FOV, 1.0f, 0.1f, 10.0f);
		gLights[i].ShadowMap = new ShadowMap(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	}

	if (!gStretchBuffer[0])
	{
		for (int i=0; i<TEXTURE_BUFFER_COUNT; i++)
		{
			gStretchBuffer[i] = new RenderTexture(BUFFER_SIZE, BUFFER_SIZE, GL_TEXTURE_2D);
			gStretchBuffer[i]->InitColor_Tex(0, format);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		//bStretchMap = false;
	}
}

int gFPS = 0;
int gElapsedTime = 0;

void CalculateFPS()
{
	static int frameCount=0, timeBase = 0, currentTime;
	static int previousTime = 0;

	frameCount++;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval  = currentTime - timeBase;

	if (timeInterval > 1000) 
	{
		gFPS = frameCount * 1000.0 / timeInterval;
		timeBase = currentTime;
		frameCount = 0;
	}
	gElapsedTime = currentTime - previousTime;
	previousTime = currentTime;
}

void SetOrthoProjection(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);
}

void LoadShaderEffect()
{
#define RETRIEVE_UNIFORM_LOCATION(loc, program, name)  loc = glGetUniformLocation(program, name); /*\
														ASSERT(loc >= 0);*/

	gPipelineEffect.ProgramID = Utility::LoadShaderEffect("FixedPipeline.vert", "FixedPipeline.frag");
	ASSERT(gPipelineEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.WorldParam, gPipelineEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.ViewParam, gPipelineEffect.ProgramID, "View");
	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.ProjectionParam, gPipelineEffect.ProgramID, "Projection");
	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.AlbedoTexParam, gPipelineEffect.ProgramID, "AlbedoTex");

	// StretchMap generate effect
	printf("Load StretchMap Effect...\n");
	gStretchEffect.ProgramID = Utility::LoadShaderEffect(StretchMapVS, StretchMapFS);
	ASSERT(gStretchEffect.ProgramID > 0);
	//Utility::PrintEffectAttribs(gStretchEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gStretchEffect.WorldParam, gStretchEffect.ProgramID, "WorldMat");
	RETRIEVE_UNIFORM_LOCATION(gStretchEffect.ScaleParam, gStretchEffect.ProgramID, "Scale");


	// ConvolutionStretchU effect
	printf("Load ConvolutionStretchU Effect...\n");
	gConvStretchUEffect.ProgramID = Utility::LoadShaderEffect(ConvolutionStretchVS, ConvolutionStretchUFS);
	ASSERT(gConvStretchUEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gConvStretchUEffect.WorldViewProjParam, gConvStretchUEffect.ProgramID, "WorldViewProjMat");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchUEffect.StretchTexParam, gConvStretchUEffect.ProgramID, "StretchTex");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchUEffect.InputTexParam, gConvStretchUEffect.ProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchUEffect.GaussWidthParam, gConvStretchUEffect.ProgramID, "GaussWidth");

	// ConvolutionStretchV effect
	printf("Load ConvolutionStretchV Effect...\n");
	gConvStretchVEffect.ProgramID = Utility::LoadShaderEffect(ConvolutionStretchVS, ConvolutionStretchVFS);
	ASSERT(gConvStretchVEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gConvStretchVEffect.WorldViewProjParam, gConvStretchVEffect.ProgramID, "WorldViewProjMat");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchVEffect.StretchTexParam, gConvStretchVEffect.ProgramID, "StretchTex");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchVEffect.InputTexParam, gConvStretchVEffect.ProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchVEffect.GaussWidthParam, gConvStretchVEffect.ProgramID, "GaussWidth");

	// Texture Space Light effect
	printf("Load Texture Space Light Effect...\n");
	gTexSpaceLightEffect.ProgramID = Utility::LoadShaderEffect("TextureSpaceLighting.vert", "TextureSpaceLighting.frag");
	ASSERT(gTexSpaceLightEffect.ProgramID > 0);
	Utility::PrintEffectAttribs(gTexSpaceLightEffect.ProgramID);
	Utility::PrintEffectUniforms(gTexSpaceLightEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.WorldParam, gTexSpaceLightEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightsColorParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightsPosParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightsAmountParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.ShadowTexParam, gTexSpaceLightEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.NormalTexParam, gTexSpaceLightEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.AlbedoTexParam, gTexSpaceLightEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.Rho_d_TexParam, gTexSpaceLightEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.EnvCubeParam, gTexSpaceLightEffect.ProgramID, "EnvCube");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.ShadowMatrixParam, gTexSpaceLightEffect.ProgramID, "ShadowMatrix[0]");

}

void LoadModel()
{
	std::string resolvedPath;
	if (gAppPath.getFilePath( "Light.obj", resolvedPath)) 
	{
		if (!gLightModel.loadModelFromFile(resolvedPath.c_str())) 
		{
			fprintf(stderr, "Error loading model '%s'\n", HeadFile);
			exit(-1);
		}

		printf("compiling light mesh...\n");
		gLightModel.compileModel( nv::Model::eptTriangles);

		BuildModel(&gLightModel, &gLightModelVBO, &gLightModelIBO);

		gLightTexID = Create2DTextureFromFile("light.dds");
	}
	
	if (gAppPath.getFilePath( "model2.OBJ", resolvedPath)) 
	{
		if (!gModel.loadModelFromFile(resolvedPath.c_str())) 
		{
			fprintf(stderr, "Error loading model '%s'\n", HeadFile);
			exit(-1);
		}

		// compute normal if not exits
		if (!gModel.getNormalCount())
		{
			printf("compute mesh normals...\n");
			gModel.computeNormals();
		}

		//compute the model dimensions
		nv::vec3f minPos, maxPos;

		gModel.computeBoundingBox( minPos, maxPos);
		nv::vec3f center = (minPos + maxPos) * 0.5f;
		gModelDiameter = nv::length( maxPos - minPos);

		gCenter = glm::vec3(center.x, center.y, center.z);
		gModelWorld = glm::translate(glm::mat4(1.0f), -gCenter);

		// make the model efficient for rendering with vertex arrays
		printf("compiling mesh...\n");
		gModel.compileModel( nv::Model::eptTriangles);

		BuildModel(&gModel, &gModelVBO, &gModelIBO);

		// load model textures
		printf("load head albedo map...\n");
		gAlbedoTexID = Create2DTextureFromFile(HeadAlbedo);

		printf("load head normal map...\n");
		//gNormalTexID = Create2DTextureFromFile(HeadNormal);

		printf("load head rho_d map...\n");
		gRhodTexID = Create2DTextureFromFile(RhodMap, GL_CLAMP_TO_EDGE);
	}

	// Load env map
	printf("load environment map...\n");
	gEnvTexID = CreateCubemapTextureFromFile("MeadowTrail");
}

void LoadPresents()
{
	std::string resolvedPath;

	if (gAppPath.getFilePath("Present0.txt", resolvedPath))
	{
		std::ifstream stream(resolvedPath, std::ifstream::in );
		stream >> gCamera;

		for (int i = 0; i < LIGHT_COUNT; ++i)
		{
			stream >> gLights[i].Camera;
			stream >> gLights[i].Color.x >> gLights[i].Color.y >> gLights[i].Color.z;
			gLights[i].Amount = 1.0f;
		}
	}
}

void SavePresents()
{
	std::string resolvedPath;

	if (gAppPath.getFilePath("Present0.txt", resolvedPath))
	{
		fstream file;
		file.open( resolvedPath.c_str(), ios_base::out | ios_base::trunc );
		
		file << gCamera << std::endl;

		for (int i = 0; i < LIGHT_COUNT; ++i)
		{
			file << gLights[i].Camera << std::endl;
			file << gLights[i].Color.x << " " << gLights[i].Color.y << " " << gLights[i].Color.z << std::endl;
		}
	}
}


void DoUI()
{
	static nv::Rect none;
	
	gHub.begin();

	gHub.beginGroup( nv::GroupFlags_GrowDownFromLeft);

	std::stringstream fps; 
	fps << "Human Skin demo, FPS: " << gFPS ; 
	gHub.doLabel(none, fps.str().c_str());
	gHub.doCheckButton(none, "Draw Wireframe", &gOptions[OPTION_WIREFRAME]);
	gHub.doCheckButton(none, "Draw Skybox", &gOptions[OPTION_DRAW_SKYBOX]);
	gHub.doCheckButton(none, "Show Light", &gOptions[OPTION_SHOW_LIGHTS]);
	
	gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
	gHub.doRadioButton(0, none, "Camera",  &gManipulator);
	gHub.endGroup();

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
		std::stringstream s;
		s << "Light" << i;
		gHub.doRadioButton(1 + i, none, s.str().c_str(),  &gManipulator);
		gHub.doHorizontalSlider(none, 0.0f, 1.0f, &gLightColor[i]);
		gHub.endGroup();
	}

	gHub.endGroup();

	// Pass non-ui mouse events to the manipulator
	//if (!gHub.isOnFocus()) 
	//{
	//	const nv::ButtonState &lbState = gHub.getMouseState( 0);
	//	const nv::ButtonState &mbState = gHub.getMouseState( 1);
	//	const nv::ButtonState &rbState =  gHub.getMouseState( 2);

	//	/*CurrentObject()->HandleMouse(button, state, glutGetModifiers(), gHub.getCursorX(), gWindowHeight - gHub.getCursorY());*/

	//	//CurrentObject()->HandleMouse(button, state, glutGetModifiers(), gHub.getCursorX(), gWindowHeight - gHub.getCursorY());
	//}

	gHub.end();
}

Camera* CurrentObject() 
{ 
	switch (gManipulator) 
	{
	case OBJECT_CAMERA:
		return &gCamera;
	default:
		return &gLights[gManipulator - OBJECT_LIGHT1].Camera;
	}
}

void DrawLights()
{
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.013f, 0.013f, 0.013f));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gLightTexID);

	glUseProgram(gPipelineEffect.ProgramID);

	glUniformMatrix4fv(gPipelineEffect.ProjectionParam, 1, false, glm::value_ptr(gCamera.GetProjectionMatrix()));
	glUniformMatrix4fv(gPipelineEffect.ViewParam, 1, false, glm::value_ptr(gCamera.GetViewMatrix()));
	glUniform1ui(gPipelineEffect.AlbedoTexParam, 0);

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		glm::vec3 lightPos = gLights[i].Camera.GetEyePosition();
		glm::vec3 targetPos = gLights[i].Camera.GetLookAtPosition();
		glm::vec3 lightDir = glm::normalize(targetPos - lightPos);

		glm::vec3 tZ = glm::vec3(0, 0, 1); 
		glm::vec3 tX = glm::cross(lightDir, tZ);
		tX = glm::normalize(tX);
		tZ = glm::cross(tX, lightDir);

		glm::mat4 orientation;
		orientation = glm::column(orientation, 0, glm::vec4(tX, 0.0f));
		orientation = glm::column(orientation, 1, glm::vec4(lightDir, 0.0f));
		orientation = glm::column(orientation, 2, glm::vec4(tZ, 0.0f));

		glm::mat4 baseRot = glm::rotate(glm::mat4(1.0f), 180.0f, glm::vec3(1, 0, 0));

		glm::mat4 world = glm::translate(glm::mat4(1.0f), lightPos) * orientation * baseRot * scale;
		glUniformMatrix4fv(gPipelineEffect.WorldParam, 1, false, glm::value_ptr(world));
		
		DrawModel(&gLightModel, gLightModelVBO, gLightModelIBO);
	}

	glUseProgram(0);
}

void DrawQuad(int w, int h)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(0.0, h);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(w, h);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(w, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(0.0, 0.0);
	glEnd();
}

void DrawViewport(RenderTexture *src)
{
	SetOrthoProjection(gWindowWidth, gWindowHeight);
	glActiveTexture(GL_TEXTURE0);
	src->Bind();

	glDisable(GL_DEPTH_TEST);

	DrawQuad(gWindowWidth, gWindowHeight);

	glEnable(GL_DEPTH_TEST);

	src->Release();
}

void MakeShadowMap()
{
	ShadowMap::SetWorldMatrix(gModelWorld);

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		if (glm::length(gLights[i].Color) > 0.0f)
		{
			gLights[i].ShadowMap->Begin(gLights[i].Camera.GetViewMatrix(), gLights[i].Camera.GetProjectionMatrix());
				DrawModel(&gModel, gModelVBO, gModelIBO);
			gLights[i].ShadowMap->End();

			//std::stringstream sss; sss<<"depth" << i << ".pfm";
			//Utility::SaveTextureToPfm(sss.str().c_str(), gLights[i].ShadowMap->GetTexture()->GetColorTex(), SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

			//sss.clear(); sss<<"depthT" << i << ".pfm";
			//Utility::SaveTextureToPfmDepth(sss.str().c_str(), gLights[i].ShadowMap->GetTexture()->GetDepthTex(), SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
		}
	}
}

void ConvolutionStretch(RenderTexture* src, RenderTexture* dest, int itr)
{
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	// convolution U
	gTempBuffer->Activate();
	//SetOrthoProjection(BUFFER_SIZE, BUFFER_SIZE);

	glUseProgram(gConvStretchUEffect.ProgramID);

	glActiveTexture(GL_TEXTURE0);
	src->Bind();
	glUniform1i(gConvStretchUEffect.InputTexParam, src->GetColorTex());
	glUniform1f(gConvStretchUEffect.GaussWidthParam, sqrtf(gConvolutionScale[itr]));
	DrawQuad(BUFFER_SIZE, BUFFER_SIZE);

	src->Release();
	gTempBuffer->Deactivate();

	// convolution V
	dest->Activate();

	glUseProgram(gConvStretchVEffect.ProgramID);

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Bind();
	glUniform1i(gConvStretchVEffect.InputTexParam, gTempBuffer->GetColorTex());
	glUniform1f(gConvStretchVEffect.GaussWidthParam, sqrtf(gConvolutionScale[itr]));
	DrawQuad(BUFFER_SIZE, BUFFER_SIZE);

	glUseProgram(0);
	gTempBuffer->Release();
	dest->Deactivate();

	glPopAttrib();
}

void MakeStretchMap()
{
	glDisable(GL_TEXTURE_2D);

	gStretchBuffer[0]->Activate();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glUseProgram(gStretchEffect.ProgramID);

	glUniformMatrix4fv(gStretchEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));
	glUniform1f(gStretchEffect.ScaleParam, 0.001f);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);
	glPopAttrib(); 

	gStretchBuffer[0]->Deactivate();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	Utility::SaveTextureToTGA("StrechXY.tga", gStretchBuffer[0]->GetColorTex(), BUFFER_SIZE, BUFFER_SIZE);

	ConvolutionStretch(gStretchBuffer[0], gStretchBuffer[1], 0);
	ConvolutionStretch(gStretchBuffer[1], gStretchBuffer[2], 1);
	ConvolutionStretch(gStretchBuffer[2], gStretchBuffer[3], 2);
	ConvolutionStretch(gStretchBuffer[3], gStretchBuffer[4], 3);	
}

void RenderNormal()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (gOptions[OPTION_WIREFRAME])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glUseProgram(gPipelineEffect.ProgramID);

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, gAlbedoTexID);
	gIrradianceBuffer[0][0]->BindColor();
	glUniform1ui(gPipelineEffect.AlbedoTexParam, 0);

	glUniformMatrix4fv(gPipelineEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));
	//glUniformMatrix4fv(gPipelineEffect.ProjectionParam, 1, false, glm::value_ptr(gCamera.GetProjectionMatrix()));
	//glUniformMatrix4fv(gPipelineEffect.ViewParam, 1, false, glm::value_ptr(gCamera.GetViewMatrix()));
	
	glUniformMatrix4fv(gPipelineEffect.ProjectionParam, 1, false, glm::value_ptr(CurrentObject()->GetProjectionMatrix()));
	glUniformMatrix4fv(gPipelineEffect.ViewParam, 1, false, glm::value_ptr(CurrentObject()->GetViewMatrix()));

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);
	if (gOptions[OPTION_SHOW_LIGHTS])
	{
		DrawLights();
	}	

	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Texture space lighting to get Irradiance 
 */
void RenderIrradiance()
{
	gIrradianceBuffer[0][0]->Activate();

	glPushAttrib(GL_VIEWPORT_BIT);	
	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(gTexSpaceLightEffect.ProgramID);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gAlbedoTexID);
	glUniform1i(gTexSpaceLightEffect.AlbedoTexParam, 0);
	
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gNormalTexID);
	glUniform1i(gTexSpaceLightEffect.NormalTexParam, 1);

	glUniformMatrix4fv(gTexSpaceLightEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));

	static glm::mat4 bais(glm::vec4(0.5, 0, 0, 0), 
		glm::vec4(0, 0.5, 0, 0),
		glm::vec4(0, 0, 0.5, 0),
		glm::vec4(0.5, 0.5, 0.5, 1.0));

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE3 + i);
		glEnable(GL_TEXTURE_2D);
		gLights[i].ShadowMap->GetTexture()->BindColor(0);
		glUniform1i(gTexSpaceLightEffect.ShadowTexParam + i, 3 + i);	

		glm::mat4 shadowMatrix = bais * gLights[i].Camera.GetProjectionMatrix() * gLights[i].Camera.GetViewMatrix();
		glUniformMatrix4fv(gTexSpaceLightEffect.ShadowMatrixParam + i, 1, false, glm::value_ptr(shadowMatrix));

		glUniform3fv(gTexSpaceLightEffect.LightsPosParam + i * 3, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gTexSpaceLightEffect.LightsColorParam + i * 3, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gTexSpaceLightEffect.LightsAmountParam + i * 3, gLights[i].Amount);
	}

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, gRhodTexID);
	glUniform1ui(gTexSpaceLightEffect.Rho_d_TexParam, 6);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvTexID);
	glUniform1ui(gTexSpaceLightEffect.EnvCubeParam, 7);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);
	glPopAttrib();

	gIrradianceBuffer[0][0]->Deactivate();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Utility::SaveTextureToPfm("irr.pfm", gIrradianceBuffer[0][0]->GetColorTex(), BUFFER_SIZE, BUFFER_SIZE);
	
	//Utility::SaveTextureToTGA("irr.tga", gIrradianceBuffer[0][0]->GetColorTex(), BUFFER_SIZE, BUFFER_SIZE);
}

void Init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);


	ShadowMap::Init();

	LoadModel();
	LoadShaderEffect();
	LoadPresents();
	CreateBuffers(GL_RGBA32F_ARB);
}

void Reshape( int w, int h)
{
	gWindowWidth = w;
	gWindowHeight = h;
	glViewport( 0, 0, gWindowWidth, gWindowHeight);
	
	gHub.reshape(w, h);

	glm::vec2 vpSize = glm::vec2((float) w, (float) h);
	float aspect = (float)gWindowWidth / gWindowHeight;

	gCamera.SetProjection(CAMERA_FOV, aspect, 0.1f, 100.0f);
	gCamera.SetViewportSize(vpSize);

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		gLights[i].Camera.SetViewportSize(vpSize);
	}
}

void Draw()
{
	CalculateFPS();

	float elapsedTime = gElapsedTime / (float)1000;

	gCamera.Update(elapsedTime);
	for (int i = 0; i < LIGHT_COUNT; i++)
		gLights[i].Camera.Update(elapsedTime);

	MakeShadowMap();
	
	/*if (gStrechMapDirty)
	{
		MakeStretchMap();
		gStrechMapDirty = false;
	}*/
	

	switch (gMode)
	{
	case RM_NORMAL:
		{
			RenderIrradiance();
			RenderNormal();
			
		}
		break;
	case RM_SHADOW_0:
	case RM_SHADOW_0 + 1:
	case RM_SHADOW_0 + 2:
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClearDepth(1.0f);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			SetOrthoProjection(gWindowWidth, gWindowHeight);
			glActiveTexture(GL_TEXTURE0);
			gLights[gMode - RM_SHADOW_0].ShadowMap->GetTexture()->Bind();
			DrawQuad(gWindowWidth, gWindowHeight);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		break;
	default:
		break;
	}

	DoUI();
	glutSwapBuffers();
}

void Idle()
{
	/*CalculateFPS();
	
	float elapsedTime = gElapsedTime / (float)1000;
	
	gCamera.Update(elapsedTime);
	for (int i = 0; i < LIGHT_COUNT; i++)
		gLights[i].Camera.Update(elapsedTime);*/

	glutPostRedisplay();
}

void Keys(unsigned char c, int x, int y) 
{
	switch(c)
	{
	case '1':
	case '2':
	case '3':
		{
			gCamera = gLights[c - '1'].Camera;
		}
	break;
	case '0':
		{
			gMode = RM_NORMAL;
		}
		break;
	case '4':
	case '5':
	case '6':
		{
			gMode = RM_SHADOW_0 + c - '4';
		}
		break;
	case 13:
		{
			SavePresents();
		}
		break;
	default:
		break;
	}
}

void Special( int c, int x, int y)
{

}

void Mouse(int button, int state, int x, int y)
{
	// Pass non-ui mouse events to the manipulator
	gHub.mouse(button, state, x, y);

	if (!gHub.isOnFocus()) 
	{
		CurrentObject()->HandleMouse(button, state, glutGetModifiers(), x, gWindowHeight - y);
	}
}

void Motion(int x, int y)
{
	gHub.mouseMotion(x, y);
	if (!gHub.isOnFocus()) 
	{
		CurrentObject()->HandleMotion(x, gWindowHeight - y);
	}
}


int main( int argc, char** argv) {
	glutInit( &argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize( gWindowWidth, gWindowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow( "Human Head Viewer");

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("Error: %s\n", glewGetErrorString(err));
	}

	if (WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(0);
	}

	gAppPath.addPath("../Media/");
	gAppPath.addPath("../Media/Models/");
	gAppPath.addPath("../Media/Shaders/");
	gAppPath.addPath("../Media/Textures/");
	gAppPath.addPath("../Media/Presents/");

	glutDisplayFunc(Draw);
	glutIdleFunc(Idle);
	glutKeyboardFunc( Keys);
	glutSpecialFunc(Special);
	glutReshapeFunc(Reshape);
	glutMouseFunc( Mouse);
	glutMotionFunc( Motion);

	Init();

	glutMainLoop();

	return 0;
}
