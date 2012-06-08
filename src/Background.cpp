#include "Background.h"

using namespace ci;

namespace TouchMovie
{

Background::Background( Rectf &rect )
: mPicture()
, mpMovie(  0 )
, mRect( rect )
, mRectOrig( rect )
{
}

void Background::setImage( ci::ImageSourceRef &imageSource )
{
	mPicture = gl::Texture( imageSource );
}

void Background::setMovie( ci::qtime::MovieGl &movie )
{
	if( mpMovie )
		delete mpMovie;

	mpMovie = new Movie( movie, mRect );
	mpMovie->setAlpha( 1.0f );
	mpMovie->play( true );
}

Background::~Background()
{
	if( mpMovie )
		delete mpMovie;
}

void Background::update()
{
	if( mpMovie )
		mpMovie->update();
}

void Background::draw()
{
	if( mPicture )
		gl::draw( mPicture, mRect );

	if( mpMovie )
		mpMovie->draw();
}

void Background::setRect( const Rectf &rect )
{
	mRect = rect;

	if( mpMovie )
		mpMovie->setRect( rect );
}

const Rectf Background::getRect() const
{
	return mRect;
}

const Rectf Background::getRectOrig() const
{
	return mRectOrig;
}

} // namespace TouchMovie
