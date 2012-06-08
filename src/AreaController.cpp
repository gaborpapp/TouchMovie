#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "AreaController.h"

using namespace ci;
using namespace ci::app;

namespace TouchMovie
{

AreaController::AreaController()
: mpBackground( 0 )
, mpAreaMouse( 0 )
{
}

AreaController::~AreaController()
{
	if( mpBackground )
		delete mpBackground;
}

void AreaController::update()
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		(*p)->update();
	}
}

void AreaController::draw()
{
	if( mpBackground )
		mpBackground->draw();

	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		(*p)->draw();
	}
}

void AreaController::mouseMove( MouseEvent event )
{
	mpAreaMouse = _getArea( event.getPos());
}

void AreaController::mouseDown( MouseEvent event )
{
}

void AreaController::mouseUp( MouseEvent event )
{
}

void AreaController::setBackground( fs::path pathPicture, int width, int height )
{
	try
	{
		Rectf rect = Rectf( 0.0f, 0.0f, (float)width, (float)height );
		fs::path xmlPath( getAssetPath( pathPicture ));
		ci::ImageSourceRef imageSource = loadImage( xmlPath );

		if( mpBackground )
			delete mpBackground;

		mpBackground = new Background( imageSource, rect );
	}
	catch( ... )
	{
		console() << "Unable to load the picture: " << pathPicture << std::endl;
	}
}

Background *AreaController::getBackground()
{
	return mpBackground;
}

void AreaController::addArea( std::string name, Rectf &rect )
{
	mAreas.push_back( new Area( name, rect ));
}

void AreaController::removeArea( std::string name )
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		if( (*p)->getName() == name )
		{
			delete *p;
			mAreas.erase( p );
			break;
		}
	}
}

void AreaController::setMovieIdle( std::string name, fs::path pathMovie )
{
	Area *pArea = _getArea( name );

	if( pArea )
	{
		ci::qtime::MovieGl movie = _loadMovie( pathMovie );
		pArea->setMovieIdle( movie );
	}
}

void AreaController::setMovieActive( std::string name, fs::path pathMovie )
{
	Area *pArea = _getArea( name );

	if( pArea )
	{
		ci::qtime::MovieGl movie = _loadMovie( pathMovie );
		pArea->setMovieActive( movie );
	}
}

qtime::MovieGl AreaController::_loadMovie( fs::path pathMovie )
{
	try
	{
		// load up the movie, set it to loop, and begin playing
		if( pathMovie.string().find( "http://") != std::string::npos )
		{
			qtime::MovieLoader movieLoader = qtime::MovieLoader( Url( pathMovie.string()));
			movieLoader.waitForPlaythroughOk();
			return qtime::MovieGl( movieLoader );
		}
		else
		{
			fs::path xmlPath( getAssetPath( pathMovie ));

			return qtime::MovieGl( xmlPath );
		}
	}
	catch( ... )
	{
		console() << "Unable to load the movie: " << pathMovie << std::endl;
	}

	return qtime::MovieGl();
}

void AreaController::setRect( std::string name, Rectf &rect )
{
	Area *pArea = _getArea( name );

	if( pArea )
		pArea->setRect( rect );
}

void AreaController::setDrawFrame( std::string name, bool drawFrame )
{
	Area *pArea = _getArea( name );

	if( pArea )
		pArea->setDrawFrame( drawFrame );
}

bool AreaController::getDrawFrame( std::string name )
{
	Area *pArea = _getArea( name );

	if( pArea )
		return pArea->getDrawFrame();

	return false;
}

void AreaController::setFadeIn( std::string name, float fadeIn )
{
	Area *pArea = _getArea( name );

	if( pArea )
		pArea->setFadeIn( fadeIn );
}

float AreaController::getFadeIn( std::string name )
{
	Area *pArea = _getArea( name );

	if( pArea )
		return pArea->getFadeIn();

	return 0;
}

void AreaController::setFadeOut( std::string name, float fadeOut )
{
	Area *pArea = _getArea( name );

	if( pArea )
		pArea->setFadeOut( fadeOut );
}

float AreaController::getFadeOut( std::string name )
{
	Area *pArea = _getArea( name );

	if( pArea )
		return pArea->getFadeOut();

	return 0;
}

void AreaController::resize()
{
	if( ! mpBackground )
		return;

	Rectf rectBackgroundOrig = mpBackground->getRectOrig();
	Rectf rectBackgroundNew  = mpBackground->getRect();

	RectMapping rectMapping( rectBackgroundOrig, rectBackgroundNew, false );

	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		Rectf rectOrig = (*p)->getRectOrig();
		Rectf rectNew  = Rectf( rectMapping.map( rectOrig.getUpperLeft()), rectMapping.map( rectOrig.getLowerRight()));

		(*p)->setRect( rectNew );
	}
}

void AreaController::setTouchPosBeg()
{
}

void AreaController::setTouchPos( ci::Vec2f pos )
{
	Area *pArea = _getArea( pos );

	if( pArea )
		mAreasAct.push_back( pArea );
}

void AreaController::setTouchPosEnd()
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		if( _isAreaAct((*p )))
		{
			(*p)->setActive( true );
		}
		else
		{
			(*p)->setActive( false );
		}
	}

	mAreasAct.clear();
}

Area *AreaController::_getArea( std::string name )
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		if((*p)->getName() == name )
		{
			return *p;
		}
	}

	return 0;
}

Area *AreaController::_getArea( const Vec2i &pos )
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		if((*p)->getRect().contains( pos ))
		{
			return *p;
		}
	}

	return 0;
}

bool AreaController::_isAreaAct( Area *pArea )
{
	if( pArea == mpAreaMouse )
		return true;

	for( std::vector<Area*>::iterator p = mAreasAct.begin(); p != mAreasAct.end(); ++p )
	{
		if((*p) == pArea )
		{
			return true;
		}
	}

	return false;
}

} // namespace TouchMovie
