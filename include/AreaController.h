#pragma once
#include "Area.h"
#include "Background.h"
#include "cinder/app/MouseEvent.h"

namespace TouchMovie
{

class AreaController
{
public:
	AreaController();
	~AreaController();
	void update();
	void draw();
	void mouseMove( ci::app::MouseEvent event );
	void mouseDown( ci::app::MouseEvent event );
	void mouseUp  ( ci::app::MouseEvent event );

	void addArea( std::string name, ci::Rectf &rect );
	void removeArea( std::string name );
	void setMovieBack( std::string name, ci::fs::path pathMovie );
	void setMovieFore( std::string name, ci::fs::path pathMovie );
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

	void        setBackground( ci::fs::path pathPicture, int width, int height );
	Background *getBackground();
private:
	Area  *_getArea( std::string name );
	Area  *_getArea( const ci::Vec2i &pos );
	bool   _isAreaAct ( Area *pArea );
	ci::qtime::MovieGl _loadMovie( ci::fs::path pathMovie );

private:
	Background         *mpBackground;
	Area               *mpAreaMouse;
	std::vector<Area*>  mAreas;
	std::vector<Area*>  mAreasAct;
};

} // namespace TouchMovie
