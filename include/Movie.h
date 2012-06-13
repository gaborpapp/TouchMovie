#pragma once
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/gl/GlslProg.h"
#include "Area.h"

#define USE_SHADER 0

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

	bool              getUseAlphaShader();
	void              setUseAlphaShader( bool useAlphaShader );

private:
	ci::qtime::MovieGl mMovie;
	ci::Rectf          mRect;

	ci::gl::Texture    mFrame;
	float              mAlpha;
	bool               mUseAlphaShader;

	static ci::gl::GlslProg sShader;
};

} // namespace TouchMovie
