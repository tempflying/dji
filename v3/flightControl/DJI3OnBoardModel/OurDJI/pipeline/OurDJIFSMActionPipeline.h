#ifndef OURDJIFSMACTIONPIPELINE_H
#define OURDJIFSMACTIONPIPELINE_H
#include "OurDJIFSMPipeline.h"
#include "OurDJI/action/OurAction.h"

class OurActionArray{
    bool stopAfterEachAction;
public:
    std::vector<OurAction*> actions;
    unsigned int currentActionId;
    OurActionArray(){
        currentActionId = 0;
        stopAfterEachAction = false;
    }

    size_t getActionCount(){
        return actions.size();
    }

    void push_back(OurAction* action){
        actions.push_back(action);
    }
    bool getStopAfterEachAction() const{
        return stopAfterEachAction;
    }
    void setStopAfterEachAction(bool value){
        stopAfterEachAction = value;
    }

    OurAction* getCurrentAction(){
        if(currentActionId >= actions.size()) return 0;
        return actions.at(currentActionId);
    }
};

class OurDJIFSMActionPipeline : public OurDJIFSMPipeline{
public:
    OurDJIFSMActionPipeline(){
        init();
    }

protected:
    void addAction(OurAction* action){
        actionArray.push_back(action);
    }


    void init(){
        isFirstInMainFlight = true;
    }

    bool isFirstInMainFlight;
    OurActionArray actionArray;
    virtual void whenInMainFlight(){
        if(isFirstInMainFlight){
            debugCheck();
            isFirstInMainFlight = false;
        }

        if(actionArray.currentActionId < actionArray.getActionCount()){
            if(actionArray.actions.at(actionArray.currentActionId)->stepOver(flight)){
                actionArray.currentActionId ++;
                if(actionArray.currentActionId < actionArray.getActionCount())
                    debugCheck();
            }
        }
        else{
            init();
            landing();
            ourCurrentState = OUR_LANDDING;
        }
    }

    void debugCheck(){
        debugAfterEachAction(actionArray.getCurrentAction());
        if(actionArray.getStopAfterEachAction() == true){
            getchar();
        }
    }

    virtual void debugAfterEachAction(OurAction* action){
        API_LOG(driver, DEBUG_LOG, "action is %p\n", action);
    }

    void printFlightInfoTitle(){
        printf("acceleration, eulerianAngle, magnet, palstance,\
               position, quaternion, velocity, status\n");
    }

    void printFlightInfo(Flight* flight){
        CommonData acc = flight->getAcceleration();
        DJI::EulerianAngle eulerAngle = flight->getEulerianAngle();
        MagnetData magnetData = flight->getMagnet();
        CommonData palstance = flight->getPalstance();
        PositionData pos = flight->getPosition();
        QuaternionData quaternion = flight->getQuaternion();
        VelocityData vel = flight->getVelocity();
        Flight::Status status = flight->getStatus();

        printf("%f %f %f, ", acc.x, acc.y, acc.z);
        printf("%lf %lf %lf, ", eulerAngle.roll, eulerAngle.pitch, eulerAngle.yaw);
        printf("%d %d %d, ", magnetData.x, magnetData.y, magnetData.z);
        printf("%f %f %f, ", palstance.x, palstance.y, palstance.z);
        printf("%lf %lf %f %f, ", pos.latitude, pos.longitude, pos.altitude, pos.height);
        printf("%f %f %f %f, ", quaternion.q0, quaternion.q1, quaternion.q2, quaternion.q3);
        printf("%f %f %f, ", vel.x, vel.y, vel.z);
        printf("%d", status);
        printf("\n");
    }
};

#endif // OURDJIFSMACTIONPIPELINE_H
