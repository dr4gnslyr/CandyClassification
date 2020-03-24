#include "stdafx.h"
#include "helperFunctions.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	//////////////////////////////////////////////BEGIN TASK 3/////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////INITIAL DECLARATIONS///////////////////////////////////////////////////////////////////
	string FINAL = "Labelled chocolate";
	namedWindow(FINAL, WINDOW_NORMAL);

	Mat hsv = Mat::zeros(2000, 3000, CV_8UC3), converted = Mat::zeros(2000, 3000, CV_8UC3);                          //declare Mat objects to use for processing
	Mat src_3 = imread("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/rectangle_arrangements.jpg", IMREAD_COLOR); //read in source image

	///////////////////////////////////////PREPROCESSING OF SOURCE IMAGE/////////////////////////////////////////////////////////////////
	cvtColor(src_3, hsv, COLOR_BGR2HSV);														//convert from BGR to HSV
	cvtColor(hsv, converted, COLOR_BGR2GRAY);													//convert from HSV to GRAY to manipulate choclates into form that is easy to threshold 
																			
	double thresh = 120;																		//declare threshold value
	double maxValue = 255;																		//declare threshold capping value
	threshold(converted, converted, thresh, maxValue, THRESH_BINARY);						    //Threshold image to form chocolate silhouttes 
	medianBlur(converted, converted, 15);														//blur to smooth image
	Canny(converted, converted, 100, 200, 3);													//detect chcolate edges
	dilate(converted, converted, getStructuringElement(2, Size(5, 5), Point(2, 2)));			//dilate to connect edges
	erode(converted, converted, getStructuringElement(2, Size(3, 3), Point(2, 2)));				//erode to decrease sze of edges

	////////////////////////////////////BEGIN PROCESSING IMAGE//////////////////////////////////////////////////////////////////////////////
	vector<vector<Point> > contours;															//declare vector of vectors of points for chocolate contours
	findContours(converted, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);						//find chocolate contours

	// get the moments of the contours  and then get the centroid of figures so that text can be plotted in centre on each chocolate piece
	vector<Moments> moment(contours.size());														//declare vector of moments for contours
	vector<Point2f> centroid(contours.size());													    //declare vector of Point2f for contour centroid positions
	vector<RotatedRect> boundRect(contours.size());										     		//declare Vector of Rotated Rectangles for minAreaRects that bound chocolate pieces 

	//*****************************Chocolate Paramaters****************************************************************************************
	//The following parameters were calculated by analysing the contour areas of each chololate piece, and the edge widths and hieghts. It is 
	//assumed that this would be part of a normal callibration process of machine process in a chocolate processing line.
															
	double areaChocolateCube = 23202.5;										//The average contour area of a single chocolate square
	double cubeWidth = 180;													//lower width of a single piece of chocolate
	double cubeHeight = 120;												//lower height of a single piece of chocolate
	double widthRange = 14;												  	//difference of max Width minus min Width of a single cube
	double heightRange = 19;												//difference of max Height minus min Height of a single cube

	//***************************CHOCOLATE COUNTING VARIABLE DECLARATIONS*****************************************************************
	vector<int> cubesPerBlock(contours.size());								//vector of ints to count the number of individual cubes of chocolate per piece

	//declare  counters for differnet types of chocolate configurations
	int one = 0;															//Counter for 1X1 configuration
	int two = 0;															//Counter for 2X1 configuration
	int oneTwo = 0;															//Counter for 1,2 configuration
	int oneOneOne = 0;														//Counter for 1,1,1 configuration
	int oneOne = 0;															//Counter for 1,1 configuration
	int twoTwo = 0;															//Counter for 2X2 configuration
	int oneThree = 0;														//Counter for 1,3 configuration


	//***************************FINAL IMAGE PROCESSING BY CLASSIFICATION******************************************************************

	for (int i = 0; i < contours.size(); i++)
	{
		moment[i] = moments(contours[i], false);													//find the moments of the contours
		centroid[i] = Point2f(moment[i].m10 / moment[i].m00, moment[i].m01 / moment[i].m00);		//find the centroids of the contours, these will be used to plot text in centre of each piece
		boundRect[i] = minAreaRect(contours[i]);													//find minimum bounding rectangle of chocolate contours

		//determine number of cubes of chocalte per piece by dividing the contour area's by the the area of one chocolate cube and rounding the answer to the nearest integer.
		cubesPerBlock[i] = int(round(contourArea(contours[i]) / areaChocolateCube));

		//*******************Logic behind the following if else statement**************************************************************************************************

		//for cubesPerBlock = 1, 2, 3 and 4, determine the chocolate configuration by checking the height and width of the rectangle that bounds the chocolate configuration.
		//Remember that boundRect[i].size.height and boundRect[i].size.height represent the height and width of the rectangles that bound the pieces of chocolate.By convention, I have
		//considered the smaller edge of a single chocolate piece to be the height, and the longer edge to be the width. Because the minAreaRect function does not always 
		//categorise height and width of the rectangle by this convention, we must account for this is our filtering conditions to come.

		if (cubesPerBlock[i] == 1) {										
			//If there is only one piece of chocolate it has to be 1X1	

			one++;
			putText(src_3, "1X1", centroid[i], FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 3, 8);
		}
		else if (cubesPerBlock[i] == 2) {									
			//If there are two pieces of chocolate, the orientations could be (1,1) or 2X1. Check for the 1,1 configuration first. If not 1,1, then configuration must be 2X1.

			if (boundRect[i].size.height < cubeHeight + heightRange || boundRect[i].size.width < cubeHeight + heightRange) {
				oneOne++;
				putText(src_3, "1,1", centroid[i], FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 3, 8);;
			}
			else {
				two++;
				putText(src_3, "2X1", centroid[i], FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 3, 8);
			}
		}
		else if (cubesPerBlock[i] == 3) {									
			//If there are three pieces of chocolate, the orientations could be (1,1,1) or (1,2). Check for the 1,1,1 configuration first. If not 1,1,1 then configuration must be (1,2).
			
			if (boundRect[i].size.height < cubeHeight + heightRange || boundRect[i].size.width < cubeHeight + heightRange) {
				oneOneOne++;
				putText(src_3, "1,1,1", centroid[i], FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 3, 8);
			}
			else {
				oneTwo++;
				putText(src_3, "1, 2", centroid[i], FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 3, 8);
			}
		}
		else if (cubesPerBlock[i] == 4) {		
			//If there are four pieces of chocolate, the orientations could be (2X2) or (1,3). Check for the (2X2) configuration first. If not (2X) then configuration must be (1,3).

			if (boundRect[i].size.height < 2 * cubeHeight + heightRange || boundRect[i].size.width < 2 * cubeHeight + heightRange) {
				twoTwo++;
				putText(src_3, "2 x 2", centroid[i], FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 3, 8);
			}
			else {
				oneThree++;
				putText(src_3, "1, 3", centroid[i], FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 3, 8);
			}
		}
	}

	/////////////////////////////////////////WRITE TO CONSOLE/////////////////////////////////////////////////////////////////////

	cout << "The total count of chocolate configurations is as follows: " << endl;
	cout << "Total Pieces with configuration 1X1 is equal to:  " << one << endl;
	cout << "Total Pieces with configuration 1,1 is equal to:  " << oneOne << endl;
	cout << "Total Pieces with configuration 2X1 is equal to:  " << two << endl;
	cout << "Total Pieces with configuration 1,2 is equal to:  " << oneTwo << endl;
	cout << "Total Pieces with configuration 1,1,1 is equal to:  " << oneOneOne << endl;
	cout << "Total Pieces with configuration 2X2 is equal to:  " << twoTwo << endl;
	cout << "Total Pieces with configuration 1,3 is equal to:  " << oneThree << endl;

	imshow(FINAL, src_3);								    		//output display of choclate with labelled configurations
	waitKey(0);                                                    //wait for user to press key on window to exit
	destroyAllWindows();										   //destroy all windows
	imwrite("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/output_images/chocolate_marked.jpg", src_3); //save final image
	
	//*****************************************END OF TASK TASK 3*****************************************************************************
}