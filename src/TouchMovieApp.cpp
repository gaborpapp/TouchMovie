#include "cinder/app/AppBasic.h"
#include "cinder/Xml.h"
#include "PParams.h"
#include "AntTweakBar.h"

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
	void loadXml( std::string xmlName );
	void showParams( bool show );

	AreaController      mAreaController;
	int                 mWidth;
	int                 mHeight;
	bool                mDrawFrame;

	params::PInterfaceGl mParams;
	float                mFPS;
	bool                 mShowParams;
	bool                 mFullScreen;

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
	loadXml( "data.xml" );
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

	mFullScreen = false;
	mShowParams = false;

	mParams = params::PInterfaceGl( "TouchMovie", Vec2i( 150, 80 ) );
	mParams.addPersistentSizeAndPosition();
	mParams.addText( "Touch Movie" );
	mParams.addParam( "FPS" , &mFPS, "", true );
	mParams.addPersistentParam( "FullScreen", &mFullScreen, false );

	TwDefine( "TW_HELP visible=false" );
	TwDefine( "TouchMovie iconified=true" );

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

	if( mFullScreen )
		hideCursor();
	showParams( mShowParams );
}

void TouchMovieApp::shutdown()
{
	params::PInterfaceGl::save();
}

void TouchMovieApp::loadXml( std::string xmlName )
{
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

		mWidth     = xmlSettings.getAttributeValue<int> ( "Width"    , 800 );
		mHeight    = xmlSettings.getAttributeValue<int> ( "Height"   , 600 );
		mDrawFrame = xmlSettings.getAttributeValue<bool>( "DrawFrame", 0   );
	}
	if( doc.hasChild( "Background" ))
	{
		XmlTree xmlBackground = doc.getChild( "Background" );

		std::string strPathImage   = xmlBackground.getAttributeValue<std::string>( "PathImage"  , ""  );
		std::string strPathMovie   = xmlBackground.getAttributeValue<std::string>( "PathMovie"  , ""  );
		int         width          = xmlBackground.getAttributeValue<int>        ( "Width"      , 800 );
		int         height         = xmlBackground.getAttributeValue<int>        ( "Height"     , 600 );
		bool        useAlphaShader = xmlBackground.getAttributeValue<bool>       ( "AlphaShader", 0   );

		mAreaController.setBackground     ( width, height );
		mAreaController.setBackgroundImage( strPathImage  );
		mAreaController.setBackgroundMovie( strPathMovie  );
		Background *pBackground = mAreaController.getBackground();
		pBackground->setUseAlphaShader( useAlphaShader );
	}
	if( doc.hasChild( "TouchMovie" ))
	{
		XmlTree xmlTouchMovie = doc.getChild( "TouchMovie" );

		for( XmlTree::Iter child = xmlTouchMovie.begin(); child != xmlTouchMovie.end(); ++child )
		{
			std::string strName            = child->getAttributeValue<std::string>( "Name"              );
			std::string strPathIdle        = child->getAttributeValue<std::string>( "PathIdle"          );
			std::string strPathActive      = child->getAttributeValue<std::string>( "PathActive"        );
			std::string strPathActiveAudio = child->getAttributeValue<std::string>( "PathActiveAudio"   );
			float       fadeIn             = child->getAttributeValue<float>      ( "FadeIn"      , 1.0 );
			float       fadeOut            = child->getAttributeValue<float>      ( "FadeOut"     , 1.0 );
			float       x                  = child->getAttributeValue<float>      ( "x"           , 1.0 );
			float       y                  = child->getAttributeValue<float>      ( "y"           , 1.0 );
			float       width              = child->getAttributeValue<float>      ( "width"       , 1.0 );
			float       height             = child->getAttributeValue<float>      ( "height"      , 1.0 );
			bool        useAlphaShader     = child->getAttributeValue<bool>       ( "AlphaShader" , 1.0 );

			Rectf rect = Rectf( x, y, x + width, y + height );

			mAreaController.addArea          ( strName, rect               );
			mAreaController.setMovieIdle     ( strName, strPathIdle        );
			mAreaController.setMovieActive   ( strName, strPathActive      );
//			mAreaController.setAudioActive   ( strName, strPathActiveAudio );
			mAreaController.setDrawFrame     ( strName, mDrawFrame         );
			mAreaController.setFadeIn        ( strName, fadeIn             );
			mAreaController.setFadeOut       ( strName, fadeOut            );
			mAreaController.setUseAlphaShader( strName, useAlphaShader     );
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
		mFullScreen = ! mFullScreen;
		setFullScreen( mFullScreen );
		if( ! mFullScreen )
			showCursor();
	}
	else if( event.getCode() == 's' )
	{
		mShowParams = ! mShowParams;
		showParams( mShowParams );
		if( mFullScreen )
		{
			if( mShowParams )
				showCursor( );
			else
				hideCursor();
		}
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
	mFPS = getAverageFps();
	mAreaController.update();

#if USE_KINECT == 1
	mKinectUser.update();
#endif /* USE_KINECT */

	if( mFullScreen != isFullScreen())
	{
		setFullScreen( mFullScreen );
	}
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

void TouchMovieApp::showParams( bool show )
{
	if( show )
		TwDefine( "TouchMovie visible=true" );
	else
		TwDefine( "TouchMovie visible=false" );

#if USE_KINECT == 1
	mKinectUser.showParams( show );
#endif /* USE_KINECT */
}

CINDER_APP_BASIC( TouchMovieApp, RendererGl(0) );

