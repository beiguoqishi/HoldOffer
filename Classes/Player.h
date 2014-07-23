//
//  Player.h
//  HoldOffer
//
//  Created by liupengtao on 14-7-19.
//
//

#ifndef __HoldOffer__Player__
#define __HoldOffer__Player__

#include "cocos2d.h"
#include "VisibleRect.h"

USING_NS_CC;
class Ball;
class Box;
class Coin;

class Player : public Layer {
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(Player);
    Player();
    virtual ~Player();
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool transformUpdated) override;
    void generateBall(float dt);
    virtual void onEnter() override;
protected:
    void onDraw(const cocos2d::Mat4 &transform, bool transformUpdated);
	cocos2d::CustomCommand _customCommand;
    
private:
    void initComponent();
    void ballToDest(Node* node);
    void reset(int barrier,bool potentialEvaluation);
    void scheduleTask();
    void unscheduleTask();
    void clearRunningBalls();
    const int barrierScore[3] = {2,3,5};
    EventListenerTouchOneByOne* listener;
    enum NodeTag {
        LEFT_BOX_TAG,
        RIGHT_BOX_TAG,
        BARRIER_SCORE_TIP_TAG,
        TIME_TIP_TAG,
        POTENTIAL_SCORE_TIP_TAG
    };
    
    const Vec2 positiveChannel[3] = {
        Vec2(VisibleRect::getVisibleWidth() / 8,VisibleRect::getVisibleHeight()),
                Vec2(VisibleRect::getVisibleWidth() * 2 / 8,VisibleRect::getVisibleHeight()),
                Vec2(VisibleRect::getVisibleWidth() * 3 / 8,VisibleRect::getVisibleHeight())
    };
    const Vec2 negativeChannel[3] = {
        Vec2(VisibleRect::getVisibleWidth() * 5 / 8,VisibleRect::getVisibleHeight()),
        Vec2(VisibleRect::getVisibleWidth() * 6 / 8,VisibleRect::getVisibleHeight()),
        Vec2(VisibleRect::getVisibleWidth() * 7 / 8,VisibleRect::getVisibleHeight())
    };
    Vec2 initBoxPos;
    void doCalculateScore(float dt);
    void doPotentialEvaluationTask(float dt);
    Vector<Ball*> balls;
    Vector<Coin*> coins;
    Box* leftBox;
    Box* rightBox;
    int barrier = 1;
    int totalScore = 0;
    int potentialTotalScore = 0;
    const int firstBarrierPassScore = 100;
    const int firstBarrierTimeLimit = 10;
    float elliapsedTime = 0;
    bool isPotentialEvaluation = false;
    const int firstRoundPotentialScore[3] = {2,3,4};
    const int secondRoundPotentialScore[3] = {4,5,6};
    const int thirdRoundPotentialScore[3] = {7,8,9};
    int round = 1;
    const std::string coin = "coin.png";
    const std::string red_heart = "red_heart.png";
    const int redHeartRate = 8;
    int potentialLife = 3;
};

#endif /* defined(__HoldOffer__Player__) */
