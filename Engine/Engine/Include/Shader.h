#pragma once
#ifndef _Shader__H

#include "Prerequisites.h"

namespace Engine
{	
	struct ShaderDesc
	{
		String Profile;
		String FuncName;

		uint32 TechPass;

		friend bool operator==(ShaderDesc const & lhs, ShaderDesc const & rhs)
		{
			return (lhs.Profile == rhs.Profile) && (lhs.FuncName == rhs.FuncName);
		}
		friend bool operator!=(ShaderDesc const & lhs, ShaderDesc const & rhs)
		{
			return !(lhs == rhs);
		}
	};

	class _ApiExport Shader
	{
	public:
		Shader(void);
		~Shader(void);

		enum ShaderType
		{
			ST_Vertex,
			ST_Pixel
		};


	};
}




#endif