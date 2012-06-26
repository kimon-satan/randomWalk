#ifndef MAZEPOINT_H
#define MAZEPOINT_H
#include "scoreboard.h"

class mazePoint
{
    public:
        mazePoint();
        virtual ~mazePoint();

    ofVec2f location;
    int sequential;
    float proximity;
    bool isActive;
    bool isOccupied;
    bool completed;
    int level;

    protected:
    private:




};

#endif // MAZEPOINT_H
