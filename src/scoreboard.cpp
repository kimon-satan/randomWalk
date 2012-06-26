#include "scoreboard.h"

bool comparebyScore (const playerScore a, const playerScore b) {
        return (a.score > b.score);
 }


scoreboard::scoreboard()
{
    //ctor
}

string scoreboard::getPlayerName(){

return thisPlayer.name;

}

void scoreboard::setPlayerName(string player){


thisPlayer.name = player;

}

void scoreboard::newPlayer(){

thisPlayer.score = 0;
timeSubTot = 0;
thisPlayer.levels = 0;
}

void scoreboard::addLevelScore(float levelTime, int levelPoints ){

levelTime -= timeSubTot;
timeSubTot = levelTime;

thisPlayer.score = thisPlayer.score + ((levelPoints/levelTime) * 100);
thisPlayer.levels +=1;

}

float scoreboard::getCurrentScore(){

return thisPlayer.score;

}


string scoreboard::getHighScores(){

string scoreString = "";

for(int i = 0; i < highScores.size(); i++){

scoreString = scoreString + highScores[i].name + "  Level: " + ofToString(highScores[i].levels)
+ "  Score: " + ofToString(highScores[i].score,3) + "\n";

}

return scoreString;

}

void scoreboard::updateHighScores(){

highScores.push_back(thisPlayer);
sort(highScores.begin(),highScores.end(), comparebyScore);
if(highScores.size() > 10){highScores.pop_back();}

}


scoreboard::~scoreboard()
{
    //dtor
}
