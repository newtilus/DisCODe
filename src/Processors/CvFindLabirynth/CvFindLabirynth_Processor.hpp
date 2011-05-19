
#ifndef CVFINDLABIRYNTH_PROCESSOR_HPP_
#define CVFINDLABIRYNTH_PROCESSOR_HPP_

#include <string.h>
#include <cv.h>
#include <boost/shared_ptr.hpp>
#include "Component_Aux.hpp"
#include "Panel_Empty.hpp"
#include "Objects3D/Chessboard.hpp"
#include "ImagePosition.hpp"

#include "LReading.hpp"

#include "xdr/xdr_iarchive.hpp"

//#include "Proxies/Mrrocpp/Mrrocpp_Proxy.hpp"
//#include "Proxies/Mrrocpp/headers.h"

#include "Drawable.hpp"
#include "Timer.hpp"

#include "Property.hpp"


namespace Processors {
namespace CvFindLabirynth {

class CvFindLabirynth_Processor: public Base::Component
{
public:
	CvFindLabirynth_Processor(const std::string & name = "");
	virtual ~CvFindLabirynth_Processor();
protected:
	/*!
	 * Method called when component is started
	 * \return true on success
	 */
	virtual bool onStart();

	/*!
	 * Method called when component is stopped
	 * \return true on success
	 */
	virtual bool onStop();

	/*!
	 * Method called when component is initialized
	 * \return true on success
	 */
	virtual bool onInit();

	/*!
	 * Method called when component is finished
	 * \return true on success
	 */
	virtual bool onFinish();

	/*!
	 * Method called when step is called
	 * \return true on success
	 */
	virtual bool onStep();


private:
	void onNewImage();
	void onRpcCall();

	void initChessboard();

	/** event handler. */
	Base::EventHandler <CvFindLabirynth_Processor> h_onNewImage;
	Base::EventHandler <CvFindLabirynth_Processor> h_onRpcCall;
	/** In stream. */
	Base::DataStreamIn <cv::Mat> in_img;
	Base::DataStreamIn <xdr_iarchive <> > rpcParam;
	/** out event and stream */
	Base::Event* rpcResult;
	Base::DataStreamOut <Types::Mrrocpp_Proxy::LReading> out_info;

	/** Located corners.*/
	std::vector<cv::Point2f> corners;
	int findLabirynthFlags;
	int temp;
	bool found;

	cv::Mat image;
	/*
	Common::Timer timer;

	cv::Mat sub_img;

	Base::Property<bool> prop_subpix;
	Base::Property<int> prop_subpix_window;
	Base::Property<bool> prop_scale;
	Base::Property<int> prop_scale_factor;
	Base::Property<int> prop_width;
	Base::Property<int> prop_height;
	Base::Property<float> prop_square_width;
	Base::Property<float> prop_square_height;

	Base::Property<bool> prop_fastCheck;
	Base::Property<bool> prop_filterQuads;
	Base::Property<bool> prop_adaptiveThreshold;
	Base::Property<bool> prop_normalizeImage;


	void sizeCallback(int old_value, int new_value);
	void flagsCallback(bool old_value, bool new_value);
*/

};

} // namespace CvFindLabirynth {

} // namespace Processors {

REGISTER_PROCESSOR_COMPONENT("CvFindLabirynth", Processors::CvFindLabirynth::CvFindLabirynth_Processor, Common::Panel_Empty)

#endif /* CVFINDLABIRYNTH_PROCESSOR_HPP_ */
