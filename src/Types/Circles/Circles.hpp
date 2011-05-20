/*
 * Circles.hpp
 *
 *  Created on: 20-05-2011
 *      Author: x
 */

#ifndef CIRCLES_HPP_
#define CIRCLES_HPP_

#include <vector>
#include <cv.h>

#include "Drawable.hpp"

namespace Types {

class Circles : public Drawable
{
public:
	Circles();
	virtual ~Circles();

	std::vector<cv::Vec3f> circles;
};

}

#endif /* CIRCLES_HPP_ */
