#pragma once

#include "cinder/Cinder.h"
#include "cinder/Shape2d.h"
#include "cinder/Rect.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"

#include "PParams.h"

#define USE_KINECT        1
#define USE_KINECT_RECORD 0

#if USE_KINECT == 1

#include "CiNI.h"

#define OUTLINE_SHADER 1

namespace TouchMovie
{

class KinectUser
{
	public:
		void setup( const ci::fs::path &path = "" );

		void update();
		void draw();

		std::vector< ci::Vec2f > &getHandPositions() { return mHandPositions; }

		void setBounds( const ci::Rectf &rect );

		void showParams( bool show );

	protected:
		mndl::ni::OpenNI mNI;
		mndl::ni::UserTracker mNIUserTracker;

		// user outline
		bool mOutlineEnable;
		float mOutlineBlurAmt;
		float mOutlineErodeAmt;
		float mOutlineDilateAmt;
		int mOutlineThres;
		float mOutlineWidth;
		float mMiterLimit;
		ci::ColorA mOutlineColor;
		float mHandSize;
		ci::ColorA mHandColor;
		float mHandSmoothing;

		ci::Shape2d mShape;

		std::vector< ci::Vec2f > mHandPositions;

		ci::Rectf mOutputRect;
		ci::RectMapping mOutputMapping;

		ci::params::PInterfaceGl mParams;

#ifdef OUTLINE_SHADER
		ci::gl::GlslProg mShader;
		std::vector< ci::gl::VboMesh > mVboMeshes;
#endif
};

} // namespace TouchMovie

#endif /* USE_KINECT */

