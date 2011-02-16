#ifndef _VertexBuffer__H
#define _VertexBuffer__H

#include "Config.h"
#include "Attributes.h"

class VertexBuffer
{
public:
	VertexBuffer(const Attributes& att, int vertexQuantity);
	~VertexBuffer(void);
	
	const Attributes& getVertexAttributes() const { return m_attributes; }
	int32 getVertexQuantity() const { return m_vertexQuantity; }


private:
	//The format of a single vertex in buffer
	Attributes	m_attributes;

	// The size of a single vertex in the buffer
	int32	m_vertexFormatSize;

	// The number of vertices in the buffer
	int32	m_vertexQuantity;

	// Real vertex buffer
	float*  m_vertexBuffer;

};

#endif