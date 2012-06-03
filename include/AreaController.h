#pragma once
#include "Area.h"
#include "cinder/app/MouseEvent.h"

namespace TouchMovie
{

class AreaController
{
public:
	AreaController();
	void update();
	void draw();
	void mouseMove( ci::app::MouseEvent event );
	void mouseDown( ci::app::MouseEvent event );
	void mouseUp  ( ci::app::MouseEvent event );

	void addArea( std::string name, ci::Rectf &rect );
	void removeArea( std::string name );
	void setMain( std::string name, bool main );
	bool setMovie( std::string name, ci::fs::path pathMovie );
	void setRect( std::string name, ci::Rectf &rect );
	void setAlpha( std::string name, float alpha );

	const int getWidth ( std::string name );
	const int getHeigth( std::string name );

	void setDrawFrame( std::string name, bool drawFrame );
	bool getDrawFrame( std::string name );

	void setFadeIn( std::string name, float fadeIn );
	float getFadeIn( std::string name );

	void setFadeOut( std::string name, float fadeOut );
	float getFadeOut( std::string name );

	Area *getAreaMain();

	void resize();

	void setTouchPosBeg();
	void setTouchPos   ( ci::Vec2f jointPos );
	void setTouchPosEnd();

private:
	Area  *_getArea( std::string name );
	Area  *_getArea( const ci::Vec2i &pos );
	bool   _isAreaAct ( Area *pArea );

private:

	Area               *mpAreaMain;
	Area               *mpAreaMouse;
	std::vector<Area*>  mAreas;
	std::vector<Area*>  mAreasAct;
};

} // namespace TouchMovie
