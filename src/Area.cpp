#include "Area.h"
#include "cinder/CinderMath.h"

using namespace ci;

namespace TouchMovie
{

Area::Area( std::string name, Rectf &rect )
: mName( name )
, mRect( rect )
, mpMovie( 0 )
, mAlphaChange( 0 )
{
}

Area::~Area()
{
	delete mpMovie;
}

void Area::update()
{
	_changeAlpha();

	if( mpMovie )
		mpMovie->update();
}

void Area::draw()
{
	if( mpMovie )
		mpMovie->draw();

	if( mDrawFrame )
	{
		gl::drawString( mName, mRect.getUpperLeft());
		gl::drawLine( mRect.getUpperLeft() , mRect.getUpperRight());
		gl::drawLine( mRect.getUpperRight(), mRect.getLowerRight());
		gl::drawLine( mRect.getLowerRight(), mRect.getLowerLeft() );
		gl::drawLine( mRect.getLowerLeft() , mRect.getUpperLeft() );
	}
}

void Area::show( bool show )
{
	if( show )
		mAlphaChange = mFadeIn;
	else
		mAlphaChange = -mFadeOut;
}

void Area::setMovie( qtime::MovieGl &movie )
{
	if( mpMovie )
		delete mpMovie;

	mpMovie = new Movie( movie, mRect );
}

const std::string Area::getName() const
{
	return mName;
}

void Area::setAlpha( const float alpha )
{
	if( mpMovie )
	{
		mpMovie->setAlpha( alpha );

		if( alpha > 0 )
		{
			if( ! mpMovie->isPlaying())
				mpMovie->play();
		}
		else
		{
			mpMovie->stop();
		}
	}
}

const float Area::getAlpha() const
{
	if( mpMovie )
		return mpMovie->getAlpha();

	return 0;
}

void Area::setRect( const Rectf &rect )
{
	mRect = rect;

	if( mpMovie )
		mpMovie->setRect( rect );
}

const Rectf Area::getRect() const
{
	return mRect;
}

const int Area::getWidth() const
{
	if( mpMovie )
		return mpMovie->getWidth();

	return 0;
}

const int Area::getHeight() const
{
	if( mpMovie )
		return mpMovie->getHeight();

	return 0;
}

void Area::setDrawFrame( const bool drawFrame )
{
	mDrawFrame = drawFrame;
}

const bool Area::getDrawFrame() const
{
	return mDrawFrame;
}

void Area::setFadeIn( const float fadeIn )
{
	mFadeIn = (1.0f/FRAME_RATE)/fadeIn;
}

const float Area::getFadeIn() const
{
	return (1.0f/FRAME_RATE)/mFadeIn;
}

void Area::setFadeOut( const float fadeOut )
{
	mFadeOut = (1.0f/FRAME_RATE)/fadeOut;
}

const float Area::getFadeOut() const
{
	return (1.0f/FRAME_RATE)/mFadeOut;
}

void Area::_changeAlpha()
{
	if( mAlphaChange == 0 )
		return;

	setAlpha( getAlpha() + mAlphaChange );

	if( ( mAlphaChange < 0 && getAlpha() == 0.0f )
	 || ( mAlphaChange > 0 && getAlpha() == 1.0f ))
	{
		mAlphaChange = 0;
	}
}

} // namespace TouchMovie
