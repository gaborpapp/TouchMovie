#pragma once
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/Audio/Io.h"
#include "cinder/Audio/Output.h"
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

	void              setAudio( ci::audio::SourceRef &audio );

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
	ci::qtime::MovieGl    mMovie;
	ci::Rectf             mRect;

	ci::audio::SourceRef  mAudio;
	ci::audio::TrackRef   mTrack;

	ci::gl::Texture       mFrame;
	float                 mAlpha;
	bool                  mUseAlphaShader;

	static ci::gl::GlslProg sShader;
};

} // namespace TouchMovie
