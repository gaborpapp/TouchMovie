#include "cinder/app/AppBasic.h"
#include "cinder/Xml.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "AreaController.h"
#include "Area.h"
#include "PParams.h"

#include "CinderOpenCV.h"
#include "opencv2/imgproc/imgproc.hpp"

#include "CiNI.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mndl;
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

	ni::OpenNI          mNI;
	ni::UserTracker     mNIUserTracker;

	// user outline
	float               mOutlineBlurAmt;
	float               mOutlineErodeAmt;
	float               mOutlineDilateAmt;
	int                 mOutlineThres;
	float				mOutlineWidth;
	ColorA				mOutlineColor;

	Shape2d             mShape;

	params::PInterfaceGl mParams;
};


void TouchMovieApp::prepareSettings( Settings *settings )
{
	settings->setFrameRate( FRAME_RATE );
}

void TouchMovieApp::setup()
{
	LoadXml( "data.xml" );
	setWindowSize( mWidth, mHeight );

	try
	{
		mNI = ni::OpenNI( ni::OpenNI::Device() );
	}
	catch ( ... )
	{
		console() << "Could not open Kinect" << endl;
		exit( -1 );
	}

	mNIUserTracker = mNI.getUserTracker();
	mNIUserTracker.setSmoothing( 0.7f );

	mNI.setMirrored();
	mNI.start();

	// params
	fs::path paramsXml( getAssetPath( "params.xml" ));
	params::PInterfaceGl::load( paramsXml );

	mParams = params::PInterfaceGl( "Parameters", Vec2i( 300, 600 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addText( " User outlines " );
	mParams.addPersistentParam( "blur", &mOutlineBlurAmt, 15.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( "erode", &mOutlineErodeAmt, 3.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( "dilate", &mOutlineDilateAmt, 7.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( "threshold", &mOutlineThres, 128, "min=1 max=255" );
	mParams.addPersistentParam( "width", &mOutlineWidth, 3, "min=.5 max=15 step=.2" );
	mParams.addPersistentParam( "color", &mOutlineColor, ColorA::hexA( 0x50ffffff ) );
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

	if ( mNI.checkNewVideoFrame() )
	{
		Surface8u maskSurface = mNIUserTracker.getUserMask();

		cv::Mat cvMask, cvMaskFiltered;
		cvMask = toOcv( Channel8u( maskSurface ) );
		cv::blur( cvMask, cvMaskFiltered, cv::Size( mOutlineBlurAmt, mOutlineBlurAmt ) );

		cv::Mat dilateElm = cv::getStructuringElement( cv::MORPH_RECT,
				cv::Size( mOutlineDilateAmt, mOutlineDilateAmt ) );
		cv::Mat erodeElm = cv::getStructuringElement( cv::MORPH_RECT,
				cv::Size( mOutlineErodeAmt, mOutlineErodeAmt ) );
		cv::erode( cvMaskFiltered, cvMaskFiltered, erodeElm, cv::Point( -1, -1 ), 1 );
		cv::dilate( cvMaskFiltered, cvMaskFiltered, dilateElm, cv::Point( -1, -1 ), 3 );
		cv::erode( cvMaskFiltered, cvMaskFiltered, erodeElm, cv::Point( -1, -1 ), 1 );
		cv::blur( cvMaskFiltered, cvMaskFiltered, cv::Size( mOutlineBlurAmt, mOutlineBlurAmt ) );

		cv::threshold( cvMaskFiltered, cvMaskFiltered, mOutlineThres, 255, CV_THRESH_BINARY);

		vector< vector< cv::Point > > contours;
		cv::findContours( cvMaskFiltered, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );

		mShape.clear();
		for ( vector< vector< cv::Point > >::const_iterator it = contours.begin();
				it != contours.end(); ++it )
		{
			vector< cv::Point >::const_iterator pit = it->begin();

			if ( it->empty() )
				continue;

			mShape.moveTo( fromOcv( *pit ) );

			++pit;
			for ( ; pit != it->end(); ++pit )
			{
				mShape.lineTo( fromOcv( *pit ) );
			}
			mShape.close();
		}
	}
}

void TouchMovieApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	mAreaController.draw();

	gl::setMatricesWindow( getWindowWidth(), getWindowHeight());

	mAreaController.setTouchPosBeg();

	gl::enableAlphaBlending();
	gl::color( mOutlineColor );

	const XnSkeletonJoint jointIds[] = { XN_SKEL_LEFT_HAND, XN_SKEL_RIGHT_HAND };
	vector< unsigned > users = mNIUserTracker.getUsers();
	for ( vector< unsigned >::const_iterator it = users.begin(); it < users.end(); ++it )
	{
		unsigned userId = *it;
		for ( int i = 0; i < sizeof( jointIds ) / sizeof( jointIds[0] ); ++i )
		{
			Vec2f jointPos = mNIUserTracker.getJoint2d( userId, jointIds[i] );
			float conf = mNIUserTracker.getJointConfidance( userId, jointIds[i] );

			if ( conf > .9 )
			{
				mAreaController.setTouchPos( jointPos );
				gl::drawSolidCircle( jointPos, 5 );
			}
		}
	}
	mAreaController.setTouchPosEnd();

	glLineWidth( mOutlineWidth );
	gl::draw( mShape );
	glLineWidth( 1.0 );

	gl::disableAlphaBlending();

	params::PInterfaceGl::draw();
}

CINDER_APP_BASIC( TouchMovieApp, RendererGl(0) );
