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
, mpMovieIdle( 0 )
, mpMovieActive( 0 )
, mFadeIn( 1.0f )
, mFadeOut( 1.0f )
, mDrawFrame( false )
, mState( AS_IDLE )
, mUseAlphaShader( true )
{
}

Area::~Area()
{
	if( mpMovieIdle )
		delete mpMovieIdle;
	if( mpMovieActive )
		delete mpMovieActive;
}

void Area::update()
{
	_changeAlpha();

	if( mpMovieIdle )
		mpMovieIdle->update();
	if( mpMovieActive )
		mpMovieActive->update();
}

void Area::draw()
{
	if( mpMovieIdle )
		mpMovieIdle->draw();
	if( mpMovieActive )
		mpMovieActive->draw();

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
	case AS_IDLE           :
		if( active )
		{
			// immediate change
			setAlphaActive( 1.0f );
			setAlphaIdle  ( 0.0f );
			mState = AS_ACTIVE;
		}
		return;
		break;
	case AS_ACTIVE_TO_IDLE :
		if( active )
			mState = AS_IDLE_TO_ACTIVE;
		else
			return;
		break;
	case AS_ACTIVE         :
	case AS_IDLE_TO_ACTIVE :
		if( active )
			return;
		else
			mState = AS_ACTIVE_TO_IDLE;
		break;
	}

	mLastTime = ci::app::getElapsedSeconds();
}

void Area::setMovieIdle( qtime::MovieGl &movie )
{
	if( mpMovieIdle )
		delete mpMovieIdle;

	mpMovieIdle = new Movie( movie, mRect );

	mpMovieIdle->setUseAlphaShader( mUseAlphaShader );
	mpMovieIdle->setAlpha( 1.0f );
	mpMovieIdle->play( true );
}

void Area::setMovieActive( qtime::MovieGl &movie )
{
	if( mpMovieActive )
		delete mpMovieActive;

	mpMovieActive = new Movie( movie, mRect );

	mpMovieActive->setUseAlphaShader( mUseAlphaShader );
	mpMovieActive->setAlpha( 0.0f );
	mpMovieActive->stop();
}

const std::string Area::getName() const
{
	return mName;
}

void Area::setAlphaIdle( const float alpha )
{
	if( mpMovieIdle )
	{
		mpMovieIdle->setAlpha( alpha );

		if( alpha > 0 )
		{
			if( ! mpMovieIdle->isPlaying())
				mpMovieIdle->play( true );
		}
		else
		{
			mpMovieIdle->stop();
			mState = AS_ACTIVE;
		}
	}
}

const float Area::getAlphaIdle() const
{
	if( mpMovieIdle )
		return mpMovieIdle->getAlpha();

	return 0;
}

void Area::setAlphaActive( const float alpha )
{
	if( mpMovieActive )
	{
		mpMovieActive->setAlpha( alpha );

		if( alpha > 0 )
		{
			if( ! mpMovieActive->isPlaying())
			{
				float time = mpMovieIdle->getCurrentTime();
				mpMovieActive->setCurrentTime( time );
				mpMovieActive->play( false );
			}
		}
		else
		{
			mpMovieActive->stop();
			mState = AS_IDLE;
		}
	}
}

const float Area::getAlphaActive() const
{
	if( mpMovieActive )
		return mpMovieActive->getAlpha();

	return 0;
}

void Area::setRect( const Rectf &rect )
{
	mRect = rect;

	if( mpMovieIdle )
		mpMovieIdle->setRect( rect );
	if( mpMovieActive )
		mpMovieActive->setRect( rect );
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

bool Area::getUseAlphaShader()
{
	return mUseAlphaShader;
}

void Area::setUseAlphaShader( bool useAlphaShader )
{
	mUseAlphaShader = useAlphaShader;

	if( mpMovieIdle )
		mpMovieIdle->setUseAlphaShader( useAlphaShader );

	if( mpMovieActive )
		mpMovieActive->setUseAlphaShader( useAlphaShader );
}

void Area::_changeAlpha()
{
	double fadeAct = 0.0f;

	switch( mState )
	{
	case AS_ACTIVE         : return;
	case AS_IDLE           : return;
	case AS_IDLE_TO_ACTIVE : fadeAct = mFadeIn;   break;
	case AS_ACTIVE_TO_IDLE : fadeAct = -mFadeOut; break;
	}

	double actTime = ci::app::getElapsedSeconds();
	double alphaChange = ( actTime - mLastTime ) / fadeAct;

	mLastTime = actTime;

	setAlphaActive( getAlphaActive() + (float)alphaChange );
	setAlphaIdle  ( getAlphaIdle()   - (float)alphaChange );
}

} // namespace TouchMovie
