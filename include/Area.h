#pragma once
#include "Movie.h"

using namespace ci;

namespace TouchMovie
{

class Area
{
public:
	Area( std::string name, Rectf &rect );
	~Area();

	void update();
	void draw();

	void show( bool show );

	void              setMovie( qtime::MovieGl &movie );
	const std::string getName() const;
	void              setAlpha( const float alpha );
	const float       getAlpha() const;
	void              setRect( const Rectf &alpha );
	const Rectf       getRect() const;

	const int         getWidth() const;
	const int         getHeight() const;

private:
	void              _changeAlpha();

private:
	std::string      mName;
	Rectf            mRect;
	Movie           *mpMovie;

	float            mAlphaChange;
};

} // namespace TouchMovie