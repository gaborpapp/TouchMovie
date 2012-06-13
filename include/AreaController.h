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

	void addArea( std::string areaName, ci::Rectf &rect );
	void removeArea( std::string areaName );
	void setMovieIdle( std::string areaName, std::string movieName );
	void setMovieActive( std::string areaName, std::string movieName );
	void setRect( std::string areaName, ci::Rectf &rect );

	void setDrawFrame( std::string areaName, bool drawFrame );
	bool getDrawFrame( std::string areaName );

	void  setFadeIn( std::string areaName, float fadeIn );
	float getFadeIn( std::string areaName );

	void  setFadeOut( std::string areaName, float fadeOut );
	float getFadeOut( std::string areaName );

	void  setUseAlphaShader( std::string areaName, bool useAlphaShader );
	bool  getUseAlphaShader( std::string areaName );

	void resize();

	void setTouchPosBeg();
	void setTouchPos   ( ci::Vec2f jointPos );
	void setTouchPosEnd();

	void        setBackground( int width, int height );
	void        setBackgroundImage( std::string strImageName );
	void        setBackgroundMovie( std::string strMovieName );
	Background *getBackground();
private:
	Area  *_getArea( std::string areaName );
	Area  *_getArea( const ci::Vec2i &pos );
	void   _actionArea( const ci::Vec2i &pos, ActionFunc pActionFunc );
	bool   _isAreaAct ( Area *pArea );

	static void _setTouchPosAction( Area *pArea, void *pvData );
	static void _setMousePosAction( Area *pArea, void *pvData );

	ci::ImageSourceRef _loadImage( std::string strImageName );
	ci::qtime::MovieGl _loadMovie( std::string strMovieName );

private:
	Background         *mpBackground;
	std::vector<Area*>  mAreas;
	std::vector<Area*>  mAreasActMouse;
	std::vector<Area*>  mAreasActTouch;
};

} // namespace TouchMovie
