#include "mesh.h"

Mesh* Mesh::s_pQuad = nullptr;

Mesh::Mesh()
	: m_glVAO( 0 ), m_glVBO( 0 ), m_glIBO( 0 ), m_iSize( 0 ), m_glRenderType( GL_TRIANGLES )
{
}

Mesh::~Mesh()
{
	if( m_glVAO != 0 )
		glDeleteVertexArrays( 1, &m_glVAO );
	if( m_glVBO != 0 )
		glDeleteBuffers( 1, &m_glVBO );
	if( m_glIBO != 0 )
		glDeleteBuffers( 1, &m_glIBO );

	// TODO: Refcount quad to remove it when no mesh is using it?
}

void Mesh::AddVertices( Vertex* vertices, int nv, GLuint* indices, int ni )
{
	// generate a vertex array object
	glGenVertexArrays( 1, &m_glVAO );
	glBindVertexArray( m_glVAO );

	// generate a vertex buffer object and send vertex information to it
	glGenBuffers( 1, &m_glVBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_glVBO );
	glBufferData( GL_ARRAY_BUFFER, nv*sizeof(Vertex), vertices, GL_STATIC_DRAW );
	
	// generate a index buffer object and send index information to it
	glGenBuffers( 1, &m_glIBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_glIBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLuint), indices, GL_STATIC_DRAW );

	glEnableVertexAttribArray( 0 ); // position
	glEnableVertexAttribArray( 1 ); // texture coordinates

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(GLfloat)*3) );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	m_iSize = ni;
}

void Mesh::Render()
{
	glBindVertexArray( m_glVAO );
	glBindBuffer( GL_ARRAY_BUFFER, m_glVBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_glIBO );

	glDrawElements( m_glRenderType, m_iSize, GL_UNSIGNED_INT, 0 );
}

void Mesh::SetRenderType( GLenum renderType ) { m_glRenderType = renderType; }
GLenum Mesh::GetRenderType() const { return m_glRenderType; }

Mesh* Mesh::GetQuad()
{
	if( s_pQuad == nullptr )
	{
		Vertex vertices[] =
		{
			{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f, 1.0f, 1.0f }
		};

		GLuint indices[] =
		{
			0, 1, 2,
			1, 3, 2
		};

		s_pQuad = new Mesh();
		s_pQuad->AddVertices( vertices, 4, indices, 6 );
	}

	return s_pQuad;
}