#pragma once
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "Area.h"

namespace TouchMovie
{

class Movie
{
public:
	Movie( ci::qtime::MovieGl &movie, ci::Rectf &rect );

	void update();
	void draw();

	bool              isPlaying();
	void              play();
	void              stop();

	void              setAlpha( const float alpha );
	float             getAlpha() const;
	void              setRect( const ci::Rectf &alpha );
	const ci::Rectf   getRect() const;

	const int         getWidth() const;
	const int         getHeight() const;

private:
	ci::qtime::MovieGl mMovie;
	ci::Rectf          mRect;

	ci::gl::Texture    mFrame;
	float              mAlpha;
};

} // namespace TouchMovie