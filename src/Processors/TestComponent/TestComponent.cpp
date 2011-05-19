/*!
 * \file
 * \brief
 */

#include <memory>
#include <string>

#include "TestComponent.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace TestComponent {

TestComponent_Processor::TestComponent_Processor(const std::string & name) : Base::Component(name)
{
	LOG(LTRACE) << "Hello TestComponent_Processor\n";
	q=0;
	path_exists = false;
	path_set = false;
	corner_LU = cv::Point2i(0,0);
	corner_RD = cv::Point2i(0,0);

}

TestComponent_Processor::~TestComponent_Processor()
{
	LOG(LTRACE) << "Good bye TestComponent_Processor\n";
}

bool TestComponent_Processor::onInit()
{
	LOG(LTRACE) << "TestComponent_Processor::initialize\n";

	// Register data streams, events and event handlers HERE!
	// Register handler.
	h_onNewImage.setup(this, &TestComponent_Processor::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);
	// Register event.
	newImage = registerEvent("newImage");
	// Register data streams.
	registerStream("in_img", &in_img);
	registerStream("out_img", &out_img);



	return true;
}

bool TestComponent_Processor::onFinish()
{
	LOG(LTRACE) << "TestComponent_Processor::finish\n";

	return true;
}

bool TestComponent_Processor::onStep()
{
	LOG(LTRACE) << "TestComponent_Processor::step\n";
	return true;
}

bool TestComponent_Processor::onStop()
{
	return true;
}

bool TestComponent_Processor::onStart()
{
	return true;
}

void TestComponent_Processor::onNewImage()
{
    LOG(LTRACE) << "TestComponent_Processor::onNewImage\n";
    try {

    	int x_rect=0;
    	int y_rect=0;
    	int eps = 4;

        // Read image from input data stream.
        cv::Mat img = in_img.read();
        /* clone image */
		//IplImage* im = cvCloneImage(image_);
		/* create new image for the grayscale version */
        cv::Mat dst(img.rows,img.cols, img.depth());
		/* CV_RGB2GRAY: convert RGB image to grayscale */
		cv::cvtColor( img, dst, CV_RGB2GRAY );

		//cv::vector<cv::Vec3f> circles;
		//cv::HoughCircles(dst,circles, CV_HOUGH_GRADIENT, 1.15, 100, 200);

		//while(!circles.empty())
		//{
		//	std::cout<<"cir: "<<circles.back()[0]<<" "<<circles.back()[1]<<" "<<circles.back()[2]<<endl;
			//ball_center.x = circles.back()[0];
			//ball_center.y = circles.back()[1];
		//			circles.pop_back();
		//}

		/* color invert	*/
		cv::bitwise_not(dst,dst);

		/* contour joy */
		cv::Mat contour(dst.rows,dst.cols, dst.depth());
		cv::Mat helper(dst.rows,dst.cols, dst.depth());
		cv::threshold(dst,contour,70,255,CV_THRESH_BINARY);


		/*
		  Canny( src, dst, 50, 200, 3 );
    cvtColor( dst, color_dst, CV_GRAY2BGR );

#if 0
    vector<Vec2f> lines;
    HoughLines( dst, lines, 1, CV_PI/180, 100 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0];
        float theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        Point pt1(cvRound(x0 + 1000*(-b)),
                  cvRound(y0 + 1000*(a)));
        Point pt2(cvRound(x0 - 1000*(-b)),
                  cvRound(y0 - 1000*(a)));
        line( color_dst, pt1, pt2, Scalar(0,0,255), 3, 8 );
    }
#else
    vector<Vec4i> lines;
    HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( color_dst, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    }

		 */

		//Canny( img, dst, 50, 200, 3 );
		//cvtColor( dst, dst, CV_GRAY2BGR );


		int minx=500, miny=500,maxx=0,maxy=0;

		cv::bitwise_not(contour,contour);
		cv::vector<cv::Vec4i> lines;
		cv::HoughLinesP(contour, lines, 1, CV_PI, 240, 210, 0);//src,contener,?,kat,treshold,minlength
	    for( size_t i = 0; i < lines.size(); i++ )
	    {
	    	//cout<<"s: "<<lines[i][0]<<", "<< lines[i][1]<<" k: "<lines[i][2]<<" "<<lines[i][3]<<endl;

	    	//x
	    	if(lines[i][0]<minx)
	    		minx=lines[i][0];
	    	if(lines[i][2]<minx)
	    		minx=lines[i][2];
	    	if(lines[i][0]>maxx)
				maxx=lines[i][0];
			if(lines[i][2]>maxx)
				maxx=lines[i][2];
			//y
			if(lines[i][1]<miny)
				miny=lines[i][1];
			if(lines[i][3]<miny)
				miny=lines[i][3];
			if(lines[i][1]>maxy)
				maxy=lines[i][1];
			if(lines[i][3]>maxy)
				maxy=lines[i][3];

	        cv::line( img, cv::Point(lines[i][0], lines[i][1]),
	            cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 1, 8 );
	    }
	    cv::bitwise_not(contour,contour);

	    cout<<"LU: "<<minx<<", "<< miny<<" RD: "<<maxx<<" "<<maxy<<endl;


		/* DONE
		 * 1) Znajdz automatycznie rogi labiryntu
		 * 2) Znajdz poszczegolne srodki pol
		 * 3) Sprawdz czy sa scianki na 4 strony swiata na odleglosc d/2 + eps
		 * 4) Zapisz dane do tablicy
		 *
		 * 5) Oblicz optymalna sciezke
		 * TODO
		 * 6) Podaj rozkazy do MROCC++
		 * !) Znalezienie polozenia kulki / ignorowanie kulki przy znajdywaniu scian
		 */

		/* find labirynth's walls */
		find_labirynth(contour, helper,0,1);
		find_labirynth(contour, helper,0,-1);
		find_labirynth(contour, helper,1,1);
		find_labirynth(contour, helper,1,-1);

		std::cout<<corner_LU.x<<" "<<corner_LU.y<<" "<<corner_RD.x<<" "<<corner_RD.y<<endl;

		//corner_LU.x = 222;
		//corner_LU.y = 30;
		//corner_RD.x = 570;
		//corner_RD.y = 410;

		/*do it once only*/
		if(q==0)
		{
			/* find walls inside labirynth */
			cv::Point2i corner((corner_LU.x + WALL),(corner_LU.y + WALL));

			for(y_rect=0; y_rect < LABIRYNT_SIZE_Y; y_rect++)
			{
				for(x_rect=0; x_rect < LABIRYNT_SIZE_X; x_rect++)
				{
					//dzielenie int'ow?
					cv::Point2i center(corner.x + (x_rect)*((corner_RD.x - corner_LU.x)/LABIRYNT_SIZE_X), corner.y + (y_rect)*((corner_RD.y - corner_LU.y)/LABIRYNT_SIZE_Y));

					printf("POLE (%d, %d)\n",x_rect+1,y_rect+1);

					cv::Point2i search = center;
					labirynt_info_array[y_rect][x_rect].n_wall = find_wall(contour, helper, 1,-1, center);
					labirynt_info_array[y_rect][x_rect].w_wall = find_wall(contour, helper, 0,-1, center);
					labirynt_info_array[y_rect][x_rect].e_wall = find_wall(contour, helper, 0, 1, center);
					labirynt_info_array[y_rect][x_rect].s_wall = find_wall(contour, helper, 1, 1, center);
					//to powinno byc w konstruktorze
					labirynt_info_array[y_rect][x_rect].value = -1;

					//cout<<labirynt_info_array[y_rect][x_rect].n_wall<<endl;
				}
			}
			//displayImage("contour Image", contour);
			//displayImage("helper Image", helper);

			//find start field

			int FIELD_X = ((corner_RD.x - corner_LU.x)/LABIRYNT_SIZE_X);
			int FIELD_Y = ((corner_RD.y - corner_LU.y)/LABIRYNT_SIZE_Y);

			ball_center.x = 415;
			ball_center.y = 215;

			int xfield,yfield;
			cv::Point2i temp = corner;

			for(int ny=0;ny<LABIRYNT_SIZE_Y;ny++)
			{
				if(ball_center.y+FIELD_Y/2 > temp.y && ball_center.y-FIELD_Y/2 < temp.y)
				{
					std::cout<<"temp.y"<<temp.y<<endl;
					yfield = ny;
					break;
				}
				temp.y+=FIELD_Y;
			}
			temp = corner;
			for(int nx=0;nx<LABIRYNT_SIZE_X;nx++)
			{
				if(ball_center.x+FIELD_X/2 > temp.x && ball_center.x-FIELD_X/2 < temp.x)
				{
					std::cout<<"temp.x"<<temp.x<<endl;
					xfield = nx;
					break;
				}
				temp.x+=FIELD_X;
			}
			std::cout<<"FIELD: "<<xfield<<" "<<yfield<<endl;


			/* find path */
			if (find_path(xfield,yfield) == false)
				printf("Labirynt nie ma drogi wyjscia!!\n");
			else
			{
				printf("Znaleziono wyjscie - Oto scontourciezka:\n");
				for(int i=0;i<path.size();i++)
				{
					printf("%d,%d\n",path[i].x,path[i].y);
				}
			}
		q=1;
		}

/*
		for (unsigned int j=0; j<img.rows; j++)
		  for (unsigned int i=0; i<img.cols; i++)
		  {
			  cv::Scalar c;
			  		c.val[0] = img.at<uchar>(j,i);

			double b = ((double)c.val[0])/255.;
			double g = ((double)c.val[1])/255.;
			double r = ((double)c.val[2])/255.;
			unsigned char f = 255*((r>0.2+g)&&(r>0.2+b));

			std::cout<<"f "<<f<<endl;

			if(r>50)
			{img.at<uchar>(j,i) = f;}

		  }
*/


        // Write changed image to output data stream.

        out_img.write(img);//contour
        // Raise event.
        newImage->raise();
    } catch (...) {
        LOG(LERROR) << "TestComponent_Processor::onNewImage failed\n";
    }
}

/**
 * find main labirynth's walls
 */
bool TestComponent_Processor::find_labirynth(cv::Mat img, cv::Mat dst, int flag, int value)
{
	cv::Point2i search;
	search.x = img.cols/2;
	search.y = img.rows/2;

	if(flag==0)
	{
		if(value==1)
			search.x = 1;
		else
			search.x = img.cols - 2;
	}
	else
	{
		if(value==1)
			search.y = 1;
		else
			search.y = img.rows - 2;
	}
	cv::Scalar pix;
	//cv::Mat_<cv::Vec3b>& image = (cv::Mat_<cv::Vec3b>&)img;
	//cv::imshow("test",dst);

	while(true)
	{
		pix.val[0] = img.at<uchar>(search.y, search.x);
		//std::cout<<pix.val[0]<<endl;//" "<<pix.val[1]<<" "<<pix.val[2]<<endl;
		if(pix.val[0] != 255)
		{
			std::cout<<search.x<<" "<<search.y<<endl;
			break;
		}


		/* rysowanie sciezki poszukiwan */
		//pix.val[0] = 0;
		//cv::Mat_<cv::Vec3b>& dstimage = (cv::Mat_<cv::Vec3b>&)dst;
		//dstimage(search.y, search.x)[0] = pix[0];

		if((search.y == 0) || (search.y == img.rows -1) || (search.x == img.cols -1) || (search.x == 0))
			break;

		if(flag == 0)
			search.x = search.x + value;
		if(flag == 1)
			search.y = search.y + value;
	}
	/* zapisz wspolrzedne */
	if(flag==0 && value==1)//X+
	{
		corner_LU.x = search.x;
	}
	if(flag==0 && value==-1)//X-
	{
		corner_RD.x = search.x;
	}
	if(flag==1 && value==1)//Y+
	{
		corner_LU.y = search.y;
	}
	if(flag==1 && value==-1)//Y-
	{
		corner_RD.y = search.y;
	}

return false;
}
/**
 * find_wall from center point
 * @param IMG
 * @param flag 0 change x 1 change y
 * @param value 1 (down) or -1 (up)
 * @param center - start point
 */
bool TestComponent_Processor::find_wall(cv::Mat img, cv::Mat dst, int flag, int value, cv::Point2i center)
{
	cv::Point2i search = center;
	cv::Scalar pix;

	for(int i=1;i<(WALL+3*EPS);i++)
	{

		pix.val[0] = img.at<uchar>(search.y, search.x);

		if(pix.val[0] != 255.0)
		{
			/*wydruki*/
			//std::cout<<search.x<<" "<<search.y<<endl;
			if(flag==0)
			{
				if(value==1)
					printf("SCIANA: NA PRAWO\n");
				else
					printf("SCIANA: NA LEWO\n");
			}
			if(flag==1)
			{
				if(value==1)
					printf("SCIANA: NA DOLE\n");
				else
					printf("SCIANA: NA GORZE\n");
			}

			/*koniec wydrukow*/

			return true;
		}

		/* rysowanie sciezki poszukiwan */
		//pix.val[0] = 0;
		//cvSet2D(dst,search.y,search.x,pix);
		//dst.at<int>(search) = pix[0];//powinno byc tylko pix!

		if(flag == 0)
			search.x = search.x + value;
		if(flag == 1)
			search.y = search.y + value;
	}
return false;
}
/**
 * find path in labirynth using labirynt_info_array
 */
bool TestComponent_Processor::find_path(int x, int y)
{
	Point start_point;
	start_point.x = x;
	start_point.y = y;
	labirynt_info_array[y][x].value = 0;

	set_neighbours(y,x);

	/*wydruk*/
	printf("JESTEM W: %d,%d\n",x,y);
	for(int i=0;i<LABIRYNT_SIZE_Y;i++)
	{
		for(int ii=0;ii<LABIRYNT_SIZE_X;ii++)
			printf("%d ",labirynt_info_array[i][ii].value);

		printf("\n");
	}
	printf("\n");
	/* * */

	Point end_point;
	end_point.x = LABIRYNT_SIZE_X-1;
	end_point.y = LABIRYNT_SIZE_Y-1;
	if(path_exists)
		set_path(end_point,start_point);

return path_exists;
}
/**
 * set numbers in labirynt_info_array
 */
void TestComponent_Processor::set_neighbours(int y, int x)
{
	/* exit exsists*/
	if((x == LABIRYNT_SIZE_X-1) && (y == LABIRYNT_SIZE_Y-1))
	{
		path_exists = true;
		return ;
	}

	/* zapisz liczby na sasiadach jezeli jeszcze nie odwiedzone (-1) lub odwiedzone po dluzszej sciezce*/

	if(labirynt_info_array[y][x].e_wall == false)
	{
		if((labirynt_info_array[y][x+1].value < 0) || (labirynt_info_array[y][x+1].value > labirynt_info_array[y][x].value))
			labirynt_info_array[y][x+1].value = labirynt_info_array[y][x].value + 1;
	}
	if(labirynt_info_array[y][x].s_wall == false)
	{
		if((labirynt_info_array[y+1][x].value < 0) || (labirynt_info_array[y+1][x].value > labirynt_info_array[y][x].value))
			labirynt_info_array[y+1][x].value = labirynt_info_array[y][x].value + 1;
	}
	if(labirynt_info_array[y][x].w_wall == false)
	{
		if((labirynt_info_array[y][x-1].value < 0) || (labirynt_info_array[y][x-1].value > labirynt_info_array[y][x].value))
			labirynt_info_array[y][x-1].value = labirynt_info_array[y][x].value + 1;
	}
	if(labirynt_info_array[y][x].n_wall == false)
	{
		if((labirynt_info_array[y-1][x].value < 0) || (labirynt_info_array[y-1][x].value > labirynt_info_array[y][x].value))
			labirynt_info_array[y-1][x].value = labirynt_info_array[y][x].value + 1;
	}

	/* poruszaj sie dalej */

	if((labirynt_info_array[y][x].e_wall == false) && (labirynt_info_array[y][x+1].value == labirynt_info_array[y][x].value+1))
		set_neighbours(y,x+1);

	if((labirynt_info_array[y][x].s_wall == false) && (labirynt_info_array[y+1][x].value == labirynt_info_array[y][x].value+1))
		set_neighbours(y+1,x);

	if((labirynt_info_array[y][x].w_wall == false) && (labirynt_info_array[y][x-1].value == labirynt_info_array[y][x].value+1))
		set_neighbours(y,x-1);

	if((labirynt_info_array[y][x].n_wall == false) && (labirynt_info_array[y-1][x].value == labirynt_info_array[y][x].value+1))
		set_neighbours(y-1,x);

return ;
}
/**
 * set path in "path" vector
 */
void TestComponent_Processor::set_path(Point p,Point s)
{
	path.push_back(p);

	if((p.x==s.x) && (p.y==s.y))
	{
		path_set = true;
		return ;
	}

	if((labirynt_info_array[p.y][p.x].n_wall == false) && (labirynt_info_array[p.y-1][p.x].value == labirynt_info_array[p.y][p.x].value-1))
	{
		p.y = p.y-1;
		set_path(p,s);
		if(path_set)
			return ;
	}
	if((labirynt_info_array[p.y][p.x].w_wall == false) && (labirynt_info_array[p.y][p.x-1].value == labirynt_info_array[p.y][p.x].value-1))
	{
		p.x = p.x-1;
		set_path(p,s);
		if(path_set)
			return ;

	}
	if((labirynt_info_array[p.y][p.x].s_wall == false) && (labirynt_info_array[p.y+1][p.x].value == labirynt_info_array[p.y][p.x].value-1))
	{
		p.y = p.y+1;
		set_path(p,s);
		if(path_set)
			return ;
	}
	if((labirynt_info_array[p.y][p.x].e_wall == false) && (labirynt_info_array[p.y][p.x+1].value == labirynt_info_array[p.y][p.x].value-1))
	{
		p.x = p.x+1;
		set_path(p,s);
		if(path_set)
			return ;
	}
return ;
}



}//: namespace TestComponent
}//: namespace Processors
