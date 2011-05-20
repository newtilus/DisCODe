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

CvHoughCircles_Processor::CvHoughCircles_Processor(const std::string & name) :
	Base::Component(name), inverseRatio("inverseRatio"), minDist("minDist"),
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

}//: namespace CvHoughCircles
}//: namespace Processors
