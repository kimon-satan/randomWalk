#include "maze.h"
#include <cstdlib>

maze::maze()
{


moving = false;
objectsPresent = false;
speedThresh = 6;
pointWait = 0;

stillTime = 6;
levels = 3;
currentLevel = 0;
totalTime = 300;
timerActive = false;
gameOver = false;
allComplete = false;


for(int i = 0; i < levels; i++){
mazeDensity.push_back(4);
mazeDiameter.push_back(3);
minDistance.push_back(10);

}
initialiseMaze(50,50);
}

maze::~maze()
{
    //dtor
}


void maze::setLevels(int temp){

if(levels > temp){

levels -= 1;

mazeDensity.pop_back();
mazeDiameter.pop_back();
minDistance.pop_back();
mazePoints.pop_back();

};

if(levels < temp){

levels += 1;

mazeDensity.push_back(4);
mazeDiameter.push_back(3);
minDistance.push_back(10);
};

initialiseMaze(50,50);

}

void maze::loadLevels(int temp){

mazeDensity.clear();
mazeDiameter.clear();
minDistance.clear();

levels = temp;

for(int i = 0; i < levels; i++){
mazeDensity.push_back(4);
mazeDiameter.push_back(3);
minDistance.push_back(10);

}

initialiseMaze(50,50);

}

int maze::getLevels(){
return levels;
}

void maze::setMazeDensity(int temp){
mazeDensity[currentLevel] = temp;
}

int maze::getMazeDensity(){

return mazeDensity[currentLevel];
}

void maze::setMazeDensity(int temp, int level){
mazeDensity[level] = temp;
}

int maze::getMazeDensity(int level){

return mazeDensity[level];
}

void maze::setMazeDiameter(float temp){
mazeDiameter[currentLevel] = temp;
}

void maze::setMazeDiameter(float temp, int level){
mazeDiameter[level] = temp;
}

float maze::getMazeDiameter(){

return mazeDiameter[currentLevel];
}

float maze::getMazeDiameter(int level){

return mazeDiameter[level];
}

void maze::initialiseMaze(int w, int h){  //could have extra bool jumblePositions when false simply returns all points to inactive

allComplete = false;
gameOver = false;
timerActive = false;
currentLevel = 0;
levelWait = 0;
pointWait = 0;
mazePoints.clear();

for(int j = 0; j < levels; j++){

vector<mazePoint> levelPoints;

int counter = 0;
int overflow = 0;

    do{

        mazePoint temp;
        bool tooClose = false;

        temp.location.x = rand()%w;
        temp.location.y = rand()%h;
        temp.location.x = max(temp.location.x, mazeDiameter[j] + 3);
        temp.location.x = min(temp.location.x, 47 - mazeDiameter[j]);
        temp.location.y = max(temp.location.y, mazeDiameter[j] +3);
        temp.location.y = min(temp.location.y, 47 - mazeDiameter[j]);

        for(int i = 0; i < levelPoints.size();i ++){

        if(temp.location.distance(levelPoints.at(i).location) < minDistance[j]){tooClose = true;}

        }


    if(!tooClose){temp.sequential = counter;
                    temp.level = j;
                    levelPoints.push_back(temp);
                    counter++;}
        overflow++;

        if(overflow > 2000){break;}

    }while(levelPoints.size() < mazeDensity[j]);

        for(int i = 0; i < levelPoints.size();i ++){

            levelPoints.at(i).proximity = 50;

         }

    mazePoints.push_back(levelPoints);

    }

thisMessenger.sendInitialise(mazePoints.at(currentLevel).size()); // change for initialise message
thisMessenger.sendMoving(moving);


}

vector<mazePoint> maze::getMazePoints(){

vector<mazePoint> levelPoints;

levelPoints = mazePoints.at(currentLevel);

return levelPoints;

}

vector<vector<mazePoint> > maze::getAllMazePoints(){

return mazePoints;

}

void maze::feedObjects(vector<trackingObject> finalObjects){

    ofVec2f temp;


        if(finalObjects.at(0).getPresent() && finalObjects.at(1).getPresent()){

            temp.set(finalObjects.at(0).getProjCentX(),finalObjects.at(0).getProjCentY());
            playerPos.set(finalObjects.at(1).getProjCentX(),finalObjects.at(1).getProjCentY());
            //temp = finalObjects[0].getAvPos();
            //playerPos = finalObjects[1].getAvPos();

            playerPos.getMiddle(temp);

        objectsPresent = true;

        }else if(finalObjects.at(0).getPresent() && !finalObjects.at(1).getPresent()) {

                playerPos.set(finalObjects.at(0).getProjCentX(),finalObjects.at(0).getProjCentY());
                //playerPos = finalObjects[0].getAvPos();

        objectsPresent = true;

        }else if(!finalObjects.at(0).getPresent() && finalObjects.at(1).getPresent()){

                playerPos.set(finalObjects.at(1).getProjCentX(),finalObjects.at(1).getProjCentY());
                //playerPos = finalObjects[1].getAvPos();

        objectsPresent = true;

        }else{

        objectsPresent = false;

        }


    if(levelWait == 0 && pointWait == 0){

        vector<float> proxs;
        float temp = -1.0f;
         for(int i = 0; i < mazePoints[currentLevel].size();i ++){

            mazePoints[currentLevel][i].proximity = max(0.01f, mazePoints[currentLevel][i].location.distance(playerPos) - (float)(mazeDiameter[currentLevel] * 0.5));


            proxs.push_back(mazePoints[currentLevel][i].proximity);

            //printf("point: %i  proximity: %f \n", i , mazePoints.at(i).proximity);
         }

    if(moving && objectsPresent){
    thisMessenger.sendProximities(proxs);
    }

    if(!moving && objectsPresent){moving = true; thisMessenger.sendMoving(1);}

    if(!objectsPresent && moving )movingBuffer +=1;

    if(!objectsPresent && mazePoints.size() > 0 && moving && movingBuffer > stillTime){

        movingBuffer = 0;
        vector<bool> activePoints;
        bool newPoint = false;
        moving = false;

        for(int i=0; i < mazePoints.at(currentLevel).size(); i++){

            if(mazePoints[currentLevel][i].proximity < 3 && !mazePoints[currentLevel][i].isActive){
            mazePoints[currentLevel][i].isOccupied = true;
            mazePoints[currentLevel][i].isActive = true;
            newPoint = true;
            }else if(mazePoints[currentLevel][i].proximity < 3){
            mazePoints[currentLevel][i].isOccupied = true;
            }
            else{mazePoints[currentLevel][i].isOccupied = false;}

        }

                int activeTotal = 0;

                for(int i = 0; i < mazePoints.at(currentLevel).size(); i ++){

                activePoints.push_back(mazePoints[currentLevel][i].isActive);
                if(mazePoints[currentLevel][i].isActive)activeTotal +=1;

                }

        if(newPoint){
        thisMessenger.sendPoints(activePoints);
        thisMessenger.sendMoving(2);
        pointWait = 1;

        }else{

        thisMessenger.sendMoving(0);
        }

        if(activePoints.size() == activeTotal && currentLevel != levels - 1){

            levelWait +=1;

        }else if(activePoints.size() == activeTotal && currentLevel == levels -1){

        levelIncrement(true);

        }

    }
}else if(levelWait > 0){
    levelWait +=1;
    pointWait +=1;

}else if(pointWait > 0){
    pointWait +=1;}

    if(levelWait == 50){levelIncrement(true);}
    if(pointWait == 30){pointWait = 0;}


}

void maze::levelIncrement(bool allActive){

levelWait = 0;
pointWait = 0;

if(!allActive){

    vector<bool> activePoints;

    for(int i = 0; i < mazePoints.at(currentLevel).size();i++){

    mazePoints[currentLevel][i].isActive = true;
    activePoints.push_back(true);

    }

    thisMessenger.sendPoints(activePoints);
    thisMessenger.sendMoving(1);
    thisMessenger.sendMoving(0);


}

    addLevelScore(totalTime - timeRemain ,mazePoints[currentLevel].size());
    currentLevel += 1;

    if(currentLevel == levels){

        updateHighScores();
        allComplete = true;
        thisMessenger.sendGameOver();
        gameOver = true;
        if(timerActive == true){timerActive = false; currentLevel -=1; gameOver = true;}else{initialiseMaze(50,50);}



        }else{


    thisMessenger.sendNextLevel(mazePoints.at(currentLevel).size());

    }

}

int maze::getCurrentLevel(){

return currentLevel;

}

bool maze::isAllComplete(){

return allComplete;
}

bool maze::isMoving(){

return moving;
}

int maze::getMinDistance(){

return minDistance[currentLevel];
}

int maze::getMinDistance(int level){

return minDistance[level];
}

void maze::setMinDistance(int temp){

minDistance[currentLevel] = temp;
}

void maze::setMinDistance(int temp, int level){

minDistance[level] = temp;
}

int maze::getStillTime(){

return stillTime;
}

void maze::setStillTime(int temp){

stillTime = temp;
}

ofVec2f maze::getPlayerPos(){

return playerPos;

}

void maze::setSpeedThresh(float temp){
    speedThresh = temp;
}

float maze::getSpeedThresh(){

return speedThresh;

}

void maze::startTimer(){

newPlayer();
initialiseMaze(50,50);
initTime = ofGetElapsedTimef();
timeRemain = totalTime;
timerActive = true;
}

float maze::getTimeRemain(){

return timeRemain;

}

int maze::getTotalTime(){

return totalTime;

}

void maze::setTotalTime(int temp){

totalTime = temp;

}

void maze::updateTime(){


if(timerActive){

float diff;
diff = ofGetElapsedTimef() - initTime;
timeRemain = totalTime - diff;

if(timeRemain < 0.1){
timeRemain = 0;
timerActive = false;
gameOver = true;
thisMessenger.sendGameOver();

updateHighScores();
}

}

}

bool maze::getGameOver(){

return gameOver;
}

void maze::saveHighScores(){

char fileName[80];
ofxXmlSettings XML;

cout<< "Save - Enter file name:";
gets(fileName);
cout <<"\n" << "saving: "<< fileName << "\n";


int tagNum = XML.addTag("HIGHSCORES");
if( XML.pushTag("HIGHSCORES", tagNum) ){

    for(int i = 0 ; i < highScores.size(); i++){
    tagNum	= XML.addTag("PLAYER");
        if( XML.pushTag("PLAYER", tagNum) ){
            XML.setValue("NAME", highScores[i].name, tagNum);
            XML.setValue("LEVELS", highScores[i].levels, tagNum);
            XML.setValue("SCORE", highScores[i].score);

            XML.popTag();
        }


    }

XML.popTag();

}

XML.saveFile(fileName);

}

void maze::loadHighScores(){

char fileName[80];
ofxXmlSettings XML;

cout<< "Open - Enter file name:";
gets(fileName);


if(XML.loadFile(fileName)){
cout <<"\n" << "opening: "<< fileName << "\n";
}else{

cout <<"\n" << "can't find file: "<< fileName << "\n";
return;
};


if( XML.pushTag("HIGHSCORES", 0) ){
highScores.clear();

for(int i = 0; i < 10; i++){

    if( XML.pushTag("PLAYER", i) ){
       // newPlayer();

        thisPlayer.name = XML.getValue("NAME","none");
        thisPlayer.levels = XML.getValue("LEVELS",0);
        thisPlayer.score = XML.getValue("SCORE",0.0f);

        highScores.push_back(thisPlayer);

        XML.popTag();
        }
    }
XML.popTag();

}

}
