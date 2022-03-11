#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <regex>

using namespace cv;
using namespace std;

void imageProcessing(string input);     // Image processing to extract strings form car plates  
void printCarNumber();                  // Print car number
string getCarNumber(string text);       // Use regular expression to get car number
char* UTF8ToANSI(const char* pszCode);  // To prevent korean broken

int main()
{
    imageProcessing("carImage/9.jpg");
    printCarNumber();

    //for (int i = 1; i < 11; i++) {    // All images in carImage will be processed
    //    char buf[256];
    //    sprintf_s(buf, "carImage/%d.jpg", i);
    //    imageProcessing(buf);
    //    printCarNumber();

    //    Mat image = imread("carImage/temp.jpg");
    //    sprintf_s(buf, "carImage/%d-1.jpg", i);
    //    imwrite(buf, image);
    //}

    return 0;
}

void imageProcessing(string input) {
    Mat image, image2, image3, image4, drawing;
    Rect rect, temp_rect;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    double ratio, delta_x, delta_y, gradient;  // Variables for 'Snake' algorithm
    int select, plate_width, plate_height, count, friend_count = 0, refinery_count = 0;
    int original_width, original_height;
    
    original_width = image.cols;
    original_height = image.rows;
    
    image = imread(input);
    imshow("Original", image);

    image.copyTo(image2);
    image.copyTo(image3);
    image.copyTo(image4);

    // Image processing for contours
    cvtColor(image2, image2, COLOR_BGR2GRAY);
    imshow("Original->Gray", image2);
    GaussianBlur(image2, image2, Size(5,5), 0);
    imshow("Gray->GausianBlur", image2);
    Canny(image2, image2, 100, 300, 3);
    imshow("GausinaBlur->Canny", image2);

    // Finding contours
    findContours(image2, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<Rect> boundRect2(contours.size());

    // Bind rectangle to every rectangle.
    for (int i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
    }

    drawing = Mat::zeros(image2.size(), CV_8UC3);

    for (int i = 0; i < contours.size(); i++) {

        ratio = (double)boundRect[i].height / boundRect[i].width;

        // Filtering rectangles height/width ratio, and size.
        if ((ratio <= 2.5) && (ratio >= 0.5) && (boundRect[i].area() <= 700) && (boundRect[i].area() >= 100)) {

            drawContours(drawing, contours, i, Scalar(0, 255, 255), 1, 8, hierarchy, 0, Point());
            rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 1, 8, 0);

            // Include only suitable rectangles.
            boundRect2[refinery_count] = boundRect[i];
            refinery_count += 1;
        }
    }

    boundRect2.resize(refinery_count);  //  Resize refinery rectangle array.

    imshow("Contours&Rectangles", drawing);

    //  Bubble Sort accordance with X-coordinate.
    for (int i = 0; i < boundRect2.size(); i++) {
        for (int j = 0; j < (boundRect2.size() - i - 1); j++) {
            if (boundRect2[j].tl().x > boundRect2[j + 1].tl().x) {
                temp_rect = boundRect2[j];
                boundRect2[j] = boundRect2[j + 1];
                boundRect2[j + 1] = temp_rect;
            }
        }
    }


    for (int i = 0; i < boundRect2.size(); i++) {

        rectangle(image3, boundRect2[i].tl(), boundRect2[i].br(), Scalar(0, 255, 0), 1, 8, 0);

        count = 0;

        //  Snake moves to right, for eating his freind.
        for (int j = i + 1; j < boundRect2.size(); j++) {

            delta_x = abs(boundRect2[j].tl().x - boundRect2[i].tl().x);

            if (delta_x > 150)  //  Can't eat snake friend too far ^-^.
                break;

            delta_y = abs(boundRect2[j].tl().y - boundRect2[i].tl().y);


            //  If delta length is 0, it causes a divide-by-zero error.
            if (delta_x == 0) {
                delta_x = 1;
            }

            if (delta_y == 0) {
                delta_y = 1;
            }

            gradient = delta_y / delta_x;  //  Get gradient.
            cout << gradient << endl;

            if (gradient < 0.25) {  //  Can eat friends only on straight line.
                count += 1;
            }
        }

        //  Find the most full snake.
        if (count > friend_count) {
            select = i;  //  Save most full snake number (select is the first car number)
            friend_count = count;  //  Renewal number of friends hunting.
            rectangle(image3, boundRect2[select].tl(), boundRect2[select].br(), Scalar(255, 0, 255), 1, 8, 0);
        }
    }

    // I know the first location of the car plate, so i can get numbers and letters on car plate
    vector<Rect> carNumber; // Space for real car numbers and letter
    count = 1;

    carNumber.push_back(boundRect2[select]);
    rectangle(image4, boundRect2[select].tl(), boundRect2[select].br(), Scalar(0, 255, 0), 1, 8, 0);

    for (int i = 0; i < boundRect2.size(); i++) {
        if (boundRect2[select].tl().x > boundRect2[i].tl().x)   // The rest of the car plate numbers are on the right side of the first number
            continue;

        delta_x = abs(boundRect2[select].tl().x - boundRect2[i].tl().x);

        if (delta_x > 50)   // Car numbers are close to each other
            continue;

        delta_y = abs(boundRect2[select].tl().y - boundRect2[i].tl().y);

        if (delta_x == 0) {
            delta_x = 1;
        }

        if (delta_y == 0) {
            delta_y = 1;
        }

        gradient = delta_y / delta_x;  //  Get gradient.
        cout << gradient << endl;

        if (gradient < 0.25) {  
            select = i;
            carNumber.push_back(boundRect2[i]);
            rectangle(image4, boundRect2[i].tl(), boundRect2[i].br(), Scalar(0, 255, 0), 1, 8, 0);
            count += 1;
        }
    }

    imshow("RectanglesOnPlate", image4);
    
    // Image processing is performed to increase the recognition rate of tesseract-OCR
    // The first is to rotate the tilted car plate straight
    Mat cropped_image;
    image.copyTo(cropped_image);
    Point center1 = (carNumber[0].tl() + carNumber[0].br()) * 0.5;  // Center of the first number
    Point center2 = (carNumber[carNumber.size()-1].tl() + carNumber[carNumber.size() - 1].br()) * 0.5;  // Center of the last number
    int plate_center_x = (int)(center1.x + center2.x) * 0.5;    // X-coordinate at the Center of car plate
    int plate_center_y = (int)(center1.y + center2.y) * 0.5;    // Y-coordinate at the Center of car plate

    // To calculate the height
    int sum_height=0;
    for (int i = 0; i < carNumber.size(); i++)
        sum_height += carNumber[i].height;

    plate_width = (-center1.x + center2.x + carNumber[carNumber.size() - 1].width) * 1.05;  // Car plate width with some paddings
    plate_height = (int)(sum_height / carNumber.size()) * 1.2;  // Car plate height with some paddings

    delta_x = center1.x - center2.x;
    delta_y = center1.y - center2.y;

    // Roatate car plate
    double angle_degree = (atan(delta_y / delta_x))*(double)(180/3.141592);
    
    Mat rotation_matrix = getRotationMatrix2D(Point(plate_center_x, plate_center_y), angle_degree, 1.0);
    warpAffine(cropped_image, cropped_image, rotation_matrix, Size(original_width, original_height));
    imshow("Rotated", cropped_image);

    getRectSubPix(image, Size(plate_width, plate_height), Point(plate_center_x, plate_center_y), cropped_image, -1);
    //imshow("Cropped", cropped_image);

    cvtColor(cropped_image, cropped_image, COLOR_BGR2GRAY);
    //imshow("CroppedGray", cropped_image);

    GaussianBlur(cropped_image, cropped_image, Size(5, 5), 0);
    //imshow("GausianBlur", cropped_image);

    adaptiveThreshold(cropped_image, cropped_image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 19, 9);
    //imshow("Threshold", cropped_image);

    //threshold(cropped_image, cropped_image, 127, 255, 0);
    //imshow("Threshold", cropped_image);

    copyMakeBorder(cropped_image, cropped_image, 10, 10, 10, 10, BORDER_CONSTANT, Scalar(0, 0, 0)); // Padding for recognition rate
    //imshow("Padded", cropped_image);

    imwrite("carImage/temp.JPG", cropped_image);  // Save the result
}

void printCarNumber() {
    Mat image;
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\tesseract-OCR\\tessdata", "kor3",tesseract::OEM_TESSERACT_ONLY)) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Set page segmentation mode to PSM_SINGLE_LINE(7), it assumes there is only one line
    api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);

    // Open car plate image
    image = imread("carImage/temp.JPG");
    resize(image, image, cv::Size(image.cols * 2, image.rows * 2), 0, 0, 1);
    imshow("EnlargedCarPlate", image);
    waitKey(0);

    // Open input image with leptonica library
    Pix* carNumber = pixRead("carImage/temp.jpg");
    api->SetImage(carNumber);
    // Get OCR result
    string outText = api->GetUTF8Text();
    string text = UTF8ToANSI(outText.c_str());
    text = getCarNumber(text);
    cout << "carNumber : " << text << endl;

    // Destroy used object and release memory
    api->End();
    delete api;
    pixDestroy(&carNumber);
}

string getCarNumber(string text) {
    int i = 0;

    //cout << test << '\n';

    // Extract "12°¡3456" or "123°¡4567"
    regex re("\\d{2,3}\\W{2}\\s{0,}\\d{4}");
    smatch match;
    if (regex_search(text, match, re)) {
        return match.str();
    }
    else {
        return "0";
    }
}

char* UTF8ToANSI(const char* pszCode) // For korean
{
    BSTR    bstrWide;
    char* pszAnsi;
    int     nLength;

    // Get nLength of the Wide Char buffer
    nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1,
        NULL, NULL);
    bstrWide = SysAllocStringLen(NULL, nLength);

    // Change UTF-8 to Unicode (UTF-16)
    MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

    // Get nLength of the multi byte buffer
    nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
    pszAnsi = new char[nLength];

    // Change from unicode to mult byte
    WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
    SysFreeString(bstrWide);

    return pszAnsi;
}