#ifndef DISPLAY_H
#define DISPLAY_H

#include "ofMain.h"
#include "mazePoint.h"
#include <algorithm>

class display
{
    public:
        display();
        virtual ~display();
	
	void setup();

    void initialize(vector<mazePoint> tempPoints);
    void draw();
    void drawChain();
    void drawInfo(float time, float score, int level);
    void drawPlayerPos(ofVec2f playerPos);
    void drawMoveShapes(vector<vector <mazePoint> > tempPoints, ofVec2f playerPos);
    void drawStillShapes(vector<vector<mazePoint> >tempPoints, ofVec2f playerPos);
    void setLeveldata(int currLev, vector<vector<mazePoint> >tempPoints);
    void updateCounter();
    void showGameOver(string highScores, bool allComp);


    protected:
    private:

    vector<int> xmults;
    vector<int> ymults;
    vector<int> xrots;
    vector<int> yrots;
    vector<int> zrots;
    vector<vector<int> > shapeTypes;
    vector<vector<int> > stillChain;


	ofLight light;
	ofEasyCam cam1;
	ofEasyCam cam2;
    int counter;
    float smallCounts[10];
    int currLev;
    bool levelComplete;
    int goCount;
    int stillcount;
    bool newPoint;
    int levPoints;

    ofTrueTypeFont          timeFont;
    ofTrueTypeFont          bigFont;
};

#endif // DISPLAY_H
