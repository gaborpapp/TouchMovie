#pragma once
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/gl/GlslProg.h"
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
	void              play( bool fromStart );
	void              stop();
	float             getCurrentTime();
	void              setCurrentTime( float time );

	void              setAlpha( const float alpha );
	float             getAlpha() const;
	void              setRect( const ci::Rectf &alpha );
	const ci::Rectf   getRect() const;

private:
	ci::qtime::MovieGl mMovie;
	ci::Rectf          mRect;

	ci::gl::Texture    mFrame;
	float              mAlpha;

	static ci::gl::GlslProg sShader;
};

} // namespace TouchMovie
