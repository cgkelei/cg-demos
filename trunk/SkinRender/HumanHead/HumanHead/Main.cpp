#include "Common.h"
#include "RenderTextureFBO.h"
#include "ShadowMap.h"
#include "Bloom.h"
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
const char* HeadAlbedo =  "Map-COL.png";
const char* HeadNormal =  "Infinite-Level_02_World_NoSmoothUV.png";
const char* EnvMap = "Env.dds";
const char* RhodMap = "rho_d.png";
const char* SeamMskMap = "HeadMask.png";

#define LIGHT_COUNT 3
#define BLOOM_BUFFER_COUNT 2
#define TEXTURE_BUFFER_COUNT 6
#define SHADOW_MAP_SIZE 1024
#define IRRADIANCE_BUFFER_SIZE 1024
#define CAMERA_FOV 20.0f
#define MATH_PI float(3.14159265358979323846264338327950288f)

struct Light
{
	glm::vec3 Color;
	float Amount;
	Camera Camera;
	glm::vec2 NearFarPlane;
	ShadowMap* ShadowMap;
};

struct Environment
{
	GLuint DiffuseEnvMap, IrradianceEnvMap, GlossyEnvMap;
};

Light gLights[LIGHT_COUNT];
Environment gEnvironment[3];
int gCurrentEnv = 0;
float gEnvLightAmount = 0.3;

int gWindowWidth = 824;
int gWindowHeight = 328;

nv::SDKPath gAppPath;
nv::GlutUIContext gHub;

float gRoughness = 0.3;
float gRho_s = 0.18;
float gStrechScale = 0.0014444444f;
float gThicknessScale = 16.0f;
float gSpecularIntensity = 1.88f;
float gExposure = 1.5f;

nv::Model gLightModel;
GLuint gLightTexID, gLightModelVBO, gLightModelIBO;

nv::Model gModel;
float gModelDiameter = 0.0f; //diameter of the model
glm::vec3 gCenter;	// center of the model
glm::mat4 gModelWorld;

GLuint gModelVBO, gModelIBO;
GLuint gAlbedoTexID, gSpecTexID, gNormalTexID, gRhodTexID, gSeamMaskTexID;

bool gStrechMapDirty = true;
bool gDebugOut = false;

Camera gCamera, gCameraTemp;

RenderTexture* gTempBuffer, *gFinalBuffer;
RenderTexture* gStretchBuffer[TEXTURE_BUFFER_COUNT - 1];	// stretch texture blur storage
RenderTexture* gIrradianceBuffer[LIGHT_COUNT][TEXTURE_BUFFER_COUNT];
RenderTexture* gBloomBuffer[2], *gBloomBlurTemp;

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
	OPTION_BLOOM,
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
	RM_Debug = 0,
	RM_NoSSS,
	RM_SSS_Without_TSM,
	RM_SSS_With_TSM,
};
//int gMode = RM_SSS_With_TSM;
int gMode = RM_NoSSS;

enum DebugMode
{
	DM_Depth0 = 0,
	DM_IrradianceMap0 = DM_Depth0 + LIGHT_COUNT,
};

int gDebugMode = DM_Depth0;

FastBloom* gFastBloom;

struct PipelineEffect
{
	GLuint ProgramID;
	GLint WorldParam, ViewProjParam, AlbedoTexParam;
} gPipelineEffect;

struct SkyBoxEffect
{
	GLuint ProgramID;
	GLint MVPParam, EnvTexParam;
} gSkyBoxEffect;

struct DebugViewEffect
{
	GLuint ProgramID;
	GLint ColorTexParam;
	GLint DepthParam;
} gDebugViewEffect;


struct StretchEffect
{
	GLuint ProgramID;
	GLint WorldParam, ScaleParam;

} gStretchEffect;

struct ConvolutionStretchEffect
{
	GLuint ProgramID;
	GLint GaussWidthParam, InputTexParam, BlurStepParam;

} gConvStretchEffect;

struct ConvolutionEffect
{
	GLuint ProgramID;
	GLint GaussWidthParam, StretchTexParam, InputTexParam, BlurStepParam;

} gConvolutionEffect;

struct LightParams
{
	GLint PosParam, ColorParam, AmountParam, NearFarPlaneParam;
};

struct TextureSpaceLightEffect
{
	GLuint ProgramID;
	GLint WorldParam, LightViewParam, LightProjParam, AlbedoTexParam, SpecTexParam, 
		  NormalTexParam, Rho_d_TexParam, IrradEnvParam, EnvAmount, ShadowTexParam, RoughnessParam, Rho_sParam;
	LightParams LightParam;

} gTexSpaceLightEffect;

struct TextureSpaceLightTSMEffect
{
	GLuint ProgramID;
	GLint WorldParam, LightViewParam, LightProjParam, AlbedoTexParam, SpecTexParam,EnvLightOnParam,
		NormalTexParam, Rho_d_TexParam, IrradEnvParam, EnvAmount, ShadowTexParam, RoughnessParam, Rho_sParam;
	LightParams LightParam;

} gTexSpaceLightTSMEffect;


struct FinalSkinEffect
{
	GLuint ProgramID;
	GLint WorldParam, ViewProjParam, LightViewParam, LightProjParam, AlbedoTexParam, SpecTexParam, SpecIntensityParam,
		NormalTexParam, Rho_d_TexParam, ShadowTexParam, EnvCubeParam, GlossyEnvParam, EnvAmount, SeamMaskTexParam,
		IrradTexParam, GaussWeightsParam, EyePosParam, DiffuseColorMixParam, RoughnessParam, Rho_sParam;
	LightParams LightParam;

} gFinalSkinEffect;

struct FinalSkinTSMEffect
{
	GLuint ProgramID;
	GLint WorldParam, ViewProjParam, LightViewParam, LightProjParam, AlbedoTexParam, SpecTexParam, StretchTexParam, SpecIntensityParam,
		NormalTexParam, Rho_d_TexParam, EnvCubeParam, EnvAmount, GlossyEnvParam, ShadowTexParam, IrradTexParam, SeamMaskTexParam,
		GaussWeightsParam, EyePosParam, DiffuseColorMixParam, RoughnessParam, Rho_sParam, ThicknessScaleParam;
	LightParams LightParam;

} gFinalSkinTSMEffect;

struct PhongEffect
{
	GLuint ProgramID;
	GLint WorldParam, ViewProjParam, LightViewParam, LightProjParam, AlbedoTexParam, NormalTexParam, SpecTexParam , SpecIntensityParam,
		Rho_d_TexParam, IrradEnvParam, GlossyEnvParam, EnvAmount,ShadowTexParam, EyePosParam, RoughnessParam, Rho_sParam;
	LightParams LightParam;

} gPhongEffect;

struct BloomEffect
{
	GLuint BlurProgramID, CombineProgramID;
	GLint GaussWidthParam, BlurInputTexParam, BlurStepParam;
	GLint CombineInputTexParam, CombineBloomTexParam, ExposureParam;

} gBloomEffect;

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

GLuint Create2DTextureFromFile(const char* fileName, GLenum wrapType = GL_REPEAT )
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
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapType);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapType);

		glTexImage2D( GL_TEXTURE_2D, 0, image.getInternalFormat(), image.getWidth(), image.getHeight(), 0,
			image.getFormat(), image.getType(), image.getLevel(0));
	}

	return tex;
}

GLuint CreateCubemapTextureFromFile(const char* fileName, bool mipMap)
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

		GLenum filter = GL_LINEAR;
		if (mipMap)  filter = GL_LINEAR_MIPMAP_LINEAR;

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

		// load face data
		

		// calculate mip map levels
		int mipMaps = image.getMipLevels();
		
		unsigned int width = static_cast<unsigned int>(image.getWidth());
		unsigned int height = static_cast<unsigned int>(image.getHeight());
		
		for (int level = 0; level < mipMaps; ++level)
		{
			for(int i=0; i<6; i++) 
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level,
					image.getInternalFormat(), width, height, 0, 
					image.getFormat(), image.getType(), image.getLevel(level, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i));
			}	

			width = std::max<unsigned int>(1U, width / 2);
			height = std::max<unsigned int>(1U, height / 2);
		}
	}

	return tex;
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

void LoadShaderEffect()
{

#define RETRIEVE_UNIFORM_LOCATION(loc, program, name)  loc = glGetUniformLocation(program, name); /*\
														ASSERT(loc >= 0);*/
	std::vector<Utility::ShaderMacro> shadowMacro;
	std::vector<Utility::ShaderMacro> blurMacro;

	/**
	 * Use shadowMacro to control shadowmap algorithm and whether use normal map or not
	 */
	
	//shaderDefines.push_back(Utility::ShaderMacro("SHADOW_PCF", ""));
	shadowMacro.push_back(Utility::ShaderMacro("SHADOW_VSM", ""));

	// if has normal map, use it
	if (gNormalTexID > 0)
	{
		shadowMacro.push_back(Utility::ShaderMacro("USE_NormalMap", ""));
	}

	printf("Load shadow map Effect...\n");
	ShadowMap::Init(&shadowMacro);

	// StretchMap generate effect
	printf("Load StretchMap Effect...\n");
	gStretchEffect.ProgramID = Utility::LoadShaderEffect("Stretch.vert", "Stretch.frag");
	ASSERT(gStretchEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gStretchEffect.WorldParam, gStretchEffect.ProgramID, "WorldMat");
	RETRIEVE_UNIFORM_LOCATION(gStretchEffect.ScaleParam, gStretchEffect.ProgramID, "Scale");

	// ConvolutionStretch effect
	printf("Load ConvolutionStretch Effect...\n");
	blurMacro.clear(); blurMacro.push_back(Utility::ShaderMacro("BLUR_Stretch", ""));
	gConvStretchEffect.ProgramID = Utility::LoadShaderEffect("Convolution.vert", "Convolution.frag", &blurMacro);
	ASSERT(gConvStretchEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gConvStretchEffect.InputTexParam, gConvStretchEffect.ProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchEffect.GaussWidthParam, gConvStretchEffect.ProgramID, "GaussWidth");
	RETRIEVE_UNIFORM_LOCATION(gConvStretchEffect.BlurStepParam, gConvStretchEffect.ProgramID, "Step");

	// Convolution effect
	printf("Load ConvolutionU Effect...\n");
	blurMacro.clear(); blurMacro.push_back(Utility::ShaderMacro("BLUR_Diffusion", ""));
	gConvolutionEffect.ProgramID = Utility::LoadShaderEffect("Convolution.vert", "Convolution.frag", &blurMacro);
	ASSERT(gConvolutionEffect.ProgramID > 0);

	RETRIEVE_UNIFORM_LOCATION(gConvolutionEffect.StretchTexParam, gConvolutionEffect.ProgramID, "StretchTex");
	RETRIEVE_UNIFORM_LOCATION(gConvolutionEffect.InputTexParam, gConvolutionEffect.ProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gConvolutionEffect.GaussWidthParam, gConvolutionEffect.ProgramID, "GaussWidth");
	RETRIEVE_UNIFORM_LOCATION(gConvolutionEffect.BlurStepParam, gConvolutionEffect.ProgramID, "Step");

	printf("Load Bloom effect...\n");
	blurMacro.clear(); blurMacro.push_back(Utility::ShaderMacro("BLUR_Bloom", ""));
	gBloomEffect.BlurProgramID = Utility::LoadShaderEffect("Convolution.vert", "Convolution.frag", &blurMacro);
	gBloomEffect.CombineProgramID = Utility::LoadShaderEffect("Convolution.vert", "FastBloom.frag");

	RETRIEVE_UNIFORM_LOCATION(gBloomEffect.GaussWidthParam, gBloomEffect.BlurProgramID, "GaussWidth");
	RETRIEVE_UNIFORM_LOCATION(gBloomEffect.BlurInputTexParam, gBloomEffect.BlurProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gBloomEffect.BlurStepParam, gBloomEffect.BlurProgramID, "Step");

	RETRIEVE_UNIFORM_LOCATION(gBloomEffect.CombineInputTexParam, gBloomEffect.CombineProgramID, "InputTex");
	RETRIEVE_UNIFORM_LOCATION(gBloomEffect.CombineBloomTexParam, gBloomEffect.CombineProgramID, "BloomTex");
	RETRIEVE_UNIFORM_LOCATION(gBloomEffect.ExposureParam, gBloomEffect.CombineProgramID, "Exposure");
	
	printf("Load Skybox effect...\n");
	gSkyBoxEffect.ProgramID = Utility::LoadShaderEffect("SkyBox.vert", "SkyBox.frag");
	RETRIEVE_UNIFORM_LOCATION(gSkyBoxEffect.MVPParam, gSkyBoxEffect.ProgramID, "MVP");
	RETRIEVE_UNIFORM_LOCATION(gSkyBoxEffect.EnvTexParam, gSkyBoxEffect.ProgramID, "EnvTex");

	printf("Load Debug view effect...\n");
	gDebugViewEffect.ProgramID = Utility::LoadShaderEffect("Convolution.vert", "DebugView.frag");
	RETRIEVE_UNIFORM_LOCATION(gDebugViewEffect.ColorTexParam, gDebugViewEffect.ProgramID, "ColorTex");
	RETRIEVE_UNIFORM_LOCATION(gDebugViewEffect.DepthParam, gDebugViewEffect.ProgramID, "ViewDepth");


	/*printf("Load FixedPipeline effect...\n");
	gPipelineEffect.ProgramID = Utility::LoadShaderEffect("FixedPipeline.vert", "FixedPipeline.frag");
	ASSERT(gPipelineEffect.ProgramID > 0);*/

	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.WorldParam, gPipelineEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.ViewProjParam, gPipelineEffect.ProgramID, "ViewProj");
	RETRIEVE_UNIFORM_LOCATION(gPipelineEffect.AlbedoTexParam, gPipelineEffect.ProgramID, "AlbedoTex");

	// Texture Space Light effect
	printf("Load Texture Space Light Without TSM  Effect...\n");
	gTexSpaceLightEffect.ProgramID = Utility::LoadShaderEffect("TextureSpaceLighting.vert", "TextureSpaceLighting.frag", &shadowMacro);
	ASSERT(gTexSpaceLightEffect.ProgramID > 0);
	//Utility::PrintEffectAttribs(gTexSpaceLightEffect.ProgramID);
	//Utility::PrintEffectUniforms(gTexSpaceLightEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.WorldParam, gTexSpaceLightEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightParam.ColorParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightParam.PosParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightParam.AmountParam, gTexSpaceLightEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightParam.NearFarPlaneParam, gTexSpaceLightEffect.ProgramID, "Lights[0].NearFarPlane");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.ShadowTexParam, gTexSpaceLightEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.NormalTexParam, gTexSpaceLightEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.AlbedoTexParam, gTexSpaceLightEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.SpecTexParam, gTexSpaceLightEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.Rho_d_TexParam, gTexSpaceLightEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.IrradEnvParam, gTexSpaceLightEffect.ProgramID, "IrradEnvMap");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.EnvAmount, gTexSpaceLightEffect.ProgramID, "EnvAmount");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightViewParam, gTexSpaceLightEffect.ProgramID, "LightView[0]");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.LightProjParam, gTexSpaceLightEffect.ProgramID, "LightProj[0]");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.RoughnessParam, gTexSpaceLightEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightEffect.Rho_sParam, gTexSpaceLightEffect.ProgramID, "Rho_s");
	

	// Final Skin Without TSM effect
	printf("Load Final Skin Without TSM effect...\n");
	gFinalSkinEffect.ProgramID = Utility::LoadShaderEffect("FinalSkin.vert", "FinalSkin.frag", &shadowMacro);
	ASSERT(gFinalSkinEffect.ProgramID > 0);
	//Utility::PrintEffectAttribs(gFinalSkinEffect.ProgramID);
	//Utility::PrintEffectUniforms(gFinalSkinEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.WorldParam, gFinalSkinEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.ViewProjParam, gFinalSkinEffect.ProgramID, "ViewProj");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightParam.ColorParam, gFinalSkinEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightParam.PosParam, gFinalSkinEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightParam.AmountParam, gFinalSkinEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightParam.NearFarPlaneParam, gFinalSkinEffect.ProgramID, "Lights[0].NearFarPlane");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.ShadowTexParam, gFinalSkinEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.NormalTexParam, gFinalSkinEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.AlbedoTexParam, gFinalSkinEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.Rho_d_TexParam, gFinalSkinEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.SpecTexParam, gFinalSkinEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightViewParam, gFinalSkinEffect.ProgramID, "LightView[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.LightProjParam, gFinalSkinEffect.ProgramID, "LightProj[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.GaussWeightsParam, gFinalSkinEffect.ProgramID, "GaussWeights[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.EyePosParam, gFinalSkinEffect.ProgramID, "EyePos");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.IrradTexParam, gFinalSkinEffect.ProgramID, "IrradTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.RoughnessParam, gFinalSkinEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.Rho_sParam, gFinalSkinEffect.ProgramID, "Rho_s");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.EnvCubeParam, gFinalSkinEffect.ProgramID, "IrradEnvMap");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.EnvAmount, gFinalSkinEffect.ProgramID, "EnvAmount");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.GlossyEnvParam, gFinalSkinEffect.ProgramID, "GlossyEnvMap");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.SpecIntensityParam, gFinalSkinEffect.ProgramID, "SpecularIntensity");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinEffect.SeamMaskTexParam, gFinalSkinEffect.ProgramID, "SeamMaskTex");

	// Texture Space Light With TSM effect
	printf("Load Texture Space Light With TSM Effect...\n");
	gTexSpaceLightTSMEffect.ProgramID = Utility::LoadShaderEffect("TextureSpaceLightingTSM.vert", "TextureSpaceLightingTSM.frag", &shadowMacro);
	ASSERT(gTexSpaceLightTSMEffect.ProgramID > 0);
	/*Utility::PrintEffectAttribs(gTexSpaceLightTSMEffect.ProgramID);
	Utility::PrintEffectUniforms(gTexSpaceLightTSMEffect.ProgramID);*/

	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.WorldParam, gTexSpaceLightTSMEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightParam.ColorParam, gTexSpaceLightTSMEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightParam.PosParam, gTexSpaceLightTSMEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightParam.AmountParam, gTexSpaceLightTSMEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightParam.NearFarPlaneParam, gTexSpaceLightTSMEffect.ProgramID, "Lights[0].NearFarPlane");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.ShadowTexParam, gTexSpaceLightTSMEffect.ProgramID,  "ShadowTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.NormalTexParam, gTexSpaceLightTSMEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.AlbedoTexParam, gTexSpaceLightTSMEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.Rho_d_TexParam, gTexSpaceLightTSMEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.SpecTexParam, gTexSpaceLightTSMEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.IrradEnvParam, gTexSpaceLightTSMEffect.ProgramID, "IrradEnvMap");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.EnvAmount, gTexSpaceLightTSMEffect.ProgramID, "EnvAmount");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightViewParam, gTexSpaceLightTSMEffect.ProgramID, "LightView");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.LightProjParam, gTexSpaceLightTSMEffect.ProgramID, "LightProj");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.RoughnessParam, gTexSpaceLightTSMEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.Rho_sParam, gTexSpaceLightTSMEffect.ProgramID, "Rho_s");
	RETRIEVE_UNIFORM_LOCATION(gTexSpaceLightTSMEffect.EnvLightOnParam, gTexSpaceLightTSMEffect.ProgramID, "EnvLightOn");
	

	//// Final Skin With TSM effect
	printf("Load Final Skin With TSM effect...\n");
	gFinalSkinTSMEffect.ProgramID = Utility::LoadShaderEffect("FinalSkinTSM.vert", "FinalSkinTSM.frag", &shadowMacro);
	ASSERT(gFinalSkinTSMEffect.ProgramID > 0);
	//Utility::PrintEffectAttribs(gFinalSkinTSMEffect.ProgramID);
	//Utility::PrintEffectUniforms(gFinalSkinTSMEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.WorldParam, gFinalSkinTSMEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.ViewProjParam, gFinalSkinTSMEffect.ProgramID, "ViewProj");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightParam.ColorParam, gFinalSkinTSMEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightParam.PosParam, gFinalSkinTSMEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightParam.AmountParam, gFinalSkinTSMEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightParam.NearFarPlaneParam, gFinalSkinTSMEffect.ProgramID, "Lights[0].NearFarPlane");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.ShadowTexParam, gFinalSkinTSMEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.NormalTexParam, gFinalSkinTSMEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.SpecTexParam, gFinalSkinTSMEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.StretchTexParam, gFinalSkinTSMEffect.ProgramID, "StretchTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.AlbedoTexParam, gFinalSkinTSMEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.Rho_d_TexParam, gFinalSkinTSMEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightViewParam, gFinalSkinTSMEffect.ProgramID, "LightView[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.LightProjParam, gFinalSkinTSMEffect.ProgramID, "LightProj[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.GaussWeightsParam, gFinalSkinTSMEffect.ProgramID, "GaussWeights[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.EyePosParam, gFinalSkinTSMEffect.ProgramID, "EyePos");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.IrradTexParam, gFinalSkinTSMEffect.ProgramID, "IrradTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.RoughnessParam, gFinalSkinTSMEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.Rho_sParam, gFinalSkinTSMEffect.ProgramID, "Rho_s");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.ThicknessScaleParam, gFinalSkinTSMEffect.ProgramID, "DepthScale");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.EnvCubeParam, gFinalSkinTSMEffect.ProgramID, "IrradEnvMap");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.EnvAmount, gFinalSkinTSMEffect.ProgramID, "EnvAmount");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.GlossyEnvParam, gFinalSkinTSMEffect.ProgramID, "GlossyEnvMap");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.SpecIntensityParam, gFinalSkinTSMEffect.ProgramID, "SpecularIntensity");
	RETRIEVE_UNIFORM_LOCATION(gFinalSkinTSMEffect.SeamMaskTexParam, gFinalSkinTSMEffect.ProgramID, "SeamMaskTex");

	printf("Load Lambert with Kelemen/Szirmay-Kalos Specular BRDF effect...\n");
	gPhongEffect.ProgramID = Utility::LoadShaderEffect("Phong.vert", "Phong.frag", &shadowMacro);
	//Utility::PrintEffectAttribs(gPhongEffect.ProgramID);
	//Utility::PrintEffectUniforms(gPhongEffect.ProgramID);

	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.WorldParam, gPhongEffect.ProgramID, "World");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.ViewProjParam, gPhongEffect.ProgramID, "ViewProj");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightParam.ColorParam, gPhongEffect.ProgramID, "Lights[0].Color");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightParam.PosParam, gPhongEffect.ProgramID, "Lights[0].Position");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightParam.AmountParam, gPhongEffect.ProgramID, "Lights[0].Amount");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightParam.NearFarPlaneParam, gPhongEffect.ProgramID, "Lights[0].NearFarPlane");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.NormalTexParam, gPhongEffect.ProgramID, "NormalTex");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.AlbedoTexParam, gPhongEffect.ProgramID, "AlbedoTex");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.Rho_d_TexParam, gPhongEffect.ProgramID, "Rho_d_Tex");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.SpecTexParam, gPhongEffect.ProgramID, "SpecTex");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.EyePosParam, gPhongEffect.ProgramID, "EyePos");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.ShadowTexParam, gPhongEffect.ProgramID,  "ShadowTex[0]");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightViewParam, gPhongEffect.ProgramID, "LightView[0]");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.LightProjParam, gPhongEffect.ProgramID, "LightProj[0]");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.IrradEnvParam, gPhongEffect.ProgramID, "IrradEnvMap");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.GlossyEnvParam, gPhongEffect.ProgramID, "GlossyEnvMap");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.EnvAmount, gPhongEffect.ProgramID, "EnvAmount");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.Rho_sParam, gPhongEffect.ProgramID, "Rho_s");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.RoughnessParam, gPhongEffect.ProgramID, "Roughness");
	RETRIEVE_UNIFORM_LOCATION(gPhongEffect.SpecIntensityParam, gPhongEffect.ProgramID, "SpecularIntensity");
}

void LoadModel()
{
	std::string resolvedPath;

	/*if (gAppPath.getFilePath( "Light.obj", resolvedPath)) 
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
	}*/
	
	if (gAppPath.getFilePath( HeadFile, resolvedPath)) 
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

		// Scale to 1.0
		gModel.rescale(1.0f);

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
		
		printf("try to load head normal map...\n");
		gNormalTexID = Create2DTextureFromFile(HeadNormal);
		if (gNormalTexID > 0)
			printf("load head normal map successed...\n");
		else
			printf("normal map doesn't exit...\n");

		//Utility::SaveTextureToPfm("normal.pfm", gNormalTexID, 6000, 6000);

		printf("load head rho_d map...\n");
		gRhodTexID = Create2DTextureFromFile(RhodMap, GL_CLAMP_TO_EDGE);

		printf("load seam mask map...\n");
		gSeamMaskTexID = Create2DTextureFromFile(SeamMskMap, GL_CLAMP_TO_EDGE);
	}

	// Load env map
	printf("load environment map...\n");
	gEnvironment[0].DiffuseEnvMap = CreateCubemapTextureFromFile("Environment/Eucalyptus/DiffuseMap.dds", false);
	gEnvironment[0].IrradianceEnvMap = CreateCubemapTextureFromFile("Environment/Eucalyptus/IrradianceMap.dds", false);
	gEnvironment[0].GlossyEnvMap =  CreateCubemapTextureFromFile("Environment/Eucalyptus/GlossyEnvMap.dds", true);
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
	static char tempBuffer[255];

	for (int i = 0; i < 8; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	gHub.begin();

	gHub.beginGroup( nv::GroupFlags_GrowDownFromLeft);

	std::stringstream fps; 
	fps << "Human Skin demo, FPS: " << gFPS ; 
	gHub.doLabel(none, fps.str().c_str());

	gHub.doLabel( none, "");
	gHub.doCheckButton(none, "Draw Wireframe", &gOptions[OPTION_WIREFRAME]);
	gHub.doCheckButton(none, "Draw Skybox", &gOptions[OPTION_DRAW_SKYBOX]);
	//gHub.doCheckButton(none, "Show Light", &gOptions[OPTION_SHOW_LIGHTS]);
	gHub.doCheckButton(none, "Bloom", &gOptions[OPTION_BLOOM]);
	gHub.doLabel( none, "");

	gHub.doLabel( none, "Render Mode");
	gHub.doRadioButton(0, none, "SSS Debug",  &gMode);
	gHub.doRadioButton(1, none, "No SSS",  &gMode);
	gHub.doRadioButton(2, none, "SSS Without TSM",  &gMode);
	if(gNormalTexID > 0) gHub.doRadioButton(3, none, "SSS With TSM",  &gMode);  // show this option when normal map exits, so can use TSM
	gHub.doLabel( none, "");

	if (RM_Debug == gMode)
	{	

		for (int i = 0; i < LIGHT_COUNT; ++i)
		{
			gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
			gHub.doLabel( none, "");
			sprintf(tempBuffer, "Depth Map %d", i);
			gHub.doRadioButton(i, none, tempBuffer,  &gDebugMode);
			gHub.endGroup();
		}

		for (int i = 0; i < 6; ++i)
		{
			gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
			gHub.doLabel( none, "");
			sprintf(tempBuffer, "Irradiance Map Blured %d", i + 1);
			gHub.doRadioButton(i + LIGHT_COUNT, none, tempBuffer,  &gDebugMode);
			gHub.endGroup();
		}
	}
	else
	{
		gHub.doLabel( none, "Render Parameters");

		gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
		sprintf(tempBuffer, "Spec Intensity %.2f", gSpecularIntensity);
		gHub.doLabel(none, tempBuffer);
		gHub.doHorizontalSlider(none, 0.0f, 4.0f, &gSpecularIntensity);
		gHub.endGroup();

		gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
		sprintf(tempBuffer, "Roughness %.2f", gRoughness);
		gHub.doLabel(none, tempBuffer);
		gHub.doHorizontalSlider(none, 0.0f, 1.0f, &gRoughness);
		gHub.endGroup();

		gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
		sprintf(tempBuffer, "Rho_s %.2f", gRho_s);
		gHub.doLabel(none, tempBuffer);
		gHub.doHorizontalSlider(none, 0.0f, 1.0f, &gRho_s);
		gHub.endGroup();

		if (gOptions[OPTION_BLOOM])
		{
			gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
			sprintf(tempBuffer, "Exposure %.2f", gExposure);
			gHub.doLabel(none, tempBuffer);
			gHub.doHorizontalSlider(none, 0.2f, 20.0f, &gExposure);
			gHub.endGroup();
		}
		
		/*gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
		sprintf(tempBuffer, "StrechScale %f", gStrechScale);
		gHub.doLabel(none, tempBuffer);
		gHub.doHorizontalSlider(none, 0.0f, 0.002f, &gStrechScale);
		gHub.endGroup();*/


		if (gMode == RM_SSS_With_TSM && gNormalTexID)
		{
			gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
			sprintf(tempBuffer, "ThincknessScale %.0f", gThicknessScale);
			gHub.doLabel(none, tempBuffer);
			gHub.doHorizontalSlider(none, 10.0f, 30.0f, &gThicknessScale);
			gHub.endGroup();
		}

		gHub.doLabel( none, "");


		gHub.doLabel( none, "Manipulator Control");
		gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
		gHub.doRadioButton(0, none, "Camera",  &gManipulator);
		gHub.doCheckButton(none, "View From Light", &gViewFromLight);
		gHub.endGroup();

		for (int i = 0; i < LIGHT_COUNT; ++i)
		{
			gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
			sprintf(tempBuffer, "Light%d: %.2f", i + 1, gLights[i].Amount);
			gHub.doRadioButton(1 + i, none, tempBuffer,  &gManipulator);
			gHub.doHorizontalSlider(none, 0.0f, 1.0f, &gLights[i].Amount);
			gHub.endGroup();
		}

		gHub.beginGroup(nv::GroupFlags_GrowLeftFromTop);
		sprintf(tempBuffer, "Environment Light: %.2f",  gEnvLightAmount);
		gHub.doLabel( none, tempBuffer);
		gHub.doHorizontalSlider(none, 0.0f, 1.0f, &gEnvLightAmount);
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

//void DrawLights()
//{
//	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.013f, 0.013f, 0.013f));
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, gLightTexID);
//
//	glUseProgram(gPipelineEffect.ProgramID);
//
//	glm::mat4 veiwProj = CurrentManipulator()->GetProjectionMatrix() * CurrentManipulator()->GetViewMatrix();
//	glUniformMatrix4fv(gPipelineEffect.ViewProjParam, 1, false, glm::value_ptr(veiwProj));
//	glUniform1ui(gPipelineEffect.AlbedoTexParam, 0);
//
//	for (int i = 0; i < LIGHT_COUNT; ++i)
//	{
//		glm::vec3 lightPos = gLights[i].Camera.GetEyePosition();
//		glm::vec3 targetPos = gLights[i].Camera.GetLookAtPosition();
//		glm::vec3 lightDir = glm::normalize(targetPos - lightPos);
//
//		glm::vec3 tZ = glm::vec3(0, 0, 1); 
//		glm::vec3 tX = glm::cross(lightDir, tZ);
//		tX = glm::normalize(tX);
//		tZ = glm::cross(tX, lightDir);
//
//		glm::mat4 orientation;
//		orientation = glm::column(orientation, 0, glm::vec4(tX, 0.0f));
//		orientation = glm::column(orientation, 1, glm::vec4(lightDir, 0.0f));
//		orientation = glm::column(orientation, 2, glm::vec4(tZ, 0.0f));
//
//		glm::mat4 baseRot = glm::rotate(glm::mat4(1.0f), 180.0f, glm::vec3(1, 0, 0));
//
//		glm::mat4 world = glm::translate(glm::mat4(1.0f), lightPos) * orientation * baseRot * scale;
//		glUniformMatrix4fv(gPipelineEffect.WorldParam, 1, false, glm::value_ptr(world));
//		
//		DrawModel(&gLightModel, gLightModelVBO, gLightModelIBO);
//	}
//
//	glUseProgram(0);
//}


void DrawFullscreenQuad()
{
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0, 1.0, 1.0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0, -1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glEnd();
}

void MakeShadowMap()
{
	ShadowMap::SetWorldMatrix(gModelWorld);

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		if (glm::length(gLights[i].Color) > 0.0f)
		{
			gLights[i].ShadowMap->Begin(&gLights[i].Camera);
				DrawModel(&gModel, gModelVBO, gModelIBO);
			gLights[i].ShadowMap->End();
		}
	}
}

void ConvolutionStretch(RenderTexture* src, RenderTexture* dest, int itr)
{
	int bufferWidth = src->GetWidth();
	int buffetHeight = src->GetHeight();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, bufferWidth, buffetHeight);

	/*float gaussWidth;
	if (itr == 0)
	{
		gaussWidth = sqrtf(gConvolutionScale[itr]);
	}
	else
	{
		gaussWidth =  sqrtf(gConvolutionScale[itr] - gConvolutionScale[itr-1]);
	}*/

	glm::vec2 stepX = glm::vec2(1.0f, 0.0f) * (1.0f / bufferWidth);
	glm::vec2 stepY = glm::vec2(0.0f, 1.0f) * (1.0f / buffetHeight);

	glUseProgram(gConvStretchEffect.ProgramID);

	// one pixel size combined with blur direction
	glUniform2f(gConvStretchEffect.BlurStepParam, stepX.x, stepX.y );

	glUniform1f(gConvStretchEffect.GaussWidthParam, gConvolutionScale[itr]);
	//glUniform1f(gConvStretchEffect.GaussWidthParam, sqrtf(gConvolutionScale[itr]));
	//glUniform1f(gConvStretchUEffect.GaussWidthParam, gaussWidth);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	src->Bind();
	glUniform1i(gConvStretchEffect.InputTexParam, 0);
	
	gTempBuffer->Activate();			
	DrawFullscreenQuad();
	gTempBuffer->Deactivate();

	// convolution V
	glUniform2f(gConvStretchEffect.BlurStepParam, stepY.x, stepY.y );

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Bind();
	glUniform1i(gConvStretchEffect.InputTexParam, 0);

	dest->Activate();
	DrawFullscreenQuad();
	dest->Deactivate();

	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Release();

	glPopAttrib();
}

void Convolution(RenderTexture* src, RenderTexture* dest, int itr)
{
	int bufferWidth = src->GetWidth();
	int buffetHeight = src->GetHeight();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, bufferWidth, buffetHeight);

	/*float gaussWidth;
	if (itr == 0)
	{
		gaussWidth = sqrtf(gConvolutionScale[itr]);
	}
	else
	{
		gaussWidth =  sqrtf(gConvolutionScale[itr] - gConvolutionScale[itr-1]);
	}*/

	// one pixel size combined with blur direction
	glm::vec2 stepX = glm::vec2(1.0f, 0.0f) * (1.0f / bufferWidth);
	glm::vec2 stepY = glm::vec2(0.0f, 1.0f) * (1.0f / buffetHeight);

	glUseProgram(gConvolutionEffect.ProgramID);
	glUniform2f(gConvolutionEffect.BlurStepParam, stepX.x, stepX.y );

	//glUniform1f(gConvolutionEffect.GaussWidthParam, sqrtf(gaussWidth));
	glUniform1f(gConvolutionEffect.GaussWidthParam, sqrtf(gConvolutionScale[itr]));
	//glUniform1f(gConvStretchEffect.GaussWidthParam, gConvolutionScale[itr]);
	

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	src->Bind();
	glUniform1i(gConvolutionEffect.InputTexParam, 0);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	gStretchBuffer[itr]->Bind();
	glUniform1i(gConvolutionEffect.StretchTexParam, 1);	

	// convolution U
	gTempBuffer->Activate();
	DrawFullscreenQuad();
	gTempBuffer->Deactivate();

	// convolution V
	glUniform2f(gConvolutionEffect.BlurStepParam, stepY.x, stepY.y );

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Bind();
	glUniform1i(gConvolutionEffect.InputTexParam, 0);

	dest->Activate();
	DrawFullscreenQuad();
	dest->Deactivate();

	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	gTempBuffer->Release();

	glActiveTexture(GL_TEXTURE1);
	gStretchBuffer[itr]->Release();
	
	glPopAttrib();
}

void BloomGuassianBlur(RenderTexture* src, RenderTexture* dest, float guassianWidth)
{
	int bufferWidth = src->GetWidth();
	int bufferHeight = src->GetHeight();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, bufferWidth, bufferHeight);

	glm::vec2 stepX = glm::vec2(1.0f, 0.0f) * (1.0f / bufferWidth);
	glm::vec2 stepY = glm::vec2(0.0f, 1.0f) * (1.0f / bufferHeight);

	glUseProgram(gBloomEffect.BlurProgramID);
	glUniform1f(gBloomEffect.GaussWidthParam, guassianWidth);
	glUniform2f(gBloomEffect.BlurStepParam, stepX.x, stepX.y );

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	src->Bind();
	glUniform1i(gBloomEffect.BlurInputTexParam, 0);

	// convolution U
	gBloomBlurTemp->Activate();
	DrawFullscreenQuad();
	gBloomBlurTemp->Deactivate();

	// convolution V
	glUniform2f(gBloomEffect.BlurStepParam, stepY.x, stepY.y );
	
	glActiveTexture(GL_TEXTURE0);
	gBloomBlurTemp->Bind();
	glUniform1i(gBloomEffect.BlurInputTexParam, 0);	

	dest->Activate();
	DrawFullscreenQuad();
	dest->Deactivate();

	glUseProgram(0);
	glPopAttrib();

	glActiveTexture(GL_TEXTURE0);
	gBloomBlurTemp->Release();
}

void MakeStretchMap()
{
	gStretchBuffer[0]->Activate();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, gStretchBuffer[0]->GetWidth(), gStretchBuffer[0]->GetHeight());

	auto w = gStretchBuffer[0]->GetWidth();
	auto h = gStretchBuffer[0]->GetHeight();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gStretchEffect.ProgramID);

	glUniformMatrix4fv(gStretchEffect.WorldParam, 1, false, glm::value_ptr(gModelWorld));
	glUniform1f(gStretchEffect.ScaleParam, gStrechScale);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);
	
	gStretchBuffer[0]->Deactivate();
	glPopAttrib(); 

	ConvolutionStretch(gStretchBuffer[0], gStretchBuffer[1], 0);
	ConvolutionStretch(gStretchBuffer[1], gStretchBuffer[2], 1);
	ConvolutionStretch(gStretchBuffer[2], gStretchBuffer[3], 2);
	ConvolutionStretch(gStretchBuffer[3], gStretchBuffer[4], 3);	

	//Utility::SaveTextureToPfm("StrechXY3.pfm", gStretchBuffer[3]->GetColorTex(), gStretchBuffer[0]->GetWidth(), gStretchBuffer[0]->GetHeight());
	//Utility::SaveTextureToPfm("StrechXY2.pfm", gStretchBuffer[2]->GetColorTex(), gStretchBuffer[0]->GetWidth(), gStretchBuffer[0]->GetHeight());
	//Utility::SaveTextureToPfm("StrechXY1.pfm", gStretchBuffer[1]->GetColorTex(), gStretchBuffer[0]->GetWidth(), gStretchBuffer[0]->GetHeight());
	//Utility::SaveTextureToPfm("StrechXY4.pfm", gStretchBuffer[4]->GetColorTex(), gStretchBuffer[0]->GetWidth(), gStretchBuffer[0]->GetHeight());
}

void RenderSkyBox()
{
	glUseProgram(gSkyBoxEffect.ProgramID);

	glm::mat4 MVP = CurrentCamera()->GetViewProjectionMatrix() * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	glUniformMatrix4fv(gSkyBoxEffect.MVPParam, 1, false, glm::value_ptr(MVP));

	/*glDisable(GL_CULL_FACE);*/
	glEnable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].DiffuseEnvMap);
	glUniform1i(gSkyBoxEffect.EnvTexParam, 0);

	glBegin(GL_QUADS);          // Start Drawing Quads
	// Front Face
	glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
	glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
	glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
	glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
	
	// Back Face
	glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
	glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
	
	// Top Face
	glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glVertex3f(-1.0f,  1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
	glVertex3f( 1.0f,  1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
	glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
	
	// Bottom Face
	glVertex3f(-1.0f, -1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glVertex3f( 1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
	glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
	
	// Right face
	glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
	glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
	glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
	glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
	
	// Left Face
	glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
	glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
	glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
	glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
	glEnd();                    // Done Drawing Quads

	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

/**
 * Texture space lighting to get Irradiance 
 */
void RenderIrradiance()
{
	int bufferWidth = gIrradianceBuffer[0][0]->GetWidth();
	int bufferHeight = gIrradianceBuffer[0][0]->GetHeight();

	gIrradianceBuffer[0][0]->Activate();

	glPushAttrib(GL_VIEWPORT_BIT);	
	glViewport(0, 0, bufferWidth, bufferHeight);

	glClearColor(0.729f, 0.596f, 0.549f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gTexSpaceLightEffect.ProgramID);

	glUniform1f(gTexSpaceLightEffect.EnvAmount, gEnvLightAmount);
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

		glUniformMatrix4fv(gTexSpaceLightEffect.LightViewParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetViewMatrix()));
		glUniformMatrix4fv(gTexSpaceLightEffect.LightProjParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetProjectionMatrix()));

		glUniform2f(gTexSpaceLightEffect.LightParam.NearFarPlaneParam+ i * 4, gLights[i].Camera.GetNearPlane(), gLights[i].Camera.GetFarPlane());
		glUniform3fv(gTexSpaceLightEffect.LightParam.PosParam + i * 4, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gTexSpaceLightEffect.LightParam.ColorParam + i * 4, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gTexSpaceLightEffect.LightParam.AmountParam + i * 4, gLights[i].Amount);
	}

	glActiveTexture(GL_TEXTURE6);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gRhodTexID);
	glUniform1i(gTexSpaceLightEffect.Rho_d_TexParam, 6);

	glActiveTexture(GL_TEXTURE7);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].IrradianceEnvMap);
	glUniform1i(gTexSpaceLightEffect.IrradEnvParam, 7);

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
	

	/*for (int i = 0; i < TEXTURE_BUFFER_COUNT; ++i)
	{
	std::stringstream sss;
	sss << "blurIrradiance" << i << ".pfm";
	Utility::SaveTextureToPfm(sss.str().c_str(), gIrradianceBuffer[0][i]->GetColorTex(), bufferWidth, bufferHeight);
	}*/
}

void RenderIrradianceTSM()
{
	glPushAttrib(GL_VIEWPORT_BIT);	
	glViewport(0, 0, IRRADIANCE_BUFFER_SIZE, IRRADIANCE_BUFFER_SIZE);

	glUseProgram(gTexSpaceLightTSMEffect.ProgramID);
	
	glUniform1f(gTexSpaceLightTSMEffect.EnvAmount, gEnvLightAmount);
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
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].IrradianceEnvMap);
	glUniform1i(gTexSpaceLightTSMEffect.IrradEnvParam, 4);

	for (int i = 0 ; i < LIGHT_COUNT; ++i)
	{
		gIrradianceBuffer[i][0]->Activate();

		glUniform1i(gTexSpaceLightTSMEffect.EnvLightOnParam , i);	


		glClearColor(0.729f / 3, 0.596f / 3, 0.549f / 3, 1.0f);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE5);
		glEnable(GL_TEXTURE_2D);
		gLights[i].ShadowMap->GetRenderTexture()->BindColor(0);
		glUniform1i(gTexSpaceLightTSMEffect.ShadowTexParam , 5);	

		glUniformMatrix4fv(gTexSpaceLightTSMEffect.LightViewParam, 1, false, glm::value_ptr(gLights[i].Camera.GetViewMatrix()));
		glUniformMatrix4fv(gTexSpaceLightTSMEffect.LightProjParam, 1, false, glm::value_ptr(gLights[i].Camera.GetProjectionMatrix()));

		glUniform2f(gTexSpaceLightTSMEffect.LightParam.NearFarPlaneParam, gLights[i].Camera.GetNearPlane(), gLights[i].Camera.GetFarPlane());
		glUniform3fv(gTexSpaceLightTSMEffect.LightParam.PosParam, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gTexSpaceLightTSMEffect.LightParam.ColorParam, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gTexSpaceLightTSMEffect.LightParam.AmountParam, gLights[i].Amount);

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

		//std::stringstream sss;
		//sss << "light" << i << "Irr.pfm";
		//Utility::SaveTextureToPfm(sss.str().c_str(), gIrradianceBuffer[i][5]->GetColorTex(), IRRADIANCE_BUFFER_SIZE, IRRADIANCE_BUFFER_SIZE);
	}
}

void RenderFinal()
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

	glUseProgram(gFinalSkinEffect.ProgramID);

	glUniform1f(gFinalSkinEffect.SpecIntensityParam, gSpecularIntensity);
	glUniform1f(gFinalSkinEffect.EnvAmount, gEnvLightAmount);
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

		glUniformMatrix4fv(gFinalSkinEffect.LightViewParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetViewMatrix()));
		glUniformMatrix4fv(gFinalSkinEffect.LightProjParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetProjectionMatrix()));

		glUniform2f(gFinalSkinEffect.LightParam.NearFarPlaneParam + i * 4, gLights[i].Camera.GetNearPlane(), gLights[i].Camera.GetFarPlane());
		glUniform3fv(gFinalSkinEffect.LightParam.PosParam + i * 4, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gFinalSkinEffect.LightParam.ColorParam + i * 4, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gFinalSkinEffect.LightParam.AmountParam + i * 4, gLights[i].Amount);
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

	glActiveTexture(GL_TEXTURE13);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].IrradianceEnvMap);
	glUniform1i(gFinalSkinEffect.EnvCubeParam, 13);

	glActiveTexture(GL_TEXTURE14);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].GlossyEnvMap);
	glUniform1i(gFinalSkinEffect.GlossyEnvParam, 14);

	glActiveTexture(GL_TEXTURE15);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gSeamMaskTexID);
	glUniform1i(gFinalSkinEffect.SeamMaskTexParam, 15);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void RenderFinalTSM()
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

	glUseProgram(gFinalSkinTSMEffect.ProgramID);

	glUniform1f(gFinalSkinTSMEffect.SpecIntensityParam, gSpecularIntensity);
	glUniform1f(gFinalSkinTSMEffect.EnvAmount, gEnvLightAmount);
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
	glUniform1f(gFinalSkinTSMEffect.ThicknessScaleParam, gThicknessScale);


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

		glUniformMatrix4fv(gFinalSkinTSMEffect.LightViewParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetViewMatrix()));
		glUniformMatrix4fv(gFinalSkinTSMEffect.LightProjParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetProjectionMatrix()));

		glUniform2f(gFinalSkinTSMEffect.LightParam.NearFarPlaneParam + i * 4, gLights[i].Camera.GetNearPlane(), gLights[i].Camera.GetFarPlane());
		glUniform3fv(gFinalSkinTSMEffect.LightParam.PosParam + i * 4, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gFinalSkinTSMEffect.LightParam.ColorParam + i * 4, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gFinalSkinTSMEffect.LightParam.AmountParam + i * 4, gLights[i].Amount);

		for (int j = 0; j < 6; ++j)
		{
			auto ss = GL_TEXTURE5 + LIGHT_COUNT + i * 6 + j - GL_TEXTURE0;
			auto p = gFinalSkinTSMEffect.IrradTexParam + i * 6 + j;

			glActiveTexture(GL_TEXTURE5 + LIGHT_COUNT + i * 6 + j);
			glEnable(GL_TEXTURE_2D);
			gIrradianceBuffer[i][j]->BindColor(0);
			glUniform1i(gFinalSkinTSMEffect.IrradTexParam + i * 6 + j, GL_TEXTURE5 + LIGHT_COUNT + i * 6 + j - GL_TEXTURE0);		
		}
	}
	
	glActiveTexture(GL_TEXTURE27);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].IrradianceEnvMap);
	glUniform1i(gFinalSkinTSMEffect.EnvCubeParam, 27);

	glActiveTexture(GL_TEXTURE28);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].GlossyEnvMap);
	glUniform1i(gFinalSkinTSMEffect.GlossyEnvParam, 28);

	glActiveTexture(GL_TEXTURE29);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gSeamMaskTexID);
	glUniform1i(gFinalSkinTSMEffect.SeamMaskTexParam, 29);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
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

	glUniform1f(gPhongEffect.EnvAmount, gEnvLightAmount);
	glUniform1f(gPhongEffect.SpecIntensityParam, gSpecularIntensity);
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
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].IrradianceEnvMap);
	glUniform1i(gPhongEffect.IrradEnvParam, 4);

	for (int i = 0; i < LIGHT_COUNT; ++i)
	{
		glActiveTexture(GL_TEXTURE5 + i);
		glEnable(GL_TEXTURE_2D);
		gLights[i].ShadowMap->GetRenderTexture()->BindColor(0);
		glUniform1i(gPhongEffect.ShadowTexParam + i, 5 + i);	

		glUniformMatrix4fv(gPhongEffect.LightViewParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetViewMatrix()));
		glUniformMatrix4fv(gPhongEffect.LightProjParam + i, 1, false, glm::value_ptr(gLights[i].Camera.GetProjectionMatrix()));

		glUniform2f(gPhongEffect.LightParam.NearFarPlaneParam + i * 4, gLights[i].Camera.GetNearPlane(), gLights[i].Camera.GetFarPlane());
		glUniform3fv(gPhongEffect.LightParam.PosParam + i * 4, 1, glm::value_ptr(gLights[i].Camera.GetEyePosition()));
		glUniform3fv(gPhongEffect.LightParam.ColorParam + i * 4, 1, glm::value_ptr(gLights[i].Color));
		glUniform1f(gPhongEffect.LightParam.AmountParam + i * 4, gLights[i].Amount);
	}

	glActiveTexture(GL_TEXTURE20);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gEnvironment[gCurrentEnv].GlossyEnvMap);
	glUniform1i(gPhongEffect.GlossyEnvParam, 20);

	DrawModel(&gModel, gModelVBO, gModelIBO);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//Utility::SaveTextureToPfm("final.pfm",gStretchBuffer[4]->GetColorTex(), gWindowWidth, gWindowWidth);
}

void BloomPass(RenderTexture* input)
{
	BloomGuassianBlur(input, gBloomBuffer[0], sqrtf(0.008));
	BloomGuassianBlur(input, gBloomBuffer[1], sqrtf(0.0576));

	/*Utility::SaveTextureToPfm("final.pfm", input->GetColorTex(), gWindowWidth, gWindowHeight);
	Utility::SaveTextureToPfm("bloom1.pfm", gBloomBuffer[0]->GetColorTex(), gWindowWidth, gWindowHeight);*/

	// Conbine
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear( GL_COLOR_BUFFER_BIT );

	glUseProgram(gBloomEffect.CombineProgramID);

	glUniform1f(gBloomEffect.ExposureParam, gExposure);	

	glActiveTexture(GL_TEXTURE0);
	input->Bind();
	glUniform1i(gBloomEffect.CombineInputTexParam, 0);	

	glActiveTexture(GL_TEXTURE1);
	gBloomBuffer[0]->Bind();
	glUniform1i(gBloomEffect.CombineBloomTexParam, 1);	

	glActiveTexture(GL_TEXTURE2);
	gBloomBuffer[1]->Bind();
	glUniform1i(gBloomEffect.CombineBloomTexParam+1, 2);

	DrawFullscreenQuad();

	glUseProgram(0);
}


void RenderScene()
{
	CalculateFPS();

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
			if ( gOptions[OPTION_BLOOM])  gFinalBuffer->Activate();	
			RenderPhong();
			if (gOptions[OPTION_DRAW_SKYBOX]) RenderSkyBox();
			if ( gOptions[OPTION_BLOOM])  gFinalBuffer->Deactivate();
			
		}
		break;
	case RM_SSS_Without_TSM:
		{
			RenderIrradiance();

			if ( gOptions[OPTION_BLOOM])  gFinalBuffer->Activate();	
			RenderFinal();
			if (gOptions[OPTION_DRAW_SKYBOX]) RenderSkyBox();
			if ( gOptions[OPTION_BLOOM])  gFinalBuffer->Deactivate();	
		}
		break;
	case RM_SSS_With_TSM:
		{
			RenderIrradianceTSM();

			if ( gOptions[OPTION_BLOOM])  gFinalBuffer->Activate();	
			RenderFinalTSM();
			if (gOptions[OPTION_DRAW_SKYBOX]) RenderSkyBox();
			if ( gOptions[OPTION_BLOOM])  gFinalBuffer->Deactivate();	
		}
		break;
	case RM_Debug:
		{
			glClear(GL_COLOR_BUFFER_BIT);

			glPushAttrib(GL_VIEWPORT_BIT);
			glViewport(0, 0, gWindowWidth, gWindowHeight);

			if (gDebugMode < LIGHT_COUNT)
			{
				// view depth map
				glUseProgram(gDebugViewEffect.ProgramID);

				glUniform1i(gDebugViewEffect.DepthParam, 1);
	
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				gLights[gDebugMode].ShadowMap->GetRenderTexture()->BindColor();
				glUniform1i(gDebugViewEffect.ColorTexParam, 0);

				DrawFullscreenQuad();

				glUseProgram(0);

				
			}
			else if (gDebugMode >= LIGHT_COUNT && gDebugMode < LIGHT_COUNT + 6)
			{
				glUseProgram(gDebugViewEffect.ProgramID);

				glUniform1i(gDebugViewEffect.DepthParam, 0);

				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				gIrradianceBuffer[0][gDebugMode - LIGHT_COUNT]->BindColor();
				glUniform1i(gDebugViewEffect.ColorTexParam, 0);

				DrawFullscreenQuad();

				glUseProgram(0);
			}

			glPopAttrib();
		}
		break;

	default:
		break;
	}

	if (gOptions[OPTION_BLOOM] && gMode != RM_Debug)
	{
		BloomPass(gFinalBuffer);
	}

	DoUI();
	glutSwapBuffers();
}


void CreateBuffer()
{
	// always use the fixed stretch buffer size
	for (int i=0; i<TEXTURE_BUFFER_COUNT - 1; i++)
	{
		SAFE_DELETE(gStretchBuffer[i]); 
		gStretchBuffer[i] = new RenderTexture(IRRADIANCE_BUFFER_SIZE, IRRADIANCE_BUFFER_SIZE, GL_TEXTURE_2D);
		gStretchBuffer[i]->InitColor_Tex(0, GL_RG32F);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	//bStretchMap = false;

	SAFE_DELETE(gTempBuffer); 
	gTempBuffer = new RenderTexture(IRRADIANCE_BUFFER_SIZE, IRRADIANCE_BUFFER_SIZE, GL_TEXTURE_2D); 
	gTempBuffer->InitColor_Tex(0, GL_RGBA32F);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		for (int j = 0; j < TEXTURE_BUFFER_COUNT; ++j)
		{
			// delete if already exits
			SAFE_DELETE(gIrradianceBuffer[i][j]); 

			gIrradianceBuffer[i][j] = new RenderTexture(IRRADIANCE_BUFFER_SIZE, IRRADIANCE_BUFFER_SIZE, GL_TEXTURE_2D); 
			gIrradianceBuffer[i][j]->InitColor_Tex(0, GL_RGBA32F);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		}
		gIrradianceBuffer[i][0]->InitDepth_Tex();
	}
}

void Init()
{
	if (GLEW_ARB_seamless_cube_map)
	{
		glEnable(GL_ARB_seamless_cube_map);
	}
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//init mode
	gOptions[OPTION_SSS_WITH_TSM] = false;
	gOptions[OPTION_SSS_WITHOUT_TSM] = true;

	gOptions[OPTION_DRAW_SKYBOX] = true;
	gOptions[OPTION_SHOW_LIGHTS] = false;
	gOptions[OPTION_WIREFRAME] = false;
	gOptions[OPTION_NoSSS] = false;

	// Create Shadow map buffers
	for (int i = 0; i < LIGHT_COUNT; i++) 
	{
		//gLights->Camera.SetDistance(2.0);
		gLights[i].Camera.SetProjection(CAMERA_FOV, 1.0f, 0.1f, 20.0f);
		gLights[i].ShadowMap = new ShadowMap(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	}
	
	CreateBuffer();

	LoadPresents();
	
	LoadModel();

	LoadShaderEffect();
}

void Reshape( int w, int h)
{
	gWindowWidth = w;
	gWindowHeight = h;
	glViewport( 0, 0, gWindowWidth, gWindowHeight);

	// we need to resize all RenderTexture
	SAFE_DELETE(gFinalBuffer); 
	gFinalBuffer = new RenderTexture(w, h, GL_TEXTURE_2D);
	gFinalBuffer->InitColor_Tex(0, GL_RGBA32F);
	gFinalBuffer->InitDepth_RB();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	for (int i = 0; i < 2; i++) 
	{
		SAFE_DELETE(gBloomBuffer[i]); 
		gBloomBuffer[i] = new RenderTexture(w, h, GL_TEXTURE_2D);
		gBloomBuffer[i]->InitColor_Tex(0, GL_RGBA32F);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	SAFE_DELETE(gBloomBlurTemp); 
	gBloomBlurTemp = new RenderTexture(w, h, GL_TEXTURE_2D);
	gBloomBlurTemp->InitColor_Tex(0, GL_RGBA32F);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// resize GUI
	gHub.reshape(w, h);

	// project matrix
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
	case 'd':
		{
			gDebugOut = true;
			Utility::SaveScreenToPfm("test.pfm", gWindowWidth, gWindowHeight);
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

	if (!gHub.isOnFocus() && gMode != RM_Debug) 
	{
		CurrentManipulator()->HandleMouse(button, state, glutGetModifiers(), x, gWindowHeight - y);
	}
}

void Motion(int x, int y)
{
	gHub.mouseMotion(x, y);
	if (!gHub.isOnFocus() && gMode != RM_Debug) 
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

	//Test();

	return 0;
}
