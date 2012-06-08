#include <iostream>
#include <cstdlib>

#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

#include "CinderOpenCV.h"
#include "opencv2/imgproc/imgproc.hpp"

#include "Resources.h"

#include "KinectUser.h"

#if USE_KINECT == 1

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

	mNI.setMirrored();
	mNI.start();

	mParams = params::PInterfaceGl( "Kinect", Vec2i( 300, 500 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addText( "User outlines" );
	mParams.addPersistentParam( " Enable", &mOutlineEnable, true );
	mParams.addPersistentParam( " Blur", &mOutlineBlurAmt, 15.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( " Erode", &mOutlineErodeAmt, 3.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( " Dilate", &mOutlineDilateAmt, 7.0, "min=1 max=15 step=.5" );
	mParams.addPersistentParam( " Threshold", &mOutlineThres, 128, "min=1 max=255" );
	mParams.addPersistentParam( " Thickness", &mOutlineWidth, 3, "min=.5 max=15 step=.2" );
	mParams.addPersistentParam( " Miter limit", &mMiterLimit, .75, "min=-1 max=1 step=.05" );
	mParams.addPersistentParam( " Color", &mOutlineColor, ColorA::hexA( 0x50ffffff ) );
	mParams.addText( "Hand" );
	mParams.addPersistentParam( " Hand size", &mHandSize, 5., "min=0 max=50 step=.5" );
	mParams.addPersistentParam( " Hand Color", &mHandColor, ColorA::hexA( 0x50ffffff ) );
	mParams.addPersistentParam( " Hand smoothing", &mHandSmoothing, .7, "min=0 max=1 step=.05" );

	setBounds( app::getWindowBounds() );

#ifdef OUTLINE_SHADER
	mShader = gl::GlslProg( app::loadResource( RES_LINE_VERT ),
							app::loadResource( RES_LINE_FRAG ),
							app::loadResource( RES_LINE_GEOM ),
							GL_LINES_ADJACENCY_EXT, GL_TRIANGLE_STRIP, 7 );
#endif
}

void KinectUser::update()
{
	mNIUserTracker.setSmoothing( mHandSmoothing );
	if ( mNI.checkNewVideoFrame() && mOutlineEnable )
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

#ifdef OUTLINE_SHADER
	// update vbo from shape points
	// based on the work of Paul Houx
	// https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader#23286000001297067
	if ( mOutlineEnable )
	{
		mVboMeshes.clear();

		for ( size_t i = 0; i < mShape.getNumContours(); ++i )
		{
			const Path2d &path = mShape.getContour( i );
			const vector< Vec2f > &points = path.getPoints();

			if ( points.size() > 1 )
			{
				// create a new vector that can contain 3D vertices
				vector< Vec3f > vertices;
				// to improve performance, make room for the vertices + 2
				// adjacency vertices
				vertices.reserve( points.size() + 2 );

				// first, add an adjacency vertex at the beginning
				vertices.push_back( 2.0f * Vec3f( points[ 0 ] ) -
						Vec3f( points[ 1 ] ) );

				// next, add all 2D points as 3D vertices
				vector< Vec2f >::const_iterator it;
				for ( it = points.begin() ; it != points.end(); ++it )
					vertices.push_back( Vec3f( *it ) );

				// next, add an adjacency vertex at the end
				size_t n = points.size();
				vertices.push_back( 2.0f * Vec3f( points[ n - 1 ] ) -
						Vec3f( points[ n - 2 ] ) );

				// now that we have a list of vertices, create the index buffer
				n = vertices.size() - 2;
				vector< uint32_t > indices;
				indices.reserve( n * 4 );

				for ( size_t i = 1; i < vertices.size() - 2; ++i )
				{
					indices.push_back( i - 1 );
					indices.push_back( i );
					indices.push_back( i + 1 );
					indices.push_back( i + 2 );
				}

				// finally, create the mesh
				gl::VboMesh::Layout layout;
				layout.setStaticPositions();
				layout.setStaticIndices();
				gl::VboMesh vboMesh = gl::VboMesh( vertices.size(), indices.size(), layout, GL_LINES_ADJACENCY_EXT );
				vboMesh.bufferPositions( &(vertices.front()), vertices.size() );
				vboMesh.bufferIndices( indices );
				vboMesh.unbindBuffers();

				mVboMeshes.push_back( vboMesh );
			}
		}
	}
#endif
}

void KinectUser::draw()
{
	gl::enable( GL_SCISSOR_TEST );
	// FIXME: video is one pixel smaller?
	glScissor( mOutputRect.getX1(), mOutputRect.getY1() + 1,
			mOutputRect.getWidth(), mOutputRect.getHeight() - 2 );

	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();

	gl::color( mHandColor );
	float sc = mOutputRect.getWidth() / 640.;
	float scaledHandSize = mHandSize * sc;
	for ( vector< Vec2f >::const_iterator it = mHandPositions.begin();
			it != mHandPositions.end(); ++it )
	{
		gl::drawSolidCircle( *it, scaledHandSize );
	}

	gl::color( mOutlineColor );
	if ( mOutlineEnable )
	{
#ifdef OUTLINE_SHADER
		mShader.bind();
		mShader.uniform( "WIN_SCALE", Vec2f( mOutputRect.getSize() ) );
		mShader.uniform( "MITER_LIMIT", mMiterLimit );
		mShader.uniform( "THICKNESS", mOutlineWidth * sc );

		for ( vector< gl::VboMesh >::const_iterator vit = mVboMeshes.begin();
				vit != mVboMeshes.end(); ++vit )
		{
			gl::draw( *vit );
		}

		mShader.unbind();
#else
		glLineWidth( mOutlineWidth * sc );
		gl::draw( mShape );
		glLineWidth( 1.0 );
#endif
	}

	gl::disableAlphaBlending();
	gl::disableDepthRead();
	gl::disableDepthWrite();

	gl::disable( GL_SCISSOR_TEST );
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

	mOutputMapping = RectMapping( kRect, dRect, true );
}

} // namespace TouchMovie

#endif /* USE_KINECT */
