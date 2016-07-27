#ifndef OURACTIONTESTPIPELINE_H
#define OURACTIONTESTPIPELINE_H

#include "OurDJI/OurDJI.h"

class OurActionTestPipeline : public OurDJIFSMActionPipeline {
public:
    OurActionTestPipeline(){
        // 设置每执行完一个Action就停下来，等待用户回车确认
        actionArray.setStopAfterEachAction(true);
        // OurMoveByUseVerticalAction 是action里预定义的一种Action
        addAction(new OurMoveByUseVerticalAction(5, 0, 0));
        addAction(new OurMoveByUseVerticalAction(0, -5, 0));
        addAction(new OurMoveByUseVerticalAction(-5, 0, 0));
        // actionArray.push_back与addAction等价
        actionArray.push_back(new OurMoveByUseVerticalAction(0, 5, 0));
    }
};

#endif // OURACTIONTESTPIPELINE_H
