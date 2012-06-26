#ifndef OSCMESS_H
#define OSCMESS_H

#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 57120

class oscMess
{
    public:
        oscMess();
        virtual ~oscMess();


        void sendMoving(int temp);
        void sendProximities(vector<float> temp);
        void sendPoints(vector<bool> temp);

        void sendInitialise(int temp);
        void sendNextLevel(int temp);
        void sendGameOver();



    protected:
    private:
     ofxOscSender sender;

     string messageString;
};

#endif // OSCMESS_H
