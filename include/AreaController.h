#pragma once
#include "Area.h"
#include "cinder/app/MouseEvent.h"

using namespace ci;
using namespace ci::app;

namespace TouchMovie
{

class AreaController
{
public:
	AreaController();
	void update();
	void draw();
	void mouseMove( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseUp  ( MouseEvent event );

	void addArea( std::string name, Rectf &rect );
	void removeArea( std::string name );
	void setMain( std::string name );
	bool setMovie( std::string name, fs::path pathMovie );
	void setRect( std::string name, Rectf &rect );
	void setAlpha( std::string name, float alpha );

	const int getWidth ( std::string name );
	const int getHeigth( std::string name );

	Area *getAreaMain();

private:
	Area *_getArea( std::string name );
	Area *_getArea( const Vec2i &pos );

private:

	Area              *mpAreaMain;
	Area              *mpAreaAct;
	std::vector<Area*> mAreas;
};

} // namespace TouchMovie
