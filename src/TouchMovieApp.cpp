#include "cinder/app/AppBasic.h"
#include "cinder/Xml.h"
#include "PParams.h"

#define USE_KINECT        1
#define USE_KINECT_RECORD 0

#include "AreaController.h"
#include "Area.h"
#include "Background.h"
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

#if USE_KINECT == 1
	KinectUser          mKinectUser;
#endif /* USE_KINECT */
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

#if USE_KINECT == 1
	try
	{
#if USE_KINECT_RECORD == 0
		// use kinect
		mKinectUser.setup();
#else
		// use openni recording
		fs::path recordingPath = getAppPath();
	#if defined( CINDER_MAC )
		recordingPath /= "..";
	#endif
		recordingPath /= "touchmovie-captured.oni";

		mKinectUser.setup( recordingPath );
#endif /* USE_KINECT_RECORD */
	}
	catch ( ... )
	{
		app::console() << "Could not open Kinect" << endl;
		quit();
	}
#endif /* USE_KINECT */
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
	if( doc.hasChild( "Background" ))
	{
		XmlTree xmlBackground = doc.getChild( "Background" );

		std::string strPath = xmlBackground.getAttributeValue<std::string>( "Path"        );
		int         width   = xmlBackground.getAttributeValue<int>        ( "Width" , 800 );
		int         height  = xmlBackground.getAttributeValue<int>        ( "Height", 600 );

		mAreaController.setBackground( strPath, width, height );
	}
	if( doc.hasChild( "TouchMovie" ))
	{
		XmlTree xmlTouchMovie = doc.getChild( "TouchMovie" );

		for( XmlTree::Iter child = xmlTouchMovie.begin(); child != xmlTouchMovie.end(); ++child )
		{
			std::string strName     = child->getAttributeValue<std::string>( "Name"         );
			std::string strPathBack = child->getAttributeValue<std::string>( "PathBack"     );
			std::string strPathFore = child->getAttributeValue<std::string>( "PathFore"     );
			float       fadeIn      = child->getAttributeValue<float>      ( "FadeIn" , 1.0 );
			float       fadeOut     = child->getAttributeValue<float>      ( "FadeOut", 1.0 );
			float       x1          = child->getAttributeValue<float>      ( "x1"     , 1.0 );
			float       y1          = child->getAttributeValue<float>      ( "y1"     , 1.0 );
			float       x2          = child->getAttributeValue<float>      ( "x2"     , 1.0 );
			float       y2          = child->getAttributeValue<float>      ( "y2"     , 1.0 );

			Rectf rect = Rectf( x1, y1, x2, y2 );

			mAreaController.addArea     ( strName, rect        );
			mAreaController.setMovieBack( strName, strPathBack );
			mAreaController.setMovieFore( strName, strPathFore );
			mAreaController.setDrawFrame( strName, drawFrame   );
			mAreaController.setFadeIn   ( strName, fadeIn      );
			mAreaController.setFadeOut  ( strName, fadeOut     );
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
	Background *pBackground = mAreaController.getBackground();

	if( pBackground )
	{
		Rectf fitRect = pBackground->getRectOrig().getCenteredFit( getWindowBounds(), true );
		pBackground->setRect( fitRect );
#if USE_KINECT == 1
		mKinectUser.setBounds( fitRect );
#endif /* USE_KINECT */
	}

	mAreaController.resize();
}

void TouchMovieApp::update()
{
	mAreaController.update();

#if USE_KINECT == 1
	mKinectUser.update();
#endif /* USE_KINECT */
}

void TouchMovieApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	mAreaController.draw();

	Background *pBackground = mAreaController.getBackground();
	Rectf viewportRect = pBackground->getRect();
	ci::Area viewport( (int)viewportRect.getX1(), (int)viewportRect.getY1(),
			(int)viewportRect.getX2(), (int)viewportRect.getY2() );

	gl::setMatricesWindow( getWindowSize() );
#if USE_KINECT == 1
	mKinectUser.draw();

	mAreaController.setTouchPosBeg();
	vector< Vec2f > joints = mKinectUser.getHandPositions();
	for ( vector< Vec2f >::const_iterator it = joints.begin();
			it != joints.end(); ++it)
	{
		mAreaController.setTouchPos( *it );
	}
#endif /* USE_KINECT */
	mAreaController.setTouchPosEnd();

	gl::setViewport( getWindowBounds());

	params::PInterfaceGl::draw();
}

CINDER_APP_BASIC( TouchMovieApp, RendererGl(0) );

