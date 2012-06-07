#pragma once
#include "cinder/gl/Texture.h"

namespace TouchMovie
{

class Background
{
public:
	Background( ci::ImageSourceRef &imageSource, ci::Rectf &rect );
	~Background();

	void draw();

	void              setRect( const ci::Rectf &rect );
	const ci::Rectf   getRect() const;
	const ci::Rectf   getRectOrig() const;

private:
	ci::gl::Texture  mPicture;
	ci::Rectf        mRect;
	ci::Rectf        mRectOrig;
};

} // namespace TouchMovie