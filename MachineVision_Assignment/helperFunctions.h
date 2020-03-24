#pragma once

#include "stdafx.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
using namespace cv;
using namespace std;


//This function takes an hsv source image and a parameter of HSV lower and upper values. It returns a gray scale blurred mask
//of pixels that were in the HSV ranges input.

Mat createMask(Mat hsvSource, double minH, double minS, double minV, double maxH, double maxS, double maxV) {

	Mat mask = Mat::zeros(2000, 3000, CV_8UC3);																//declare Mat object to manipulate
	inRange(hsvSource, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), mask);							//find desried HSV range
	medianBlur(mask, mask, 15);																				//blur image

	return mask;
};

//This function takes a grey scale mask, a lower threshold and hupper threshold value for using a canny edge detector,
//and a dilation size and erosion size integer. It returns a grayscale clean edge of the oringial mask.

Mat imageProcessing(Mat mask, double lowerThresh, double upperThresh, int dilationSize, int erosionSize) {

	Mat element_d = getStructuringElement(2, Size(2 * dilationSize + 1, 2 * dilationSize + 1), Point(dilationSize, dilationSize)); //create dilation kernal size
	Mat element_e = getStructuringElement(2, Size(2 * erosionSize + 1, 2 * erosionSize + 1), Point(erosionSize, erosionSize));  //create erosion kernal size

	Canny(mask, mask, lowerThresh, upperThresh, 3);														// process edge of input mask
	dilate(mask, mask, element_d);																		//increase the white pixels so that any unconnected desired edges can connect
	erode(mask, mask, element_e);																		//erode edge to decrease edge thickness

	return mask;

};

//This function takes a mask of eggs as an input, an area threshold value, a source image to draw contours on and a contour drawing colour
//as inputs. It loops throu
int contourImage(Mat mask, int areaThreshold, Mat src, Scalar drawColour) {

	int eggCount = 0;													//declare counter variable to count eggs
	vector<vector<Point> > contours;									//declare vector to store contours of eggs
	findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	//find contours of eggs

	vector<vector<Point> > hull(contours.size());						//declare vector of vector of points for convex hulls

	for (int i = 0; i < contours.size(); i++) {
		if (contourArea(contours[i]) > areaThreshold) {					//if contour area is greater than threshold, count an egg
			convexHull(Mat(contours[i]), hull[i]);						//use convexHull of egg contours to draw eggs as provides a cleaner output
			drawContours(src, hull, i, drawColour, 3);
			eggCount++;
		}
	}
	return eggCount;
};

