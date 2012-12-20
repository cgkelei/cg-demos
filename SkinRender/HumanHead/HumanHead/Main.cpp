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

const char* HeadFile = "Infinite-Level_02.OBJ";
const char* HeadAlbedo =  "Map-COL.dds";
const char* HeadNormal =  "Infinite-Level_02_World_SmoothUV.dds";
const char* EnvMap = "Env.dds";
const char* RhodMap = "rho_d.png";

#define LIGHT_COUNT 3
#define BLOOM_BUFFER_COUNT 2
#define TEXTURE_BUFFER_COUNT 6
#define BUFFER_SIZE 800
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


int gWindowWidth = 800;
int gWindowHeight = 800;

nv::SDKPath gAppPath;
nv::GlutUIContext gHub;

float gRoughness = 0.3;
float gRho_s = 0.18;
float gStrechScale = 0.0014444444f;

nv::Model gLightModel;
GLuint gLightTexID, gLightModelVBO, gLightModelIBO;

nv::Model gModel;
float gModelDiameter = 0.0f; //diameter of the model
glm::vec3 gCenter;	// center of the model
glm::mat4 gModelWorld;

GLuint gModelVBO, gModelIBO;
GLuint gAlbedoTexID, gSpecTexID, gNormalTexID, gRhodTexID;
GLuint gEnvTexID;

bool gStrechMapDirty = true;		

Camera gCamera, gCameraTemp;

RenderTexture* gTempBuffer, *gFinalBuffer;
RenderTexture* gStretchBuffer[TEXTURE_BUFFER_COUNT];	// stretch texture blur storage
RenderTexture* gIrradianceBuffer[LIGHT_COUNT][TEXTURE_BUFFER_COUNT];

/**
 * First blur is so narrow, so we don't do it, just use the original irradiance texture
 */
float gConvolutionScale[TEXTURE_BUFFER_COUNT-1] = { /*0.0064, */0.0484f, 0.187f, 0.567f, 1.99f, 7.41f};


enum Object { OBJECT_CAMERA = 0, OBJECT_LIGHT1, OBJECT_LIGHT2, OBJECT_LIGHT3 };
int gManipulator = OBJECT_CAMERA;
bool gViewFromLight = false;

enum UIOption {
	OPTION_WIREFRAME = 0,
	OPTION_DRAW_SKYBOX,
	OPTION_SHOW_LIGHTS,
	OPTION_MANIPULATE_CAMERA,
	OPTION_MANIPULATE_LIGHT0,
	OPTION_SSS_WITH_TSM = OPTION_MANIPULATE_LIGHT0 + LIGHT_COUNT,
	OPTION_SSS_WITHOUT_TSM,
	OPTION_NoSSS,
	OPTION_COUNT,
};

bool gOptions[OPTION_COUNT];

enum RenderMode
{
	RM_NoSSS = 0,
	RM_SSS_Without_TSM,
	RM_SSS_With_TSM,
};
//int gMode = RM_SSS_With_TSM;
int gMode = RM_NoSSS;

struct PipelineEffect
{
	GLuint ProgramID;
	GLint WorldParam, ViewProjParam, AlbedoTexParam;
} gPipelineEffect;

struct StretchEffect
{
	GLuint ProgramID;
	GLint WorldParam, ScaleParam;

} gStretchEffect;

struct ConvolutionStretchEffect
{
	GLuint ProgramID;
	GLint GaussWidthParam, InputTexParam;

} gConvStretchUEffect, gConvStretchVEffect;

struct ConvolutionEffect
{
	GLuint ProgramID;
	GLint GaussWidthParam, StretchTexParam, InputTexParam;

} gConvolutionUEffect, gConvolutionVEffect;

struct TextureSpaceLightEffect
{
	GLuint ProgramID;
	GLint WorldParam, ShadowMatrixParam, AlbedoTexParam, SpecTexParam,
		  NormalTexParam, Rho_d_TexParam, EnvCubeParam, ShadowTexParam, RoughnessParam, Rho_sParam;
	GLint LightsPosParam, LightsColorParam, LightsAmountParam;

} gTexSpaceLightEffect;

struct TextureSpaceLightTSMEffect
{
	GLuint ProgramID;
	GLint WorldParam, ShadowMatrixParam, AlbedoTexParam, SpecTexParam,
		NormalTexParam, Rho_d_TexParam, EnvCubeParam, ShadowTexParam, RoughnessParam, Rho_sParam;
	GLint LightsPosParam, LightsColorParam, LightsAmountParam;

} gTexSpaceLightTSMEffect;


struct FinalSkinEffect
{
	GLuint ProgramID;
	GLint WorldParam, ViewProjParam, ShadowMatrixParam, AlbedoTexParam, SpecTexParam,
		NormalTexParam, Rho_d_TexParam, ShadowTexParam, IrradTexParam, GaussWeightsParam, EyePosParam;
	GLint LightsPosParam, LightsColorParam, LightsAmountParam;
	GLint DiffuseColorMixParam, RoughnessParam, Rho_sParam;

} gFinalSkinEffect;

struct FinalSkinTSMEffect
{
	GLuint ProgramID;
	GLint WorldParam, ViewProjParam, ShadowMatrixParam, AlbedoTexParam, SpecTexParam, StretchTexParam,
		NormalTexParam, Rho_d_TexParam, EnvCubeParam, ShadowTexParam, IrradTexParam, GaussWeightsParam, EyePosParam;
	GLint LightsPosParam, LightsColorParam, LightsAmountParam;
	GLint DiffuseColorMixParam, RoughnessParam, Rho_sParam;

} gFinalSkinTSMEffect;

struct PhongEffect
{
	GLuint ProgramID;
	GLint WorldParam, ShadowMatrixParam, ViewProjParam;
	GLint AlbedoTexParam, NormalTexParam, SpecTexParam , Rho_d_TexParam, EnvCubeParam, ShadowTexParam, EyePosParam;
	GLint LightsPosParam, LightsColorParam, LightsAmountParam, RoughnessParam, Rho_sParam;

} gPhongEffect;

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

//	static int i = 0;
//
//	*vbo = ++i;
//
//	glNewList(*vbo, GL_COMPILE);
//	
//	glVertexPointer( model->getPositionSize(), GL_FLOAT, model->getCompiledVertexSize() * sizeof(float), model->getCompiledVertices());
//	glNormalPointer( GL_FLOAT, model->getCompiledVertexSize() * sizeof(float), model->getCompiledVertices() + model->getCompiledNormalOffset());
//	glTexCoordPointer (model->getTexCoordSize(), GL_FLOAT, model->getCompiledVertexSize() * sizeof(float), model->getCompiledVertices() + model->getCompiledTexCoordOffset());
//
//	glEnableClientState( GL_VERTEX_ARRAY);
//	glEnableClientState( GL_NORMAL_ARRAY);
//	glEnableClientState( GL_TEXTURE_COORD_ARRAY);
//
//	glDrawElements( GL_TRIANGLES, model->getCompiledIndexCount( nv::Model::eptTriangles), GL_UNSIGNED_INT, model->getCompiledIndices( nv::Model::eptTriangles));
//
//	glDisableClientState( GL_VERTEX_ARRAY);
//	glDisableClientState( GL_NORMAL_ARRAY);
//	glDisableClientState( GL_TEXTURE_COORD_ARRAY);
//
//	glEndList();
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

	//glCallList(vbo);
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

	if (!gTempBuffer)
	{
		gTempBuffer = new RenderTexture(BUFFER_SIZE, BUFFER_SIZE, target); 
		gTempBuffer->InitColor_Tex(0, format);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		for (int j = 0; j < TEXTURE_BUFFER_COUNT; ++j)
		{
			gIrradianceBuffer[i][j] = new RenderTexture(BUFFER_SIZE, BUFFER_SIZE, target); 
			gIrradianceBuffer[i][j]->InitColor_Tex(0, format);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		}
		gIrradianceBuffer[i][0]->InitDepth_Tex();
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
	std::vector<Utility::ShaderMacro> shaderDefines;

	gPipelineEffect.ProgramID = Utility::LoadShaderEffect("FixedPipeline.vert", "FixedPipeline.frag");
	ASSERT(gPipelineEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.WorldParam, gPipelineEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.ViewProjParam, gPipelineEffect.ProgramID, "ViewProj");
	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.AlbedoTexParam, gPipelineEffect.ProgramID, "AlbedoTex");

	// StretchMap generate effect
	printf("Load StretchMap Effect...\n");
	gStretchEffect.ProgramID = Utility::LoadShaderEffect("Stretch.vert", "Stretch.frag");
	ASSERT(gStretchEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gStretchEffect.WorldParam, gStretchEffect.ProgramID, "WorldMat");
	RETRIEVE_UNIFORM_LOCATION(gStretchEffect.ScaleParam, gStretchEffect.ProgramID, "Scale");

	// ConvolutionStretchU effect
	printf("Load ConvolutionStretchU Effect...\n");
	gConvStretchUEffect.ProgramID = Utility::LoadShaderEffect("Convolution.vert", "ConvolutionStretchU.frag");
	ASSERT(gConvStretchUEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gConvStretchUEffect.InputTexParam, gConvStretchUEffect.ProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchUEffect.GaussWidthParam, gConvStretchUEffect.ProgramID, "GaussWidth");

	// ConvolutionStretchV effect
	printf("Load ConvolutionStretchV Effect...\n");
	gConvStretchVEffect.ProgramID = Utility::LoadShaderEffect("Convolution.vert", "ConvolutionStretchV.frag");
	ASSERT(gConvStretchVEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gConvStretchVEffect.InputTexParam, gConvStretchVEffect.ProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchVEffect.GaussWidthParam, gConvStretchVEffect.ProgramID, "GaussWidth");

	// ConvolutionU effect
	printf("Load ConvolutionU Effect...\n");
	gConvolutionUEffect.ProgramID = Utility::LoadShaderEffect("Convolution.vert", "ConvolutionU.frag");
	ASSERT(gConvolutionUEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gConvolutionUEffect.StretchTexParam, gConvolutionUEffect.ProgramID, "StretchTex");
	RETRIEVE_UNIFORM_LOCATION(gConvolutionUEffect.InputTexParam, gConvolutionUEffect.ProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gConvolutionUEffect.GaussWidthParam, gConvolutionUEffect.ProgramID, "GaussWidth");

	// ConvolutionV effect
	printf("Load ConvolutionV Effect...\n");
	gConvolutionVEffect.ProgramID = Utility::LoadShaderEffect("Convolution.vert", "ConvolutionV.frag");
	ASSERT(gConvolutionVEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gConvolutionVEffect.StretchTexParam, gConvolutionVEffect.ProgramID, "StretchTex");
	RETRIEVE_UNIFORM_LOCATION(gConvolutionVEffect.InputTexParam, gConvolutionVEffect.ProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gConvolutionVEffect.GaussWidthParam, gConvolutionVEffect.ProgramID, "GaussWidth");

	// Texture Space Light effect
	printf("Load Texture Space Light Without TSM  Effect...\n");
	shaderDefines.push_back(Utility::ShaderMacro("SHADOW_PCF", ""));
	gTexSpaceLightEffect.ProgramID = Utility::LoadShaderEffect("TextureSpaceLighting.vert", "TextureSpaceLighting.frag", &shaderDefines);
	ASSERT(gTexSpaceLightEffect.ProgramID > 0);
	//Utility::PrintEffectAttribs(gTexSpaceLightEffect.ProgramID);
	//Utility::PrintEffectUniforms(gTexSpaceLightEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.WorldParam, gTexSpaceLightEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightsColorParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightsPosParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightsAmountParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.ShadowTexParam, gTexSpaceLightEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.NormalTexParam, gTexSpaceLightEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.AlbedoTexParam, gTexSpaceLightEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.SpecTexParam, gTexSpaceLightEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.Rho_d_TexParam, gTexSpaceLightEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.EnvCubeParam, gTexSpaceLightEffect.ProgramID, "EnvCube");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.ShadowMatrixParam, gTexSpaceLightEffect.ProgramID, "ShadowMatrix[0]");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.RoughnessParam, gTexSpaceLightEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.Rho_sParam, gTexSpaceLightEffect.ProgramID, "Rho_s");

	// Final Skin Without TSM effect
	printf("Load Final Skin Without TSM effect...\n");
	gFinalSkinEffect.ProgramID = Utility::LoadShaderEffect("FinalSkin.vert", "FinalSkin.frag", &shaderDefines);
	ASSERT(gFinalSkinEffect.ProgramID > 0);
	//Utility::PrintEffectAttribs(gFinalSkinEffect.ProgramID);
	//Utility::PrintEffectUniforms(gFinalSkinEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.WorldParam, gFinalSkinEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.ViewProjParam, gFinalSkinEffect.ProgramID, "ViewProj");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightsColorParam, gFinalSkinEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightsPosParam, gFinalSkinEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightsAmountParam, gFinalSkinEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.ShadowTexParam, gFinalSkinEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.NormalTexParam, gFinalSkinEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.AlbedoTexParam, gFinalSkinEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.Rho_d_TexParam, gFinalSkinEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.SpecTexParam, gFinalSkinEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.ShadowMatrixParam, gFinalSkinEffect.ProgramID, "ShadowMatrix[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.GaussWeightsParam, gFinalSkinEffect.ProgramID, "GaussWeights[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.EyePosParam, gFinalSkinEffect.ProgramID, "EyePos");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.IrradTexParam, gFinalSkinEffect.ProgramID, "IrradTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.RoughnessParam, gFinalSkinEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.Rho_sParam, gFinalSkinEffect.ProgramID, "Rho_s");

	// Texture Space Light With TSM effect
	printf("Load Texture Space Light With TSM Effect...\n");
	gTexSpaceLightTSMEffect.ProgramID = Utility::LoadShaderEffect("TextureSpaceLightingTSM.vert", "TextureSpaceLightingTSM.frag", &shaderDefines);
	ASSERT(gTexSpaceLightTSMEffect.ProgramID > 0);
	Utility::PrintEffectAttribs(gTexSpaceLightTSMEffect.ProgramID);
	Utility::PrintEffectUniforms(gTexSpaceLightTSMEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.WorldParam, gTexSpaceLightTSMEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightsColorParam, gTexSpaceLightTSMEffect.ProgramID, "LightColor");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightsPosParam, gTexSpaceLightTSMEffect.ProgramID, "LightPos");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightsAmountParam, gTexSpaceLightTSMEffect.ProgramID, "LightAmount");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.ShadowTexParam, gTexSpaceLightTSMEffect.ProgramID,  "ShadowTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.NormalTexParam, gTexSpaceLightTSMEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.AlbedoTexParam, gTexSpaceLightTSMEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.Rho_d_TexParam, gTexSpaceLightTSMEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.SpecTexParam, gTexSpaceLightTSMEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.EnvCubeParam, gTexSpaceLightTSMEffect.ProgramID, "EnvCube");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.ShadowMatrixParam, gTexSpaceLightTSMEffect.ProgramID, "ShadowMatrix");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.RoughnessParam, gTexSpaceLightTSMEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.Rho_sParam, gTexSpaceLightTSMEffect.ProgramID, "Rho_s");

	//// Final Skin With TSM effect
	printf("Load Final Skin With TSM effect...\n");
	gFinalSkinTSMEffect.ProgramID = Utility::LoadShaderEffect("FinalSkinTSM.vert", "FinalSkinTSM.frag", &shaderDefines);
	ASSERT(gFinalSkinTSMEffect.ProgramID > 0);
	Utility::PrintEffectAttribs(gFinalSkinTSMEffect.ProgramID);
	Utility::PrintEffectUniforms(gFinalSkinTSMEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.WorldParam, gFinalSkinTSMEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.ViewProjParam, gFinalSkinTSMEffect.ProgramID, "ViewProj");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightsColorParam, gFinalSkinTSMEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightsPosParam, gFinalSkinTSMEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightsAmountParam, gFinalSkinTSMEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.ShadowTexParam, gFinalSkinTSMEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.NormalTexParam, gFinalSkinTSMEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.SpecTexParam, gFinalSkinTSMEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.StretchTexParam, gFinalSkinTSMEffect.ProgramID, "StretchTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.AlbedoTexParam, gFinalSkinTSMEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.Rho_d_TexParam, gFinalSkinTSMEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.ShadowMatrixParam, gFinalSkinTSMEffect.ProgramID, "ShadowMatrix[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.Rho_d_TexParam, gFinalSkinTSMEffect.ProgramID, "IrradTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.GaussWeightsParam, gFinalSkinTSMEffect.ProgramID, "GaussWeights[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.EyePosParam, gFinalSkinTSMEffect.ProgramID, "EyePos");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.IrradTexParam, gFinalSkinTSMEffect.ProgramID, "IrradTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.RoughnessParam, gFinalSkinTSMEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.Rho_sParam, gFinalSkinTSMEffect.ProgramID, "Rho_s");

	printf("Load Phong with Kelemen/Szirmay-Kalos Specular BRDF effect...\n");
	gPhongEffect.ProgramID = Utility::LoadShaderEffect("Phong.vert", "Phong.frag", &shaderDefines);
	//Utility::PrintEffectAttribs(gPhongEffect.ProgramID);
	//Utility::PrintEffectUniforms(gPhongEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.WorldParam, gPhongEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.ViewProjParam, gPhongEffect.ProgramID, "ViewProj");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightsColorParam, gPhongEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightsPosParam, gPhongEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightsAmountParam, gPhongEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.NormalTexParam, gPhongEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.AlbedoTexParam, gPhongEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.Rho_d_TexParam, gPhongEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.SpecTexParam, gPhongEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.EyePosParam, gPhongEffect.ProgramID, "EyePos");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.ShadowTexParam, gPhongEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.ShadowMatrixParam, gPhongEffect.ProgramID, "ShadowMatrix[0]");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.EnvCubeParam, gPhongEffect.ProgramID, "EnvCube");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.Rho_sParam, gPhongEffect.ProgramID, "Rho_s");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.RoughnessParam, gPhongEffect.ProgramID, "Roughness");
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
		gNormalTexID = Create2DTextureFromFile(HeadNormal);

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
			gLights[i].Amount = 0.3f;
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
			file << gLights[i].Camera;
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

	gHub.doLabel( none, "");
	gHub.doCheckButton(none, "Draw Wireframe", &gOptions[OPTION_WIREFRAME]);
	gHub.doCheckButton(none, "Draw Skybox", &gOptions[OPTION_DRAW_SKYBOX]);
	gHub.doCheckButton(none, "Show Light", &gOptions[OPTION_SHOW_LIGHTS]);
	gHub.doLabel( none, "");

	// TSM 
	gHub.doLabel( none, "Render Mode");
	gHub.doRadioButton(0, none, "No SSS",  &gMode);
	gHub.doRadioButton(1, none, "SSS Without TSM",  &gMode);
	gHub.doRadioButton(2, none, "SSS With TSM",  &gMode);
	gHub.doLabel( none, "");

	gHub.doLabel( none, "Render Parameters");
	gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
	gHub.doLabel(none, "Roughness");
	gHub.doHorizontalSlider(none, 0.0f, 1.0f, &gRoughness);
	gHub.endGroup();

	gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
	gHub.doLabel(none, "Rho_s");
	gHub.doHorizontalSlider(none, 0.0f, 1.0f, &gRho_s);
	gHub.endGroup();

	gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
	gHub.doLabel(none, "StrechScale");
	gHub.doHorizontalSlider(none, 0.0f, 0.002f, &gStrechScale);
	gHub.endGroup();

	gHub.doLabel( none, "");


	gHub.doLabel( none, "Manipulator Control");
	gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
	gHub.doRadioButton(0, none, "Camera",  &gManipulator);
	gHub.doCheckButton(none, "View From Light", &gViewFromLight);
	gHub.endGroup();

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
		std::stringstream s;
		s << "Light" << i;
		gHub.doRadioButton(1 + i, none, s.str().c_str(),  &gManipulator);
		gHub.doHorizontalSlider(none, 0.0f, 1.0f, &gLights[i].Amount);
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

Camera* CurrentManipulator() 
{ 
	switch (gManipulator) 
	{
	case OBJECT_CAMERA:
		return &gCamera;
	default:
		{
			return &gLights[gManipulator - OBJECT_LIGHT1].Camera;
		}	
	}
}

Camera* CurrentCamera()
{
	if ((gManipulator != OBJECT_CAMERA) && gViewFromLight)
	{
		return &gLights[gManipulator - OBJECT_LIGHT1].Camera;
	}

	return &gCamera;
}

void DrawLights()
{
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.013f, 0.013f, 0.013f));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gLightTexID);

	glUseProgram(gPipelineEffect.ProgramID);

	glm::mat4 veiwProj = CurrentManipulator()->GetProjectionMatrix() * CurrentManipulator()->GetViewMatrix();
	glUniformMatrix4fv(gPipelineEffect.ViewProjParam, 1, false, glm::value_ptr(veiwProj));
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
			gLights[i].ShadowMap->Begin(gLights[i].Camera.GetViewProjectionMatrix());
				DrawModel(&gModel, gModelVBO, gModelIBO);
			gLights[i].ShadowMap->End();
		}
	}
}

void ConvolutionStretch(RenderTexture* src, RenderTexture* dest, int itr)
{
	glPushAttrib(GL_VIEWPORT_BIT);

	// convolution U
	gTempBuffer->Activate();
	SetOrthoProjection(BUFFER_SIZE, BUFFER_SIZE);

	glUseProgram(gConvStretchUEffect.ProgramID);
	glUniform1f(gConvStretchUEffect.GaussWidthParam, sqrtf(gConvolutionScale[itr]));

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	src->Bind();
	glUniform1i(gConvStretchUEffect.InputTexParam, 0);
	
	DrawQuad(BUFFER_SIZE, BUFFER_SIZE);

	glActiveTexture(GL_TEXTURE0);
	src->Release();

	gTempBuffer->Deactivate();

	// convolution V
	dest->Activate();

	glUseProgram(gConvStretchVEffect.ProgramID);

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Bind();
	glUniform1i(gConvStretchVEffect.InputTexParam, 0);
	//glUniform1f(gConvStretchVEffect.GaussWidthParam, sqrtf(gConvolutionScale[itr]));
	
	float gaussWidth;
	if (itr == 0)
	{
		gaussWidth = sqrtf(gConvolutionScale[itr]);
	}
	else
	{
		gaussWidth =  sqrtf(gConvolutionScale[itr] - gConvolutionScale[itr-1]);
	}
	glUniform1f(gConvolutionVEffect.GaussWidthParam, gaussWidth);

	DrawQuad(BUFFER_SIZE, BUFFER_SIZE);

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Release();

	dest->Deactivate();

	glUseProgram(0);
	glPopAttrib();
}

void Convolution(RenderTexture* src, RenderTexture* dest, int itr)
{
	glPushAttrib(GL_VIEWPORT_BIT);

	// convolution U
	gTempBuffer->Activate();
	SetOrthoProjection(BUFFER_SIZE, BUFFER_SIZE);
	
	glUseProgram(gConvolutionUEffect.ProgramID);
	glUniform1f(gConvolutionUEffect.GaussWidthParam, sqrtf(gConvolutionScale[itr]));

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	src->Bind();
	glUniform1i(gConvolutionUEffect.InputTexParam, 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	gStretchBuffer[itr]->Bind();
	glUniform1i(gConvolutionUEffect.StretchTexParam, 1);	

	DrawQuad(BUFFER_SIZE, BUFFER_SIZE);
	
	gTempBuffer->Deactivate();

	// convolution V
	dest->Activate();

	glUseProgram(gConvolutionVEffect.ProgramID);
	//glUniform1f(gConvolutionVEffect.GaussWidthParam, sqrtf(gConvolutionScale[itr]));

	float gaussWidth;
	if (itr == 0)
	{
		gaussWidth = sqrtf(gConvolutionScale[itr]);
	}
	else
	{
		gaussWidth =  sqrtf(gConvolutionScale[itr] - gConvolutionScale[itr-1]);
	}
	glUniform1f(gConvolutionVEffect.GaussWidthParam, gaussWidth);
	

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Bind();
	glUniform1i(gConvolutionVEffect.InputTexParam, 0);

	glActiveTexture(GL_TEXTURE1);
	gStretchBuffer[itr]->Bind();
	glUniform1i(gConvolutionVEffect.StretchTexParam, 1);	

	DrawQuad(BUFFER_SIZE, BUFFER_SIZE);

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Release();

	glActiveTexture(GL_TEXTURE1);
	gStretchBuffer[itr]->Release();

	dest->Deactivate();

	glUseProgram(0);
	glPopAttrib();
}

void MakeStretchMap()
{
	gStretchBuffer[0]->Activate();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gStretchEffect.ProgramID);

	glUniformMatrix4fv(gStretchEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));
	glUniform1f(gStretchEffect.ScaleParam, gStrechScale);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);
	
	gStretchBuffer[0]->Deactivate();
	glPopAttrib(); 

	//Utility::SaveTextureToPfm("StrechXY0.pfm", gStretchBuffer[0]->GetColorTex(), BUFFER_SIZE, BUFFER_SIZE);
	ConvolutionStretch(gStretchBuffer[0], gStretchBuffer[1], 0);
	ConvolutionStretch(gStretchBuffer[1], gStretchBuffer[2], 1);
	ConvolutionStretch(gStretchBuffer[2], gStretchBuffer[3], 2);
	ConvolutionStretch(gStretchBuffer[3], gStretchBuffer[4], 3);	
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

	glUseProgram(gTexSpaceLightEffect.ProgramID);

	glUniform1f(gTexSpaceLightEffect.Rho_sParam, gRho_s);
	glUniform1f(gTexSpaceLightEffect.RoughnessParam, gRoughness);
	glUniformMatrix4fv(gTexSpaceLightEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gAlbedoTexID);
	glUniform1i(gTexSpaceLightEffect.AlbedoTexParam, 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gNormalTexID);
	glUniform1i(gTexSpaceLightEffect.NormalTexParam, 1);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gSpecTexID);
	glUniform1i(gTexSpaceLightEffect.SpecTexParam, 2);

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE3 + i);
		glEnable(GL_TEXTURE_2D);
		gLights[i].ShadowMap->GetRenderTexture()->BindColor(0);
		glUniform1i(gTexSpaceLightEffect.ShadowTexParam + i, 3 + i);	

		glUniformMatrix4fv(gTexSpaceLightEffect.ShadowMatrixParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetShadowMatrix()));

		glUniform3fv(gTexSpaceLightEffect.LightsPosParam + i * 3, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gTexSpaceLightEffect.LightsColorParam + i * 3, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gTexSpaceLightEffect.LightsAmountParam + i * 3, gLights[i].Amount);
	}

	glActiveTexture(GL_TEXTURE6);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gRhodTexID);
	glUniform1i(gTexSpaceLightEffect.Rho_d_TexParam, 6);

	glActiveTexture(GL_TEXTURE7);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvTexID);
	glUniform1i(gTexSpaceLightEffect.EnvCubeParam, 7);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	gIrradianceBuffer[0][0]->Deactivate();
	glUseProgram(0);
	glPopAttrib();

	//convolve 5 times!!!!!
	Convolution(gIrradianceBuffer[0][0], gIrradianceBuffer[0][1], 0);
	Convolution(gIrradianceBuffer[0][1], gIrradianceBuffer[0][2], 1);
	Convolution(gIrradianceBuffer[0][2], gIrradianceBuffer[0][3], 2);
	Convolution(gIrradianceBuffer[0][3], gIrradianceBuffer[0][4], 3);
	Convolution(gIrradianceBuffer[0][4], gIrradianceBuffer[0][5], 4);
	

	//for (int i = 0; i < TEXTURE_BUFFER_COUNT; ++i)
	//{
	//	std::stringstream sss;
	//	sss << "blurIrradiance" << i << ".pfm";
	//	Utility::SaveTextureToPfm(sss.str().c_str(), gIrradianceBuffer[0][i]->GetColorTex(), BUFFER_SIZE, BUFFER_SIZE);
	//}
}

void RenderIrradianceTSM()
{
	glPushAttrib(GL_VIEWPORT_BIT);	
	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	glUseProgram(gTexSpaceLightTSMEffect.ProgramID);

	glUniform1f(gTexSpaceLightTSMEffect.Rho_sParam, gRho_s);
	glUniform1f(gTexSpaceLightTSMEffect.RoughnessParam, gRoughness);
	glUniformMatrix4fv(gTexSpaceLightTSMEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gAlbedoTexID);
	glUniform1i(gTexSpaceLightTSMEffect.AlbedoTexParam, 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gNormalTexID);
	glUniform1i(gTexSpaceLightTSMEffect.NormalTexParam, 1);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gSpecTexID);
	glUniform1i(gTexSpaceLightTSMEffect.SpecTexParam, 2);

	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gRhodTexID);
	glUniform1i(gTexSpaceLightTSMEffect.Rho_d_TexParam, 3);

	glActiveTexture(GL_TEXTURE4);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvTexID);
	glUniform1i(gTexSpaceLightTSMEffect.EnvCubeParam, 4);

	for (int i = 0 ; i < LIGHT_COUNT; ++i)
	{
		gIrradianceBuffer[i][0]->Activate();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE5);
		glEnable(GL_TEXTURE_2D);
		gLights[i].ShadowMap->GetRenderTexture()->BindColor(0);
		glUniform1i(gTexSpaceLightTSMEffect.ShadowTexParam , 5);	

		glUniformMatrix4fv(gTexSpaceLightTSMEffect.ShadowMatrixParam , 1, false, glm::value_ptr(gLights[i].Camera.GetShadowMatrix()));

		glUniform3fv(gTexSpaceLightTSMEffect.LightsPosParam, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gTexSpaceLightTSMEffect.LightsColorParam , 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gTexSpaceLightTSMEffect.LightsAmountParam, gLights[i].Amount);

		DrawModel(&gModel, gModelVBO, gModelIBO);

		gIrradianceBuffer[i][0]->Deactivate();
	}
	
	glUseProgram(0);
	glPopAttrib();
	
	
	//convolve 5 times!!!!!
	for (int i = 0 ; i < LIGHT_COUNT; ++i)
	{
		Convolution(gIrradianceBuffer[i][0], gIrradianceBuffer[i][1], 0);
		Convolution(gIrradianceBuffer[i][1], gIrradianceBuffer[i][2], 1);
		Convolution(gIrradianceBuffer[i][2], gIrradianceBuffer[i][3], 2);
		Convolution(gIrradianceBuffer[i][3], gIrradianceBuffer[i][4], 3);
		Convolution(gIrradianceBuffer[i][4], gIrradianceBuffer[i][5], 4);

		/*std::stringstream sss;
		sss << "light" << i << "Irr.pfm";
		Utility::SaveTextureToPfm(sss.str().c_str(), gIrradianceBuffer[i][0]->GetColorTex(), BUFFER_SIZE, BUFFER_SIZE);*/
	}
}

void RenderFinal()
{	
	//gIrradianceBuffer[1][0]->Activate();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (gOptions[OPTION_WIREFRAME])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glUseProgram(gFinalSkinEffect.ProgramID);

	glUniformMatrix4fv(gFinalSkinEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));

	Camera* currentCam = CurrentCamera();
	glUniformMatrix4fv(gFinalSkinEffect.ViewProjParam, 1, false, glm::value_ptr(currentCam->GetViewProjectionMatrix()));
	glUniform3fv(gFinalSkinEffect.EyePosParam, 1, glm::value_ptr(currentCam->GetEyePosition()));

	glUniform3f(gFinalSkinEffect.GaussWeightsParam + 0, 0.233f, 0.455f, 0.649f);
	glUniform3f(gFinalSkinEffect.GaussWeightsParam + 1, 0.1f, 0.336f, 0.344f);
	glUniform3f(gFinalSkinEffect.GaussWeightsParam + 2, 0.118f, 0.198f, 0.0f);
	glUniform3f(gFinalSkinEffect.GaussWeightsParam + 3, 0.113f, 0.007f, 0.007f);
	glUniform3f(gFinalSkinEffect.GaussWeightsParam + 4, 0.358f, 0.004f, 0.0f);
	glUniform3f(gFinalSkinEffect.GaussWeightsParam + 5, 0.078f, 0.0f, 0.0f);

	glUniform1f(gFinalSkinEffect.Rho_sParam, gRho_s);
	glUniform1f(gFinalSkinEffect.RoughnessParam, gRoughness);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gAlbedoTexID);
	glUniform1i(gFinalSkinEffect.AlbedoTexParam, 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gNormalTexID);
	glUniform1i(gFinalSkinEffect.NormalTexParam, 1);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gSpecTexID);
	glUniform1i(gFinalSkinEffect.SpecTexParam, 2);

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE3 + i);
		glEnable(GL_TEXTURE_2D);
		gLights[i].ShadowMap->GetRenderTexture()->Bind();
		glUniform1i(gFinalSkinEffect.ShadowTexParam + i, 3 + i);	

		glUniformMatrix4fv(gFinalSkinEffect.ShadowMatrixParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetShadowMatrix()));

		glUniform3fv(gFinalSkinEffect.LightsPosParam + i * 3, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gFinalSkinEffect.LightsColorParam + i * 3, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gFinalSkinEffect.LightsAmountParam + i * 3, gLights[i].Amount);
	}

	for (int i = 0; i < 6; ++i)
	{
		glActiveTexture(GL_TEXTURE3 + LIGHT_COUNT + i);
		glEnable(GL_TEXTURE_2D);
		gIrradianceBuffer[0][i]->BindColor(0);
		glUniform1i(gFinalSkinEffect.IrradTexParam + i, 6 + i);	
	}

	glActiveTexture(GL_TEXTURE12);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gRhodTexID);
	glUniform1i(gFinalSkinEffect.Rho_d_TexParam, 12);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	//gIrradianceBuffer[1][0]->Deactivate();

	//Utility::SaveTextureToPfm("final.pfm",gIrradianceBuffer[1][0]->GetColorTex(), BUFFER_SIZE, BUFFER_SIZE);

	for (int i = 0; i < 8; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
}

void RenderFinalTSM()
{
	//gStretchBuffer[4]->Activate();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (gOptions[OPTION_WIREFRAME])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glUseProgram(gFinalSkinTSMEffect.ProgramID);

	glUniformMatrix4fv(gFinalSkinTSMEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));

	Camera* currentCam = CurrentCamera();
	glUniformMatrix4fv(gFinalSkinTSMEffect.ViewProjParam, 1, false, glm::value_ptr(currentCam->GetViewProjectionMatrix()));
	glUniform3fv(gFinalSkinTSMEffect.EyePosParam, 1, glm::value_ptr(currentCam->GetEyePosition()));

	glUniform3f(gFinalSkinTSMEffect.GaussWeightsParam + 0, 0.233f, 0.455f, 0.649f);
	glUniform3f(gFinalSkinTSMEffect.GaussWeightsParam + 1, 0.1f, 0.336f, 0.344f);
	glUniform3f(gFinalSkinTSMEffect.GaussWeightsParam + 2, 0.118f, 0.198f, 0.0f);
	glUniform3f(gFinalSkinTSMEffect.GaussWeightsParam + 3, 0.113f, 0.007f, 0.007f);
	glUniform3f(gFinalSkinTSMEffect.GaussWeightsParam + 4, 0.358f, 0.004f, 0.0f);
	glUniform3f(gFinalSkinTSMEffect.GaussWeightsParam + 5, 0.078f, 0.0f, 0.0f);

	glUniform1f(gFinalSkinTSMEffect.Rho_sParam, gRho_s);
	glUniform1f(gFinalSkinTSMEffect.RoughnessParam, gRoughness);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gAlbedoTexID);
	glUniform1i(gFinalSkinTSMEffect.AlbedoTexParam, 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gNormalTexID);
	glUniform1i(gFinalSkinTSMEffect.NormalTexParam, 1);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gSpecTexID);
	glUniform1i(gFinalSkinTSMEffect.SpecTexParam, 2);

	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	gStretchBuffer[2]->Bind();
	glUniform1i(gFinalSkinTSMEffect.StretchTexParam, 3);

	glActiveTexture(GL_TEXTURE4);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gRhodTexID);
	glUniform1i(gFinalSkinTSMEffect.Rho_d_TexParam, 4);


	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE5 + i);
		glEnable(GL_TEXTURE_2D);
		gLights[i].ShadowMap->GetRenderTexture()->Bind();
		glUniform1i(gFinalSkinTSMEffect.ShadowTexParam + i, 5 + i);	

		glUniformMatrix4fv(gFinalSkinTSMEffect.ShadowMatrixParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetShadowMatrix()));

		glUniform3fv(gFinalSkinTSMEffect.LightsPosParam + i * 3, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gFinalSkinTSMEffect.LightsColorParam + i * 3, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gFinalSkinTSMEffect.LightsAmountParam + i * 3, gLights[i].Amount);

		for (int j = 0; j < 6; ++j)
		{
			glActiveTexture(GL_TEXTURE5 + LIGHT_COUNT + i * 6 + j);
			glEnable(GL_TEXTURE_2D);
			gIrradianceBuffer[i][j]->BindColor(0);
			auto ss = GL_TEXTURE5 + LIGHT_COUNT + i * 6 + j - GL_TEXTURE0;
			auto p = gFinalSkinTSMEffect.IrradTexParam + i * 6 + j;
			glUniform1i(gFinalSkinTSMEffect.IrradTexParam + i * 6 + j, GL_TEXTURE5 + LIGHT_COUNT + i * 6 + j - GL_TEXTURE0);		
		}
	}
	
	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//gStretchBuffer[4]->Deactivate();

	//Utility::SaveTextureToPfm("final.pfm",gStretchBuffer[4]->GetColorTex(), BUFFER_SIZE, BUFFER_SIZE);

	for (int i = 0; i < 8; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void RenderNormal()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (gOptions[OPTION_WIREFRAME])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glUseProgram(gPipelineEffect.ProgramID);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, gAlbedoTexID);
	gIrradianceBuffer[0][0]->BindColor();
	glUniform1i(gPipelineEffect.AlbedoTexParam, 0);

	glUniformMatrix4fv(gPipelineEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));

	auto eye = CurrentManipulator()->GetEyePosition();

	Camera* currentCam = CurrentManipulator();
	glm::mat4 viewProj = currentCam->GetProjectionMatrix() * currentCam->GetViewMatrix();
	glUniformMatrix4fv(gPipelineEffect.ViewProjParam, 1, false, glm::value_ptr(viewProj));

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);
	if (gOptions[OPTION_SHOW_LIGHTS])
	{
		DrawLights();
	}	


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderPhong()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (gOptions[OPTION_WIREFRAME])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glUseProgram(gPhongEffect.ProgramID);

	glUniformMatrix4fv(gPhongEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));

	Camera* currentCam = CurrentCamera();
	glm::mat4 viewProj = currentCam->GetProjectionMatrix() * currentCam->GetViewMatrix();
	glUniformMatrix4fv(gPhongEffect.ViewProjParam, 1, false, glm::value_ptr(viewProj));
	glUniform3fv(gPhongEffect.EyePosParam, 1, glm::value_ptr(currentCam->GetEyePosition()));

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gAlbedoTexID);
	glUniform1i(gPhongEffect.AlbedoTexParam, 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gNormalTexID);
	glUniform1i(gPhongEffect.NormalTexParam, 1);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gSpecTexID);
	glUniform1i(gPhongEffect.SpecTexParam, 2);

	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gRhodTexID);
	glUniform1i(gPhongEffect.Rho_d_TexParam, 3);

	glActiveTexture(GL_TEXTURE4);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvTexID);
	glUniform1i(gPhongEffect.EnvCubeParam, 4);

	static glm::mat4 bais(glm::vec4(0.5, 0, 0, 0), 
		glm::vec4(0, 0.5, 0, 0),
		glm::vec4(0, 0, 0.5, 0),
		glm::vec4(0.5, 0.5, 0.5, 1.0));

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE5 + i);
		glEnable(GL_TEXTURE_2D);
		gLights[i].ShadowMap->GetRenderTexture()->BindColor(0);
		glUniform1i(gPhongEffect.ShadowTexParam + i, 5 + i);	

		glm::mat4 shadowMatrix = bais * gLights[i].Camera.GetProjectionMatrix() * gLights[i].Camera.GetViewMatrix();
		glUniformMatrix4fv(gPhongEffect.ShadowMatrixParam + i, 1, false, glm::value_ptr(shadowMatrix));

		glUniform3fv(gPhongEffect.LightsPosParam + i * 3, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gPhongEffect.LightsColorParam + i * 3, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gPhongEffect.LightsAmountParam + i * 3, gLights[i].Amount);
	}

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);
	if (gOptions[OPTION_SHOW_LIGHTS])
	{
		DrawLights();
	}	


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);


	for (int i = 0; i < 8; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void RenderScene()
{
	CalculateFPS();

	/*char strFPS[100];
	sprintf(strFPS, "%dfps", gFPS);
	glutSetWindowTitle(strFPS);*/

	float elapsedTime = gElapsedTime / (float)1000;

	gCamera.Update(elapsedTime);
	for (int i = 0; i < LIGHT_COUNT; i++)
		gLights[i].Camera.Update(elapsedTime);


	MakeShadowMap();

	if (gStrechMapDirty)
	{
		MakeStretchMap();
		gStrechMapDirty = false;
	}

	switch (gMode)
	{
	case RM_NoSSS:
		{
			RenderPhong();
		}
		break;
	case RM_SSS_Without_TSM:
		{
			RenderIrradiance();
			RenderFinal();
		}
		break;
	case RM_SSS_With_TSM:
		{
			RenderIrradianceTSM();
			RenderFinalTSM();
		}
		break;

	default:
		break;
	}

	if (gOptions[OPTION_SHOW_LIGHTS])
	{
		DrawLights();
	}

	if (gOptions[OPTION_DRAW_SKYBOX])
	{

	}
	
	DoUI();
	glutSwapBuffers();
}


void Init()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	//init mode
	gOptions[OPTION_SSS_WITH_TSM] = false;
	gOptions[OPTION_SSS_WITHOUT_TSM] = true;

	/*gOptions[OPTION_SSS_WITH_TSM] = true;
	gOptions[OPTION_SSS_WITHOUT_TSM] = false;*/

	gOptions[OPTION_DRAW_SKYBOX] = true;
	gOptions[OPTION_SHOW_LIGHTS] = false;
	gOptions[OPTION_WIREFRAME] = false;
	gOptions[OPTION_NoSSS] = false;

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

void Idle()
{
	glutPostRedisplay();
}

void Keys(unsigned char c, int x, int y) 
{
	switch(c)
	{
	case ' ':
		SavePresents();
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
		CurrentManipulator()->HandleMouse(button, state, glutGetModifiers(), x, gWindowHeight - y);
	}
}

void Motion(int x, int y)
{
	gHub.mouseMotion(x, y);
	if (!gHub.isOnFocus()) 
	{
		CurrentManipulator()->HandleMotion(x, gWindowHeight - y);
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

	glutDisplayFunc(RenderScene);
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
