#pragma once
#ifndef _GraphicsCommon__H
#define _GraphicsCommon__H

//  [8/16/2011 hustruan]



namespace Engine
{

	/** Specifies the buffer to use when calling Clear
	*/
	enum ClearOptions
	{
		/** A render target
		*/
		CO_Target = 1,
		
		/** A depth buffer
		*/
		CO_DepthBuffer = 2,
		
		/** A stencil buffer
		*/
		CO_Stencil = 4
	};
		

	enum ShadingType
	{
		ST_Flat,
		ST_Gouraud,
		ST_Phong,
	};

	enum CullingMode
	{
		/** Hardware never culls triangles and render everything it receives.
		*/
		CM_None = 1,

		/** Hardware culls triangles whose vertices are listed clockwise in the view (default).
		*/
		CM_Front = 2,
		
		/** Hardware culls triangles whose vertices are listed anticlockwise in the view.
		*/
		CM_Back = 3
	};
	

	/** The polygon mode to use when rasterising.
	 */
	enum PolygonMode
	{
		/** Only points are rendered.
		*/
		FM_Points = 1,

		/** Wireframe models are rendered.
		*/
		FM_WireFrame = 2,

		/** Solid polygons are rendered.
		*/
		FM_Solid = 3
	};
		
	enum ColorMask
	{
		CM_Red   = 1UL << 3,
		CM_Green = 1UL << 2,
		CM_Blue  = 1UL << 1,
		CM_Aplha = 1UL << 0,
		CM_All   = CM_Red | CM_Green | CM_Blue | CM_Aplha
	};
	

	/** Comparison functions used for the depth/stencil buffer operations and 
	others. */
	enum CompareFunction
	{
		/** Never pass the comparison.
		*/
		CMPF_Always_Fail = 1,

		/** Always pass the comparison.
		*/
		CMPF_Always_Pass = 2,
		
		/** Pass if the source data is less than the destination data
		*/
		CMPF_Less = 3,

		/** Pass if the source data is less than or equal to the destination 
		*/
		CMPF_Less_Equal = 4,

		/** Pass if the source data is equal to the destination data
		*/
		CMPF_Equal = 5,

		/** Pass if the source data is not equal to the destination data
		*/
		CMPF_Not_Equal = 6,

		/** Pass if the source data is greater than or equal to the destination data
		*/
		CMPF_Greater_Equal = 7,

		/** Pass if the source data is greater than the destination data
		*/
		CMPF_Greater = 8,

		CMPF_Count = 9
	};
	
	enum StencilOperation
	{
		/** Keep the existing stencil data.
		*/	
		SOP_Keep,

		/** Set the stencil data to 0.
		*/
		SOP_Zero,

		/** Set the stencil value to the reference value
		*/
		SOP_Replace,

		/** Increase the stencil value by 1, clamping at the maximum value
		*/
		SOP_Increment_Sat,

		/** Decrease the stencil value by 1, clamping at 0
		*/
		SOP_Decrement_Sat,

		/** Increase the stencil value by 1, wrapping back to 0 when incrementing the maximum value
		*/
		SOP_Increment_Wrap,

		/** Decrease the stencil value by 1, wrapping when decrementing 0
		*/
		SOP_Decrement_Wrap,

		/** Invert the stencil data.
		*/
		SOP_Invert
	};

	/// <summary>
	/// Define how to combine a source color with the destination color already 
	/// on the render target for blending
	/// </summary>
	enum BlendFactor
	{
		BF_Zero,
		BF_One,
		BF_Src_Color,
		BF_Inv_Src_Color,
		BF_Src_Alpha,
		BF_Inv_Src_Alpha,
		BF_Dst_Color,
		BF_Inv_Dst_Color,
		BF_Dst_Alpha,
		BF_Inv_Dst_Alpha,
		BF_Src_Alpha_Saturation,
		BF_Both_Src_Alpha
	};
	
	/// <summary>
	/// Define how to combine a source color with the destination color already 
	/// on the render target for blending
	/// </summary>
	enum BlendFunction
	{
		/// <summary>
		/// The result is the destination added to the source. Result = Source + Destination
		/// </summary>
		BF_Add,
		
		/// <summary>
		/// The result is the destination subtracted from to the source. Result = Source - Destination
		/// </summary>
		BF_Subtract,
		
		/// <summary>
		/// The result is the source subtracted from the destination. Result = Destination - Source
		/// </summary>		
		BF_Reverse_Subtract,
		
		/// <summary>
		/// The result is the minimum of the source and destination. Result = MIN(Source, Destination)
		/// </summary>		
		BF_Min,
		
		/// <summary>
		/// The result is the maximum of the source and destination. Result = MAX(Source, Destination)
		/// </summary>	
		BF_Max
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



	enum IndexBufferType
	{
		IBT_Bit16,
		IBT_Bit32
	};

	enum PrimitiveType
	{
		/** Renders the vertices as a collection of isolated points. 
		*   This value is unsupported for indexed primitives
		*/
		PT_PointList,

		/** Renders the vertices as a list of isolated straight line segments.
		*/
		PF_LineList,
	
		/** Renders the vertices as a single polyline.
		*/
		PF_LineStrip,
		
		/** Renders the specified vertices as a sequence of isolated triangles.
		*   Each group of three vertices defines a separate triangle. 
		*   Back-face culling is affected by the current winding-order render state.
		*/
		PF_TriangleList,
		
		/** Renders the vertices as a triangle strip. 
		*   The backface-culling flag is automatically flipped on even-numbered triangles. 
		*/
		PF_TriangleStrip,

		/** Renders the vertices as a triangle fan. 
		*/
		PF_TriangleFan
	};

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

}



#endif // _GraphicsCommon__H