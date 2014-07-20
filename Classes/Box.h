//
//  Box.h
//  HoldOffer
//
//  Created by liupengtao on 14-7-19.
//
//

#ifndef __HoldOffer__Box__
#define __HoldOffer__Box__

#include "cocos2d.h"

USING_NS_CC;

class Box : public Layer {
public:
    virtual bool init();
    CREATE_FUNC(Box);
    Box();
    virtual ~Box();
    CC_SYNTHESIZE(Sprite*, m_sprite, BoxSprite)
};

#endif /* defined(__HoldOffer__Box__) */
