#include "cinder/app/App.h"
#include "cinder/CinderMath.h"

#include "Resources.h"
#include "Movie.h"

using namespace ci;

namespace TouchMovie
{

ci::gl::GlslProg Movie::sShader;

Movie::Movie( qtime::MovieGl &movie, Rectf &rect )
: mMovie( movie )
, mRect( rect )
, mFrame()
, mAlpha( 0.0f )
, mUseAlphaShader( false )
{
	mMovie.seekToStart();
	mMovie.setLoop();
	mMovie.stop();
	if ( !sShader )
	{
		sShader = gl::GlslProg( app::loadResource( RES_MOVIE_VERT ),
				app::loadResource( RES_MOVIE_FRAG ) );
	}
}

void Movie::update()
{
	if( mMovie && isPlaying())
	{
		mMovie.setVolume( mAlpha );
		mFrame = mMovie.getTexture();
	}

	if( mTrack && mTrack->isPlaying())
	{
		mTrack->setVolume( mAlpha );
	}
}

void Movie::draw()
{
	gl::enableAlphaBlending();

	if( mFrame )
	{
		Rectf rect = mRect;

		if( mRect.getX1() == -1
		 && mRect.getY1() == -1
		 && mRect.getX2() == -1
		 && mRect.getY2() == -1 )
		{
			rect = Rectf( mFrame.getBounds());
		}

		if( mUseAlphaShader )
		{
			sShader.bind();
			sShader.uniform( "tex", 0 );
			sShader.uniform( "size", static_cast< Vec2f >( mFrame.getSize() ) );
		}
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAlpha )); // red, green, blue, alpha
		gl::draw( mFrame, mRect );

		if( mUseAlphaShader )
		{
			sShader.unbind();
		}
	}

	gl::disableAlphaBlending();
}

void Movie::setAudio( audio::SourceRef &audio )
{
	mAudio = audio;
	mTrack = audio::Output::addTrack( mAudio );
	mTrack->setTime( 0 );
	mTrack->setLooping( true );
	mTrack->stop();
}

bool Movie::isPlaying()
{
	return mMovie.isPlaying();
}

void Movie::play( bool fromStart )
{
	mMovie.setVolume( mAlpha );

	if( mTrack )
		mTrack->setVolume( mAlpha );

	if( fromStart )
		mMovie.seekToStart();
	mMovie.play();

	if( mTrack )
	{
		if( fromStart )
			mTrack->setTime( 0 );
		mTrack->play();
	}
}

void Movie::stop()
{
	mMovie.stop();
	mFrame.reset();

	if( mTrack )
		mTrack->stop();
}

float Movie::getCurrentTime()
{
	return mMovie.getCurrentTime();
}

void Movie::setCurrentTime( float time )
{
	mMovie.seekToTime( time );

	if( mTrack )
		mTrack->setTime( time );
}

void Movie::setAlpha( const float alpha )
{
	mAlpha = math<float>::max( math<float>::min( alpha, 1.0f ), 0.0f );
}

float Movie::getAlpha() const
{
	return mAlpha;
}

void Movie::setRect( const Rectf &rect )
{
	mRect = rect;
}

const Rectf Movie::getRect() const
{
	return mRect;
}

bool Movie::getUseAlphaShader()
{
	return mUseAlphaShader;
}

void Movie::setUseAlphaShader( bool useAlphaShader )
{
	mUseAlphaShader = useAlphaShader;
}

} // namespace TouchMovie
