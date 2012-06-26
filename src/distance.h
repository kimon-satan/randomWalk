#ifndef DISTANCE_H
#define DISTANCE_H


class distance
{
    public:
        distance(int tempNPerf, int tempFPerf, float tempDistance);

    int getNperf();
    int getFperf();
    float getDistance();

    protected:
    private:

    int nPerf;
    int fPerf;
    float dist;
};

#endif // DISTANCE_H
