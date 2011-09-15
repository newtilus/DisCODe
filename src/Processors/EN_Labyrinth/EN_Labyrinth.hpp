/*!
 * \file EN_Labyrinth.hpp
 * \brief
 * \author kwasak
 * \date 2010-11-11
 */

#ifndef EN_Labyrinth_HPP_
#define EN_Labyrinth_HPP_

#define DIMENSION_X 7
#define DIMENSION_Y 7
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define HORIZONTAL 0
#define VERTICAL 1

#include <cv.h>
#include <highgui.h>
#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"
#include "Common/xdr/xdr_iarchive.hpp"

#include "EN_Labyrinth_Reading.hpp"

namespace Processors {
namespace EN_Labyrinth {

using namespace cv;

/*!
 * \brief KW_mean_skin properties
 */

struct Props: public Base::Props
{
	/*!
	 * \copydoc Base::Props::load
	 */
	void load(const ptree & pt)
	{
		maskSize.width = pt.get("width", 3);
		maskSize.height = pt.get("height", 3);
		calibrationResults = pt.get <string> ("calibrationResults", "calibration_results.xml");
		x1 = pt.get("x1", 0);
		x2 = pt.get("x2", 0);
		y1 = pt.get("y1", 0);
		y2 = pt.get("y2", 0);
		threshold = pt.get("threshold", 20);
		segmentation_threshold = pt.get("segmentation_threshold", 20);
		min_length = pt.get("min_length", 100);
		max_gap = pt.get("max_gap", 5);
	}

	/*!
	 * \copydoc Base::Props::save
	 */
	void save(ptree & pt)
	{
		pt.put("width", maskSize.width);
		pt.put("height", maskSize.height);
	}

	cv::Size maskSize;
	string calibrationResults;
	int x1;
	int x2;
	int y1;
	int y2;
	int threshold;
	int segmentation_threshold;
	int min_length;
	int max_gap;
};


/*!
 * \class EN_Labyrinth
 */
class EN_Labyrinth: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	EN_Labyrinth(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~EN_Labyrinth();

	/*!
	 * Return window properties
	 */
	Base::Props * getProperties()
	{
		return &props;
	}

protected:

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

	/*!
	 * Event handler function.
	 */
	void onNewImage();

	/*!
	 * Event handler function.
	 */
	void onRpcCall();

	/// handlers
	Base::EventHandler <EN_Labyrinth> h_onNewImage; // new image of labyrinth arrived
	Base::EventHandler <EN_Labyrinth> h_onRpcCall; // RPC call from mrrocpp arrived

	/// inputs
	Base::DataStreamIn <Mat, Base::DataStreamBuffer::Newest> in_img; // new image of labyrinth
	//Base::DataStreamIn <Mat> in_img;
	Base::DataStreamIn <xdr_iarchive <> > rpcParam; // data from mrrocpp

	/// output events
	Base::Event* rpcResult; // the data ready to sent to mrrocpp
	Base::Event* newImage; // new image of solved labyrinth ready to be presented

	/// outputs
	Base::DataStreamOut <Mat> out_img;	//new image of solved labyrinth
	Base::DataStreamOut <Types::Mrrocpp_Proxy::EN_Labyrinth_Reading> result_to_mrrocpp; // the data to be sent to mrrocpp

	// Load calibration parameters from file.
	bool loadParameters();

	/// Properties
	Props props;

private:
	cv::Mat hue_img;
	cv::Mat saturation_img;
	cv::Mat value_img;
	cv::Mat segments_img;
	// The 3x3 camera matrix containing focal lengths fx,fy and displacement of the center of coordinates cx,cy.
	cv::Mat cameraMatrix;
	// Vector with distortion coefficients k_1, k_2, p_1, p_2, k_3.
	cv::Mat distCoeffs;
	// Matrices storing partial undistortion results.
	//Mat map1, map2;
	Mat labyrinth_first;
	Mat first_image;
	int k;
	bool showImage;
	bool showProcessed;
	bool solve;
	bool calibrate;
	int x1;
	int x2;
	int y1;
	int y2;
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	double rotation;
	int segmentation_threshold;
	int threshold;
	int min_length;
	int max_gap;
	// Indicates if the file with calibration parameters was found
	bool file_found;
	bool first_image_saved;
	bool labyrinth_found;
	bool labyrinth_solved;
	int path[];
	int path_size;
	int start_point[];
	int end_point[];
};

class Cell
{

private:

public:
	int x;
	int y;
	int height;
	int width;

	Cell();
	Cell(int x, int y, int width, int height);

};

class Labyrinth
{
private:
	bool transitions[DIMENSION_X][DIMENSION_Y][4]; // array representing possible moves
	vector <pair <int, int> > path[DIMENSION_X][DIMENSION_Y]; // array representing path leading to the point
	vector <pair <int, int> > query; // vector of point waiting to be checked
	bool waiting[DIMENSION_X][DIMENSION_Y]; // array representing the fact that cell is waiting to be checked
	Cell* cells[DIMENSION_X][DIMENSION_Y];

public:
	Labyrinth();
	//~Labyrinth();
	bool insert_cell(int pos_x, int pos_y, int x, int y, int width, int heigth);
	Cell* get_cell(int pos_x, int pos_y);
	void set_transition(int pos_x, int pos_y, int direction);
	void remove_transition(int pos_x, int pos_y, int direction);
	bool get_transition(int pos_x, int pos_y, int direction);
	bool get_waiting(int pos_x, int pos_y);
	bool set_waiting(int pos_x, int pos_y);
	vector <pair <int, int> > solve(pair <int, int> start, pair <int, int> end);

};

}//: namespace EN_Labyrinth
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("EN_Labyrinth", Processors::EN_Labyrinth::EN_Labyrinth, Common::Panel_Empty)

#endif /* EN_Labyrinth_HPP_ */

