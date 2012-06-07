#include "Background.h"

using namespace ci;

namespace TouchMovie
{

Background::Background( ci::ImageSourceRef &imageSource, Rectf &rect )
: mPicture()
, mRect( rect )
, mRectOrig( rect )
{
	mPicture = gl::Texture( imageSource );
}

Background::~Background()
{
}

void Background::draw()
{
	if( mPicture )
		gl::draw( mPicture, mRect );
}

void Background::setRect( const Rectf &rect )
{
	mRect = rect;
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
