#include "cinder/app/AppBasic.h"
#include "cinder/Xml.h"
#include "cinder/gl/gl.h"
#include "PParams.h"

#include "AreaController.h"
#include "Area.h"
#include "KinectUser.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace TouchMovie;

class TouchMovieApp : public AppBasic {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void shutdown();
	void keyDown( KeyEvent event );
	void mouseMove( MouseEvent event );
	void resize( ResizeEvent event );

	void update();
	void draw();

private:
	void LoadXml( std::string xmlName );

	AreaController      mAreaController;
	int                 mWidth;
	int                 mHeight;

	KinectUser			mKinectUser;
};


void TouchMovieApp::prepareSettings( Settings *settings )
{
	settings->setFrameRate( FRAME_RATE );
}

void TouchMovieApp::setup()
{
	LoadXml( "data.xml" );
	setWindowSize( mWidth, mHeight );

	// params
	fs::path paramsXml( getAssetPath( "params.xml" ));
	if ( paramsXml.empty() )
	{
#if defined( CINDER_MAC )
		fs::path assetPath( getResourcePath() / "assets" );
#else
		fs::path assetPath( getAppPath() / "assets" );
#endif
		createDirectories( assetPath );
		paramsXml = assetPath / "params.xml" ;
	}
	params::PInterfaceGl::load( paramsXml );

	mKinectUser.setup();
}

void TouchMovieApp::shutdown()
{
	params::PInterfaceGl::save();
}

void TouchMovieApp::LoadXml( std::string xmlName )
{
	bool drawFrame = false;

	fs::path xmlPath( getAssetPath( xmlName ) );
	if( ! fs::exists( xmlPath ))
	{
		xmlPath = getOpenFilePath( getAppPath() );

		if( ! fs::exists( xmlPath ))
			return;
	}

	XmlTree doc( loadFile( xmlPath ));

	if( doc.hasChild( "Settings" ))
	{
		XmlTree xmlSettings = doc.getChild( "Settings" );

		mWidth    = xmlSettings.getAttributeValue<int> ( "Width"    , 800 );
		mHeight   = xmlSettings.getAttributeValue<int> ( "Height"   , 600 );
		drawFrame = xmlSettings.getAttributeValue<bool>( "DrawFrame", 0   );
	}
	if( doc.hasChild( "TouchMovie" ))
	{
		XmlTree xmlTouchMovie = doc.getChild( "TouchMovie" );

		for( XmlTree::Iter child = xmlTouchMovie.begin(); child != xmlTouchMovie.end(); ++child )
		{
			std::string strName   = child->getAttributeValue<std::string>( "Name"         );
			std::string strPath   = child->getAttributeValue<std::string>( "Path"         );
			bool        main      = child->getAttributeValue<bool>       ( "Main"   , 0   );
			float       fadeIn    = child->getAttributeValue<float>      ( "FadeIn" , 1.0 );
			float       fadeOut   = child->getAttributeValue<float>      ( "FadeOut", 1.0 );
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

			Rectf rect = Rectf((float)x1, (float)y1, (float)x2, (float)y2 );

			mAreaController.addArea     ( strName, rect      );
			mAreaController.setMovie    ( strName, strPath   );
			mAreaController.setMain     ( strName, main      );
			mAreaController.setDrawFrame( strName, drawFrame );
			mAreaController.setFadeIn   ( strName, fadeIn    );
			mAreaController.setFadeOut  ( strName, fadeOut   );
		}
	}
}

void TouchMovieApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_ESCAPE )
	{
		quit();
	}
	else if( event.getCode() == 'f' )
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

	mAreaController.resize();
}

void TouchMovieApp::update()
{
	mAreaController.update();

	mKinectUser.update();
}

void TouchMovieApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	mAreaController.draw();

	gl::setMatricesWindow( getWindowWidth(), getWindowHeight());
	mKinectUser.draw();

	mAreaController.setTouchPosBeg();
	vector< Vec2f > joints = mKinectUser.getHandPositions();
	for ( vector< Vec2f >::const_iterator it = joints.begin();
			it != joints.end(); ++it)
	{
		mAreaController.setTouchPos( *it );
	}
	mAreaController.setTouchPosEnd();

	params::PInterfaceGl::draw();
}

CINDER_APP_BASIC( TouchMovieApp, RendererGl(0) );

