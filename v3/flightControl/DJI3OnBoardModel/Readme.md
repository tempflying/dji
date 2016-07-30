#DJI3OnBoardModel
- 目录说明
- 如何使用这个模板
- pipeline
- action
- OurByteConvertUtil

## 目录说明
目录 | 说明
---        | ---
lib        | DJI OnBoard SDK 的源码
OurDJI | 我们对DJI OnBoard SDK的简单封装
OurDJI/action    | 这是简单封装里的概念，action实现飞行控制逻辑
OurDJI/pipeline | 这是简单封装里的概念，pipeline实现飞行控制的整个流程
OurDJI/pipelineExample | 这是pipeline使用的一些DEMO，里面也有Action创建的一些方法

## 如何使用这个模板
1. 这是一个QT Console Project，可以使用QT creator打开（笔者使用的是QT 5.3）
2. 创建一个类继承一个pipeline(推荐继承OurDJIFSMActionPipeline或OurDJIFSMPipeline)
    ```cpp
    
 #include "OurDJI/OurDJI.h"
    class OurActionTestPipeline : public OurDJIFSMActionPipeline{
        ......
    }
    ```
3. 定义Action或使用预定义的Action（在action目录下）

    ```cpp
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
    ```
4. 在main函数中使用这个pipeline
        
    ```cpp
    #include <QCoreApplication>
    #include <QSerialPort>
    #include <QonboardSDK.h>
    #include "OurDJI/pipelineExample/OurActionTestPipeline.h"
    int main(int argc, char *argv[])
    {
        QCoreApplication a(argc, argv);        
        OurDJIPipelineInterface* pipeline = new OurActionTestPipeline();
        pipeline->start(20); // 帧间隔为20ms,就是每秒50帧
        return a.exec();                        
    }
    ```

## pipeline
####什么是pipeline？
使用官方onboard sdk书写飞行控制程序要有一下几个步骤：
- 打开串口
- 激活
- 获得飞行控制权
- 起飞
- **执行飞行控制程序**
- 降落
- 释放飞行控制权
- 关闭串口

实现这个完整的流程就是一个pipeline。
####pipeline模板源代码实现
这些步骤中除了 执行飞行控制程序外，其他的都是和飞控逻辑不相关的内容，而且具有重复性，所以该简单封装提供了几个pipeline作为模板，开发者可以根据需要重写pipeline中相应的步骤，不需要关心的则使用它默认就可以了。<br/>
由于onboard sdk3.0使用的是回调函数的机制，导致代码的可读性和结构变得很差，所以这里pipeline模板基于FSM(Finite-state machine)的思想对这种机制进行了改写。<br/>
这些pipeline源代码在目录pipeline下。<br/>
下面分别说明相关pipeline实现的功能
- OurDJIPipelineInterface
   这个是所有pipeline模板的基类，它实现了pipeline模板的核心功能，包括：<br/>
   
   功能 | 描述 | 主要相关函数
   ---  | --- | ---
   打开串口 | 可以指定打开串口的名称和波特率(baudrate)，默认使用第一个描述符以USB-Serial开头的串口名称和baudrate=230400 | reOpenPort
   激活 | 可以指定激活id(因为3.0后激活的key已经没有作用了)，默认使用id=1023480 | active;pActivationCallback
   获得飞行控制权|onboard3.0获取飞行控制权的命令可能需要发送两次，第一次可能失败|setControl;pSetControlCallBack
   起飞|这里使用onboard SDK提供的自动起飞函数，但是无法判断起飞是否成功，这里使用的判断起飞成功的策略是起飞经过了某个阈值时间同时一秒内飞机高度变化不大|takeoff;pAutoFlightCallBack
   执行飞行控制程序|这里默认没有执行任何动作|whenInMainFlight
   降落|使用onbaordS DK提供的自动降落函数，但是无法判断降落是否完成，这里判断降落完成的策略是降落经过了某个阈值时间同时一秒内飞机高度变化不大|landing;pAutoFlightCallBack
   释放飞行控制权|使用onboard sdk默认的函数|setControl;pSetControlCallBack
   关闭串口|使用onboard sdk默认的函数|closePort
   发送数据|向mobile sdk(手机端)发送字节流数据，但每次发送数据不超过100B |sendDataTOMobile
   接收数据|从mobile sdk(手机端)接收字节流数据, 但每次接收数据不超过100B |getReceiveDataFromMobile
   
   - update函数：整个OurDJIPipelineInterface使用帧刷新的模式，核心就是这个update函数，该函数每一帧都会执行，实现的功能时根据当前所处的状态(比如【起飞】)执行相应的操作。
   
   使用该pipeline,只需要自己创建一个类继承于OurDJIPipelineInterface，重写whenInMainFlight函数，在该函数里写出飞行控制逻辑即可，但是要使用帧刷新的模式去写（后面会解释）。或者可以直接重写update函数。（后面继承于OurDJIPipelineInterface的OurDJIFSMPipeline就是重写了update函数）<br/>

- OurDJIFSMPipeline<br/>
这个类基于有限状态机对OurDJIPipelineInterface进行了改写，其实在OurDJIPipelineInterface中就已经使用了有限状态机的思想对onboard SDK的回调机制进行了改写。<br/>
OurDJIPipelineInterface中声明出了相应的回调函数，在内部进行状态的切换，而主程序每次根据所处的状态执行对应的操作。<br/>
但是OurDJIPipelineInterface只对外提供了处于【执行飞行控制程序】时应该执行的函数whenInMainFlight的接口，所以如果不使用其他状态的默认执行逻辑，就必须重写整个update函数。<br/>
于是OurDJIPipelineInterface对这一点进行了结构优化，它主要是把主要的状态和一个独立的虚函数对应起来，改写某一个状态的逻辑只需要重写该状态对应的虚函数即可。<br/>
以下是部分重写后的update函数<br/>

    ``` cpp
void update(){
                ........
        switch(ourCurrentState){            // 当前所处的状态
        case OUR_WAITTING:
            break;
        case OUR_BEGIN:                     // 暂时无用 1
            ourCurrentState = OUR_WAITTING;
            break;
        case OUR_ACTIVATED:                 // 刚刚激活成功 2
            whenActived();
            break;
        case OUR_OBTAINCONTROL_RUNNING:     // 控制权第一步成功 3
            whenObtainControlRunning();
            break;
            ........
    ```
   
   状态 | 枚举型变量 | 对应的虚函数
   ------ | --- | ---
等待 |OUR_WAITTING| 无(一般等待用户输入时切换到该状态)
开始 |OUR_BEGIN|无(暂时没有用处)
激活完成 |OUR_ACTIVATED|whenActived()
获得控制权第一步成功|OUR_OBTAINCONTROL_RUNNING|whenObtainControlRunning()
获得控制权成功|OUR_OBTAINCONTROL_SUCCESS|whenObtainControlSuccess()
起飞|OUR_TAKEOFF|whenStartTakeOff()
飞行控制程序|OUR_MAINFLIGHT|whenInMainFlight()
降落|OUR_LANDDING|whenStartLanding()
释放控制权第一步成功|OUR_RELEASECONTROL_RUNNING|whenReleaseControlRunning()
释放控制权成功|OUR_RELEASECONTROL_SUCCESS|whenReleaseControlSuccess

    其实对于绝大多数的情况，只需要修改2-3个状态时的逻辑，其他的使用默认的就好。

- OurDJIFSMActionPipeline<br/>
    这个类继承于OurDJIFSMPipeline，所以OurDJIFSMPipeline的特性他也拥有。这个类主要是着眼于飞行控制程序，因此他其实只是重写了whenInMainFlight()这个函数，引入了Action的概念（参考后面Action）
    
- OurDJIDataSolverPipeline<br/>
    这是一个没有完成的类。继承于OurDJIFSMActionPipeline，因为OurDJIPipelineInterface虽然提供和Mobile SDK进行数据交互的接口，但是每次发送的数据有上限，因此这个类的要实现的是数据大小不限的进行数据传输。（通过将数据分片的思想）

####pipeline的帧刷新模式
这个模式的实现是通过QT的基类QObject的函数 void timerEvent(QTimerEvent *) 和int startTimer(int,..)组合形成的定时器来实现的,startTimer开启这个定时器并设定定时器的执行间隔时间，然后timerEvent每隔一个这段时间就会执行一次（默认是1秒执行50次，当然要保证主线程没有阻塞）。<br/>
但是为了对QT解耦合，这个后面修改为不使用QT组件进行实现<br/>
OurDJIPipelineInterface中timerEvent的实现很简单

```cpp
void OurDJIPipelineInterface::timerEvent(QTimerEvent *)
{
    // 尝试从串口中读取信息,并根据信息调用回到函数
    api->readPoll();
    this->update();
    // 尝试把缓存里存下的指令写入串口（每次发送指令都会把数据指令存入缓存，
    // 每次接收到指令的ack指令信号,都会把该指令缓存中移除）
    api->sendPoll();
}
```
所以OurDJIPipelineInterface的核心流程函数update才会是每隔一段时间执行一次，就像游戏里的帧一样，所以成这种模式为帧模式，默认帧率为50（一般会小于50）。<br/>
也因此update里的当前状态对应的函数也是帧调用执行的函数。<br/>

这个模式是由于onboard SDK3本身的设计理念决定的.<br/>通过查看它的源码，我们可以知道api->readpoll()就是从无人机那里获取数据，回调函数也在这里被执行；而api->sendpoll()就是向无人机发送数据，所有的指令真正发送出去都是这个函数执行的时候。<br/>所以要保证能够及时的和无人机进行通讯，就必须保证这两个函数每个一个较短的时间周期就执行一次，就必须采用帧模式或者多线程模式（但是QT的线程保护机制会使得这种方法很麻烦复杂）。

##Action
####帧模式的短时要求和长时间飞行需求间的矛盾
帧模式要求每一帧的任何一个环节都不能执行太长的时间，否则可能会阻塞主线程，但是有时候需求就是要求长时间执行，比如要求飞机向某一个方向飞行2s。在onboard SDKv2中是直接使用usleep来执行,这是一定会阻塞的。

```cpp
int i = 0;
while(i < 1000){
    sendFlyCommand();   // 每个一段时间发送一次飞行指令，保证指令的执行
    i++;
    usleep(2000);      // 沉睡2ms
}
```
为了解决这个矛盾，就有了Action，它的思想时把长时间的任务分解成若干个小任务，每一帧执行一个小任务，直到任务完成。
####一个简单的例子
所有继承OurAction的类都是一个Action。<br/>
Action的核心就是要实现 OurAction里的 纯虚函数virtual bool stepOver(Flight* flight)=0;、<br/>
比如下面就是一个简单的Action，它实现的就是向某一个方向飞100帧的时间，每一帧都会执行stepOver函数。stepOver返回false,表示这个Action还没有结束，下一帧还应该执行这个函数；stepOver返回true,表示这个Action结束，下一帧不再执行这个函数。

```cpp
class OurActionTest : public OurAction{
public:
    OurActionTest(){
        count = 0;
    }
    int count;
    bool stepOver(Flight* flight){
        if(count < 100){
            uint8_t flag = Flight::HORIZONTAL_POSITION | Flight::VERTICAL_VELOCITY | Flight::YAW_ANGLE
                    | Flight::HORIZONTAL_BODY | Flight::SMOOTH_ENABLE;
            flight->control(flag, 5, 0, 0, 0);
        }
        count++;
        if(count <= 100) return false;
        else return true;
    }
};
```
具体例子可以参考action目录下的例子

##OurByteConvertUtil
这个类是一个辅助性的类，因为onboard SDK和mobile SDK之间的交互数据是字节流（java里的byte[]和cpp里的char*)<br/>
onboard SDK应是运行在迷你PC上的cpp实现的程序<br/>
mobile SDK是运行在安卓智能手机上的java实现的程序（或IOS上的Object-C程序）<br/>
所以他们之间的通信不仅是跨设备的，还是跨语言的，所以通信数据统一选用字节流，再通过内存类型转换获得想要的数据类型。<br/>
OurByteConcvertUtil提供简单的数据类型和char*之间的内存类型转方式<br/>
下面例子是将short型数据通过内存类型准换为char*

```cpp
static char* getBytes(short s){
        short* temp = new short[1];
        temp[0] = s;
        char* bytes = (char*)temp;
        return bytes;
    }
```