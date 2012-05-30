#pragma once
#include "Movie.h"

namespace TouchMovie
{

static const float FRAME_RATE   = 60.0f;

class Area
{
public:
	Area( std::string name, ci::Rectf &rect );
	~Area();

	void update();
	void draw();

	void show( bool show );

	void              setMovie( ci::qtime::MovieGl &movie );
	const std::string getName() const;
	void              setAlpha( const float alpha );
	const float       getAlpha() const;
	void              setRect( const ci::Rectf &alpha );
	const ci::Rectf   getRect() const;

	const int         getWidth() const;
	const int         getHeight() const;

	void              setDrawFrame( const bool drawFrame );
	const bool        getDrawFrame() const;

	void              setFadeIn( const float fadeIn );
	const float       getFadeIn() const;

	void              setFadeOut( const float fadeOut );
	const float       getFadeOut() const;
private:
	void              _changeAlpha();

private:
	std::string      mName;
	ci::Rectf        mRect;
	Movie           *mpMovie;

	bool             mDrawFrame;

	float            mFadeIn;
	float            mFadeOut;

	float            mAlphaChange;
};

} // namespace TouchMovie