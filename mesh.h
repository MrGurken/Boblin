#ifndef MESH_H
#define MESH_H

#include "LibIncluder.h"

struct Vertex
{
	float x, y, z;		// position
	float u, v;			// texture coordinates
	//float nx, ny, nz; // normal
	//float tx, ty, tz; // tangent
	//float bx, by, bz; // bitangent
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void	AddVertices( Vertex* vertices, int nv, GLuint* indices, int ni );
	void	Render();

	void	SetRenderType( GLenum renderType );
	GLenum	GetRenderType() const;

	static Mesh* GetQuad();

private:
	GLuint m_glVAO;
	GLuint m_glVBO;
	GLuint m_glIBO;
	GLenum m_glRenderType;
	int m_iSize;

	static Mesh* s_pQuad;
};

#endif