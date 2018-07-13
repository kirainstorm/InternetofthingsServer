// FaceRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>//frontal_face_detector.h
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
//using namespace dlib;
using namespace std;


#ifdef _DEBUG
#pragma comment( lib, "dlib19.14.0_debug_64bit_msvc1900.lib" )
#pragma comment( lib, "opencv_world342d.lib" )
#else
#pragma comment( lib, "dlib19.14.0_release_64bit_msvc1900.lib" )
#pragma comment( lib, "opencv_world342.lib" )
#endif


//头文件
#include<opencv2/objdetect/objdetect.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;

//人脸检测的类
CascadeClassifier faceCascade;

int main()
{
	faceCascade.load("C:/output32/bin64/data/haarcascades/haarcascade_frontalface_alt2.xml");   //加载分类器，注意文件路径

	Mat img = imread("e:/assd.png");
	Mat imgGray;
	vector<Rect> faces;

	if (img.empty())
	{
		return 1;
	}

	if (img.channels() == 3)
	{
		cvtColor(img, imgGray, CV_RGB2GRAY);
	}
	else
	{
		imgGray = img;
	}

	faceCascade.detectMultiScale(imgGray, faces, 1.2, 5, CV_HAAR_DO_CANNY_PRUNING, Size(30, 30));   //检测人脸

	if (faces.size() > 0)
	{
		for (int i = 0; i < faces.size(); i++)
		{
			rectangle(img, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height),
				Scalar(0, 255, 0), 1, 8);    //框出人脸位置
		}
	}

	imshow("FacesOfPrettyGirl", img);

	waitKey(0);




// 	char photoomg[1024] = {"e:/assd.png"};
// 	try
// 	{
// // 		if (argc == 1)
// // 		{
// // 			cout << "Give some image files as arguments to this program." << endl;
// // 			return 0;
// // 		}
// 
// 		dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
// 		dlib::image_window win;
// 
// 		// Loop over all the images provided on the command line.
// 		//for (int i = 1; i < argc; ++i)
// 		{
// 			cout << "processing image " << photoomg << endl;
// 			dlib::array2d<dlib::rgb_pixel> img;
// 			dlib::load_image(img, photoomg);
// 			// Make the image bigger by a factor of two.  This is useful since
// 			// the face detector looks for faces that are about 80 by 80 pixels
// 			// or larger.  Therefore, if you want to find faces that are smaller
// 			// than that then you need to upsample the image as we do here by
// 			// calling pyramid_up().  So this will allow it to detect faces that
// 			// are at least 40 by 40 pixels in size.  We could call pyramid_up()
// 			// again to find even smaller faces, but note that every time we
// 			// upsample the image we make the detector run slower since it must
// 			// process a larger image.
// 			dlib::pyramid_up(img);
// 
// 			// Now tell the face detector to give us a list of bounding boxes
// 			// around all the faces it can find in the image.
// 			std::vector<dlib::rectangle> dets = detector(img);
// 
// 			cout << "Number of faces detected: " << dets.size() << endl;
// 			// Now we show the image on the screen and the face detections as
// 			// red overlay boxes.
// 			win.clear_overlay();
// 			win.set_image(img);
// 			win.add_overlay(dets, dlib::rgb_pixel(255, 0, 0));
// 
// 			cout << "Hit enter to process the next image..." << endl;
// 			cin.get();
// 		}
// 	}
// 	catch (exception& e)
// 	{
// 		cout << "\nexception thrown!" << endl;
// 		cout << e.what() << endl;
// 	}
    return 0;
}

