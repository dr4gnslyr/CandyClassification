#include "stdafx.h"
#include "helperFunctions.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	//////////////////////////////////////////////////BEGIN TASK 4//////////////////////////////////////////////////////

	//*note: This task has been coded to classify all types of lollies in the image

	//***************************************INITIAL DECLARATIONS*******************************************************
	Mat hsv = Mat::zeros(2000, 3000, CV_8UC3), mask = Mat::zeros(2000, 3000, CV_8UC3);				//declare Mat objects for manipulation
	Mat converted = Mat::zeros(2000, 3000, CV_8UC3), chocolate = Mat::zeros(2000, 3000, CV_8UC3);	//declare Mat objects for manipulation
	Mat src_4 = imread("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/chocolate_gummy.jpg", IMREAD_COLOR); //read in source

																					//declare lolly counter integers
	int chocPieces = 0;																//chocolate counter
	int heartPieces = 0;															//heart counter				
	int eggPieces = 0;																//gummy egg counter
	int ringPieces = 0;																//gummy ring counter
	int blocksPieces = 0;															//block lolly counter
	int cokePieces = 0;																//coke lolly counter
	int gummyBearPieces = 0;														//gummy bear counter

	//*****************************************PREPROCESSING*************************************************************
	cvtColor(src_4, hsv, COLOR_BGR2HSV);											//convert source to HSV
	converted = hsv;																//make a copy of HSV

	//****************************************CHOCOLATE******************************************************************
	chocolate = createMask(hsv, 6.5, 100, 100, 15, 255, 255);						//create mask of chocolate pieces in source image
	chocolate = imageProcessing(chocolate, 100, 200, 2, 1);							//process chocolate edges: canny, dilate, erode, blur

	vector<vector<Point> > chocContours;											//declare vector of vector of points for contours
	findContours(chocolate, chocContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);		//contour the chocolate pieces

	cvtColor(converted, converted, COLOR_HSV2BGR);		//convert the HSV image to BGR so that chocolate pieces can be blocked out using fillConvexPoly
				
	for (int i = 0; i < chocContours.size(); i++) {
		if (contourArea(chocContours[i]) > 13000) {
			chocPieces++;

			//uncomment following code to classify lollies not specified in the brief
			//fill in the chocolate pieces with the background colour so that the block lollies of similar Hue do not get confused when using inRange later on
			//fillConvexPoly(converted, chocContours[i], Scalar(62, 75, 91));		
		}
	}
	cvtColor(converted, converted, COLOR_BGR2HSV);									//convert back to HSV again

	//************************************GUMMY EGGS***************************************************************************************
	Mat gummyEgg = createMask(converted, 20, 58.65, 100, 23, 255, 255);			//create mask of gummy egg, inRange, medianBlur
	gummyEgg = imageProcessing(gummyEgg, 100,200,2,1);							//Process edge --> Canny, erode, dilate
	
	vector<vector<Point> > eggContours;											//declare vector of vector of points for egg contour

	findContours(gummyEgg, eggContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	//find egg contours

	for (int i = 0; i < eggContours.size(); i++) {								//check against area threshold
		if (contourArea(eggContours[i]) > 10200) {
			eggPieces++;
		}
	}


	//************************************************BLOCK LOLLIES*************************************************************************************

	//declare Mat object for masks for different coloured block lollies
	Mat blockGreen = Mat::zeros(2000, 3000, CV_8UC3);												//green block lolly mat object
	Mat blockBrown = Mat::zeros(2000, 3000, CV_8UC3);												//brown block lolly mat object	
	Mat blockRed = Mat::zeros(2000, 3000, CV_8UC3);													//red block lolly mat object
	Mat blockYellow = Mat::zeros(2000, 3000, CV_8UC3);												//yellow block lolly mat object
	Mat blocks = Mat::zeros(2000, 3000, CV_8UC3);													//overall block lolly mat object

	inRange(converted, Scalar(23, 176, 199), Scalar(25, 235, 214.2), blockYellow);					//find yellow blocks
	inRange(converted, Scalar(33, 102, 114.75), Scalar(39, 178.5, 137.7), blockGreen);				//find green blocks
	inRange(converted, Scalar(1.5, 117.5, 183.6), Scalar(7, 175.95, 234.6), blockRed);				//find red blocks
	inRange(converted, Scalar(9.5, 117.3, 114.75), Scalar(13.5, 170.85, 204), blockBrown);			//find brown blocks

	bitwise_or(blockYellow, blockGreen, blocks);													//combine masks for different colours
	bitwise_or(blockRed, blocks, blocks);															//combine masks for different colours
	bitwise_or(blockBrown, blocks, blocks);															//combine masks for different colours

	dilate(blocks, blocks, getStructuringElement(2, Size(7, 7), Point(3, 3)));						//connect edges
	erode(blocks, blocks, getStructuringElement(2, Size(3, 3), Point(2, 2)));						//decrease edge size

	vector<vector<Point> > blocksContours;															//declare vector of vector of points for block contours
	findContours(blocks, blocksContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);						//find block contours
	vector<vector<Point> > hullBlock(blocksContours.size());										//declare vector of vector of points for convex hull								

	blocksPieces = 0;
	for (int i = 0; i < blocksContours.size(); i++) {
		convexHull(Mat(blocksContours[i]), hullBlock[i]);											//create convex hulls of contours
		if ((hullBlock[i].size() > 25 ) && (hullBlock[i].size() < 37)) {							//check hull size against threshold size range
			blocksPieces++;
			drawContours(src_4, hullBlock, i, 3, 8);
		}
	}

	//write to console
	cout << "There are " << blocksPieces << " pieces of block lollies." << endl;
	cout << "There are " << chocPieces << " pieces of chocolate." << endl;
	cout << "There are " << eggPieces << " gummy egg lollies." << endl;
																		//Uncomment following section to classify all other lollies
	/*
	//***************************************HEARTS***************************************************************************

	Mat hearts = createMask(converted, 2, 100, 100, 6, 255, 255);				//Create mask for heart lollies-->InRange,MedianBlur
	hearts = imageProcessing(hearts, 100, 200, 2, 1);							//Process heart edge --> Canny, erode, dilate
	

	vector<vector<Point> > heartsContours;										//declare vector of vector of points to store contours
	findContours(hearts, heartsContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	//find heart contours
	cvtColor(converted, converted, COLOR_HSV2BGR);								//convert to from HSV to BGR so fillConvexPolly can block in hearts that are found
	
	for (int i = 0; i < heartsContours.size(); i++) {						
		if (contourArea(heartsContours[i]) > 14000) {							//if contour area is greater than threshold area consider valid heart
			heartPieces++;		
			fillConvexPoly(converted, heartsContours[i], Scalar(62, 75, 91));  //fill in heart lollies ith background colour so that small red block lollies not confused later on
		}

	}
	cvtColor(converted, converted, COLOR_BGR2HSV);								//convert bak to HSV


	//**********************************GUMMY RINGS**********************************************************************************
	//gummy rings are multicoloured so declare different Mat objects for masks for each
	Mat rings = Mat::zeros(2000, 3000, CV_8UC3);											//Mat object for final mask
	Mat rings_yellow = Mat::zeros(2000, 3000, CV_8UC3);										//Mat object for yellow sections
	Mat rings_red_low = Mat::zeros(2000, 3000, CV_8UC3);									//Mat object for light red sections
	Mat rings_red_high = Mat::zeros(2000, 3000, CV_8UC3);									//Mat object for dark red sections
			
	inRange(converted, Scalar(17.5, 212, 100), Scalar(25, 255, 170), rings_yellow);			//find yellow sections
	inRange(converted, Scalar(2, 100, 102), Scalar(4, 255, 153), rings_red_low);			//find light red sections
	inRange(converted, Scalar(178, 100, 100), Scalar(180, 255, 255), rings_red_high);		//find dark red sections

	bitwise_or(rings_red_low, rings_red_high, rings);										//combine the ring lollie masks together
	bitwise_or(rings, rings_yellow, rings);													//combine the ring lollie masks together
	medianBlur(rings, rings, 3);															//blur mask
	Canny(rings, rings, 100, 200, 3);														//detect edge
	dilate(rings, rings, getStructuringElement(2, Size(19, 19), Point(9, 9)));				//connect edges

	vector<vector<Point> > ringsContours;													//declare vector of vector of points for rings
	findContours(rings, ringsContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);					//find contours of ring lollies
	vector<vector<Point> > hull(ringsContours.size());										//create vector of hulls of contours to use 
	
	for (int i = 0; i < ringsContours.size(); i++) {
		convexHull(Mat(ringsContours[i]), hull[i]);
		if ((contourArea(ringsContours[i]) > 7000) && (hull[i]).size() > 36) {				//check against threshold contour area size and threshold  hull size 
			ringPieces++;
		}
	}

	//***************************************GUMMY BEARS*********************************************************
	//gummy bears of different colours, declare mask for each
	Mat gummyBears = Mat::zeros(2000, 3000, CV_8UC3);											//final gummy bear mask
	Mat gummyBears_yellow = Mat::zeros(2000, 3000, CV_8UC3);									//yellow gummy bear mask
	Mat gummyBears_green = Mat::zeros(2000, 3000, CV_8UC3);										//green gummy bear mask
	Mat gummyBears_orange = Mat::zeros(2000, 3000, CV_8UC3);									//orange gummy bear mask

	inRange(converted, Scalar(20, 170.85, 122.4), Scalar(23, 221.85, 170.85), gummyBears_yellow);	//find yellow sections
	inRange(converted, Scalar(24, 102, 40.8), Scalar(35, 180, 80), gummyBears_green);				//find green sections
	inRange(converted, Scalar(14, 168.3, 114.75), Scalar(15, 232.05, 155.55), gummyBears_orange);	//find orange sections

	bitwise_or(gummyBears_green, gummyBears_yellow, gummyBears);									//combine gummy bear masks
	bitwise_or(gummyBears_orange, gummyBears, gummyBears);											//combine gummy bear masks

	gummyBears = imageProcessing(gummyBears, 230, 250, 5, 1);										//process gummy bear edges

	
	vector<vector<Point> > gummyBearContours;														//declare vector of vector of points for contours					
	findContours(gummyBears, gummyBearContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);				//find gummy bear contours

	for (int i = 0; i < gummyBearContours.size(); i++) {											//check gummy bear contour areas are within threshold range area
		if (contourArea(gummyBearContours[i]) > 10000 && (contourArea(gummyBearContours[i]) < 12000)) {
			gummyBearPieces++;
		}
	}

	//**************************************************COKE BOTTLES************************************************************************************

	//coke bottle, multi coloured so declare appropriate masks
	Mat coke = Mat::zeros(2000, 3000, CV_8UC3);														//overall mat object for mask
	Mat cokeLight = Mat::zeros(2000, 3000, CV_8UC3);												//mat object for light mask
	Mat cokeDark = Mat::zeros(2000, 3000, CV_8UC3);													//mat object for dark mask

	inRange(converted, Scalar(16.5, 107.1, 104.55), Scalar(20.5, 186.15, 160.65), cokeLight);		//find light sections
	inRange(converted, Scalar(4.5, 191.25, 53.55), Scalar(7.5, 242.25, 79.05), cokeDark);			//find dark sections
	bitwise_or(cokeLight, cokeDark, coke);															//combine masks
	coke = imageProcessing(coke, 230, 255, 3, 1);													//process edges

	vector<vector<Point> > cokeContours;															//declare vector of vector for contour points
	findContours(coke, cokeContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);		                    //find contours

	for (int i = 0; i < cokeContours.size(); i++) {
		if (contourArea(cokeContours[i]) > 5000) {													//check contour area against threshold area
			cokePieces++;
		}
	}
																								//write to console a summary of the lollies that have been classfied

	cout << "There are " << cokePieces << " coke bottles." << endl;
	cout << "There are " << gummyBearPieces << " gummy bears." << endl;
	cout << "There are " << heartPieces << " gummy hearts." << endl;
	cout << "There are " << ringPieces << " gummy ring lollies." << endl;
	*/

}