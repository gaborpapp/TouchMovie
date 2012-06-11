#include "cinder/app/App.h"
#include "cinder/CinderMath.h"

#include "Resources.h"
#include "Movie.h"

using namespace ci;

namespace TouchMovie
{

#if USE_SHADER == 1
ci::gl::GlslProg Movie::sShader;
#endif

Movie::Movie( qtime::MovieGl &movie, Rectf &rect )
: mMovie( movie )
, mRect( rect )
, mFrame()
, mAlpha( 0.0f )
{
	mMovie.seekToStart();
	mMovie.setLoop();
	mMovie.stop();
#if USE_SHADER == 1
	if ( !sShader )
	{
		sShader = gl::GlslProg( app::loadResource( RES_MOVIE_VERT ),
				app::loadResource( RES_MOVIE_FRAG ) );
	}
#endif
}

void Movie::update()
{
	if( mMovie && isPlaying())
	{
		mMovie.setVolume( mAlpha );
		mFrame = mMovie.getTexture();
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

#if USE_SHADER == 1
		sShader.bind();
		sShader.uniform( "tex", 0 );
		sShader.uniform( "size", static_cast< Vec2f >( mFrame.getSize() ) );
#endif
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAlpha )); // red, green, blue, alpha
		gl::draw( mFrame, mRect );
#if USE_SHADER == 1
		sShader.unbind();
#endif
	}

	gl::disableAlphaBlending();
}

bool Movie::isPlaying()
{
	return mMovie.isPlaying();
}

void Movie::play( bool fromStart )
{
	if( fromStart )
		mMovie.seekToStart();
	mMovie.play();
}

void Movie::stop()
{
	mMovie.stop();
}

float Movie::getCurrentTime()
{
	return mMovie.getCurrentTime();
}

void Movie::setCurrentTime( float time )
{
	mMovie.seekToTime( time );
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

} // namespace TouchMovie
