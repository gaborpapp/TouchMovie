#pragma once
#include "Area.h"
#include "Background.h"
#include "cinder/app/MouseEvent.h"

namespace TouchMovie
{

class AreaController
{
public:
	typedef void (*ActionFunc)( Area *pArea, void *pvData );

	AreaController();
	~AreaController();
	void update();
	void draw();
	void mouseMove( ci::app::MouseEvent event );
	void mouseDown( ci::app::MouseEvent event );
	void mouseUp  ( ci::app::MouseEvent event );

	void addArea( std::string name, ci::Rectf &rect );
	void removeArea( std::string name );
	void setMovieIdle( std::string name, ci::fs::path pathMovie );
	void setMovieActive( std::string name, ci::fs::path pathMovie );
	void setRect( std::string name, ci::Rectf &rect );

	void setDrawFrame( std::string name, bool drawFrame );
	bool getDrawFrame( std::string name );

	void setFadeIn( std::string name, float fadeIn );
	float getFadeIn( std::string name );

	void setFadeOut( std::string name, float fadeOut );
	float getFadeOut( std::string name );

	void resize();

	void setTouchPosBeg();
	void setTouchPos   ( ci::Vec2f jointPos );
	void setTouchPosEnd();

	void        setBackground( int width, int height );
	void        setBackgroundImage( ci::fs::path pathImage );
	void        setBackgroundMovie( ci::fs::path pathMovie );
	Background *getBackground();
private:
	Area  *_getArea( std::string name );
	Area  *_getArea( const ci::Vec2i &pos );
	void   _actionArea( const ci::Vec2i &pos, ActionFunc pActionFunc );
	bool   _isAreaAct ( Area *pArea );

	static void _setTouchPosAction( Area *pArea, void *pvData );
	static void _setMousePosAction( Area *pArea, void *pvData );

	ci::qtime::MovieGl _loadMovie( ci::fs::path pathMovie );
	ci::ImageSourceRef _loadImage( ci::fs::path pathImage );

private:
	Background         *mpBackground;
	std::vector<Area*>  mAreas;
	std::vector<Area*>  mAreasActMouse;
	std::vector<Area*>  mAreasActTouch;
};

} // namespace TouchMovie
