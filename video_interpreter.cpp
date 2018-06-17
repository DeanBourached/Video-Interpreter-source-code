#include "video_interpreter.h"

Video_Interpreter::Video_Interpreter(QWidget *parent)
	: QMainWindow(parent)
{	
	ui.setupUi(this);
	hoverEventFilter = new HoverEventFilter(this);
	ui.label_2->installEventFilter(hoverEventFilter);	
	ui.textBrowser_2->setText("\nHELP WINDOW......?\nHover over widgets to find out more about their function(s)");
	
	cont = false;
	analyse = false;
	recordLoadPic = 0;
	loopCounter = 0;
	image = false;
	tidyUpHelpWindows = false;
	videoFinished = false;
	recordStop = ui.pushButton;
	recordStop->installEventFilter(hoverEventFilter);
	beginAnalysis = ui.pushButton_2;
	beginAnalysis->installEventFilter(hoverEventFilter);
	continue_ = ui.pushButton_3;
	loaderVideo = ui.pushButton_4;
	loaderVideo->installEventFilter(hoverEventFilter);
	loaderImage = ui.pushButton_5;
	loaderImage->installEventFilter(hoverEventFilter);
	exit2 = ui.pushButton_6;
	exit2->installEventFilter(hoverEventFilter);
	fastForward = ui.pushButton_8;
	rewind = ui.pushButton_9;
	pauseResume = ui.pushButton_10;
	output = ui.textBrowser;
	spinbox1 = ui.spinBox;
	spinbox1->installEventFilter(hoverEventFilter);
	connect(hoverEventFilter, SIGNAL(HoverIn(QObject*)), this, SLOT(onHoverIn(QObject*)));
	connect(hoverEventFilter, SIGNAL(HoverOut(QObject*)), this, SLOT(onHoverOut(QObject*)));
	connect(recordStop, SIGNAL(clicked()), this, SLOT(switch2()));
	connect(beginAnalysis, SIGNAL(clicked()), this, SLOT(allowAnalysis()));
	connect(continue_, SIGNAL(clicked()), this, SLOT(setDisplay()));
	connect(loaderVideo, SIGNAL(clicked()), this, SLOT(loadingOption()));
	connect(loaderImage, SIGNAL(clicked()), this, SLOT(loadingImageOption()));
	connect(exit2, SIGNAL(clicked()), this, SLOT(loadingOption()));
	connect(fastForward, SIGNAL(clicked()), this, SLOT(forFastForward()));
	connect(rewind, SIGNAL(clicked()), this, SLOT(forRewind()));
	connect(pauseResume, SIGNAL(clicked()), this, SLOT(forPauseResume()));
	recordStop->setVisible(false);
	beginAnalysis->setVisible(false);
	loaderVideo->setDisabled(true);
	loaderImage->setDisabled(true);
	exit2->setVisible(false);
	fastForward->setDisabled(true);
	rewind->setDisabled(true);
	pauseResume->setDisabled(true);
	output->setText("\nANALYSIS WINDOW\n_________________________");
}

Video_Interpreter::~Video_Interpreter()
{
	delete &ui;
}

