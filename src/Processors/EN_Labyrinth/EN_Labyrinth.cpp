/*!
 * \file EN_Labyrinth.cpp
 * \brief
 * \author enatil
 */

#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include "Logger.hpp"

#include "EN_Labyrinth.hpp"


using namespace std;
using Types::Mrrocpp_Proxy::EN_Labyrinth_Reading;

namespace Processors {
namespace EN_Labyrinth {

// OpenCV writes hue in range 0..180 instead of 0..360
#define H(x) (x>>1)

bool check_connection(Mat* img, Point from, Point to, int cell_width, int cell_height)
{
	// check how both cells are oriented
	int direction;
	if (abs(from.x - to.x) > abs(from.y - to.y))
		direction = HORIZONTAL;
	else
		direction = VERTICAL;

	Point tmp;
	Mat roi;

	// crop the two cells from the image
	if (direction == HORIZONTAL) {
		if (from.x > to.x) {
			tmp = to;
			to = from;
			from = tmp;
		}
		roi = Mat(*img, Rect(from.x, from.y - cell_height / 2, to.x - from.x, cell_height));
	} else {
		if (from.y > to.y) {
			tmp = to;
			to = from;
			from = tmp;
		}
		roi = Mat(*img, Rect(from.x - cell_width / 2, from.y, cell_width, to.y - from.y));
	}

	// BW, threshold and find lines
	Mat bw;
	cvtColor(roi, bw, CV_BGR2GRAY);
	Mat after_threshold;
	cv::threshold(bw, after_threshold, 220, 255, CV_THRESH_BINARY);
	vector<Vec4i> lines;
	int count_proper_lines = 0;
	if (direction == HORIZONTAL)
		HoughLinesP(after_threshold, lines, 1, CV_PI/2, 12,cell_height*0.4, 1);
	else
		HoughLinesP(after_threshold, lines, 1, CV_PI/2, 12,cell_width*0.4, 1);

	// count the lines of the correct size to threat them as walls
	for (size_t i = 0; i < lines.size(); i++) {
		if (direction == HORIZONTAL) {
			if (abs(lines[i][0] - lines[i][2]) < 2)
				++count_proper_lines;
		} else {
			if (abs(lines[i][1] - lines[i][3]) < 2)
				++count_proper_lines;
		}
	}

	// if there are no walls, paint the line and return true
	if (count_proper_lines < 1) {
		line(*img, from, to, CV_RGB(0, 255, 0), 2, CV_AA, 0);
		return true;
	} else
		return false;

}

EN_Labyrinth::EN_Labyrinth(const std::string & name) :
	Base::Component(name)
{
	LOG(LTRACE) << "Hello EN_Labyrinth\n";
	k = 0;
	showImage = false;
	showProcessed = false;
	solve = false;
	calibrate = false;
}

EN_Labyrinth::~EN_Labyrinth()
{
	LOG(LTRACE) << "Good bye EN_Labyrinth\n";
}

bool EN_Labyrinth::onInit()
{
	LOG(LTRACE) << "EN_Labyrinth::initialize\n";

	h_onNewImage.setup(this, &EN_Labyrinth::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);
	h_onRpcCall.setup(this, &EN_Labyrinth::onRpcCall);
	registerHandler("onRpcCall", &h_onRpcCall);

	registerStream("in_img", &in_img);
	registerStream("rpcParam", &rpcParam);
	registerStream("rpcResult", &result_to_mrrocpp);

	newImage = registerEvent("newImage");
	rpcResult = registerEvent("rpcResult");

	//registerStream("out_hue", &out_hue);
	//registerStream("out_saturation", &out_saturation);
	registerStream("out_img", &out_img);

	file_found = loadParameters();
	labyrinth_found = false;
	labyrinth_solved = false;

	return true;
}

bool EN_Labyrinth::onFinish()
{
	LOG(LTRACE) << "EN_Labyrinth::finish\n";

	return true;
}

bool EN_Labyrinth::onStep()
{
	LOG(LTRACE) << "EN_Labyrinth::step\n";
	return true;
}

bool EN_Labyrinth::onStop()
{
	return true;
}

bool EN_Labyrinth::onStart()
{
	return true;
}

void EN_Labyrinth::onRpcCall()
{
	LOG(LNOTICE) << "void EN_Labyrinth::onRpcCall() begin\n";

	xdr_iarchive <> mrrocpp_info = rpcParam.read();
	double param;
	mrrocpp_info >> param;
	LOG(LNOTICE) << "CvFindLabirynth_Processor::onRpcCall(): param=" << param;

	EN_Labyrinth_Reading reading;

	reading.labyrinth_solved = true;
	reading.path_size = 9;
	reading.start_point_x = 1;
	reading.start_point_y = 2;
	reading.end_point_x = 3;
	reading.end_point_y = 4;
//	for(int i=0; i<path_size; ++i)
//		reading.path[i] = i;


	printf("EN_Labyrinth onRpcCall():\n");
	printf("Solved: %d\n", reading.labyrinth_solved);
	printf("Path Size: %i\n", reading.path_size);
	printf("Start_pt: (%i, %i)\n", reading.start_point_x, reading.start_point_y);
	printf("End_pt: (%i, %i)\n", reading.end_point_x, reading.end_point_y);

	result_to_mrrocpp.write(reading);

	rpcResult->raise();
}

void EN_Labyrinth::onNewImage()
{
	LOG(LTRACE) << "ImageLabyrinth_Processor::onNewImage\n";

	x1 = props.x1;
	x2 = props.x2;
	y1 = props.y1;
	y2 = props.y2;
	threshold = props.threshold;
	segmentation_threshold = props.segmentation_threshold;
	min_length = props.min_length;
	max_gap = props.max_gap;

	Mat image = in_img.read();
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	// TODO Delete this to continue with labyrinth solving
	out_img.write(image);
	newImage->raise();
	return;
	//

	Mat out_calib;
	// Calibration based on file, if file doesn't exist, don't calibrate
	if (file_found) {
		Mat map1, map2;
		Size imageSize = image.size();
		initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0), imageSize, CV_32FC1, map1, map2);
		remap(image, out_calib, map1, map2, INTER_LINEAR);
		image = out_calib;
	}


	int first_image_width = image.size().width;
	int first_image_height = image.size().height;
	Mat rotatedImg;
	Mat src;
	Mat src_bw;
	Mat after_threshold;
	Mat src_lines;
	int min_x = 0;
	int max_x = 0;
	int min_y = 0;
	int max_y = 0;

	int line_number_left = 0;
	int line_number_right = 0;
	int line_number_up = 0;
	int line_number_down = 0;
	int min_line_length = 50;
	double rotation = 1.0;

	// if the labyrinth was not found, rotate image and try to find walls, remember the rotation
	for (; rotation < 180.0 && !labyrinth_found; rotation += 1) {
		LOG(LNOTICE) << " rotationao! " << rotation;

		Point2f src_center(image.cols/2.0F, image.rows/2.0F);
	    Mat rot_mat = getRotationMatrix2D(src_center, rotation, 1.0);
	    warpAffine(image, rotatedImg, rot_mat, image.size());

		src = rotatedImg.clone();

		CvSize src_image_size = src.size();

		// Source bw image
		cvtColor(src, src_bw, CV_BGR2GRAY);

		// Threshold
		after_threshold = cvCreateImage(src_image_size, IPL_DEPTH_8U, 1);
		cv::threshold(src_bw, after_threshold, segmentation_threshold, 255, CV_THRESH_BINARY);
		//src_bw = src_bw > segmentation_threshold;			// fajny efekt

		vector<Vec4i> lines;
		src_lines = src.clone();

	    HoughLinesP(after_threshold, lines, 1, CV_PI/2, threshold, min_length, max_gap);

		if (lines.size() == 0)
			continue;

//		// Draw lines
//		for (int i = 0; i < lines.size(); i++) {
//	        line(src_lines, Point(lines[i][0], lines[i][1]),  Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
//		}

		min_x = 0;
		max_x = 0;
		min_y = 0;
		max_y = 0;
		line_number_left = 0;
		line_number_right = 0;
		line_number_up = 0;
		line_number_down = 0;
		for (size_t i = 0; i < lines.size(); i++) {

			int x0 = lines[i][0];
			int y0 = lines[i][1];
			int x1 = lines[i][2];
			int y1 = lines[i][3];

			// vertical lines
			// calculate the coordinates of the middle of the line
			int middle = (x0 + x1) / 2;

			// the first line on the left?
			if ((middle < min_x || min_x == 0) && abs(y0 - y1) > min_line_length) {
				min_x = middle;
				line_number_left = i;
			}

			// the first line on the right?
			if (middle > max_x && abs(y0 - y1) > min_line_length) {
				max_x = middle;
				line_number_right = i;
			}

			// horizontal lines
			// calculate the coordinates of the middle of the line
			middle = (y0 + y1) / 2;
			// the first line up?
			if ((middle < min_y || min_y == 0) && abs(x0 - x1) > min_line_length) {
				min_y = middle;
				line_number_up = i;
			}

			// the first line down?
			if (middle > max_y && abs(x0 - x1) > min_line_length) {
				max_y = middle;
				line_number_down = i;
			}

		}

		// if the walls were found and the labyrinth is right size
		if (abs(max_x-min_x) > first_image_width / 3 && abs(max_y-min_y) > first_image_height / 3) {
			//crop the image of labyrinth
//			rectangle(src_lines, cvPoint(min_x, min_y), cvPoint(max_x, max_y), cvScalar(0,0,255), 2);
			Mat roi(src_lines, Rect(min_x,min_y,max_x-min_x,max_y-min_y));
			labyrinth_first = roi.clone();

//			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,line_number_left);
//			cvLine(src_lines, cvPoint(line[0].x, line[0].y), cvPoint(line[1].x, line[1].y), cvScalar(0,255,0), 4);
//			line = (CvPoint*)cvGetSeqElem(lines,line_number_right);
//			cvLine(src_lines, cvPoint(line[0].x, line[0].y), cvPoint(line[1].x, line[1].y), cvScalar(0,255,0), 4);
//			line = (CvPoint*)cvGetSeqElem(lines,line_number_up);
//			cvLine(src_lines, cvPoint(line[0].x, line[0].y), cvPoint(line[1].x, line[1].y), cvScalar(0,255,0), 4);
//			line = (CvPoint*)cvGetSeqElem(lines,line_number_down);
//			cvLine(src_lines, cvPoint(line[0].x, line[0].y), cvPoint(line[1].x, line[1].y), cvScalar(0,255,0), 4);

			LOG(LNOTICE) << "Znaleziono labirynt";
			LOG(LNOTICE) << " min_x " << min_x << " min_y " << min_y << " max_x " << max_x << " max_y " << max_y << " rotation " << rotation;

			labyrinth_found = true;
		}
	}


	if(!labyrinth_found)
	{
		LOG(LWARNING) << "Labyrint nie został odnaleziony.";
		return;
	}

	Mat labyrinth_cropped = labyrinth_first.clone();

	Labyrinth labyrinth;

	int cell_width = labyrinth_cropped.size().width / DIMENSION_X;
	int cell_height = labyrinth_cropped.size().height / DIMENSION_Y;

	for (int i = 0; i < DIMENSION_X; i++) {
		for (int j = 0; j < DIMENSION_Y; j++) {
			//bool insert_cell(int pos_x, int pos_y, int x, int y, int width, int heigth);
			labyrinth.insert_cell(i, j, labyrinth_cropped.size().width / DIMENSION_X * i + cell_width / 2, labyrinth_cropped.size().height
					/ DIMENSION_Y * j + cell_height / 2, cell_width, cell_height);
			circle(labyrinth_cropped, cvPoint(labyrinth_cropped.size().width / DIMENSION_X * i + cell_width / 2, labyrinth_cropped.size().height
					/ DIMENSION_Y * j + cell_height / 2), 2, cvScalar(255, 255, 0), 2, 8, 0);
		}
	}

	for (int j = 0; j < DIMENSION_Y; ++j) {
		for (int i = 0; i < DIMENSION_X - 1; ++i) {
			bool connection_exists =
							check_connection(&labyrinth_cropped, Point(labyrinth_cropped.size().width / DIMENSION_X * i
									+ cell_width / 2, labyrinth_cropped.size().height / DIMENSION_Y * j + cell_height / 2), Point(labyrinth_cropped.size().width
									/ DIMENSION_X * (i + 1) + cell_width / 2, labyrinth_cropped.size().height / DIMENSION_Y
									* j + cell_height / 2), cell_width, cell_height);
			if (connection_exists) {
				labyrinth.set_transition(i, j, RIGHT);
				labyrinth.set_transition(i + 1, j, LEFT);
			}
		}
	}

	for (int j = 0; j < DIMENSION_X; ++j) {
		for (int i = 0; i < DIMENSION_Y - 1; ++i) {
			bool connection_exists =
							check_connection(&labyrinth_cropped, Point(labyrinth_cropped.size().width / DIMENSION_X * j
									+ cell_width / 2, labyrinth_cropped.size().height / DIMENSION_Y * i + cell_height / 2), Point(labyrinth_cropped.size().width
									/ DIMENSION_X * j + cell_width / 2, labyrinth_cropped.size().height / DIMENSION_Y * (i
									+ 1) + cell_height / 2), cell_width, cell_height);
			if (connection_exists) {
				labyrinth.set_transition(j, i, DOWN);
				labyrinth.set_transition(j, i + 1, UP);
			}
		}
	}

	vector <pair <int, int> > v = labyrinth.solve(make_pair(0, 0), make_pair(DIMENSION_X - 1, DIMENSION_Y - 1));
	vector <pair <int, int> >::iterator it1;
	vector <pair <int, int> >::iterator it2;

	for (it1 = v.begin(), it2 = ++(v.begin()); it2 != v.end() && it1 != v.end(); ++it1, ++it2) {
		Point from, to;
		from = Point((labyrinth.get_cell((*it1).first, (*it1).second))->x, (labyrinth.get_cell((*it1).first, (*it1).second))->y);
		to = Point((labyrinth.get_cell((*it2).first, (*it2).second))->x, (labyrinth.get_cell((*it2).first, (*it2).second))->y);
		line(labyrinth_cropped, from, to, CV_RGB(255, 255, 0), 2, CV_AA, 0);
	}

	out_img.write(labyrinth_cropped.clone());
	newImage->raise();
}

bool EN_Labyrinth::loadParameters()
{
	LOG(LTRACE) << "EN_Labyrinth::loadParameters()";

	FileStorage fs(props.calibrationResults, FileStorage::READ);
	if (!fs.isOpened()) {
		LOG(LNOTICE) << "Coudn't open file " << props.calibrationResults << " with calibration parameters.";
		return false;
	} else {
		fs["camera_matrix"] >> cameraMatrix;
		fs["distortion_coefficients"] >> distCoeffs;

		LOG(LNOTICE) << "Loaded camera matrix";
		LOG(LNOTICE) << cameraMatrix.at <double> (0, 0) << " " << cameraMatrix.at <double> (0, 1) << " "
				<< cameraMatrix.at <double> (0, 2);
		LOG(LNOTICE) << cameraMatrix.at <double> (1, 0) << " " << cameraMatrix.at <double> (1, 1) << " "
				<< cameraMatrix.at <double> (1, 2);
		LOG(LNOTICE) << cameraMatrix.at <double> (2, 0) << " " << cameraMatrix.at <double> (2, 1) << " "
				<< cameraMatrix.at <double> (2, 2);
		LOG(LNOTICE) << "Loaded distortion coefficients";
		LOG(LNOTICE) << distCoeffs.at <double> (0, 0) << " " << distCoeffs.at <double> (1, 0) << " "
				<< distCoeffs.at <double> (2, 0) << " " << distCoeffs.at <double> (3, 0) << " " << distCoeffs.at <
				double> (4, 0) << " " << distCoeffs.at <double> (5, 0) << " " << distCoeffs.at <double> (6, 0) << " "
				<< distCoeffs.at <double> (7, 0);
	}
	return true;
}

Labyrinth::Labyrinth()
{
	for (int i = 0; i < DIMENSION_X; ++i)
		for (int j = 0; j < DIMENSION_Y; ++j)
			for (int k = 0; k < 4; ++k)
				transitions[i][j][k] = false;

	for (int i = 0; i < DIMENSION_X; ++i)
		for (int j = 0; j < DIMENSION_Y; ++j)
			cells[i][j] = NULL;

	for (int i = 0; i < DIMENSION_X; ++i)
		for (int j = 0; j < DIMENSION_Y; ++j)
			waiting[i][j] = false;

}
;
bool Labyrinth::insert_cell(int pos_x, int pos_y, int x, int y, int width, int height)
{
	cells[pos_x][pos_y] = new Cell(x, y, width, height);

	return true;
}
;

Cell* Labyrinth::get_cell(int pos_x, int pos_y)
{
	return cells[pos_x][pos_y];
}
;

void Labyrinth::set_transition(int pos_x, int pos_y, int direction)
{
	transitions[pos_x][pos_y][direction] = true;
}
;

void Labyrinth::remove_transition(int pos_x, int pos_y, int direction)
{
	transitions[pos_x][pos_y][direction] = false;
}
;

bool Labyrinth::get_transition(int pos_x, int pos_y, int direction)
{
	return transitions[pos_x][pos_y][direction];
}
;

bool Labyrinth::get_waiting(int pos_x, int pos_y)
{
	return waiting[pos_x][pos_y];
}
;

bool Labyrinth::set_waiting(int pos_x, int pos_y)
{
	return waiting[pos_x][pos_y] = true;
}
;

vector <pair <int, int> > Labyrinth::solve(pair <int, int> start, pair <int, int> end)
{
	query.push_back(start);
	path[start.first][start.second].push_back(make_pair(start.first, start.second));

	ofstream file;
	file.open("/home/enatil/solve_maze.txt");

	file << "Transitions:" << endl << "UP" << endl;
	for (int j = 0; j < DIMENSION_Y; ++j) {
		for (int i = 0; i < DIMENSION_X; ++i) {
			file << transitions[i][j][UP] << "\t";
		}
		file << endl;
	}

	file << "RIGHT" << endl;
	for (int j = 0; j < DIMENSION_Y; ++j) {
		for (int i = 0; i < DIMENSION_X; ++i) {
			file << transitions[i][j][RIGHT] << "\t";
		}
		file << endl;
	}

	file << "DOWN" << endl;
	for (int j = 0; j < DIMENSION_Y; ++j) {
		for (int i = 0; i < DIMENSION_X; ++i) {
			file << transitions[i][j][DOWN] << "\t";
		}
		file << endl;
	}

	file << "LEFT" << endl;
	for (int j = 0; j < DIMENSION_Y; ++j) {
		for (int i = 0; i < DIMENSION_X; ++i) {
			file << transitions[i][j][LEFT] << "\t";
		}
		file << endl;
	}

	while (!query.empty()) {
		pair <int, int> actual_point = *(query.begin());

		file << "Actual point: " << actual_point.first << " " << actual_point.second << endl;

		query.erase(query.begin());

		// check up
		if (get_transition(actual_point.first, actual_point.second, UP) && path[actual_point.first][actual_point.second
				- 1].empty()) {
			file << "UP is possible" << endl;
			path[actual_point.first][actual_point.second - 1] = path[actual_point.first][actual_point.second];
			path[actual_point.first][actual_point.second - 1].push_back(make_pair(actual_point.first, actual_point.second));
			query.push_back(make_pair(actual_point.first, actual_point.second - 1));
		}

		// check right
		if (get_transition(actual_point.first, actual_point.second, RIGHT)
				&& path[actual_point.first + 1][actual_point.second].empty()) {
			file << "RIGHT is possible" << endl;
			path[actual_point.first + 1][actual_point.second] = path[actual_point.first][actual_point.second];
			path[actual_point.first + 1][actual_point.second].push_back(make_pair(actual_point.first, actual_point.second));
			query.push_back(make_pair(actual_point.first + 1, actual_point.second));
		}

		// check down
		if (get_transition(actual_point.first, actual_point.second, DOWN)
				&& path[actual_point.first][actual_point.second + 1].empty()) {
			file << "DOWN is possible" << endl;
			path[actual_point.first][actual_point.second + 1] = path[actual_point.first][actual_point.second];
			path[actual_point.first][actual_point.second + 1].push_back(make_pair(actual_point.first, actual_point.second));
			query.push_back(make_pair(actual_point.first, actual_point.second + 1));
		}

		// check left
		if (get_transition(actual_point.first, actual_point.second, LEFT)
				&& path[actual_point.first - 1][actual_point.second].empty()) {
			file << "LEFT is possible" << endl;
			path[actual_point.first - 1][actual_point.second] = path[actual_point.first][actual_point.second];
			path[actual_point.first - 1][actual_point.second].push_back(make_pair(actual_point.first, actual_point.second));
			query.push_back(make_pair(actual_point.first - 1, actual_point.second));
		}

		file << "Path: ";
		vector <pair <int, int> >::iterator it;
		for (it = path[actual_point.first][actual_point.second].begin(); it
				!= path[actual_point.first][actual_point.second].end(); ++it)
			file << (*it).first << "-" << (*it).second << "; ";
		file << endl;

		if (actual_point.first == end.first && actual_point.second == end.second) {
			file.close();
			path[end.first][end.second].push_back(make_pair(end.first, end.second));
			return path[end.first][end.second];
		}

	}
	file.close();
	return path[end.first][end.second];

}
;

Cell::Cell()
{
	x = y = width = height = 0;
}
;

Cell::Cell(int x_, int y_, int width_, int height_)
{
	x = x_;
	y = y_;
	width = width_;
	height = height_;
}
;

}//: namespace EN_Labyrinth
}//: namespace Processors
