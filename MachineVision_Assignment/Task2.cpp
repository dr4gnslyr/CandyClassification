#include "stdafx.h"
#include "helperFunctions.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
using namespace cv;
using namespace std;


int main(int argc, char** argv) {
	/////////////////////////////////////////BEGIN TASK 2///////////////////////////////////////////////////////////////

	//////////////////////////////////////INITIAL DECLARATIONS/////////////////////////////////////////////////////////
	
	string FINAL = "Eggs that aren't broken";											//declare window to show image at the end of task
	namedWindow(FINAL, WINDOW_NORMAL);													//create window

	Mat hsv = Mat::zeros(2000, 3000, CV_8UC3), converted = Mat::zeros(2000, 3000, CV_8UC3); //declare Mat objects for image manipulation
	Mat src_2 = imread("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/broken_eggs.jpg", IMREAD_COLOR); //read in source image
	
	
	//////////////////////////////////////IMAGE PREPROCESSING//////////////////////////////////////////////////////////

	cvtColor(src_2, hsv, COLOR_BGR2HSV);												//convert source to HSV format to aid next tasks	
	converted = createMask(hsv, 0, 100, 100, 12, 255, 255);								//create a mask of the source image for the red coloured pixels (red eggs)
	converted = imageProcessing(converted, 100, 200, 2, 1);                            //canny, dilate and erode

	////////////////////////////////////PROCESSING//////////////////////////////////////////////////////////////////////
	
	vector<vector<Point> > contours;													//declare vector of vector of points to store contours of the red eggs 
	findContours(converted, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);				//find contours of red eggs

	vector<vector<Point> > hull(contours.size());										//declare a hull vector to pass the contours into 

	int eggCount = 0;																	//declare a counting variable for valid eggs

	//loop through the contours and convex hulls and check that they exceed the contour area value and hull szie value to be considered a valid egg
	for (int i = 0; i < contours.size(); i++) {
		convexHull(Mat(contours[i]), hull[i]);											//create a vector of convex hulls for the contours that were found
		if ((contourArea(contours[i]) > 9000) && hull[i].size() > 45) {	
			eggCount++;
			drawContours(src_2, hull, i,Scalar(255,255,255), 3);								//draw on source image
		}
	}

		
	cout << "Total whole eggs present = " << eggCount << endl;												//write to console
	imwrite("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/output_images/whole_eggs_marked.jpg", src_2);  //save new image
	imshow(FINAL, src_2);
	waitKey(0);
	destroyAllWindows();

	//////////////////////////////////////////////END TASK 2///////////////////////////////////////////////////////////////////
}