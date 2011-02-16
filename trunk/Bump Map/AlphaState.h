#ifndef _AlphaState__H
#define _AlphaState__H


class AlphaState
{
public:

	enum SrcBlendMode
	{
		SBM_ZERO,
		SBM_ONE,
		SBM_DST_COLOR,
		SBM_ONE_MINUS_DST_COLOR,
		SBM_SRC_ALPHA,
		SBM_ONE_MINUS_SRC_ALPHA,
		SBM_DST_ALPHA,
		SBM_ONE_MINUS_DST_ALPHA,
		SBM_SRC_ALPHA_SATURATE,
		SBM_CONSTANT_COLOR,
		SBM_ONE_MINUS_CONSTANT_COLOR,
		SBM_CONSTANT_ALPHA,
		SBM_ONE_MINUS_CONSTANT_ALPHA,
		SBM_QUANTITY
	};

	enum DstBlendMode
	{
		DBM_ZERO,
		DBM_ONE,
		DBM_SRC_COLOR,
		DBM_ONE_MINUS_SRC_COLOR,
		DBM_SRC_ALPHA,
		DBM_ONE_MINUS_SRC_ALPHA,
		DBM_DST_ALPHA,
		DBM_ONE_MINUS_DST_ALPHA,
		DBM_CONSTANT_COLOR,
		DBM_ONE_MINUS_CONSTANT_COLOR,
		DBM_CONSTANT_ALPHA,
		DBM_ONE_MINUS_CONSTANT_ALPHA,
		DBM_QUANTITY
	};

	enum TestMode
	{
		TM_NEVER,
		TM_LESS,
		TM_EQUAL,
		TM_LEQUAL,
		TM_GREATER,
		TM_NOTEQUAL,
		TM_GEQUAL,
		TM_ALWAYS,
		TM_QUANTITY
	};

public:
	AlphaState() {};
	virtual ~AlphaState() {};

private:
	bool			m_blendEnabled;      // default: false
	SrcBlendMode	m_srcBlend;			 // default: SBF_SRC_ALPHA
	DstBlendMode	m_dstBlend;			 // default: DBF_ONE_MINUS_SRC_ALPHA
	TestMode		m_test;			     // default: TF_ALWAYS
	bool			m_testEnabled;       // default: false;
	float			m_reference;         // default: 0, always in [0,1]

};





#endif