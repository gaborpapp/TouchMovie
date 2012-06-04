#include <iostream>
#include <cstdlib>

#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

#include "CinderOpenCV.h"
#include "opencv2/imgproc/imgproc.hpp"

#include "KinectUser.h"

using namespace std;
using namespace ci;
using namespace mndl;

namespace TouchMovie
{

void KinectUser::setup( const fs::path &path )
{
	if ( path.empty() )
		mNI = ni::OpenNI( ni::OpenNI::Device() );
	else
		mNI = ni::OpenNI( path );

	mNIUserTracker = mNI.getUserTracker();
	mNIUserTracker.setSmoothing( 0.7f );

	mNI.setMirrored();
	mNI.start();

	mParams = params::PInterfaceGl( "Kinect", Vec2i( 300, 500 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addText( "User outlines" );
	mParams.addPersistentParam( " Blur", &mOutlineBlurAmt, 15.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( " Erode", &mOutlineErodeAmt, 3.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( " Dilate", &mOutlineDilateAmt, 7.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( " Threshold", &mOutlineThres, 128, "min=1 max=255" );
	mParams.addPersistentParam( " Width", &mOutlineWidth, 3, "min=.5 max=15 step=.2" );
	mParams.addPersistentParam( " Color", &mOutlineColor, ColorA::hexA( 0x50ffffff ) );
	mParams.addPersistentParam( " Hand size", &mHandSize, 5., "min=0 max=50 step=.5" );

	setBounds( app::getWindowBounds() );
}

void KinectUser::update()
{
	if ( mNI.checkNewVideoFrame() )
	{
		// generate user outline shapes
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

			mShape.moveTo( mOutputMapping.map( fromOcv( *pit ) ) );

			++pit;
			for ( ; pit != it->end(); ++pit )
			{
				mShape.lineTo( mOutputMapping.map( fromOcv( *pit ) ) );
			}
			mShape.close();
		}
	}

	// retrieve hand positions
	mHandPositions.clear();

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
				mHandPositions.push_back( mOutputMapping.map( jointPos ) );
			}
		}
	}
}

void KinectUser::draw()
{
	gl::enableAlphaBlending();
	gl::color( mOutlineColor );

	float sc = mOutputRect.getWidth() / 640.;
	float scaledHandSize = mHandSize * sc;
	for ( vector< Vec2f >::const_iterator it = mHandPositions.begin();
			it != mHandPositions.end(); ++it )
	{
		gl::drawSolidCircle( *it, scaledHandSize );
	}

	glLineWidth( mOutlineWidth * sc );
	gl::draw( mShape );
	glLineWidth( 1.0 );

	gl::disableAlphaBlending();
}

void KinectUser::setBounds( const Rectf &rect )
{
	mOutputRect = rect;

	Rectf kRect( 0, 0, 640, 480 ); // kinect image rect
	Rectf dRect = kRect.getCenteredFit( mOutputRect, true );
	if ( mOutputRect.getAspectRatio() > dRect.getAspectRatio() )
		dRect.scaleCentered( mOutputRect.getWidth() / dRect.getWidth() );
	else
		dRect.scaleCentered( mOutputRect.getHeight() / dRect.getHeight() );

	mOutputMapping = RectMapping( kRect, dRect );
}

} // namespace TouchMovie

