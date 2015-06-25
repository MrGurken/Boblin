#ifndef MATHS_H
#define MATHS_H

#include "glm.hpp"
using namespace glm;

class rect
{
public:
	rect( float value = 0.0f );
	rect( float x, float y, float w, float h );
	rect( const rect& ref );
	~rect();

	rect& operator=( const rect& ref );
	
	void Left( float val );
	void Right( float val );
	void Top( float val );
	void Bottom( float val );

	void TopLeft( vec2 val );
	void TopRight( vec2 val );
	void BottomLeft( vec2 val );
	void BottomRight( vec2 val );

	void Center( vec2 val );

	float Left() const;
	float Right() const;
	float Top() const;
	float Bottom() const;

	vec2 TopLeft() const;
	vec2 TopRight() const;
	vec2 BottomLeft() const;
	vec2 BottomRight() const;

	vec2 Center() const;

	bool Intersect( const rect& ref ) const;
	bool Contains( vec2 point ) const;

	float x, y, w, h;
};

#endif