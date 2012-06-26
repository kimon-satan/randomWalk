#ifndef MAZE_H
#define MAZE_H

#include "trackingObject.h"
#include "mazePoint.h"
#include "oscMess.h"
#include "ofxXmlSettings.h"
#include <iostream>
#include <cstdio>



class maze : public scoreboard
{
    public:
        maze();
        virtual ~maze();

    void feedObjects(vector<trackingObject> finalObjects);
    bool isMoving();
    float detectProximity(mazePoint temp, trackingObject thisObject);
    void initialiseMaze(int w, int h);

    vector<mazePoint> getMazePoints();
    vector<vector<mazePoint> > getAllMazePoints();

    void setLevels(int temp);
    void loadLevels(int temp);
    int getLevels();

    void setMazeDensity(int temp);
    void setMazeDiameter(float temp);
    void setMinDistance(int temp);
    void setMazeDensity(int temp, int level);
    void setMazeDiameter(float temp, int level);
    void setMinDistance(int temp, int level);


    int getMazeDensity();
    float getMazeDiameter();
    int getMinDistance();
     int getMazeDensity(int level);
    float getMazeDiameter(int level);
    int getMinDistance(int level);
    ofVec2f getPlayerPos();
    float getSpeedThresh();
    void setSpeedThresh(float temp);
    int getStillTime();
    void setStillTime(int temp);

    void setCurrentLevel(int temp);
    int getCurrentLevel();
    void levelIncrement(bool allActive);

    void updateTime();
    void startTimer();
    float getTimeRemain();
    int getTotalTime();
    void setTotalTime(int temp);
    bool getGameOver();
    bool isAllComplete();

    void saveHighScores();
    void loadHighScores();


    protected:
    private:

    int totalTime;
    float timeRemain;
    float initTime;
    bool timerActive;
    int currentLevel;
    int stillTime;
    int levels;
    float speedThresh;
    int levelWait;
    bool gameOver;


    vector<int> mazeDensity;
    vector<float> mazeDiameter;
    vector<int> minDistance;

    bool moving;
    bool objectsPresent;
    int movingBuffer;
    int pointWait;
    bool allComplete;

    ofVec2f playerPos;

    float pointDiameter;
    vector<vector<mazePoint> > mazePoints;
    oscMess thisMessenger;

};

#endif // MAZE_H
