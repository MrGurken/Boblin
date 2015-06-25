#include "maths.h"

rect::rect( float value )
	: x( value ), y( value ), w( value ), h( value )
{
}

rect::rect( float _x, float _y, float _w, float _h )
	: x(_x), y(_y), w(_w), h(_h)
{
}

rect::rect( const rect& ref )
	: x( ref.x ), y( ref.y ), w( ref.w ), h( ref.h )
{
}

rect::~rect()
{
}

rect& rect::operator=( const rect& ref )
{
	x = ref.x;
	y = ref.y;
	w = ref.w;
	h = ref.h;
	return *this;
}

void rect::Left( float val ) { x = val; }
void rect::Right( float val ) { x = val - w; }
void rect::Top( float val ) { y = val; }
void rect::Bottom( float val ) { y = val - h; }
void rect::TopLeft( vec2 val ) { Top( val.y ); Left( val.x ); }
void rect::TopRight( vec2 val ) { Top( val.y ); Right( val.x ); }
void rect::BottomLeft( vec2 val ) { Bottom( val.y ); Left( val.x ); }
void rect::BottomRight( vec2 val ) { Bottom( val.y ); Right( val.x ); }
void rect::Center( vec2 val ) { x = val.x - w*0.5f; y = val.y - h * 0.5f; }

float rect::Left() const { return x; }
float rect::Right() const { return x+w; }
float rect::Top() const { return y; }
float rect::Bottom() const { return y+h; }
vec2 rect::TopLeft() const { return vec2( x, y ); }
vec2 rect::TopRight() const { return vec2( x+w, y ); }
vec2 rect::BottomLeft() const { return vec2( x, y+h ); }
vec2 rect::BottomRight() const { return vec2( x+w, y+h ); }
vec2 rect::Center() const { return vec2( x+w*0.5f, y+h*0.5f ); }

bool rect::Intersect( const rect& ref ) const
{
	if( Left() > ref.Right() ) return false;
	if( Right() < ref.Left() ) return false;
	if( Top() > ref.Bottom() ) return false;
	if( Bottom() < ref.Top() ) return false;
	return true;
}

bool rect::Contains( vec2 point ) const
{
	return ( point.x >= Left() && point.x <= Right() && point.y >= Top() && point.y <= Bottom() );
}