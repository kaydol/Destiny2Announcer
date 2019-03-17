
#include "pch.h"
#include <fstream>
#include <iostream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <String>
#include <filesystem>

#pragma comment(lib, "Winmm.lib")

using namespace cv;
using namespace std;
using namespace experimental;

void GetCVScreenCap(Mat& matBitmap, HBITMAP& hBitmap, Rect roi) {
	
	HDC hwindowDC = GetDC(NULL);
	HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	matBitmap.create(roi.height, roi.width, CV_8UC4);

	// Initialize a bitmap
	hBitmap = CreateCompatibleBitmap(hwindowDC, roi.width, roi.height);
	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = roi.width;
	// The negative height is required -- removing the inversion will make the image appear upside-down.
	bi.bmiHeader.biHeight = -roi.height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = 0;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	SelectObject(hwindowCompatibleDC, hBitmap);
	// Copy from the window device context to the bitmap device context
	// Use BitBlt to do a copy without any stretching -- the output is of the same dimensions as the target area.
	BitBlt(hwindowCompatibleDC, 0, 0, roi.width, roi.height, hwindowDC, roi.x, roi.y, SRCCOPY);
	// Copy into our own buffer as device-independent bitmap
	GetDIBits(hwindowCompatibleDC, hBitmap, 0, roi.height, matBitmap.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

	// Clean up memory to avoid leaks
	DeleteObject(hBitmap);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(NULL, hwindowDC);
};

bool fexists(const std::string& filename) {
	ifstream ifile(filename.c_str());
	return (bool)ifile;
}

float linearConversion(float minFrom, float maxFrom, float t, float minTo, float maxTo) {
	float progress = (t - minFrom) / (maxFrom - minFrom);
	return (1 - progress) * minTo + progress * maxTo;
}

class Event {

private:
	Mat tmpl;
	string snd = "";

public:
	Event() {}
	Event(const string templ, const string sound) {

		tmpl = imread(templ, IMREAD_GRAYSCALE);
		snd = sound;

		if (!fexists(snd))
			cout << "File "<< sound << " not found " << endl;
		if (!tmpl.data)
			cout << "File " << templ << " not found" << endl;
	}

	float findMatch(const Mat &canny) {
		
		if (!tmpl.data)
			return 0;

		// Match template
		Mat result;
		int result_cols = canny.cols - tmpl.cols + 1;
		int result_rows = canny.rows - tmpl.rows + 1;
		result.create(result_rows, result_cols, CV_32FC1);

		matchTemplate(canny, tmpl, result, TM_CCORR_NORMED);

		// Parse match output
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
		Point matchLoc = maxLoc;

		//cout << "Certainty: " << maxVal << endl;

		result.release();

		return maxVal;
	}

	void play() {
		if (fexists(snd))
			PlaySound(snd.c_str(), NULL, SND_FILENAME);
	}

};

void preprocessImage(const Mat &image, Mat &processed) {
	
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);
	
	Mat blurImage;
	blur(gray, blurImage, Size(3, 3));

	Mat thresh;
	threshold(blurImage, thresh, 127, 255, THRESH_BINARY);

	double r = (double)countNonZero(thresh) / (thresh.cols * thresh.rows);

	int threshold1 = linearConversion(0, 1, r, 233, 0); // 0
	int threshold2 = linearConversion(0, 1, r, 303, 0); // 160

	// binarize
	Canny(blurImage, processed, threshold1, threshold2, 3);
};


void debug() {

	string path = "D:/_screenshots/";

	Mat row;
	Mat holder;

	float scale = 1.0;
	int rowlim = 3;
	int count = 0;
	for (const auto & entry : filesystem::directory_iterator(path)) {

		cout << entry.path() << endl;

		Mat pic = imread(entry.path().string(), IMREAD_COLOR);
		pic.adjustROI(-700, -200, -30, -1400);
		Mat processed;
		preprocessImage(pic, processed);
		pic.release();

		imshow("Grid", processed);
		waitKey(0);

		resize(processed, processed, Size(), scale, scale);

		int border = 1;
		Scalar value(255, 255, 255);
		copyMakeBorder(processed, processed, border, border, border, border, BORDER_CONSTANT, value);

		++count;

		if (count % rowlim == 1) {
			processed.copyTo(row);
			continue;
		}

		Mat arr[] = { row , processed };
		hconcat(arr, 2, row);
		processed.release();

		if (count % rowlim == 0) {
			if (!holder.data)
				row.copyTo(holder);
			else {
				Mat arr[] = { holder , row };
				vconcat(arr, 2, holder);
				row.release();
			}
			row.release();
			continue;
		}

	}

	if (count % rowlim != 0) {	// fit the last not full row

		int picsInRow = count % rowlim;
		int w = row.cols / picsInRow;
		int h = row.rows;
		Mat whitepic(h, w, CV_8UC1, Scalar(255));

		while (count++ % rowlim != 0) {
			Mat arr1[] = { row , whitepic };
			hconcat(arr1, 2, row);
		}

		Mat arr2[] = { holder , row };
		vconcat(arr2, 2, holder);

		row.release();
		whitepic.release();
	}

	imshow("Grid", holder);
	waitKey(0);
};

int main()
{
	cout << "===============================================================================" << endl;
	cout << "                     Destiny 2 Announcer by kaydol (c) 2019" << endl;
	cout << "              Github: https://github.com/kaydol/Destiny2Announcer" << endl;
	cout << "===============================================================================" << endl;
	cout << "                       ----Read README for more info----" << endl << endl;
	cout << "Monitoring the left bottom corner of the screen..." << endl;

	list<Event> events = list<Event>();
	events.push_back(Event("./templates/nearby.png", "./sounds/nearby.wav"));
	events.push_back(Event("./templates/departing.png", "./sounds/departing.wav"));
	events.push_back(Event("./templates/dropped_plunder.png", "./sounds/dropped_plunder.wav"));
	
	// Crop screenshot
	Rect roi;
	roi.x = 30;
	roi.y = 700;
	roi.width = 580;
	roi.height = 324;

	Mat matBitmap;
	HBITMAP hBitmap;
	
	while (true) {

		GetCVScreenCap(matBitmap, hBitmap, roi);

		// Grayscaling, blurring, canny-ing
		Mat processed;
		preprocessImage(matBitmap, processed);

		Event ev;
		float maxConfidence = 0;
		for (Event e : events) {
			float conf = e.findMatch(processed);
			if (conf > maxConfidence) {
				maxConfidence = conf;
				ev = e;
			}
		}

		if (maxConfidence > 0.7) {
			ev.play();
			this_thread::sleep_for(chrono::milliseconds(2000));
		}

		DeleteObject(hBitmap);
		matBitmap.release();
		
		this_thread::sleep_for(chrono::milliseconds(1000));
	}

	return 0;
    
}


