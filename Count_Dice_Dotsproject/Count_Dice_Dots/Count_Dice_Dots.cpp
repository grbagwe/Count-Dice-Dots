// Count_Dice_Dots.cpp : Defines the entry point for the console application.
//

// openCV_Sample.cpp : Defines the entry point for the console application.
//
#include<opencv2/opencv.hpp>
#include<iostream>


using namespace std;
using namespace cv;
int main()
{
	Mat img = imread("dice6.png");                /* aquire the image */
	Mat img_gray,img_dialate,img_erode,img_bb;    /* declare the variables*/
	Mat img_temp,img_bw, edge_dice;
	Mat ROI_circles;
	RNG rng(12345);
	Mat A; //for 1D array
	Mat diceROI, diceROIer;
	int thickness = 5;
	int sum;
	Mat dice_point,dice_erode;
	int a[5];
	int sumd=0;
	ostringstream ss;
	string str_target = "C:\\Users\\grbag\\OneDrive\\Documents\\Count_Dice_Dotsproject\\Count_Dice_Dots\\out_dice6.png";
	/* display the orignal image*/
	// convert the image  to gray 
	imshow("image1", img);
	cvtColor(img, img_gray, cv::COLOR_RGB2GRAY);   /* RGB to Gray*/
	img_bw = img_gray > 220;                        /* Convert the image into binary form*/
	//imshow("image2", img_gray);


	/* Create elements for morphologival operations*/
	Mat elementdl = getStructuringElement(MORPH_RECT,Size(2 * 2+ 1, 2 * 2 + 1),Point(1, 1));   /* dilate*/
	Mat elementer = getStructuringElement(MORPH_RECT, Size(2 * 1 + 1, 2 * 1 + 1), Point(1, 1));   /* erode*/
	Mat element = getStructuringElement(MORPH_RECT, Size(2 * 1 + 1, 2 * 1 + 1), Point(1, 1));


	Mat img_floodfill = img_bw.clone();
	floodFill(img_floodfill, cv::Point(0, 0), Scalar(255));      /* seprate the foreground from background*/
	Mat img_floodfill_inv;
	bitwise_not(img_floodfill, img_floodfill_inv);

	//// Combine the two images to get the foreground.
	Mat img_out = (img_bw | img_floodfill_inv);

	Canny(img_out, edge_dice, 400, 400, 3);                /* Find the edges*/
	

	// apply the morphological operations to get clearer features
	dilate(edge_dice, img_dialate, elementdl, Point(-1, -1), 2, 1, 1);
	erode(img_dialate, img_erode, elementdl, Point(-1, -1), 2, 1, 1);
	
	//imshow("image2", img_floodfill);

	vector<vector<Point> > img_contors;
	// find the contors - the region of interest - dices in the image
	findContours(img_erode, img_contors, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); /*find the external contors */
	img_bb = img_dialate.clone();

	//imshow("image", img_erode);

	vector<vector<Point> > contours_poly(img_contors.size());
	vector<Rect> boundRect_img(img_contors.size());
	vector<Point2f>centers_img(img_contors.size());
	vector<float>radius_img(img_contors.size());
	//vector<Mat>
	for (size_t i = 0; i < img_contors.size(); i++)
	{
		approxPolyDP(img_contors[i], contours_poly[i], 3, true);  // Contours_poly is the poly bounding box on the image
		boundRect_img[i] = boundingRect(contours_poly[i]);    // the bounding boxes on the roi
	}

	for (size_t i = 0; i < 5 ; i++)
	{
		a[i] = 0;  // initialize an array to store the dice points
	}

	// count the dots 
	sum = 0;  // intialize the sum of the points on the dice

	for (size_t i = 0; i< img_contors.size(); i++)
	{
		
		Mat diceROI_bg = img_out(boundRect_img[i]); // dice background
		Mat diceROI_fg = img_bw(boundRect_img[i]); // dice foreground 
		// find the forground of the dice
		bitwise_xor(diceROI_bg, diceROI_fg, diceROI);

		Mat diceROIc = diceROI.clone();
		floodFill(diceROIc, cv::Point(0, 0), Scalar(255));
		Mat ff_diceROI_inv;
		bitwise_not(diceROIc, ff_diceROI_inv);
	
		erode(diceROI, diceROIer, element, Point(-1, -1), 2, 1, 1);
		Canny(diceROIer, ROI_circles, 400, 400, 3);                /* Find the edges of the points on the dice*/

		vector<vector<Point> > ROI_contors;
		findContours(ROI_circles, ROI_contors, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		vector<vector<Point> > ROI_poly(ROI_contors.size());
		vector<Rect> boundRect(ROI_contors.size());
		vector<Point2f>centers(ROI_contors.size());
		vector<float>radius(ROI_contors.size());
		sum = 0;
		for (size_t ii = 0; ii < ROI_contors.size(); ii++)
		{
			approxPolyDP(ROI_contors[ii], ROI_poly[ii], 3, true);
			boundRect[i] = boundingRect(ROI_poly[ii]);    
			minEnclosingCircle(ROI_poly[ii], centers[ii], radius[i]);  // bounding circles on the points 
			sum = sum + 1; 

		}
		a[i]=sum;
		//cout << sum;  

		Mat drawing1 = Mat::zeros(ROI_circles.size(), CV_8UC3);

		for (size_t i = 0; i< ROI_contors.size(); i++)
		{
			Scalar color = Scalar(0, 255, 0);
			drawContours(drawing1, ROI_poly, (int)i, color, thickness);
			circle(drawing1, centers[i], (int)radius[i], color, 2);
		}
		

	}
	
	// image drawing
	Rect rect;
	for (size_t i = 0; i< img_contors.size(); i++)
	{ 
		rect = boundRect_img[i];
		ostringstream ss1;
		ss1 << a[i];
		string ans1 = ss1.str();
		Scalar color = Scalar(0, 255, 0);
		Point center = Point(rect.x + (rect.width ), rect.y + (rect.height));
		drawContours(img, contours_poly, (int)i, color, thickness);
		putText(img,ans1 , center, CV_FONT_HERSHEY_SIMPLEX, 0.8, cvScalar(0, 255, 0));
		diceROI = img(boundRect_img[i]);
	}


	bitwise_xor(img_out,img_bw, dice_erode);
	erode(dice_erode,dice_point, element, Point(-1, -1), 2, 1, 1);
	vector<vector<Point> > dicepoints_contours;
    vector<Vec4i> hierarchy;
	findContours(dice_point, dicepoints_contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat drawing2 = Mat::zeros(dice_point.size(), CV_8UC3);
	for (size_t i = 0; i< dicepoints_contours.size(); i++)
	{
		Scalar color = Scalar(0,255,0);
		drawContours(img, dicepoints_contours, (int)i, color, 2, 8, hierarchy, 0, Point());
	}

	for (size_t i = 0; i < 5; i++)
	{

		sumd = sumd + a[i];
	}
	ss << "Sum : " << sumd;
	string ans = ss.str();

	Scalar color = Scalar(0, 255, 0);
	putText(img, ans, cvPoint(30, 30),
		FONT_HERSHEY_COMPLEX, 0.8, cvScalar(0, 255, 0));

	imshow("Output", img);
	imwrite(str_target, img);
	
	waitKey(0);
	return 0;

}
