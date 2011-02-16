#ifndef _Attributes__H
#define _Attributes__H
#include <vector>

class Attributes
{
public:
	Attributes(void);
	~Attributes(void);

public:
	void setPositionChannels(int pChannels);
	void setNormalChannels(int nChannels);
	void setTangentChannels(int tangentChannels);
	void setBitangentChannels(int bitangentChannels);
	void setTexCoordChannels(int unit, int tChannels);
	void setColorChannels(int unit, int cChannels);
	//void setfogChannels(int fchannels);

	int getTexCoordChannels(int unit) const;
	bool hasTexCoord(int unit) const ;

	int getColorChannels(int unit) const;
	bool hasColor(int unit) const ;

	
	

private:
	int						m_positionChannels;
	int						m_normalChannels;
	int						m_tangentChannels;
	int						m_bitangentChannels;
	std::vector<int>		m_texCoordChnnels;
	std::vector<int>		m_colorChannels;


	/*
	int						m_fogChannels;
	int						m_pointSizeChannels;
	int						m_blendIndicesChannels;
	*/

	

};


#endif



