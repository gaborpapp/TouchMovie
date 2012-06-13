#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Audio/Io.h"
#include "AreaController.h"

using namespace ci;
using namespace ci::app;

namespace TouchMovie
{

AreaController::AreaController()
: mpBackground( 0 )
, mDrawFrame( false )
{
}

AreaController::~AreaController()
{
//	if( mpBackground )
//		delete mpBackground;
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

void AreaController::setBackgroundImage( std::string strImageName )
{
	if( mpBackground && ! strImageName.empty())
	{
		ci::ImageSourceRef imageSource = _loadImage( strImageName );
		if( imageSource )
			mpBackground->setImage( strImageName, imageSource );
	}
}

void AreaController::setBackgroundMovie( std::string strMovieName )
{
	if( mpBackground && ! strMovieName.empty())
	{
		ci::qtime::MovieGl movie = _loadMovie( strMovieName );
		if( movie )
			mpBackground->setMovie( strMovieName, movie );
	}
}

Background *AreaController::getBackground()
{
	return mpBackground;
}

void AreaController::addArea( std::string areaName, Rectf &rect )
{
	mAreas.push_back( new Area( areaName, rect ));
}

void AreaController::removeArea( std::string areaName )
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		if( (*p)->getName() == areaName )
		{
			delete *p;
			mAreas.erase( p );
			break;
		}
	}
}

void AreaController::setMovieIdle( std::string areaName, std::string movieName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
	{
		ci::qtime::MovieGl movie = _loadMovie( movieName );
		if( movie )
			pArea->setMovieIdle( movie );
	}
}

void AreaController::setMovieActive( std::string areaName, std::string movieName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
	{
		ci::qtime::MovieGl movie = _loadMovie( movieName );
		if( movie )
			pArea->setMovieActive( movie );
	}
}

void AreaController::setAudioIdle( std::string areaName, std::string audioName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
	{
		ci::audio::SourceRef audio = _loadAudio( audioName );
		if( audio )
			pArea->setAudioIdle( audio );
	}
}

void AreaController::setAudioActive( std::string areaName, std::string audioName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
	{
		ci::audio::SourceRef audio = _loadAudio( audioName );
		if( audio )
			pArea->setAudioActive( audio );
	}
}

qtime::MovieGl AreaController::_loadMovie( std::string strMovieName )
{
	if( ! strMovieName.empty())
	{
		try
		{
			// load up the movie, set it to loop, and begin playing
			if( strMovieName.find( "http://") != std::string::npos )
			{
				qtime::MovieLoader movieLoader = qtime::MovieLoader( Url( strMovieName ));
				movieLoader.waitForPlaythroughOk();
				return qtime::MovieGl( movieLoader );
			}
			else
			{
				fs::path xmlPath( getAssetPath( strMovieName ));

				return qtime::MovieGl( xmlPath );
			}
		}
		catch( ... )
		{
			console() << "Unable to load the movie: " << strMovieName << std::endl;
		}
	}

	return qtime::MovieGl();
}

ImageSourceRef AreaController::_loadImage( std::string strImageName )
{
	if( ! strImageName.empty())
	{
		try
		{
			fs::path xmlPath( getAssetPath( strImageName ));
			return loadImage( xmlPath );
		}
		catch( ... )
		{
			console() << "Unable to load the image: " << strImageName << std::endl;
		}
	}
	return ImageSourceRef();
}

audio::SourceRef AreaController::_loadAudio( std::string strAudioName )
{
	if( ! strAudioName.empty())
	{
		try
		{
			fs::path xmlPath( getAssetPath( strAudioName ));
			return audio::load( xmlPath.string());
		}
		catch( ... )
		{
			console() << "Unable to load the audio: " << strAudioName << std::endl;
		}
	}
	return audio::SourceRef();
}

void AreaController::setRect( std::string areaName, Rectf &rect )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		pArea->setRect( rect );
}

void AreaController::setMarginH( std::string areaName, float marginH )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		pArea->setMarginH( marginH );
}

float AreaController::getMarginH( std::string areaName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		return pArea->getMarginH();

	return 0;
}

void AreaController::setMarginV( std::string areaName, float marginV )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		pArea->setMarginV( marginV );
}

float AreaController::getMarginV( std::string areaName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		return pArea->getMarginV();

	return 0;
}

void AreaController::setDrawFrame( bool drawFrame )
{
	mDrawFrame = drawFrame;

	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		(*p)->setDrawFrame( drawFrame );
	}
}

bool AreaController::getDrawFrame()
{
	return mDrawFrame;
}

void AreaController::setFadeIn( std::string areaName, float fadeIn )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		pArea->setFadeIn( fadeIn );
}

float AreaController::getFadeIn( std::string areaName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		return pArea->getFadeIn();

	return 0;
}

void AreaController::setFadeOut( std::string areaName, float fadeOut )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		pArea->setFadeOut( fadeOut );
}

float AreaController::getFadeOut( std::string areaName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		return pArea->getFadeOut();

	return 0;
}

void AreaController::setUseAlphaShader( std::string areaName, bool useAlphaShader )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		pArea->setUseAlphaShader( useAlphaShader );
}

bool AreaController::getUseAlphaShader( std::string areaName )
{
	Area *pArea = _getArea( areaName );

	if( pArea )
		return pArea->getUseAlphaShader();

	return false;
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
		Rectf rectOrig         = (*p)->getRectOrig();
		Rectf rectNew          = Rectf( rectMapping.map( rectOrig.getUpperLeft()), rectMapping.map( rectOrig.getLowerRight()));
		Rectf rectSensitive    = rectOrig.inflated( Vec2f( -(*p)->getMarginH(), -(*p)->getMarginV()));
		Rectf rectSensitiveNew = Rectf( rectMapping.map( rectSensitive.getUpperLeft()), rectMapping.map( rectSensitive.getLowerRight()));

		(*p)->setRect( rectNew );
		(*p)->setRectSensitive( rectSensitiveNew );
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

Area *AreaController::_getArea( std::string areaName )
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		if((*p)->getName() == areaName )
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
		if((*p)->getRectSensitive().contains( pos ))
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
