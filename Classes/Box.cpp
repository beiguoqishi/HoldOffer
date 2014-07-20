//
//  Box.cpp
//  HoldOffer
//
//  Created by liupengtao on 14-7-19.
//
//

#include "Box.h"

Box::Box(){}

Box::~Box(){}

bool Box::init() {
    if (!Layer::init()) {
        return false;
    }
    
    auto boxSprite = Sprite::create("box.png");
    boxSprite->setAnchorPoint(Vec2(0.5,0));
    boxSprite->setPosition(Vec2::ZERO);
    addChild(boxSprite);
    
    setBoxSprite(boxSprite);
    
    return true;
}