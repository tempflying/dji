#ifndef OURWAYPOINTPIPELINE_H
#define OURWAYPOINTPIPELINE_H

#include "OurDJI.h"

class Point{
public:
    Point(float xx, float yy){
        x = xx;
        y = yy;
    }
    float x;
    float y;
};

class OurWayPointPipeline : public OurDJIDataSovlerPipeline{
public:
    OurWayPointPipeline(){
        wayPointUploadOK = false;
        //actionArray.setStopAfterEachAction(true);
    }

protected:
    bool wayPointUploadOK;

    virtual void whenActived(){
        if(wayPointUploadOK == false){
            getWayPointFromMobile();
        }
        else {
            setControl(OBTAIN_CONTROL);
            wayPointUploadOK = false;
            ourCurrentState = OUR_WAITTING;
        }
    }

    virtual void whenReleaseControlSuccess(){
        wayPoints.clear();
        ourCurrentState = OUR_ACTIVATED;
    }

    std::vector<Point> wayPoints;
    void getWayPointFromMobile(){
        ReceivedData receivedData = getReceiveDataFromMobile();
        float* data;
        if(receivedData.length > 0){
            printReceiveDataInInt(receivedData);
            data = OurByteConvertUtil::getFloatArray(receivedData.data, receivedData.length);
            for(size_t i = 0; i < receivedData.length / sizeof(float) ; i++){
                printf("%f ", data[i]);
            }
            printf("\n");

            for(size_t i = 0; i < receivedData.length / sizeof(float) / 2; i++){
                wayPoints.push_back(Point(data[2*i], data[2*i+1]));
            }

            receivedData.destroy();
            delete[] data;

            setActionByWayPoints();
            wayPointUploadOK = true;
        }
    }

    void setActionByWayPoints(){
        for(int i = 0; i < int(wayPoints.size()); i++){
            Point point = wayPoints.at(i);
//            printf("%f, %f\n", point.x, point.y);
            actionArray.push_back(new OurMoveByUseVerticalAction(point.x, point.y, 0));
            actionArray.push_back(new OurStableAction(10));
        }
    }

//    void debugAfterEachAction(OurAction* action){
//        OurMoveByUseVerticalAction* ourAction = (OurMoveByUseVerticalAction*)action;
//        printf("%f %f %f\n", ourAction->offset_m.x, ourAction->offset_m.y, ourAction->offset_m.z);
//    }
};

#endif // OURWAYPOINTPIPELINE_H
