#pragma once

#include "cinder/Cinder.h"
#include "cinder/Shape2d.h"
#include "cinder/Rect.h"

#include "PParams.h"

#include "CiNI.h"

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

	protected:
		mndl::ni::OpenNI          mNI;
		mndl::ni::UserTracker     mNIUserTracker;

		// user outline
		float               mOutlineBlurAmt;
		float               mOutlineErodeAmt;
		float               mOutlineDilateAmt;
		int                 mOutlineThres;
		float				mOutlineWidth;
		ci::ColorA			mOutlineColor;
		float				mHandSize;

		ci::Shape2d			mShape;

		std::vector< ci::Vec2f > mHandPositions;

		ci::Rectf			mOutputRect;
		ci::RectMapping		mOutputMapping;

		ci::params::PInterfaceGl mParams;
};

} // namespace TouchMovie

