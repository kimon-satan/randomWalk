#ifndef _CAPTURE
#define _CAPTURE

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "color.h"
#include "trackingObject.h"
#include "objectTracker.h"
#include "maze.h"
#include "mazePoint.h"
#include "display.h"
#include "ofxXmlSettings.h"
#include <iostream>
#include <cstdio>


class capture : public ofBaseApp{

	public:

        virtual ~capture();
		void setup();
		void update();
		void draw();
		void save3DMask();
		void draw3DMask();
		void draw3DFrame(bool topFrame);
		void draw3DBlobs();
		void draw3DTrackedBlobs();
		void drawMaze();
		void draw0();
		void draw1();
		void draw2();
		void colorTrack(float scale);

		void reshape(int x, int y, int w, int h);
		void reload();
		void keyPressed  (int key);

		void viewKeys1 (int key);
		void viewKeys0 (int key);
		void viewKeys2 (int key);

		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void loadSettings();
		void saveSettings();


        private:
	
		

		ofFbo displayText;
	
        int camNo;
        int camWidth;
        int camHeight;
        float projScale;
        ofVec2f testPoint;
        bool learnBg;
        bool mouseTest;
        bool bFrameDiff;
        vector<trackingObject> mouseObj;

        ofVideoGrabber 		vidGrabber;

        ofxCvColorImage		colorImg;
        ofxCvColorImage     colorImageHSV;


        ofxCvGrayscaleImage 	hueImage;
		ofxCvGrayscaleImage 	satImage;
		ofxCvGrayscaleImage 	briImage;
		ofxCvGrayscaleImage     grImage;
		ofxCvGrayscaleImage     bgImage;
		ofxCvGrayscaleImage     bgDiff;


        ofxCvContourFinder 	cTrackFinder;


        ofxCvGrayscaleImage     cTrackCont;

        unsigned char*          cTrackPixels;
        ofTexture               cTrackTexture;


        color cTrackColor[2];

        objectTracker thisObjectTracker;
        maze thisMaze;


        int viewMode;
        int roiArray[4];
		float maskArray[7];
		float maskHeight;
		float viewX;
		float viewY;

		int projYdisplace;
		GLint viewport[4];
        GLdouble mvmatrix[16];
        GLdouble projmatrix[16];
		double position[3];
		int selectedItem;
		int selectedMenu;
		int selectedImage;
		int selectedColor;

		string colBlobString;


		bool changed;
		int gauss[4];
		int minBlob;

		int hueThresh, satThresh, briThresh, threshold;
		display thisDisplay;

        int counter;
		vector<trackingObject>nowBlobs;


};

#endif
