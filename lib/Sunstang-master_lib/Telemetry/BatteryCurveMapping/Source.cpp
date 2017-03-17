/*
Name: Dustin Dobransky
ID:   250575030
Date: 26/09/2014

Description:
Main file.  Makes calls to the Drawer class, which draws the Bresenhams lines.
*/


#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

static const char wndname[] = "Battery Curve";

Scalar thresh;

bool within(int target, int val)
{
    int magicThreshold = 15;
    return (val < target + magicThreshold &&
            val > target - magicThreshold);
}

bool isCurve(Mat image, int x, int y)
{
    Vec3b px = image.at<Vec3b>(y, x);
    return (within(thresh[0], px[0]) &&
            within(thresh[1], px[1]) &&
            within(thresh[2], px[2]));
}

int min(int a, int b)
{
    return (a<b ? a : b);
}

int main() {

    ofstream out;
    out.open("mapping,txt");

    Mat threshIm = imread("threshold.png", CV_LOAD_IMAGE_COLOR);

    if (!threshIm.data)                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }

    thresh = mean(threshIm);
    cout << thresh << endl;

    Mat image = imread("BatteryCurve.png", CV_LOAD_IMAGE_COLOR);   // Read the file

    if (!image.data)                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }

    imshow(wndname, image);

    int maxLineWidth = 6; // px

    int width = image.cols;
    int height = image.rows;
    int xOffset = 5;
    int yOffset = 5;

    double voltageMin = 2.5;
    double voltageMax = 4.1;
    int mAhMin = 0;
    int mAhMax = 10000;

    int numSteps = 1000;

    double lastVoltage = 0;

    for (int i = 0; i < numSteps; i++) {
        int x = min(width-1, ((double)i/numSteps) * width + xOffset);

        int y0 = 0, y1 = 0;

        int y = height - yOffset;
        while (y > 0 && !isCurve(image, x, y))
        {
            y--;
        }

        y0 = y; // bottom of line

        while (y > 0 && y1-y < maxLineWidth && isCurve(image, x, y)) {
            y--;
        }

        y1 = y; // top of line

        y = (y0+y1)/2; // middle of line (average of top/bottom)

        printf("y0: %d, y1: %d\n", y0, y1);

        int mAh = (((double)i)/numSteps) * (mAhMax - mAhMin) + mAhMin;
        cout << (height - (double)y) / height << endl;
        cout << voltageMax - voltageMin << endl;
        cout << (height - (double)y) / height * (voltageMax - voltageMin) << endl;
        double voltage = ((height - (double)y) / height) * (voltageMax - voltageMin) + voltageMin;

        // use last recorded value if the line is missed
        if (voltage >= voltageMax) {
            voltage = lastVoltage;
        }

        lastVoltage = voltage;

        //printf("%d\t%.4f\n", mAh, voltage);

        out << mAh << "\t" << voltage << endl;
    }

    out.close();

    waitKey();
}
