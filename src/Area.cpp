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
, mpMovieBack( 0 )
, mpMovieFore( 0 )
, mFadeIn( 1.0f )
, mFadeOut( 1.0f )
, mDrawFrame( false )
, mState( AS_BACK )
{
}

Area::~Area()
{
	if( mpMovieBack )
		delete mpMovieBack;
	if( mpMovieFore )
		delete mpMovieFore;
}

void Area::update()
{
	_changeAlpha();

	if( mpMovieBack )
		mpMovieBack->update();
	if( mpMovieFore )
		mpMovieFore->update();
}

void Area::draw()
{
	if( mpMovieBack )
		mpMovieBack->draw();
	if( mpMovieFore )
		mpMovieFore->draw();

	if( mDrawFrame )
	{
		gl::drawString( mName, mRect.getUpperLeft());
		gl::drawStrokedRect( mRect );
	}
}

void Area::setActive( bool active )
{
	switch( mState )
	{
	case AS_BACK         :
	case AS_FORE_TO_BACK :
		if( active )
			mState = AS_BACK_TO_FORE;
		else
			return;
		break;
	case AS_FORE         :
	case AS_BACK_TO_FORE :
		if( active )
			return;
		else
			mState = AS_FORE_TO_BACK;
		break;
	}

	mLastTime = ci::app::getElapsedSeconds();
}

void Area::setMovieBack( qtime::MovieGl &movie )
{
	if( mpMovieBack )
		delete mpMovieBack;

	mpMovieBack = new Movie( movie, mRect );

	mpMovieBack->setAlpha( 1.0f );
	mpMovieBack->play( true );
}

void Area::setMovieFore( qtime::MovieGl &movie )
{
	if( mpMovieFore )
		delete mpMovieFore;

	mpMovieFore = new Movie( movie, mRect );

	mpMovieFore->setAlpha( 0.0f );
	mpMovieFore->stop();
}

const std::string Area::getName() const
{
	return mName;
}

void Area::setAlphaBack( const float alpha )
{
	if( mpMovieBack )
	{
		mpMovieBack->setAlpha( alpha );

		if( alpha > 0 )
		{
			if( ! mpMovieBack->isPlaying())
				mpMovieBack->play( true );
		}
		else
		{
			mpMovieBack->stop();
			mState = AS_FORE;
		}
	}
}

const float Area::getAlphaBack() const
{
	if( mpMovieBack )
		return mpMovieBack->getAlpha();

	return 0;
}

void Area::setAlphaFore( const float alpha )
{
	if( mpMovieFore )
	{
		mpMovieFore->setAlpha( alpha );

		if( alpha > 0 )
		{
			if( ! mpMovieFore->isPlaying())
			{
				float time = mpMovieBack->getCurrentTime();
				mpMovieFore->setCurrentTime( time );
				mpMovieFore->play( false );
			}
		}
		else
		{
			mpMovieFore->stop();
			mState = AS_BACK;
		}
	}
}

const float Area::getAlphaFore() const
{
	if( mpMovieFore )
		return mpMovieFore->getAlpha();

	return 0;
}

void Area::setRect( const Rectf &rect )
{
	mRect = rect;

	if( mpMovieBack )
		mpMovieBack->setRect( rect );
	if( mpMovieFore )
		mpMovieFore->setRect( rect );
}

const Rectf Area::getRect() const
{
	return mRect;
}

const Rectf Area::getRectOrig() const
{
	return mRectOrig;
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
	double fadeAct = 0.0f;

	switch( mState )
	{
	case AS_FORE         : return;
	case AS_BACK         : return;
	case AS_BACK_TO_FORE : fadeAct = mFadeIn;   break;
	case AS_FORE_TO_BACK : fadeAct = -mFadeOut; break;
	}

	double actTime = ci::app::getElapsedSeconds();
	double alphaChange = ( actTime - mLastTime ) / fadeAct;

	mLastTime = actTime;

	setAlphaFore( getAlphaFore() + (float)alphaChange );
	setAlphaBack( getAlphaBack() - (float)alphaChange );
}

} // namespace TouchMovie
