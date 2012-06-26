#ifndef OBJECTTRACKER_H
#define OBJECTTRACKER_H
#include "ofMain.h"
#include "trackingObject.h"
#include <algorithm>



struct distListItem {
    int    np;
    int    fp;
    float  dist;


};


class objectTracker
{
    public:
        objectTracker();

        void feedObjects(vector<trackingObject> nowObjects);
        vector<trackingObject> getFinals();
        void reset();
        void setGhostFrames(int tempGhost);
        void setMaxDist(int tempMaxDist);
        void setChangeThresh(float temp);


        int getGhostFrames();
        int getMaxDist();
        float getChangeThresh();





    protected:
    private:

    vector<distListItem>distanceList;
    vector<trackingObject> finalObjects;
    int maxDist;
    int ghostFrames;
    float changeThresh;



};


#endif // OBJECTTRACKER_H
