#include "cinder/app/App.h"
#include "AreaController.h"

namespace TouchMovie
{

AreaController::AreaController()
: mpAreaMain( 0 )
, mpAreaAct( 0 )
{
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
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		(*p)->draw();
	}
}

void AreaController::mouseMove( MouseEvent event )
{
	Area *pArea = _getArea( event.getPos());

	if( mpAreaAct == pArea )
		return;

	if( mpAreaAct )
		mpAreaAct->show( false );

	mpAreaAct = pArea;

	if( mpAreaAct )
		mpAreaAct->show( true );
}

void AreaController::mouseDown( MouseEvent event )
{
}

void AreaController::mouseUp( MouseEvent event )
{
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

void AreaController::setMain( std::string name )
{
	mpAreaMain = _getArea( name );

	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		(*p)->setAlpha( 0.0f );
	}

	if( mpAreaMain )
		mpAreaMain->setAlpha( 1.0f );
}

bool AreaController::setMovie( std::string name, fs::path pathMovie )
{
	Area *pArea = _getArea( name );

	if( pArea )
	{
		try
		{
			// load up the movie, set it to loop, and begin playing
			if( pathMovie.string().find( "http://") != std::string::npos )
			{
				qtime::MovieLoader movieLoader = qtime::MovieLoader( Url( pathMovie.string()));
				movieLoader.waitForPlaythroughOk();
				qtime::MovieGl movie = qtime::MovieGl( movieLoader );
				pArea->setMovie( movie );
			}
			else
			{
				qtime::MovieGl movie = qtime::MovieGl( pathMovie );
				pArea->setMovie( movie );
			}
		}
		catch( ... )
		{
			console() << "Unable to load the movie: " << name << " - (" << pathMovie << ")" << std::endl;
			return false;
		}

		return true;
	}

	return false;
}

void AreaController::setRect( std::string name, Rectf &rect )
{
	Area *pArea = _getArea( name );

	if( pArea )
		pArea->setRect( rect );
}

void AreaController::setAlpha( std::string name, float alpha )
{
	Area *pArea = _getArea( name );

	if( pArea )
		pArea->setAlpha( alpha );
}

const int AreaController::getWidth( std::string name )
{
	Area *pArea = _getArea( name );

	if( pArea )
		return pArea->getWidth();

	return 0;
}

const int AreaController::getHeigth( std::string name )
{
	Area *pArea = _getArea( name );

	if( pArea )
		return pArea->getHeight();

	return 0;
}

Area *AreaController::getAreaMain()
{
	return mpAreaMain;
}

Area *AreaController::_getArea( std::string name )
{
	for( std::vector<Area*>::iterator p = mAreas.begin(); p != mAreas.end(); ++p )
	{
		if( (*p)->getName() == name )
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
		if( (*p) != mpAreaMain
		 && (*p)->getRect().contains( pos ))
		{
			return *p;
		}
	}

	return 0;
}

} // namespace TouchMovie
