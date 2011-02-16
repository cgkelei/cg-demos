#include "Attributes.h"
#include <assert.h>

Attributes::Attributes(void)
{
}

Attributes::~Attributes(void)
{
}

void Attributes::setPositionChannels(int pChannels)
{
	 assert(0 <= pChannels && pChannels <= 4);
	 m_positionChannels = pChannels;
}

void Attributes::setNormalChannels(int nChannels)
{
	assert(0 <= nChannels && nChannels <= 4);
	m_normalChannels = nChannels;
}

void Attributes::setTexCoordChannels(int unit, int tChannels)
{	
	assert(0 <= unit && 0 <= tChannels && tChannels <= 4);
	int maxUnits = (int) m_texCoordChnnels.size();
	if (unit >= maxUnits)
	{
		m_texCoordChnnels.resize(unit+1);
	}
	m_texCoordChnnels[unit] = tChannels;
}

void Attributes::setTangentChannels(int tangentChannels)
{
	assert(0 <= tangentChannels && tangentChannels <= 4);
	m_tangentChannels = tangentChannels;
}


void Attributes::setBitangentChannels(int bitangentChannels)
{
	assert(0 <= bitangentChannels && bitangentChannels <= 4);
	m_bitangentChannels = bitangentChannels;
}

void Attributes::setColorChannels(int unit, int cChannels)
{
	assert(0 <= unit && 0 <= cChannels && cChannels <= 4);
	int maxChannels = (int)m_colorChannels.size();
	if(unit >= maxChannels)
	{
		m_colorChannels.reserve(unit+1);
	}
	m_colorChannels[unit] = cChannels;
}



/*
void Attributes::setfogChannels(int fchannels)
{
	assert(0 <= fchannels && fchannels <= 4);
	m_fogChannels = fchannels;
}
*/


int Attributes::getTexCoordChannels(int unit) const
{
	if(0 <= unit && unit < (int)m_texCoordChnnels.size())
	{
		return m_texCoordChnnels[unit];
	}
	return 0;
}


int Attributes::getColorChannels(int unit) const
{
	if(0 <= unit && unit < (int)m_colorChannels.size())
	{
		return m_colorChannels[unit];
	}
	return 0;
}


bool Attributes::hasColor(int unit) const 
{
	if(0 <= unit && unit < (int)m_colorChannels.size())
	{
		return m_colorChannels[unit] > 0 ;
	}
	return false;
}

bool Attributes::hasTexCoord(int unit) const 
{
	if(0 <= unit && unit < (int)m_texCoordChnnels.size())
	{
		return m_texCoordChnnels[unit] > 0;
	}
	return false;
}
