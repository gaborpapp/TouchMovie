#include "cinder/app/AppBasic.h"
#include "cinder/Xml.h"
#include "AreaController.h"
#include "Area.h"

using namespace ci;
using namespace std;
using namespace TouchMovie;

class TouchMovieApp : public AppBasic {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );
	void mouseMove( MouseEvent event );
	void resize( ResizeEvent event );

	void update();
	void draw();

	AreaController mAreaController;
};


void TouchMovieApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen();
}

void TouchMovieApp::setup()
{
	fs::path xmlPath( "./data.xml" );
	if( ! fs::exists( xmlPath ))
	{
		xmlPath = getOpenFilePath( "./" );

		if( ! fs::exists( xmlPath ))
			return;
	}

	XmlTree doc( loadFile( xmlPath ));
	if( doc.hasChild( "TouchMovie" ))
	{
		XmlTree xmlTouchMovie = doc.getChild( "TouchMovie" );
	
		for( XmlTree::Iter child = xmlTouchMovie.begin(); child != xmlTouchMovie.end(); ++child )
		{
			std::string strName = child->getAttributeValue<std::string>( "Name" );
			std::string strPath = child->getAttributeValue<std::string>( "Path" );
			int         main    = child->getAttributeValue<int>( "Main", 0 );
			int         x1 = 0;
			int         y1 = 0;
			int         x2 = 0;
			int         y2 = 0;
	
			if( child->hasChild( "Rect" ))
			{
				XmlTree xmlRect = child->getChild( "Rect" );
	
				x1 = xmlRect.getAttributeValue<int>( "x1", 0 );
				y1 = xmlRect.getAttributeValue<int>( "y1", 0 );
				x2 = xmlRect.getAttributeValue<int>( "x2", 0 );
				y2 = xmlRect.getAttributeValue<int>( "y2", 0 );
			}
	
			Rectf rect = Rectf( (float)x1, (float)y1, (float)x2, (float)y2 );
	
			mAreaController.addArea( strName, rect );
			if( ! mAreaController.setMovie( strName, strPath ))
				continue;
	
			if( main )
				mAreaController.setMain( strName );
		}
	}
}

void TouchMovieApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_ESCAPE )
	{
		setFullScreen( ! isFullScreen());
	}
}

void TouchMovieApp::mouseMove( MouseEvent event )
{
	mAreaController.mouseMove( event );
}

void TouchMovieApp::resize( ResizeEvent event )
{
	TouchMovie::Area *pAreaMain = mAreaController.getAreaMain();

	if( pAreaMain )
		pAreaMain->setRect( Rectf( 0, 0, (float)pAreaMain->getWidth(), (float)pAreaMain->getHeight()).getCenteredFit( getWindowBounds(), true ));
}

void TouchMovieApp::update()
{
	mAreaController.update();
}

void TouchMovieApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();

	mAreaController.draw();

	gl::disableAlphaBlending();
}

CINDER_APP_BASIC( TouchMovieApp, RendererGl(0) );
