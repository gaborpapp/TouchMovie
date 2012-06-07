#pragma once
#include "Movie.h"

namespace TouchMovie
{

static const float FRAME_RATE   = 60.0f;

class Area
{
private:
	enum State
	{
		AS_BACK,
		AS_BACK_TO_FORE,
		AS_FORE,
		AS_FORE_TO_BACK
	};

public:
	Area( std::string name, ci::Rectf &rect );
	~Area();

	void update();
	void draw();

	void setActive( bool active );

	void              setMovieBack( ci::qtime::MovieGl &movie );
	void              setMovieFore( ci::qtime::MovieGl &movie );
	const std::string getName() const;
	void              setAlphaBack( const float Alpha );
	const float       getAlphaBack() const;
	void              setAlphaFore( const float Alpha );
	const float       getAlphaFore() const;
	void              setRect( const ci::Rectf &rect );
	const ci::Rectf   getRect() const;
	const ci::Rectf   getRectOrig() const;

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
	ci::Rectf        mRectOrig;
	Movie           *mpMovieBack;
	Movie           *mpMovieFore;

	bool             mDrawFrame;
	State            mState;
	double           mLastTime;

	float            mFadeIn;
	float            mFadeOut;
};

} // namespace TouchMovie