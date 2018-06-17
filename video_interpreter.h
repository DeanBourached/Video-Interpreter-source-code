#ifndef VIDEO_INTERPRETER_H5
#define VIDEO_INTERPRETER_H

#include <QtWidgets/QMainWindow>
#include "ui_video_interpreter.h"

#include<iostream>
#include<stdio.h>
#include<sapi.h>

#include "hovereventfilter.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"

#include <QPushButton>
#include <QMessageBox>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixMap>
#include <QTextBrowser>
#include <QFileDialog>

using namespace cv;
using namespace std;

class Video_Interpreter : public QMainWindow
{
	Q_OBJECT

public:
	Video_Interpreter(QWidget *parent = 0);
	~Video_Interpreter();
	bool cont;
	bool analyse;
	int recordLoadPic;
	int loopCounter;
	bool image;
	bool tidyUpHelpWindows;
	bool videoFinished;
	QSpinBox* spinbox1;
	QTextBrowser* output;
	
public slots:
	void  finalDisable()
	{
		ui.textBrowser_2->setText("");
		fastForward->setDisabled(true);
		rewind->setDisabled(true);
		pauseResume->setDisabled(true);
		exit2->setDisabled(true);
		beginAnalysis->setDisabled(true);
		videoFinished = true;

		string message = "Video Interpreter Video Reel has been cut...\nThank-you for your using Video Interpreter!";
		QString toDisplay = QString::fromStdString(message);
		QMessageBox msgBox;
		msgBox.setWindowTitle("Video Interpreter");
		msgBox.setText(toDisplay);
		msgBox.exec();
	}

	void onHoverIn(QObject *object)
	{
		QString objectName = object->objectName();

		if (!videoFinished) {

			if (objectName == "pushButton") {
				if (recordLoadPic == 2)
					ui.textBrowser_2->setText("\nClick to analyse current image, or chose a new image to load into Image Interpreter!");
				else if (image)
					ui.textBrowser_2->setText("\nClick to finish analysis of the current image and close Image Interpreter!");
				else if (cont)
					ui.textBrowser_2->setText("\nClick to Start Recording a video for analysis!");
				else
					ui.textBrowser_2->setText("\nClick to End the current recording and move on to the analysis phase!");
			}
			else if (objectName == "pushButton_2") {
				if (analyse)
					ui.textBrowser_2->setText("\nCut Video Reel* -> Click to break current video stream and Video Interpreter operations!");
				else
					ui.textBrowser_2->setText("\nClick the Begin Analysis button to begin the Video Analsyis process!");
			}
			else if (objectName == "pushButton_4") {
				if (loaderVideo->isEnabled())
					ui.textBrowser_2->setText("\nClick to choose a video for loading (Select and analyse a video from your computer).\nVideo Interpreter uses .avi format video file types.");
			}
			else if (objectName == "pushButton_5") {
				if (loaderVideo->isEnabled())
					ui.textBrowser_2->setText("\nClick to enter Image analysis mode (Select and analyse individual images from your computer).\nImage Interpreter supports .png .xpm .jpg format image file types.");
			}
			else if (objectName == "label_2") {
				if ((!image || recordLoadPic != 2) && tidyUpHelpWindows == false)
					ui.textBrowser_2->setText("\nEvery video is made up of frames (images), if you don't wish for every 40 frames to be analysed (as this may be too slow for you), choose how many frames of the video shall pass before comments are produced.");
				else
					ui.textBrowser_2->setText("\nWelcome to the Image Analysis function of Video Interpreter.\nHit the Analyse/Load button to get started, or hit the FIN button to close Image Interpreter!");
			}
			else if (objectName == "spinBox") {

				string asString = "\nCurrently every ";
				asString += to_string(spinbox1->value());
				asString += " frames (images) are being analysed for background image and colour.\nEvery ";
				asString += to_string(2 * spinbox1->value());
				asString += " frames an averaged person analysis will be generated.";
				QString toPrint = QString::fromStdString(asString);
				ui.textBrowser_2->setText(toPrint);
			}
			else if (recordLoadPic == 2 && image) {

				output->setText("\nANALYSIS WINDOW\n_________________________");
				if (cont)
					loopCounter = 1;
				else
					loopCounter = 0;
				ui.textBrowser_2->setText("\nFIN* -> \nUser satisfied that image(s) have been described satisfactorily!\nHover to clear analysis window, or click to close Image Interpreter!");
			}
			else if (image && !analyse) {

				ui.textBrowser_2->setText("\nFIN button has been pressed.\nImage Interpreter will close once current proccess has been complete!");
			}
			else {

				output->setText("\nANALYSIS WINDOW\n_________________________");
				ui.textBrowser_2->setText("\nQ.E.D.* -> Quod Erat Demonstrandum (thus it has been demonstrated ~Latin)\nUser satisfied that video has been described satisfactorily!\nHover to clear analysis window, or click to close Video Interpreter!");
			}
		}
		else
			ui.textBrowser_2->setText("");
	}

	void onHoverOut(QObject* object)
	{
		if(!videoFinished)
		ui.textBrowser_2->setText("\nHELP WINDOW......?\nHover over widgets to find out more about their function(s)");
	}

	void forPauseResume()
	{
		if (recordLoadPic == 2 || image)
		{
			recordLoadPic = 0;
			image = false;
			pauseResume->setText("||");
			rewind->setDisabled(false);
			fastForward->setDisabled(false);
		}
		else if (loopCounter > 0)
		{
			loopCounter = 0;
			fastForward->setDisabled(false);
			rewind->setDisabled(false);
			pauseResume->setText("||");
		}
		else
		{
			loopCounter = 1;
			fastForward->setDisabled(true);
			rewind->setDisabled(true);	
			pauseResume->setText(">");
		}
	}

	void forFastForward()
	{	
		recordLoadPic = 2;
		pauseResume->setText(">");
		fastForward->setDisabled(true);
		rewind->setDisabled(true);		
	}

	void forRewind()
	{	
		image = true;
		pauseResume->setText(">");
		fastForward->setDisabled(true);
		rewind->setDisabled(true);
	}

	void loadDisplay(string path)
	{
		loaderVideo->setDisabled(true);
		loaderImage->setDisabled(true);
		string message="Video:\n";
		message += path;	
		message += "\n has been loaded into Video Interpreter!";
		QString toDisplay= QString::fromStdString(message);
		QMessageBox msgBox;
		msgBox.setWindowTitle("Video Interpreter");
		msgBox.setText(toDisplay);
		msgBox.exec();
	}

	void displayImage(const Mat& mat)
	{
		// Copy input Mat
		const uchar *qImageBuffer = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		ui.label->setPixmap(QPixmap::fromImage(img.rgbSwapped()).scaled(ui.label->width(), ui.label->height(), Qt::KeepAspectRatio));
	}

	void loadingOption()
	{
		if (image)
		{
			recordStop->setText("Finish/Close");
			exit2->setDisabled(true);
			cont = true;
		}
		else
			switch2();
		recordLoadPic = 1;
	}

	void loadingImageOption()
	{
		fastForward->setDisabled(true);
		rewind->setDisabled(true);
		pauseResume->setDisabled(true);
		spinbox1->setVisible(false);
		ui.label_2->setText("\n    I M A G E   A N A L Y S I S");
		cont = false;
		recordLoadPic = 2;
	}
	
	void analysisWindow(QString set)
	{
		output->setVisible(true);
		output->append("\n" + set);
	}

	string loader()
	{	 
		QString fileName = QFileDialog::getOpenFileName(
			this,
			"Select a movie (.avi) file to load",
			"",
			"movies (*.avi)");

		QPixmap coreImage("VideoInterpreter_Image.jpg");
		ui.label->setPixmap(coreImage);

		if(fileName != "")
			loadDisplay(fileName.toLocal8Bit().constData());
		return fileName.toLocal8Bit().constData();
	}

	void end(bool VideoFirstIteration, bool success)
	{
		exit2->setText("Q. E. D.*");
		exit2->setVisible(true);
		if (VideoFirstIteration)
		{
			fastForward->setDisabled(false);
			pauseResume->setDisabled(false);
			rewind->setDisabled(false);
		}
		else if (!success)
		{
			fastForward->setDisabled(true);		
			fastForward->setText(">>");
			rewind->setDisabled(false); 
			pauseResume->setDisabled(true);
			recordLoadPic = 0;
		}
		else
		{
			pauseResume->setDisabled(false);
			if (!fastForward->isEnabled())
				rewind->setDisabled(true);
		}
	}

	string ImageLoader()
	{
		cont = true;

		loaderVideo->setDisabled(true);
		loaderImage->setDisabled(true);
		recordStop->setText("Analyse/Load");
		exit2->setVisible(true);
		exit2->setText("F I N*");
			
		QString fileName = QFileDialog::getOpenFileName(
			this, 
			tr("Select an image to load"),
			"/home",
			tr("Images (*.png *.xpm *.jpg)"));	

		return fileName.toLocal8Bit().constData();
	}

	void allowAnalysis()
	{
		if (analyse)
			analyse = false;
		else
			analyse = true;
	}

	void buttonsDisplay()
	{
		beginAnalysis->setVisible(true);
		recordStop->setVisible(false);
	}

	void recordAnalysePrompts()
	{
		loaderVideo->setDisabled(true);
		loaderImage->setDisabled(true);
		if (cont || recordLoadPic == 1)
			beginAnalysis->setText("Cut Video Reel*");
		else
			recordStop->setText("Stop Recording \\//");
	}

	void setEnabled()
	{
		loaderVideo->setDisabled(false);
		loaderImage->setDisabled(false);
	}

	void setDisplay()
	{
		recordStop->setVisible(true);
		beginAnalysis->setVisible(false);
		continue_->setDisabled(true);
		switch2();
	}

	void switch2() 
	{
		if (cont)
			cont = false;
		else
			cont = true;
	}

	void topLevelAnalysis(int topLevelBuffer[], bool firstAnalysisPhase, bool image)
	{
		string toPrint = "";
		if (!firstAnalysisPhase || image)
		{
			if (topLevelBuffer[0] < 2)
			{
				if (topLevelBuffer[0] == 0 && topLevelBuffer[1] == 0)
					toPrint = " No one can be seen";
				if (topLevelBuffer[0] == 0 && topLevelBuffer[1] == 1)
					toPrint = " Someone is about";
				if (topLevelBuffer[0] == 0 && topLevelBuffer[1] == 2)
					toPrint = " We have a close up";
				if (topLevelBuffer[0] == 1 && topLevelBuffer[1] == 0)
					toPrint = " One person has stepped forward";
				if (topLevelBuffer[0] == 1 && topLevelBuffer[1] == 1)
					toPrint = " This one looks happy";
				if (topLevelBuffer[0] == 1 && topLevelBuffer[1] == 2)
					toPrint = " Good, we have an intent listener";
			}
			else
			{
				if (topLevelBuffer[1] == (2 * topLevelBuffer[0]))
				{
					toPrint = " ";
					toPrint += to_string(topLevelBuffer[0]);
					toPrint += " people are here, and ready for a pic";
				}
				else
				{
					toPrint = " ";
					toPrint += to_string(topLevelBuffer[0]);
					toPrint += " people are with us";
				}
			}

			if (topLevelBuffer[2] >= 30)
				toPrint = "There's little more than a blur; camera is out of focus!";
			else if (topLevelBuffer[2] >= 20 && topLevelBuffer[0] > 1)
			{
				toPrint += "\n and ";
				toPrint += to_string(topLevelBuffer[0]);
				toPrint += "  people seem to be engaging with us!";
			}
			else if (topLevelBuffer[2] >= 20 && topLevelBuffer[0] == 1)
			{
				toPrint += "\n and this person seems to be engaging with us!";
			}
			else if (topLevelBuffer[2] >= 10 && topLevelBuffer[0] > 1)
			{
				toPrint += "\n and these ";
				toPrint += to_string(topLevelBuffer[0]);
				toPrint += "  people here don't seem very interested!";
			}
			else if (topLevelBuffer[2] >= 10 && topLevelBuffer[0] == 1)
			{
				toPrint += ";\n someone is here, but isn't doing much!";
			}
			else if (topLevelBuffer[2] > 10)
				toPrint += ",\n and the atmosphere is glum!";
			else
				toPrint += ".";
		}
		
		if (firstAnalysisPhase || image)
		{
			if (topLevelBuffer[3] != 0 && topLevelBuffer[4] != 0)
			{
				if (topLevelBuffer[3] >= 96 && topLevelBuffer[3] <= 213 && topLevelBuffer[4] >= 79 && topLevelBuffer[4] <= 154)
					toPrint += " A red object is in the centre of the screen!";
				else
				{
					toPrint += " A red object is in the ";
					if (topLevelBuffer[4] < 105)
						toPrint += " top ";
					else
						toPrint += " bottom ";
					if (topLevelBuffer[3] < 150)
						toPrint += " left ";
					else
						toPrint += " right ";
					toPrint += " of the screen!";
				}
			}

			if (topLevelBuffer[5] != 0 && topLevelBuffer[6] != 0)
			{
				if (topLevelBuffer[5] >= 96 && topLevelBuffer[5] <= 213 && topLevelBuffer[6] >= 79 && topLevelBuffer[6] <= 154)
					toPrint += " A blue object is in the centre of the screen!";
				else
				{
					toPrint += " A blue object is in the ";
					if (topLevelBuffer[6] < 105)
						toPrint += " top ";
					else
						toPrint += " bottom ";
					if (topLevelBuffer[5] < 150)
						toPrint += " left ";
					else
						toPrint += " right ";
					toPrint += " of the screen!";
				}
			}

			if (topLevelBuffer[7] != 0 && topLevelBuffer[8] != 0 && topLevelBuffer[3] == 0 && topLevelBuffer[4] == 0)
			{
				if (topLevelBuffer[7] >= 96 && topLevelBuffer[7] <= 213 && topLevelBuffer[8] >= 79 && topLevelBuffer[8] <= 154)
					toPrint += " A reddish orange object is in the centre of the screen!";
				else
				{
					toPrint += " A reddish orange object is in the ";
					if (topLevelBuffer[8] < 105)
						toPrint += " top ";
					else
						toPrint += " bottom ";
					if (topLevelBuffer[7] < 150)
						toPrint += " left ";
					else
						toPrint += " right ";
					toPrint += " of the screen!";
				}
			}

			if (topLevelBuffer[9] != 0 && topLevelBuffer[10] != 0)
			{
				if (topLevelBuffer[9] >= 96 && topLevelBuffer[9] <= 213 && topLevelBuffer[10] >= 79 && topLevelBuffer[10] <= 154)
					toPrint += " A green object is in the centre of the screen!";
				else
				{
					toPrint += " A green object is in the ";
					if (topLevelBuffer[10] < 105)
						toPrint += " top ";
					else
						toPrint += " bottom ";
					if (topLevelBuffer[9] < 150)
						toPrint += " left ";
					else
						toPrint += " right ";
					toPrint += " of the screen!";
				}
			}

			//Next Stanza

			if (topLevelBuffer[11] != 0)
			{
				if (topLevelBuffer[11] == 1)
				{
					toPrint += " We have someone in the image background!";
				}
				else if (topLevelBuffer[11] <= 3)
				{
					toPrint += " We have several standing people in the image background!";
				}
				else
					toPrint += " Looks like there's a party going on!";
			}
		}

		if (toPrint != "")
		{
			wstringstream cls;
			cls << toPrint.c_str();
			wstring forSpeech = cls.str();
			LPCWSTR alpha = forSpeech.c_str();

			ISpVoice * pVoice = NULL;

			HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);

			hr = pVoice->Speak(alpha, 0, NULL);

			QString qstr = QString::fromStdString(toPrint);
			analysisWindow(qstr);
		}
	}

private:
	QPushButton* recordStop;
	QPushButton* beginAnalysis;
	QPushButton* continue_;
	QPushButton* loaderVideo;
	QPushButton* loaderImage;
	QPushButton* exit2;
	QPushButton* rewind;
	QPushButton* fastForward;
	QPushButton* pauseResume;
	HoverEventFilter* hoverEventFilter;
	Ui::Video_InterpreterClass ui;
};

#endif // VIDEO_INTERPRETER_H
