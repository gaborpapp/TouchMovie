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
		AS_IDLE,
		AS_IDLE_TO_ACTIVE,
		AS_ACTIVE,
		AS_ACTIVE_TO_IDLE
	};

public:
	Area( std::string name, ci::Rectf &rect );
	~Area();

	void update();
	void draw();

	void setActive( bool active );

	void              setMovieIdle( ci::qtime::MovieGl &movie );
	void              setMovieActive( ci::qtime::MovieGl &movie );
	const std::string getName() const;
	void              setAlphaIdle( const float Alpha );
	const float       getAlphaIdle() const;
	void              setAlphaActive( const float Alpha );
	const float       getAlphaActive() const;
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
	Movie           *mpMovieIdle;
	Movie           *mpMovieActive;

	bool             mDrawFrame;
	State            mState;
	double           mLastTime;

	float            mFadeIn;
	float            mFadeOut;
};

} // namespace TouchMovie