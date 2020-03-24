#include "stdafx.h"
#include "helperFunctions.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;


int main(int argc, char** argv){
	//////////////////////////////////////////////BEGIN TASK 1///////////////////////////////////////////////////////////

	//***************************************INITIAL DECLARATIONS********************************************************

	//declare colours that will be used to draw contour around each egg
	Scalar whiteColor = Scalar(255, 255, 255);                          //white
	Scalar orangeColor = Scalar(0, 69, 255);							//orange
	Scalar greenColor = Scalar(0, 255, 0);								//green
	Scalar yellowColor = Scalar(0, 255, 255);							//yellow
	Scalar redColor = Scalar(0, 0, 255);								//red	
	Scalar blueColor = Scalar(255, 0, 0);								//blue

	string FINAL = "Egg Colour Classification";								//declare window to display the final image 
	namedWindow(FINAL, WINDOW_NORMAL);										//create window
	Mat hsv = Mat::zeros(2000, 3000, CV_8UC3);								//declare image class objects that will be used in processing

	//********************************************PREPROCESSING************************************************************

	Mat src = imread("C:/Users/maxmb/Desktop/Part IV Engineering/709/AssignmentHandoutImages/chocolate_eggs.jpg", IMREAD_COLOR); 	//load in initial image
	cvtColor(src, hsv, COLOR_BGR2HSV);																			//convert src to hsv

	//*************************************CREATE EGG MASKS***************************************************************
	//create a mask that captures each colour egg in source photo. 

	//Mat createMask(Mat hsvSource, double minH, double minS, double minV, double maxH, double maxS, double maxV)
	Mat converted_blue = createMask(hsv, 75, 0, 0, 125, 255, 255);
	Mat converted_red = createMask(hsv, 0, 100, 100, 5, 255, 255);
	Mat converted_green = createMask(hsv, 55, 100, 100, 80, 255, 255);
	Mat converted_yellow = createMask(hsv, 16, 100, 100, 30, 255, 255);
	Mat converted_orange = createMask(hsv, 11, 88, 100, 15, 255, 255);
	Mat converted_white = createMask(hsv, 0, 0, 100, 25, 150, 255);


	//****************************************PROCESS EGG MASKS**********************************************************

	//process corresponding egg masks into clean edges 
	//Mat imageProcessing(Mat mask, double lowerThresh, double upperThresh, int dilationSize, int erosionSize){
	converted_blue = imageProcessing(converted_blue, 100, 200, 10, 1);
	converted_red = imageProcessing(converted_red, 100, 200, 10, 1);
	converted_green = imageProcessing(converted_green, 100, 200, 10, 1);
	converted_white = imageProcessing(converted_white, 100, 200, 10, 1);
	converted_yellow = imageProcessing(converted_yellow, 100, 200, 10, 1);
	converted_orange = imageProcessing(converted_orange, 100, 200, 10, 1);

	//***************************************CONTOUR EGG MASKS**********************************************************
	//for each mask of corresponding egg colour, loop through and if the contour area is greater than a threshold value,
	// count that contour as an egg and draw it's contour on the orginal source image.

	//int eggCount = contourImage(Mat mask, int areaThreshold, Mat src, Scalar drawColour)
	int whiteCount = contourImage(converted_white, 6000, src, whiteColor);
	int redCount = contourImage(converted_red, 7000, src, redColor);
	int yellowCount = contourImage(converted_yellow, 4000, src, yellowColor);
	int greenCount = contourImage(converted_green, 4000, src, greenColor);
	int orangeCount = contourImage(converted_orange, 6000, src, orangeColor);
	int blueCount = contourImage(converted_blue, 3000, src, blueColor);

																					//write to console total egg counts
	cout << "There are " << blueCount << " blue eggs." << endl;
	cout << "There are " << greenCount << " green eggs." << endl;
	cout << "There are " << redCount << " red eggs." << endl;
	cout << "There are " << yellowCount << " yellow eggs." << endl;
	cout << "There are " << whiteCount << " white eggs." << endl;
	cout << "There are " << orangeCount << " orange eggs." << endl;


	imshow(FINAL, src);																//display original image with contours on it	
	imwrite("C:/Users/maxmb/Desktop/Part IV Engineering/709/AssignmentHandoutImages/output_images/eggs_marked.jpg", src); //Save image to drive
	waitKey(0);
	destroyAllWindows();

	//////////////////////////////////////////END OF TASK 1/////////////////////////////////////////////////////////////////
}