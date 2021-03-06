#include "stdafx.h"
#include "helperFunctions.h"
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
using namespace cv;
using namespace std;


//This function takes an hsv source image and a parameter of HSV lower and upper values. It returns a gray scale blurred mask
//of pixels that were in the HSV ranges input.


int main(int argc, char** argv)
{
	int task;
	task = 3;

	if (task == 4) {
		string FINAL = "Result";
		namedWindow(FINAL, WINDOW_NORMAL);

		Mat src_4;
		Mat hsv;
		Mat mask;
		Mat chocolate = Mat::zeros(2000, 3000, CV_8UC3);
		Mat converted = Mat::zeros(2000, 3000, CV_8UC3);
		src_4 = imread("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/chocolate_gummy.jpg", IMREAD_COLOR);


		cvtColor(src_4, hsv, COLOR_BGR2HSV);
		converted = hsv;



		//create mask of chocolate pieces in source image
		chocolate = createMask(hsv, 6.5, 100, 100, 15, 255, 255);

		Canny(chocolate, chocolate, 100, 200, 3);
		dilate(chocolate, chocolate, getStructuringElement(2, Size(5, 5), Point(2, 2)));
		erode(chocolate, chocolate, getStructuringElement(2, Size(3, 3), Point(2, 2)));

		imshow(FINAL, chocolate);

		//contour the chocolate pieces
		vector<vector<Point> > chocContours;
		findContours(chocolate, chocContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//convert the HSV image to BGR so that contour chocolate can be blocked out using fillConvexPoly
		cvtColor(converted, converted, COLOR_HSV2BGR);

		int chocPieces = 0;

		for (int i = 0; i < chocContours.size(); i++) {
			if (contourArea(chocContours[i]) > 13000) {
				chocPieces++;
				fillConvexPoly(converted, chocContours[i], Scalar(62, 75, 91));
				//fill in the chocolate pieces with the background colour so that the block lollies of similar colour do not get confused later on
			}
		}


		//convert back to HSV again
		cvtColor(converted, converted, COLOR_BGR2HSV);

		//Create mask for heart lollies
		Mat hearts = createMask(converted, 2, 100, 100, 6, 255, 255);
		Canny(hearts, hearts, 100, 200, 3);
		dilate(hearts, hearts, getStructuringElement(2, Size(5, 5), Point(2, 2)));
		erode(hearts, hearts, getStructuringElement(2, Size(3, 3), Point(2, 2)));

		vector<vector<Point> > heartsContours;
		int heartPieces = 0;
		findContours(hearts, heartsContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		cvtColor(converted, converted, COLOR_HSV2BGR);
		for (int i = 0; i < heartsContours.size(); i++) {
			//cout << contourArea(heartsContours[i]) << endl;
			if (contourArea(heartsContours[i]) > 14000) {
				heartPieces++;
				drawContours(src_4, heartsContours, i, (255, 255, 0), 3);
				fillConvexPoly(converted, heartsContours[i], Scalar(62, 75, 91));
				//fill in heart lollies ith background colour so that small red block lollies not confused later on
			}

		}


		//convert bak to HSV
		cvtColor(converted, converted, COLOR_BGR2HSV);
		Mat gummyEgg = createMask(converted, 20, 58.65, 100, 23, 255, 255);

		Canny(gummyEgg, gummyEgg, 100, 200, 3); //edge detecting
		dilate(gummyEgg, gummyEgg, getStructuringElement(2, Size(5, 5), Point(2, 2))); //connect edges
		erode(gummyEgg, gummyEgg, getStructuringElement(2, Size(3, 3), Point(2, 2))); //decrease size of edges

		vector<vector<Point> > eggContours;
		int eggPieces = 0;

		//find egg contours
		findContours(gummyEgg, eggContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//check against area threshold
		for (int i = 0; i < eggContours.size(); i++) {
			if (contourArea(eggContours[i]) > 10200) {
				eggPieces++;
			}
		}

		//gummy rings are multicoloured so declare different masks for each
		Mat rings = Mat::zeros(2000, 3000, CV_8UC3);
		Mat rings_yellow = Mat::zeros(2000, 3000, CV_8UC3);
		Mat rings_red_low = Mat::zeros(2000, 3000, CV_8UC3);
		Mat rings_red_high = Mat::zeros(2000, 3000, CV_8UC3);

		inRange(converted, Scalar(17.5, 212, 100), Scalar(25, 255, 170), rings_yellow);
		inRange(converted, Scalar(2, 100, 102), Scalar(4, 255, 153), rings_red_low);
		inRange(converted, Scalar(178, 100, 100), Scalar(180, 255, 255), rings_red_high);

		//combine the rng lollie masks together
		bitwise_or(rings_red_low, rings_red_high, rings);
		bitwise_or(rings, rings_yellow, rings);
		medianBlur(rings, rings, 3);

		//detect edge
		Canny(rings, rings, 100, 200, 3);

		//connect edges
		dilate(rings, rings, getStructuringElement(2, Size(19, 19), Point(9, 9)));

		vector<vector<Point> > ringsContours;
		int ringPieces = 0;

		//find contours of ring lollies
		findContours(rings, ringsContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//create vector of hulls of contours to use 
		vector<vector<Point> > hull(ringsContours.size());
		for (int i = 0; i < ringsContours.size(); i++) {
			convexHull(Mat(ringsContours[i]), hull[i]);
		}

		//check against threshold contour area size and threshold  hull size 
		for (int i = 0; i < hull.size(); i++) {
			if ((contourArea(ringsContours[i]) > 7000) && (hull[i]).size() > 36) {
				ringPieces++;
			}
		}



		//gummy bears of different colours, declare mask for each
		Mat gummyBears = Mat::zeros(2000, 3000, CV_8UC3);
		Mat gummyBears_yellow = Mat::zeros(2000, 3000, CV_8UC3);
		Mat gummyBears_green = Mat::zeros(2000, 3000, CV_8UC3);
		Mat gummyBears_orange = Mat::zeros(2000, 3000, CV_8UC3);
		inRange(converted, Scalar(20, 170.85, 122.4), Scalar(23, 221.85, 170.85), gummyBears_yellow);
		inRange(converted, Scalar(24, 102, 40.8), Scalar(35, 180, 80), gummyBears_green);
		inRange(converted, Scalar(14, 168.3, 114.75), Scalar(15, 232.05, 155.55), gummyBears_orange);

		//combine gummy bear masks
		bitwise_or(gummyBears_green, gummyBears_yellow, gummyBears);
		bitwise_or(gummyBears_orange, gummyBears, gummyBears);

		//detect edges
		Canny(gummyBears, gummyBears, 230, 255, 3);

		//connect edges
		dilate(gummyBears, gummyBears, getStructuringElement(2, Size(11, 11), Point(5, 5)));

		//decrease width of edges
		erode(gummyBears, gummyBears, getStructuringElement(2, Size(3, 3), Point(2, 2)));

		//find gummy bear contours
		vector<vector<Point> > gummyBearContours;
		int gummyBearPieces = 0;
		findContours(gummyBears, gummyBearContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//check hummy bear contours are within threshold range
		for (int i = 0; i < gummyBearContours.size(); i++) {
			if (contourArea(gummyBearContours[i]) > 10000 && (contourArea(gummyBearContours[i]) < 12000)) { // && (contourArea(gummyBearContours[i]) < 10000) //(contourArea(gummyBearContours[i]) > 3000)
				gummyBearPieces++;
			}
		}

		//declare masks for different coloured block lollies
		Mat blockGreen = Mat::zeros(2000, 3000, CV_8UC3);
		Mat blockBrown = Mat::zeros(2000, 3000, CV_8UC3);
		Mat blockRed = Mat::zeros(2000, 3000, CV_8UC3);
		Mat blockYellow = Mat::zeros(2000, 3000, CV_8UC3);
		Mat blocks = Mat::zeros(2000, 3000, CV_8UC3);
		inRange(converted, Scalar(23, 176, 199), Scalar(25, 235, 214.2), blockYellow);
		inRange(converted, Scalar(33, 102, 114.75), Scalar(39, 178.5, 137.7), blockGreen);
		inRange(converted, Scalar(1.5, 117.5, 183.6), Scalar(7, 175.95, 234.6), blockRed);
		inRange(converted, Scalar(9.5, 117.3, 114.75), Scalar(13.5, 170.85, 204), blockBrown);

		//combine masks for different colours
		bitwise_or(blockYellow, blockGreen, blocks);
		bitwise_or(blockRed, blocks, blocks);
		bitwise_or(blockBrown, blocks, blocks);

		//connect edges then decrease edge size
		dilate(blocks, blocks, getStructuringElement(2, Size(7, 7), Point(3, 3)));
		erode(blocks, blocks, getStructuringElement(2, Size(3, 3), Point(2, 2)));

		//find block contours
		vector<vector<Point> > blocksContours;
		int blocksPieces = 0;
		findContours(blocks, blocksContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//find convexhull of contours
		vector<vector<Point> > hullBlock(blocksContours.size());
		for (int i = 0; i < blocksContours.size(); i++) {
			convexHull(Mat(blocksContours[i]), hullBlock[i]);
		}

		//check hull size against threshold size
		for (int i = 0; i < blocksContours.size(); i++) {
			if (hullBlock[i].size() > 20) {
				blocksPieces++;
			}
		}


		//coke bottle, multi coloured so declare appropriate masks
		Mat coke = Mat::zeros(2000, 3000, CV_8UC3);
		Mat cokeLight = Mat::zeros(2000, 3000, CV_8UC3);
		Mat cokeDark = Mat::zeros(2000, 3000, CV_8UC3);
		inRange(converted, Scalar(16.5, 107.1, 104.55), Scalar(20.5, 186.15, 160.65), cokeLight);
		inRange(converted, Scalar(4.5, 191.25, 53.55), Scalar(7.5, 242.25, 79.05), cokeDark);

		//combine masks
		bitwise_or(cokeLight, cokeDark, coke);

		//detect edge
		Canny(coke, coke, 230, 255, 3);

		//connect edges
		dilate(coke, coke, getStructuringElement(2, Size(7, 7), Point(3, 3)));

		//decrease size of edges
		erode(coke, coke, getStructuringElement(2, Size(3, 3), Point(2, 2)));

		//find contours
		vector<vector<Point> > cokeContours;
		int cokePieces = 0;
		findContours(coke, cokeContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//check contour area against threshold area
		for (int i = 0; i < cokeContours.size(); i++) {
			if (contourArea(cokeContours[i]) > 5000) {
				cokePieces++;
			}
		}
		//write to console
		cout << "There are " << cokePieces << " coke bottles." << endl;
		cout << "There are " << gummyBearPieces << " gummy bears." << endl;
		cout << "There are " << heartPieces << " gummy hearts." << endl;
		cout << "There are " << chocPieces << " pieces of cholate." << endl;
		cout << "There are " << blocksPieces << " pieces of block lollies." << endl;
		cout << "There are " << eggPieces << " gummy egg lollies." << endl;
		cout << "There are " << ringPieces << " gummy ring lollies." << endl;

		waitKey(0);
	}

	if (task == 3) {

		string FINAL = "Result";
		namedWindow(FINAL, WINDOW_NORMAL);

		Mat src_3;
		Mat hsv;
		Mat mask;
		Mat converted = Mat::zeros(2000, 3000, CV_8UC3);;
		src_3 = imread("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/rectangle_arrangements.jpg", IMREAD_COLOR);
		//src_3.convertTo(src_3,-1, 1, 100);
		cvtColor(src_3, hsv, COLOR_BGR2HSV);

		//convert image to HSV format
		inRange(hsv, Scalar(10, 100, 100), Scalar(15, 255, 255), mask);
		cvtColor(hsv, hsv, COLOR_BGR2GRAY);

		//Threshold image to form chocolate silhouttes 
		double thresh = 120;
		double maxValue = 255;
		threshold(hsv, converted, thresh, maxValue, THRESH_BINARY);

		//blur to smooth image
		medianBlur(converted, converted, 15);

		//detect chcolate edges
		Canny(converted, converted, 100, 200, 3);


		//dilate and erode to connect edges
		dilate(converted, converted, getStructuringElement(2, Size(5, 5), Point(2, 2)));
		erode(converted, converted, getStructuringElement(2, Size(3, 3), Point(2, 2)));

		//find chocolate contours
		vector<vector<Point> > contours;
		findContours(converted, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		// get the moments of the contours  and then get the centroid of figures so that text can be plotted in centre on each chocolate piece
		vector<Moments> mu(contours.size());
		vector<Point2f> mc(contours.size());

		//find min area bounded rectangles of contours
		vector<vector<Point> > contours_poly(contours.size());
		vector<RotatedRect> boundRect(contours.size());
		RotatedRect temp;

		//caluclate single cube of chocolate's area
		double cubeArea = contourArea(contours[0]); //23202.5
		vector<double> cubesPerBlock(contours.size());

		//declare variables as counters for differnet types of chocolate configurations
		int one = 0;			//Counter for 1X1 configuration
		int two = 0;			//Counter for 2X1 configuration
		int oneTwo = 0;			//Counter for 1,2 configuration
		int oneOneOne = 0;		//Counter for 1,1,1 configuration
		int oneOne = 0;			//Counter for 1,1 configuration
		int twoTwo = 0;			//Counter for 2X2 configuration
		int oneThree = 0;		//Counter for 1,3 configuration

								//declare double paramaters found for a single cube of chocolate - these were determined experimentally by printing chocolate cube sizes to the console
		double areaChocolateCube = 23202.5;										 //area of a single piece of chocolate
		double cubeWidth = 180;													 //lower width of a single piece of chocolate
		double cubeHeight = 120;												 //lower height of a single piece of chocolate
		double widthRange = 14;												  	//range of max Width - min Width of a single cube
		double heightRange = 19;												//range of max Height - min Height of a single cube
		for (int i = 0; i < contours.size(); i++)
		{
			mu[i] = moments(contours[i], false);
			mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);

			approxPolyDP(contours[i], contours_poly[i], 3, true);
			boundRect[i] = minAreaRect(Mat(contours_poly[i]));


			//determine number of cubes of chocalte per piece by dividing the contour area's by the the area of one chocolate cube and rounding the answer to the nearest integer.
			cubesPerBlock[i] = round(contourArea(contours[i]) / areaChocolateCube);

			//*******************Logic behind the following if else statement**************************************************************************************************
			//for cubesPerBlock = 1, 2, 3 and 4, determine the chocolate configuration by checking the height and width of the rectangle that bounds the chocolate configuration.
			//Remember that boundRect[i].size.height and boundRect[i].size.height represent the height and width of the rectangles that bound the pieces of chocolate.By convention, I have
			//considered the smaller edge of a single chocolate piece to be the height, and the longer edge to be the width. Because the minAreaRect function does not always 
			//categorise height and width of the rectangle by this convention, we must account for this is our filtering conditions to come.

			if (cubesPerBlock[i] == 1) {
				//If there is only one piece of chocolate it has to be 1X1
				one++;
				putText(src_3, "1X1", mc[i], FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 3, 8);
			}
			else if (cubesPerBlock[i] == 2) {
				//If there are two pieces of chocolate, the orientations could be (1,1) or 2X1. Check for the 1,1 configuration first. If not 1,1, then configuration must be 2X1.
				if (boundRect[i].size.height < cubeHeight + heightRange || boundRect[i].size.width < cubeHeight + heightRange) {
					oneOne++;
					putText(src_3, "1,1", mc[i], FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 3, 8);;
				}
				else {
					two++;
					putText(src_3, "2X1", mc[i], FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 3, 8);
				}
			}
			else if (cubesPerBlock[i] == 3) {
				//If there are three pieces of chocolate, the orientations could be (1,1,1) or (1,2). Check for the 1,1,1 configuration first. If not 1,1,1 then configuration must be (1,2).
				if (boundRect[i].size.height < cubeHeight + heightRange || boundRect[i].size.width < cubeHeight + heightRange) {
					oneOneOne++;
					putText(src_3, "1,1,1", mc[i], FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 3, 8);
				}
				else {
					oneTwo++;
					putText(src_3, "1, 2", mc[i], FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 3, 8);
				}
			}
			else if (cubesPerBlock[i] == 4) {
				//If there are four pieces of chocolate, the orientations could be (2X2) or (1,3). Check for the (2X2) configuration first. If not (2X) then configuration must be (1,3).
				if (boundRect[i].size.height < 2 * cubeHeight + heightRange || boundRect[i].size.width < 2 * cubeHeight + heightRange) {
					twoTwo++;
					putText(src_3, "2 x 2", mc[i], FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 3, 8);
				}
				else {
					oneThree++;
					putText(src_3, "1, 3", mc[i], FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 3, 8);
				}
			}
		}

		//Write to console the number of each type of configurations of chocolate present
		cout << "one: " << one << endl << "One One: " << oneOne << endl << " Two: " << two << endl << " One Two: " << oneTwo << endl << " One One One: " << oneOneOne << endl << " Two Two " << twoTwo << endl << " One Three: " << oneThree << endl;

		//Display the final image with the labelled configurations
		imshow(FINAL, src_3);
		imwrite("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/output_images/chocolate_marked.jpg", src_3);
		waitKey(0);
	}



	if (task == 1) {
		//declare window to show image at the end of task
		string FINAL = "Result";
		namedWindow(FINAL, WINDOW_NORMAL);

		Mat hsv;
		Mat converted = Mat::zeros(2000, 3000, CV_8UC3);;

		//read in source image
		Mat src_2 = imread("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/broken_eggs.jpg", IMREAD_COLOR);

		//convert source to HSV format
		cvtColor(src_2, hsv, COLOR_BGR2HSV);

		//create a mask of the source image for the red coloured pixels
		converted = createMask(hsv, 0, 100, 100, 12, 255, 255);

		//create an edge of the mask
		Canny(converted, converted, 100, 200, 3);

		//dilate to connect edges
		dilate(converted, converted, getStructuringElement(2, Size(5, 5), Point(2, 2)));

		//erode to decrease size of edge
		erode(converted, converted, getStructuringElement(2, Size(3, 3), Point(2, 2)));

		//declare vector to store contours of the red eggs and then use find contour to find the red pieces of egg
		vector<vector<Point> > contours;
		findContours(converted, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//declare a hull vector to pass the contours into 
		vector<vector<Point> > hull(contours.size());

		int eggCount = 0;

		//create a vector of convex hulls for the contours that were found
		for (int i = 0; i < contours.size(); i++) {
			convexHull(Mat(contours[i]), hull[i]);
		}

		//loop through the contours and hulls and check that they exceed the contour area value and hull szie value to
		//be considered a valid egg
		for (int i = 0; i < hull.size(); i++) {
			if ((contourArea(contours[i]) > 9000) && (hull[i]).size() > 45) {
				eggCount++;
				drawContours(src_2, hull, i, (255, 255, 255), 3);
			}
		}

		cout << "Total whole eggs present = " << eggCount << endl;
		imwrite("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/output_images/whole_eggs_marked.jpg", src_2);
		imshow(FINAL, src_2);
		waitKey(0);
		destroyAllWindows();

	}


	if (task == 0) { //Run the processing required to acheive task 1 of the machine vision assignment 

					 //declare window to display the final image 
		string FINAL = "Result";
		namedWindow(FINAL, WINDOW_NORMAL);

		//declare image class objects that will be used in processing
		Mat hsv = Mat::zeros(2000, 3000, CV_8UC3);

		//load in initial image
		Mat src = imread("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/chocolate_eggs.jpg", IMREAD_COLOR);

		//convert src to hsv
		cvtColor(src, hsv, COLOR_BGR2HSV);

		//create a mask that captures each colour egg in source photo. 
		//Mat createMask(Mat hsvSource, double minH, double minS, double minV, double maxH, double maxS, double maxV)
		Mat converted_blue = createMask(hsv, 75, 0, 0, 125, 255, 255);
		Mat converted_red = createMask(hsv, 0, 100, 100, 5, 255, 255);
		Mat converted_green = createMask(hsv, 55, 100, 100, 80, 255, 255);
		Mat converted_yellow = createMask(hsv, 16, 100, 100, 30, 255, 255);
		Mat converted_orange = createMask(hsv, 11, 88, 100, 15, 255, 255);
		Mat converted_white = createMask(hsv, 0, 0, 100, 25, 150, 255);

		//process corresponding egg masks into clean edges 
		//Mat imageProcessing(Mat mask, double lowerThresh, double upperThresh, int dilationSize, int erosionSize){
		converted_blue = imageProcessing(converted_blue, 100, 200, 10, 1);
		converted_red = imageProcessing(converted_red, 100, 200, 10, 1);
		converted_green = imageProcessing(converted_green, 100, 200, 10, 1);
		converted_white = imageProcessing(converted_white, 100, 200, 10, 1);
		converted_yellow = imageProcessing(converted_yellow, 100, 200, 10, 1);
		converted_orange = imageProcessing(converted_orange, 100, 200, 10, 1);

		//declare colours that will be used to draw contour around each egg
		Scalar whiteColor = Scalar(255, 255, 255);
		Scalar orangeColor = Scalar(0, 69, 255);
		Scalar greenColor = Scalar(0, 255, 0);
		Scalar yellowColor = Scalar(0, 255, 255);
		Scalar redColor = Scalar(0, 0, 255);
		Scalar blueColor = Scalar(255, 0, 0);

		//for each mask of corresponding egg colour, loop through and if the contour area is greater than a threshold value,
		// count that contour as an egg and draw it's contour on the orginal source image.

		//int eggCount = contourImage(Mat mask, int areaThreshold, Mat &src, Scalar drawColour)

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

		//display original image with contours on it
		imshow(FINAL, src);
		imwrite("C:/Users/maxmb/Desktop/709/AssignmentHandoutImages/output_images/eggs_marked.jpg", src);
		waitKey(0);
	}


}


