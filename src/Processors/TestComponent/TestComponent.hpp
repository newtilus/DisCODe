/*!
 * \file
 * \brief
 */

#ifndef TESTCOMPONENT_PROCESSOR_HPP_
#define TESTCOMPONENT_PROCESSOR_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"

#include <cv.h>
//using namespace cv;
#include <highgui.h>

#include <vector>

#define LABIRYNT_SIZE_X 8
#define LABIRYNT_SIZE_Y 9
#define EPS 5
/**
 * wall/2 lenght
 */
#define WALL 27
#define FIELD WALL*2

/**
 * info about walls for one rectangle in Labirynth
 * */
struct walls_info
{
	bool n_wall;
	bool e_wall;
	bool w_wall;
	bool s_wall;
	int value;
};
/**
 * Point struct
 * */
struct Point
{
	int x;
	int y;
};


namespace Processors {
namespace TestComponent {

/*!
 * \brief TestComponent properties
 */
struct TestComponent_Props: public Base::Props
{

	/*!
	 * \copydoc Base::Props::load
	 */
	void load(const ptree & pt)
	{

	}

	/*!
	 * \copydoc Base::Props::save
	 */
	void save(ptree & pt)
	{

	}

};

/*!
 * \class TestComponent_Processor
 * \brief TestComponent processor class.
 */
class TestComponent_Processor: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	TestComponent_Processor(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~TestComponent_Processor();

	/*!
	 * Return window properties
	 */
	Base::Props * getProperties()
	{
		return &props;
	}

protected:

	walls_info labirynt_info_array [LABIRYNT_SIZE_Y][LABIRYNT_SIZE_X];
	int q;
	bool path_exists;
	bool path_set;
	vector<Point> path;

	cv::Point2i corner_LU;
	cv::Point2i corner_RD;

	cv::Point2i ball_center;


	 // Input data stream
	Base::DataStreamIn <cv::Mat> in_img;

	// Output data stream - processed image
	Base::DataStreamOut <cv::Mat> out_img;
	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();

	/// Properties
	TestComponent_Props props;

	// Event handler function.
	void onNewImage();

	// Event handler.
	Base::EventHandler <TestComponent_Processor> h_onNewImage;

	// Event emited after the image is processed.
	Base::Event * newImage;

	/**
	   * find labirynth's walls
	   */
	  bool find_labirynth(cv::Mat img, cv::Mat dst, int flag, int value);

	 /**
	  * find walls inside labirynth
	  */
	  bool find_wall(cv::Mat img, cv::Mat dst, int flag, int value, cv::Point2i center);

	  /**
	   * find optimal path
	   */
	  bool find_path(int=0,int=0);
	  /**
	   * helper method for find_path
	   */
	  void set_neighbours(int y, int x);

	  /**
	   * helper method for find_path
	   */
	  void set_path(Point end, Point start);

};

}//: namespace TestComponent
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("TestComponent", Processors::TestComponent::TestComponent_Processor, Common::Panel_Empty)

#endif /* TESTCOMPONENT_PROCESSOR_HPP_ */
