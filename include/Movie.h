#pragma once
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "Area.h"

using namespace ci;

namespace TouchMovie
{

class Movie
{
public:
	Movie( qtime::MovieGl &movie, Rectf &rect );

	void update();
	void draw();

	bool              isPlaying();
	void              play();
	void              stop();

	void              setAlpha( const float alpha );
	float             getAlpha() const;
	void              setRect( const Rectf &alpha );
	const Rectf       getRect() const;

	const int         getWidth() const;
	const int         getHeight() const;

private:
	qtime::MovieGl   mMovie;
	Rectf            mRect;

	gl::Texture      mFrame;
	float            mAlpha;
};

} // namespace TouchMovie