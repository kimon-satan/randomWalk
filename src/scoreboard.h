#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "ofMain.h"

struct playerScore{

string name;
int levels;
float score;

};

class scoreboard
{
    public:
        scoreboard();
        virtual ~scoreboard();
    void newPlayer();
    string getPlayerName();
    void setPlayerName(string player);
    void addLevelScore(float levelTime, int levelPoints);
    float getCurrentScore();
    string getHighScores();
    void updateHighScores();

    protected:
    float timeSubTot;
    playerScore thisPlayer;
    vector<playerScore> highScores;
    private:


};

#endif // SCOREBOARD_H
