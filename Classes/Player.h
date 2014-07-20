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
    const std::string firstBarrierScore[3] = {"2%","3%","5%"};
    EventListenerTouchOneByOne* listener;
    enum NodeTag {
        LEFT_BOX_TAG,
        RIGHT_BOX_TAG
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
    Vector<Ball*> balls;
    Box* leftBox;
    Box* rightBox;
};

#endif /* defined(__HoldOffer__Player__) */
