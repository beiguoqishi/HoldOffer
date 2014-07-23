//
//  Coin.h
//  HoldOffer
//
//  Created by liupengtao on 14-7-23.
//
//

#ifndef __HoldOffer__Coin__
#define __HoldOffer__Coin__

#include "cocos2d.h"

USING_NS_CC;

class Box;

class Coin : public Sprite {
public:
    virtual bool init();
    static Coin* create(const std::string& filename,const std::string& text);
    Coin();
    virtual ~Coin();
    bool isCollisionBox(Box* box);
    CC_SYNTHESIZE(int, rewardType, RewardType);
    CC_SYNTHESIZE(int, score, Score);
};

#endif /* defined(__HoldOffer__Coin__) */
