//
//  Ball.h
//  HoldOffer
//
//  Created by liupengtao on 14-7-20.
//
//

#ifndef __HoldOffer__Ball__
#define __HoldOffer__Ball__

#include "cocos2d.h"

USING_NS_CC;
class Box;
class Ball : public Node {
public:
    virtual bool init();
    static Ball* create(const Color4F& bColor,const Color4F& iColor,const std::string& text);
    CREATE_FUNC(Ball);
    Ball();
    virtual ~Ball();
    bool isCollisionBox(Box* box);
//    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool transformUpdated) override;
protected:
//    void onDraw(const cocos2d::Mat4 &transform, bool transformUpdated);
//	cocos2d::CustomCommand _customCommand;
private:
    Color4F borderColor = Color4F(1,0,0,1);
    Color4F innerColor = Color4F(1,1,0,1);
    std::string text = "";
    const float outerRadius = 20;
    const float innerRadius = 18;
    CC_SYNTHESIZE(int, partial, Partial);
};

#endif /* defined(__HoldOffer__Ball__) */
