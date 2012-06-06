#include "cinder/app/AppBasic.h"
#include "Area.h"
#include "cinder/CinderMath.h"

using namespace ci;

namespace TouchMovie
{

Area::Area( std::string name, Rectf &rect )
: mName( name )
, mRect( rect )
, mRectOrig( rect )
, mpMovie( 0 )
, mFadeIn( 1.0f )
, mFadeOut( 1.0f )
, mDrawFrame( false )
, mShow( false )
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
		gl::drawStrokedRect( mRect );
	}
}

void Area::show( bool show )
{
	if( mShow == show )
		return;

	mShow     = show;
	mLastTime = ci::app::getElapsedSeconds();
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

const Rectf Area::getRectOrig() const
{
	return mRectOrig;
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
	mFadeIn = fadeIn;
}

const float Area::getFadeIn() const
{
	return mFadeIn;
}

void Area::setFadeOut( const float fadeOut )
{
	mFadeOut = fadeOut;
}

const float Area::getFadeOut() const
{
	return mFadeOut;
}

void Area::_changeAlpha()
{
	if( ( mShow == false && getAlpha() == 0.0f )
	 || ( mShow == true  && getAlpha() == 1.0f ))
		return;

	double fadeAct = mShow ? mFadeIn : - mFadeOut;
	double actTime = ci::app::getElapsedSeconds();
	double alphaChange = ( actTime - mLastTime ) / fadeAct;

	mLastTime = actTime;

	setAlpha( getAlpha() + (float)alphaChange );
}

} // namespace TouchMovie
