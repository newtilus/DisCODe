#include "CvFindLabirynth_Processor.hpp"

#include "LReading.hpp"

namespace Processors {

namespace CvFindLabirynth {

using namespace std;
using namespace boost;
using namespace cv;
using namespace Types::Objects3D;
using Types::Mrrocpp_Proxy::LReading;

CvFindLabirynth_Processor::CvFindLabirynth_Processor(const std::string & name) :
	Component(name)/*,

	prop_subpix("subpix", true),
	prop_subpix_window("subpix_window", 9, "range"),
	prop_scale("scale", true),
	prop_scale_factor("scale_factor", 2, "range"),

	prop_width("chessboard.width", 9),
	prop_height("chessboard.height", 6),
	prop_square_width("chessboard.square_width", 1),
	prop_square_height("chessboard.square_height", 1),

	prop_fastCheck("flags.fast_check", true),
	prop_filterQuads("flags.filter_quads", true),
	prop_adaptiveThreshold("flags.adaptive_treshold", true),
	prop_normalizeImage("flags.normalize_image", true)*/
{

	findLabirynthFlags = 0;
	temp=90;
	found=true;
/*
	registerProperty(prop_subpix);

	prop_subpix_window.addConstraint("3");
	prop_subpix_window.addConstraint("11");
	registerProperty(prop_subpix_window);

	registerProperty(prop_scale);

	prop_scale_factor.addConstraint("1");
	prop_scale_factor.addConstraint("8");
	registerProperty(prop_scale_factor);

	registerProperty(prop_width);
	registerProperty(prop_height);
	registerProperty(prop_square_width);
	registerProperty(prop_square_height);

	prop_fastCheck.setCallback(boost::bind(&CvFindLabirynth_Processor::flagsCallback, this, _1, _2));
	registerProperty(prop_fastCheck);

	prop_filterQuads.setCallback(boost::bind(&CvFindLabirynth_Processor::flagsCallback, this, _1, _2));
	registerProperty(prop_filterQuads);

	prop_adaptiveThreshold.setCallback(boost::bind(&CvFindLabirynth_Processor::flagsCallback, this, _1, _2));
	registerProperty(prop_adaptiveThreshold);

	prop_normalizeImage.setCallback(boost::bind(&CvFindLabirynth_Processor::flagsCallback, this, _1, _2));
	registerProperty(prop_normalizeImage);

	prop_width.setCallback(boost::bind(&CvFindLabirynth_Processor::sizeCallback, this, _1, _2));
	prop_height.setCallback(boost::bind(&CvFindLabirynth_Processor::sizeCallback, this, _1, _2));
*/
}

CvFindLabirynth_Processor::~CvFindLabirynth_Processor()
{
}

bool CvFindLabirynth_Processor::onFinish()
{
	return true;
}

bool CvFindLabirynth_Processor::onStop()
{
	return true;
}

bool CvFindLabirynth_Processor::onInit()
{
	h_onNewImage.setup(this, &CvFindLabirynth_Processor::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);
	h_onRpcCall.setup(this, &CvFindLabirynth_Processor::onRpcCall);
	registerHandler("onRpcCall", &h_onRpcCall);

	registerStream("in_img", &in_img);
	registerStream("rpcParam", &rpcParam);

	registerStream("rpcResult", &out_info);

	rpcResult = registerEvent("rpcResult");

	LOG(LTRACE) << "component initialized\n";
	return true;
}

bool CvFindLabirynth_Processor::onStart()
{
	return true;
}

bool CvFindLabirynth_Processor::onStep()
{
	return true;
}

void CvFindLabirynth_Processor::onRpcCall()
{
	LOG(LTRACE) << "void CvFindLabirynth_Processor::onRpcCall() begin\n";

	LReading lr;
	lr.info = 2.5;

	out_info.write(lr);
/*
	cv::Point2f point((image.size().width / 2),(image.size().height / 2));
	corners.push_back(point);
	corners.push_back(point);


	Types::ImagePosition imagePosition;
	double maxPixels = std::max(image.size().width, image.size().height);
	imagePosition.elements[0] = (corners[0].x - image.size().width / 2) / maxPixels;
	imagePosition.elements[1] = (corners[0].y - image.size().height / 2) / maxPixels;
	imagePosition.elements[2] = 0;
	imagePosition.elements[3] = - atan2(corners[1].y - corners[0].y, corners[1].x - corners[0].x);
	out_imagePosition.write(imagePosition);

	lr.info = "info3";
	out_info.write(lr);

	labirynthFound->raise();

	corners.pop_back();
	corners.pop_back();
*/

	rpcResult->raise();
}


void CvFindLabirynth_Processor::onNewImage()
{
	LOG(LTRACE) << "void CvFindLabirynth_Processor::onNewImage() begin\n";
	try
	{
		if(in_img.empty())
		{
			return;
		}

		image = in_img.read();


	} catch (const Exception& e) {
		LOG(LERROR) << e.what() << "\n";
	}
	LOG(LTRACE) << "void CvFindLabirynth_Processor::onNewImage() end\n";
}

} // namespace CvFindLabirynth {
} // namespace Processors {
