#ifndef TRACKINGOBJECT_H
#define TRACKINGOBJECT_H

#include "ofMain.h"

class trackingObject
{
    public:
        trackingObject();
        virtual ~trackingObject();

        float getScrNearX();
        void setScrNearX(float temp);
        float getScrNearY();
        void setScrNearY(float temp);
        float getScrFarX();
        void setScrFarX(float temp);
        float getScrFarY();
        void setScrFarY(float temp);

        float getProjCentX();
        void setProjCentX(float temp);
        float getProjCentY();
        void setProjCentY(float temp);

        int getColorNumber();
        void setColorNumber(int temp);
        bool getAssigned();
        void setAssigned(bool temp);
        bool getPresent();
        void setPresent(bool temp);
        bool getChanged();
        void setChanged(bool temp);

        void ghostIncrement();
        void ghostReset();
        int getGhost();
        void reset();
        void updateHistory();
        float getSpeed();
        ofVec2f getAvPos();
        void setFakeSpeed(float temp);
        void setFake(bool temp);

    protected:
    private:

    float scrNearX;
    float scrNearY;
    float scrFarX;
    float scrFarY;
    float projCentX;
    float projCentY;
    vector<ofVec2f> history;
    bool objChanged;
    bool assigned;
    bool present;
    int ghostCounter;
    int colNum;
    float fakeSpeed;
    bool fakeObj;

};

#endif // TRACKINGOBJECT_H
