/*!
 * \file
 * \brief
 */

#include <memory>
#include <string>

#include "CvHoughCircles.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace CvHoughCircles {

using namespace cv;
using namespace std;
using Types::Circles;

CvHoughCircles_Processor::CvHoughCircles_Processor(const std::string & name) :
	Base::Component(name), inverseRatioOfAccumulatorResolution("inverseRatioOfAccumulatorResolution"), minDist("minDist"),
			cannyHigherThreshold("cannyHigherThreshold"), accumulatorThreshold("accumulatorThreshold"),
			minCircleRadius("minCircleRadius"), maxCircleRadius("maxCircleRadius")
{
	LOG(LTRACE) << "Hello CvHoughCircles_Processor\n";
}

CvHoughCircles_Processor::~CvHoughCircles_Processor()
{
	LOG(LTRACE) << "Good bye CvHoughCircles_Processor\n";
}

bool CvHoughCircles_Processor::onInit()
{
	LOG(LTRACE) << "CvHoughCircles_Processor::initialize\n";

	// Register data streams, events and event handlers HERE!
	newCircles = registerEvent("newCircles");

	h_onNewImage.setup(this, &CvHoughCircles_Processor::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	registerStream("in_img", &in_img);
	registerStream("out_circles", &out_circles);


	return true;
}

bool CvHoughCircles_Processor::onFinish()
{
	LOG(LTRACE) << "CvHoughCircles_Processor::finish\n";

	return true;
}

bool CvHoughCircles_Processor::onStep()
{
	LOG(LTRACE) << "CvHoughCircles_Processor::step\n";
	return true;
}

bool CvHoughCircles_Processor::onStop()
{
	return true;
}

bool CvHoughCircles_Processor::onStart()
{
	return true;
}

void CvHoughCircles_Processor::onNewImage()
{
	if(in_img.empty()){
		LOG(LFATAL) << "Component " << name() << " " << "in_img input stream is empty.";
		return;
	}
	Mat image = in_img.read();

	if(image.channels() != 1){
		LOG(LFATAL) << "Component " << name() << " " << "in_img received image must have only one channel.";
		return;
	}

	Circles c;

	HoughCircles(image, c.circles, CV_HOUGH_GRADIENT, inverseRatioOfAccumulatorResolution, minDist, cannyHigherThreshold, accumulatorThreshold, minCircleRadius, maxCircleRadius);

	out_circles.write(c);
	newCircles->raise();
}

}//: namespace CvHoughCircles
}//: namespace Processors
