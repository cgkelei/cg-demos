#ifndef GraphicsCommon_h__
#define GraphicsCommon_h__

#include <Core/Prerequisites.h>

namespace RcEngine {
namespace RenderSystem {

enum VertexElementFormat
{
	/** Single-component, 32-bit floating-point, expanded to (float, 0, 0, 1).
	*/
	VEF_Single = 0,

	/** Two-component, 32-bit floating-point, expanded to (float, Float32 value, 0, 1).
	*/
	VEF_Vector2 = 1,
		
	/* Three-component, 32-bit floating point, expanded to (float, float, float, 1).
	*/
	VEF_Vector3 = 2,
		
	/** Four-component, 32-bit floating point, expanded to (float, float, float, float).
	*/
	VEF_Vector4 = 3,
		
	/** Four-component, packed, unsigned byte, mapped to 0 to 1 range. 
	* Input is in Int32 format (ARGB) expanded to (R, G, B, A).
	*/
	VEF_Color = 4,  
		
	/**¡¡Four-component, unsigned byte.
	*/
	VEF_UByte4 = 5,
		
	/** Two-component, signed short expanded to (value, value, 0, 1).
	*/
	VEF_Short2 = 6,
		
	/** Four-component, signed short expanded to (value, value, value, value).
	*/
	VEF_Short4 = 7,
		
	/** Normalized, two-component, signed short, expanded to 
	* (first short/32767.0, second short/32767.0, 0, 1). 
	* This type is valid for vertex shader version 2.0 or higher.
	*/
	VEF_NormalizedShort2 = 8,
		
	/** Normalized, four-component, signed short, expanded to 
	*  (first short/32767.0, second short/32767.0, third short/32767.0, fourth short/32767.0).
	* This type is valid for vertex shader version 2.0 or higher.
	*/
	VEF_NormalizedShort4 = 9,

	/** Two-component, 16-bit floating point expanded to (value, value, value, value). 
	* This type is valid for vertex shader version 2.0 or higher.
	*/
	VEF_HalfVector2 = 10,

	/** Four-component, 16-bit floating-point expanded to (value, value, value, value). 
	* This type is valid for vertex shader version 2.0 or higher.
	*/
	VEF_HalfVector4 = 11,

	VEF_Count = 12
};


enum VertexElementUsage
{
	/** Position, 3 float per vertex
	*/
	VEU_Position = 1, 
		
	/** Blending weight data
	*/
	VEU_BlendWeight = 2,
		
	/** Blending indices data
	*/
	VEU_BlendIndices = 3, 
		
	/** Vertex normal data.
	*/
	VEU_Normal = 4,
		
	/** Diffuse Color
	*/
	VEU_Diffuse = 5,
		
	/** Specular Color
	*/
	VEU_Specular = 6,
	
	/**  Texture coordinates
	*/
	VEU_TextureCoordinate = 7,

	/** Vertex binormal data.
	*/
	VEU_Binormal = 8,
		
	/** Vertex tangent data.
	*/
	VEU_Tangent = 9,	
};

enum TextureMapAccess
{
	TMA_Read_Only,
	TMA_Write_Only,
	TMA_Read_Write
};

enum TextureAddressMode
{
	/** Tile the texture at every integer junction. For example,
	*	for u values between 0 and 3, the texture is repeated three times.
	*/
	TAM_Wrap,

	/** Flip the texture at every integer junction. For u values between 0 and 1,
	*	for example, the texture is addressed normally; between 1 and 2, the texture is flipped (mirrored);
	*	between 2 and 3, the texture is normal again; and so on.
	*/
	TAM_Mirror,
		
	/** Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively.
	*/
	TAM_Clamp,
		
	/** Texture coordinates outside the range [0.0, 1.0] are set to the border color
	*/
	TAM_Border,

	/** Similar to TAM_Mirror and TAM_Clamp. Takes the absolute value of the texture coordinate 
	*	thus, mirroring around 0, and then clamps to the maximum value.
	*/
	TAM_Mirror_Once
};

/** Filtering options for textures / mipmaps. */
enum TextureFilter
{
	/** When used with D3DSAMP_MIPFILTER, disables mipmapping.
	*/
	TF_None,

	/** Point filtering used as a texture magnification or minification filter. The
	*   texel with coordinates nearest to the desired pixel value is used. The texture
	*   filter used between mipmap levels is based on the nearest point; that is,
	*   the rasterizer uses the color from the texel of the nearest mipmap texture.
	*/
	FO_Point,
		
	/** Bilinear interpolation filtering used as a texture magnification or minification
	*   filter. A weighted average of a 2¡Á2 area of texels surrounding the desired
	*   pixel is used. The texture filter used between mipmap levels is trilinear
	*   mipmap interpolation, in which the rasterizer performs linear interpolation
	*   on pixel color, using the texels of the two nearest mipmap textures.
	*/
	FO_Linear,
		
	/** Anisotropic texture filtering used as a texture magnification or minification
	*   filter. This type of filtering compensates for distortion caused by the difference
	*   in angle between the texture polygon and the plane of the screen.
	*/
	FO_Anisotropic 
};
	
enum CubeMapFace
{
	CMF_PositiveX,
	CMF_NegativeX,
	CMF_PositiveY,
	CMF_NegativeY,
	CMF_PositiveZ,
	CMF_NegativeZ,
};

enum TextureType
{
	TT_Texture1D,
	TT_Texture2D,
	TT_Texture3D,
	TT_TextureCube
};
	
enum FilterType
{
	FT_Min,
	FT_Mag,
	FT_MipMap
};

enum FilterOp
{
	Filter_Min_Mag_Mip_Point                            = 0,
	Filter_Min_Mag_Point_Mip_Linear                     = 0x1,
	Filter_Min_Point_Mag_Linear_Mip_Point               = 0x4,
	Filter_Min_Point_Mag_Mip_Linear                     = 0x5,
	Filter_Min_Linear_Mag_Mip_Point                     = 0x10,
	Filter_Min_Linear_Mag_Point_Mip_Linear              = 0x11,
	Filter_Min_Mag_Linear_Mip_Point                     = 0x14,
	Filter_Min_Mag_Mip_Linear                           = 0x15,
	Filter_Anisotropic                                  = 0x55,
	Filter_Cmp_Min_Mag_Mip_Point                        = 0x80,
	Filter_Cmp_Min_Mag_Point_Mip_Linear                 = 0x81,
	Filter_Cmp_Min_Point_Mag_Linear_Mip_Point           = 0x84,
	Filter_Cmp_Min_Point_Mag_Mip_Linear                 = 0x85,
	Filter_Cmp_Min_Linear_Mag_Mip_Point                 = 0x90,
	Filter_Cmp_Min_Linear_Mag_Point_Mip_Linear          = 0x91,
	Filter_Cmp_Min_Mag_Linear_Mip_Point                 = 0x94,
	Filter_Cmp_Min_Mag_Mip_Linear                       = 0x95,
	Filter_Cmp_Anisotropic                              = 0xd5,

};

enum PrimitiveType
{
	PT_Point_List = 1, 
	PT_Line_List = 2, 
	PT_Line_Strip = 3, 
	PT_Triangle_List = 4, 
	PT_Triangle_Strip = 5, 
	PT_Triangle_Fan = 6 
};


enum BufferUsage
{
	BU_Static,
	BU_Dynamic
};

enum BufferAccess
{
	BA_Read_Only,
	BA_Write_Only,
	BA_Read_Write
};

enum ClearFlag
{
	CF_Color = 0x0001, /** Clear color buffer */
	CF_Depth = 0x0002, /** Clear z-buffer */
	CF_Stencil = 0x0004, /** Clear stencil buffer */
};

enum IndexBufferType
{
	IBT_Bit16,
	IBT_Bit32
};

enum ElementAccessHint
{
	EAH_CPU_Read = 1UL << 0,
	EAH_CPU_Write = 1UL << 1,
	EAH_GPU_Read = 1UL << 2,
	EAH_GPU_Write = 1UL << 3,
};

enum Attachment
{
	ATT_DepthStencil,
	ATT_Color0,
	ATT_Color1,
	ATT_Color2,
	ATT_Color3,
	ATT_Color4,
	ATT_Color5,
	ATT_Color6,
	ATT_Color7
};

struct ElementInitData
{
	const void* pData;
	uint32 rowPitch;
	uint32 slicePitch;
};


} // RenderSystem
} // RcEngine


#endif // GraphicsCommon_h__





