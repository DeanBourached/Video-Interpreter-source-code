#include "video_interpreter.h"
#include <QtWidgets/QApplication>

void imageAnalysis(Mat frame, bool firstAnalysisPhase);

int imageDataBuffer[12];
int userSpeedChoice;
bool firstAnalysisPhase;

static vector<Rect> faces;
static vector<Rect> eyes;
static vector<Rect> smile;
static vector<Rect> bodies;

CascadeClassifier face_cascade, eye_cascade, smile_cascade, body_cascade;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Video_Interpreter w;
	w.show();

	//(width,height)
	w.setFixedSize(940, 575);

	while (1) { QApplication::processEvents(); if (w.cont)break; }
	w.setEnabled();

	VideoCapture cap(0);

	while (1)
	{
		Mat frame;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			break;
		}
	
		w.displayImage(frame);

		QApplication::processEvents();
		if (!w.cont)
		{
			break;
		}
	}

	face_cascade.load("haarcascade_frontalface_alt.xml");
	eye_cascade.load("haarcascade_eye.xml");
	smile_cascade.load("haarcascade_smile.xml");
	body_cascade.load("haarcascade_fullbody.xml");
	
	while (w.recordLoadPic == 2)
	{
		w.image = true;
		w.tidyUpHelpWindows = true;
		
		cap.~VideoCapture();
		string path = w.ImageLoader();
		if (path != "")
		{
			w.loopCounter++;
			Mat pic = imread(path);
			w.displayImage(pic);
			while (1) { QApplication::processEvents(); if (!w.cont)break; }
			if (w.loopCounter == 1)
				w.analysisWindow("(image analysis summary)\n---->");
			string digit = to_string(w.loopCounter);
			digit += "/";
			QString num = QString::fromStdString(digit);
			w.analysisWindow(num);
			imageAnalysis(pic, true);
			w.topLevelAnalysis(imageDataBuffer, true, true);
			for (int i = 0; i < 12; i++) {
				imageDataBuffer[i] = 0;
			}
		}
		else
		{
			string message = "No Image Has been selected...\nPlease choose an image to upload to the Image Interpreter!";
			QString toDisplay = QString::fromStdString(message);
			QMessageBox msgBox;
			msgBox.setWindowTitle("Image Interpreter");
			msgBox.setText(toDisplay);
			msgBox.exec();
		}
		while (1) { QApplication::processEvents(); if (w.cont)break; }	

		if (w.recordLoadPic != 2)
			exit(0);
	}

	VideoCapture cap2;

	//Loading...
	string toBeLoaded = "";
	while (w.recordLoadPic == 1 && toBeLoaded == "")
	{
		cap.~VideoCapture();
		toBeLoaded = w.loader();
		if (toBeLoaded != "")
		{
			cap2.open(toBeLoaded);
		}		
		else
		{
			string message = "No Video Has been selected...\nPlease choose a video to upload to Video Interpreter!";
			QString toDisplay = QString::fromStdString(message);
			QMessageBox msgBox;
			msgBox.setWindowTitle("Video Interpreter");
			msgBox.setText(toDisplay);
			msgBox.exec();
		}		
	}

	if (w.recordLoadPic == 0)
	{
		double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
		double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

		Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

		VideoWriter oVideoWriter("video.avi", CV_FOURCC('D', 'I', 'V', '3'), 15, frameSize, true);
		w.recordAnalysePrompts();

		while (1)
		{
			Mat frame;

			bool bSuccess = cap.read(frame); // read a new frame from video

			if (!bSuccess) //if not success, break loop
			{
				break;
			}

			oVideoWriter.write(frame);

			w.displayImage(frame);

			QApplication::processEvents();
			if (w.cont)
			{
				break;
			}
		}
		oVideoWriter.release();
	}	
	cap.~VideoCapture();
	w.buttonsDisplay();

	while (1) { QApplication::processEvents(); if (w.analyse)break; }


	if (w.recordLoadPic == 0)
	{
		cap2.open("Video.avi"); // open the video file for reading
	}
	
	w.recordAnalysePrompts();

	w.output->setText("");
	w.analysisWindow("ANALYSIS WINDOW\n_________________________");

	int frameFrequency = 0;
	bool videoFirstIteration = true;
	
	firstAnalysisPhase = true;
	w.cont = false;
	while (1)
	{
		userSpeedChoice = w.spinbox1->value();

		Mat frame;

		bool bSuccess = cap2.read(frame); // read a new frame from video
		w.end(videoFirstIteration, bSuccess);
		videoFirstIteration = false;

		cap2.set(CV_CAP_PROP_POS_FRAMES, frameFrequency);
		
		if (bSuccess && !w.image && w.recordLoadPic != 2) 
		{
			frameFrequency++;
		
			if (frameFrequency % userSpeedChoice == 0)
			{
				imageAnalysis(frame, firstAnalysisPhase);

				for (int i = 3; i < 12; i++) {
					imageDataBuffer[i] = imageDataBuffer[i] / 2;
				}

				if(firstAnalysisPhase)
				w.topLevelAnalysis(imageDataBuffer, firstAnalysisPhase, false);

				for (int i = 3; i < 12; i++) {
					imageDataBuffer[i] = 0;
				}

				if (firstAnalysisPhase)
					firstAnalysisPhase = false;
			}
			//NEXT
			if (frameFrequency % (userSpeedChoice*2) == 0)
			{
				for (int i = 0; i < 3; i++) {
					imageDataBuffer[i] = imageDataBuffer[i] / 2;
				}
							
				w.topLevelAnalysis(imageDataBuffer, firstAnalysisPhase, false);
				firstAnalysisPhase = true;

				for (int i = 0; i < 3; i++) {
					imageDataBuffer[i] = 0;
				}
			}
		}
		w.displayImage(frame);

		while (1) { QApplication::processEvents(); if (w.loopCounter == 0)break; }

		QApplication::processEvents();
		if (w.image)
		{
			frameFrequency = frameFrequency - 3;
			if (frameFrequency < 0)
				frameFrequency = 0;
		}
		else if (w.recordLoadPic == 2)
		{
			if(bSuccess)
			frameFrequency = frameFrequency + 3;
		}

		QApplication::processEvents();
		if (!w.analyse)
			break;
		else if (w.cont)
			exit(0);
	}

	w.finalDisable();
	a.exec();
	exit(0);
}

void imageAnalysis(Mat frame, bool firstAnalysisPhase)
{
	Mat frame_gray, imgHSV, imgThresholded;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//Face Count  
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	imageDataBuffer[0] += faces.size();

	//Eye Count	
	eye_cascade.detectMultiScale(frame_gray, eyes, 1.1, 50, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
	imageDataBuffer[1] += eyes.size();

	//Smile/atmosphere Detection
	smile_cascade.detectMultiScale(frame_gray, smile, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
	imageDataBuffer[2] += smile.size();
	
	if(firstAnalysisPhase)
	{
		//Prep for red object detection...
		//try changing 170 to 177
		cvtColor(frame, imgHSV, COLOR_BGR2HSV);
		inRange(imgHSV, Scalar(177, 150, 60), Scalar(179, 255, 255), imgThresholded);

		Moments oMoments = moments(imgThresholded);
		double xCoordinate = oMoments.m10;
		double yCoordinate = oMoments.m01;
		double screenArea = oMoments.m00;

		if (screenArea > 10000)
		{
			imageDataBuffer[3] = xCoordinate / screenArea;
			imageDataBuffer[4] = yCoordinate / screenArea;
		}

		//Prep for blue object detection...
		//try changing 110 to 118
		cvtColor(frame, imgHSV, COLOR_BGR2HSV);
		inRange(imgHSV, Scalar(118, 150, 60), Scalar(130, 255, 255), imgThresholded);

		Moments oMomentsTwo = moments(imgThresholded);
		xCoordinate = oMomentsTwo.m10;
		yCoordinate = oMomentsTwo.m01;
		screenArea = oMomentsTwo.m00;

		if (screenArea > 10000)
		{
			imageDataBuffer[5] = xCoordinate / screenArea;
			imageDataBuffer[6] = yCoordinate / screenArea;
		}

		//Prep for orange object detection..
		//try changing 15 to 5
		cvtColor(frame, imgHSV, COLOR_BGR2HSV);
		inRange(imgHSV, Scalar(5, 150, 60), Scalar(5, 255, 255), imgThresholded);

		Moments oMomentsThree = moments(imgThresholded);
		xCoordinate = oMomentsThree.m10;
		yCoordinate = oMomentsThree.m01;
		screenArea = oMomentsThree.m00;

		if (screenArea > 10000)
		{
			imageDataBuffer[7] = xCoordinate / screenArea;
			imageDataBuffer[8] = yCoordinate / screenArea;
		}

		//Prep for green object detection..
		cvtColor(frame, imgHSV, COLOR_BGR2HSV);
		inRange(imgHSV, Scalar(50, 50, 50), Scalar(60, 255, 255), imgThresholded);

		Moments oMomentsFour = moments(imgThresholded);
		xCoordinate = oMomentsFour.m10;
		yCoordinate = oMomentsFour.m01;
		screenArea = oMomentsFour.m00;

		if (screenArea > 10000)
		{
			imageDataBuffer[9] = xCoordinate / screenArea;
			imageDataBuffer[10] = yCoordinate / screenArea;
		}

		body_cascade.detectMultiScale(frame_gray, bodies, 1.1, 2, 18 | 9, Size(3, 7));
		imageDataBuffer[11] = bodies.size();
	}
}
