#ifndef GraphicsCommon_h__
#define GraphicsCommon_h__

#include <Core/Prerequisites.h>

namespace RcEngine {
namespace Render {

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
	TF_Min_Mag_Mip_Point                            = 0,
	TF_Min_Mag_Point_Mip_Linear                     = 0x1,
	TF_Min_Point_Mag_Linear_Mip_Point               = 0x4,
	TF_Min_Point_Mag_Mip_Linear                     = 0x5,
	TF_Min_Linear_Mag_Mip_Point                     = 0x10,
	TF_Min_Linear_Mag_Point_Mip_Linear              = 0x11,
	TF_Min_Mag_Linear_Mip_Point                     = 0x14,
	TF_Min_Mag_Mip_Linear                           = 0x15,
	TF_Anisotropic                                  = 0x55,
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
	
enum PrimitiveType
{
	PT_Point_List = 1, 
	PT_Line_List = 2, 
	PT_Line_Strip = 3, 
	PT_Triangle_List = 4, 
	PT_Triangle_Strip = 5, 
	PT_Triangle_Fan = 6 
};

enum BlendOperation
{
	BOP_Add		= 1,
	BOP_Sub		= 2,
	BOP_Rev_Sub	= 3,
	BOP_Min		= 4,
	BOP_Max		= 5,
};

enum AlphaBlendFactor
{
	ABF_Zero,
	ABF_One,
	ABF_Src_Alpha,
	ABF_Dst_Alpha,
	ABF_Inv_Src_Alpha,
	ABF_Inv_Dst_Alpha,
	ABF_Src_Color,
	ABF_Dst_Color,
	ABF_Inv_Src_Color,
	ABF_Inv_Dst_Color,
	ABF_Src_Alpha_Sat
};

enum CompareFunction
{
	CF_AlwaysFail,
	CF_AlwaysPass,
	CF_Less,
	CF_LessEqual,
	CF_Equal,
	CF_NotEqual,
	CF_GreaterEqual,
	CF_Greater
};

enum StencilOperation
{
	// Leave the stencil buffer unchanged
	SOP_Keep,
	// Set the stencil value to zero
	SOP_Zero,
	// Set the stencil value to the reference value
	SOP_Replace,
	// Increase the stencil value by 1, clamping at the maximum value
	SOP_Incr,
	// Decrease the stencil value by 1, clamping at 0
	SOP_Decr,
	// Invert the bits of the stencil buffer
	SOP_Invert,
	// Increase the stencil value by 1, wrap the result if necessary
	SOP_Incr_Wrap,
	// Decrease the stencil value by 1, wrap the result if necessary
	SOP_Decr_Wrap
};

enum ColorWriteMask
{
	CWM_Red     = 1,
	CWM_Green   = 2,
	CWM_Blue    = 4,
	CWM_Alpha   = 8,
	CWM_All     = ( CWM_Red | CWM_Green |  CWM_Blue | CWM_Alpha ) 
};

enum FillMode
{
	FM_Solid,
	FM_WireFrame,
};

enum CullMode 
{
	CM_None    = 1,
	CM_Front   = 2,
	CM_Back    = 3 
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
	IBT_Bit16 = 0,
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





