#ifndef _ColorRGBA__H
#define _ColorRGBA__H

#include "Config.h"

class ColorRGBA
{
public:
	ColorRGBA() ;  // initial values (0,0,0)
	ColorRGBA(float red, float green, float blue, float alpha);
	ColorRGBA(const float* tuple);
	ColorRGBA(const ColorRGBA& color);


	float R () const  { return m_tuple[0]; }
	float G () const  { return m_tuple[1]; }
	float B () const  { return m_tuple[2]; }
	float A () const  { return m_tuple[3]; }
	
	float operator[] (int i) const ;
	float& operator[] (int i);

private:
	float	m_tuple[3];
};


inline ColorRGBA::ColorRGBA() 
{
	m_tuple[0] = 0.0f;
	m_tuple[1] = 0.0f;
	m_tuple[2] = 0.0f;
	m_tuple[3] = 0.0f;
}

inline ColorRGBA::ColorRGBA(float red, float green, float blue, float alpha)
{
	m_tuple[0] = red;
	m_tuple[1] = green;
	m_tuple[2] = blue;
	m_tuple[3] = alpha;
}

inline ColorRGBA::ColorRGBA(const float* tuple)
{
	for(int i = 0; i < 4; i++)
		m_tuple[i] = tuple[i];
}

inline ColorRGBA::ColorRGBA(const ColorRGBA& color)
{
	if(&color == this)
		return ;
	m_tuple[0] = color.m_tuple[0];
	m_tuple[1] = color.m_tuple[1];
	m_tuple[2] = color.m_tuple[2];
	m_tuple[3] = color.m_tuple[3];
}

inline float& ColorRGBA::operator[] (int i) 
{
	assert( 0 <= i && i <= 3);
	return m_tuple[i];
}

inline float ColorRGBA::operator[] (int i) const 
{
	assert( 0 <= i && i <= 3);
	return m_tuple[i];
}

#endif