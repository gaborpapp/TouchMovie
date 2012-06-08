#pragma once
#include "cinder/gl/Texture.h"
#include "Movie.h"

namespace TouchMovie
{

class Background
{
public:
	Background( ci::Rectf &rect );
	~Background();

	void update();
	void draw();

	void              setImage( ci::ImageSourceRef &imageSource );
	void              setMovie( ci::qtime::MovieGl &movie       );

	void              setRect( const ci::Rectf &rect );
	const ci::Rectf   getRect() const;
	const ci::Rectf   getRectOrig() const;

private:
	ci::gl::Texture  mPicture;
	Movie           *mpMovie;
	ci::Rectf        mRect;
	ci::Rectf        mRectOrig;
};

} // namespace TouchMovie