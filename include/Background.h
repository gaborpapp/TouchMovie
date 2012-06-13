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

	void              setImage( std::string strImageName, ci::ImageSourceRef &imageSource );
	void              setMovie( std::string strMovieName, ci::qtime::MovieGl &movie       );

	std::string       getImageName();
	std::string       getMovieName();

	void              setRect( const ci::Rectf &rect );
	const ci::Rectf   getRect() const;
	const ci::Rectf   getRectOrig() const;

	bool              getUseAlphaShader();
	void              setUseAlphaShader( bool useAlphaShader );

private:
	std::string      mImageName;
	ci::gl::Texture  mPicture;
	std::string      mMovieName;
	Movie           *mpMovie;
	ci::Rectf        mRect;
	ci::Rectf        mRectOrig;
	bool             mUseAlphaShader;
};

} // namespace TouchMovie