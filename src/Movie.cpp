#include "Movie.h"
#include "cinder/CinderMath.h"

using namespace ci;

namespace TouchMovie
{

Movie::Movie( qtime::MovieGl &movie, Rectf &rect )
: mMovie( movie )
, mRect( rect )
, mFrame()
, mAlpha( 0.0f )
{
	mMovie.play();
	mMovie.seekToStart();
	mMovie.setLoop();
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

		gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAlpha )); // red, green, blue, alpha
		gl::draw( mFrame, mRect );
	}
}

bool Movie::isPlaying()
{
	return mMovie.isPlaying();
}

void Movie::play()
{
	mMovie.seekToStart();
	mMovie.play();
}

void Movie::stop()
{
	mMovie.stop();
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

const int Movie::getWidth() const
{
	return mMovie.getWidth();
}

const int Movie::getHeight() const
{
	return mMovie.getHeight();
}

} // namespace TouchMovie
