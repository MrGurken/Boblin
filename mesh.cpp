#include "mesh.h"

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
	//glEnableVertexAttribArray( 2 ); // normal
	//glEnableVertexAttribArray( 3 ); // tangent
	//glEnableVertexAttribArray( 4 ); // bitangent

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(GLfloat)*3) );
	//glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(GLfloat)*5) );
	//glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(GLfloat)*8) );
	//glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(GLfloat)*11) );

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