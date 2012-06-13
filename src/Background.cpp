#include "Background.h"

using namespace ci;

namespace TouchMovie
{

Background::Background( Rectf &rect )
: mPicture()
, mpMovie(  0 )
, mRect( rect )
, mRectOrig( rect )
, mUseAlphaShader( false )
{
}

void Background::setImage( std::string strImageName, ci::ImageSourceRef &imageSource )
{
	mImageName = strImageName;
	mPicture   = gl::Texture( imageSource );
}

void Background::setMovie( std::string strMovieName, ci::qtime::MovieGl &movie )
{
	if( mpMovie )
		delete mpMovie;

	mMovieName = strMovieName;
	mpMovie    = new Movie( movie, mRect );
	mpMovie->setUseAlphaShader( mUseAlphaShader );
	mpMovie->setAlpha( 1.0f );
	mpMovie->play( true );
}

std::string Background::getImageName()
{
	return mImageName;
}

std::string Background::getMovieName()
{
	return mMovieName;
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

bool Background::getUseAlphaShader()
{
	return mUseAlphaShader;
}

void Background::setUseAlphaShader( bool useAlphaShader )
{
	mUseAlphaShader = useAlphaShader;

	if( mpMovie )
		mpMovie->setUseAlphaShader( useAlphaShader );
}

} // namespace TouchMovie
