#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "AreaController.h"

using namespace ci;
using namespace ci::app;

namespace TouchMovie
{

AreaController::AreaController()
: mpBackground( 0 )
{
}

AreaController::~AreaController()
{
	if( mpBackground )
		delete mpBackground;
}

void AreaController::update()
{
	if( mpBackground )
		mpBackground->update();

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
	mAreasActMouse.clear();
	_actionArea( event.getPos(), _setMousePosAction );
}

void AreaController::mouseDown( MouseEvent event )
{
}

void AreaController::mouseUp( MouseEvent event )
{
}

void AreaController::setBackground( int width, int height )
{
	if( mpBackground )
		delete mpBackground;

	Rectf rect = Rectf( 0.0f, 0.0f, (float)width, (float)height );
	mpBackground = new Background( rect );
}

void AreaController::setBackgroundImage( fs::path pathImage )
{
	if( mpBackground && ! pathImage.empty())
	{
		ci::ImageSourceRef imageSource = _loadImage( pathImage );
		if( imageSource )
			mpBackground->setImage( imageSource );
	}
}

void AreaController::setBackgroundMovie( fs::path pathMovie )
{
	if( mpBackground && ! pathMovie.empty())
	{
		ci::qtime::MovieGl movie = _loadMovie( pathMovie );
		if( movie )
			mpBackground->setMovie( movie );
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
		if( movie )
			pArea->setMovieIdle( movie );
	}
}

void AreaController::setMovieActive( std::string name, fs::path pathMovie )
{
	Area *pArea = _getArea( name );

	if( pArea )
	{
		ci::qtime::MovieGl movie = _loadMovie( pathMovie );
		if( movie )
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

ImageSourceRef AreaController::_loadImage( fs::path pathImage )
{
	try
	{
		fs::path xmlPath( getAssetPath( pathImage ));
		return loadImage( xmlPath );
	}
	catch( ... )
	{
		console() << "Unable to load the image: " << pathImage << std::endl;
	}

	return ImageSourceRef();
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
	mAreasActTouch.clear();
}

void AreaController::setTouchPos( ci::Vec2f pos )
{
	_actionArea( pos, _setTouchPosAction );
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
	for( std::vector<Area*>::iterator p = mAreasActMouse.begin(); p != mAreasActMouse.end(); ++p )
	{
		if((*p) == pArea )
		{
			return true;
		}
	}

	for( std::vector<Area*>::iterator p = mAreasActTouch.begin(); p != mAreasActTouch.end(); ++p )
	{
		if((*p) == pArea )
		{
			return true;
		}
	}

	return false;
}

void AreaController::_actionArea( const ci::Vec2i &pos, ActionFunc pActionFunc )
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		if((*p)->getRect().contains( pos ))
		{
			pActionFunc( *p, this );
		}
	}
}

void AreaController::_setTouchPosAction( Area *pArea, void *pvData )
{
	AreaController *pAreaController = (AreaController*)pvData;
	pAreaController->mAreasActTouch.push_back( pArea );
}

void AreaController::_setMousePosAction( Area *pArea, void *pvData )
{
	AreaController *pAreaController = (AreaController*)pvData;
	pAreaController->mAreasActMouse.push_back( pArea );
}

} // namespace TouchMovie
