#ifndef StretchMap_h__
#define StretchMap_h__

#include "Common.h"
#include <nvMath.h>

/**
 * This class define a shader effect to generate StretchUV Map.
 */

class StretchMap
{
public:
	StretchMap(void);
	~StretchMap(void);

	void Begin();
	void End();

	void SetScale(float scale);
	void SetWorldMatrix(const nv::matrix4f& world);

public:
	static void Init();
	static void Release();

private:
	static GLuint msStretchProgramID;
	static GLint msScaleParam, msWorldMatParam;
};

#endif // StretchMap_h__