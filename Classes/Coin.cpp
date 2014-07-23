//
//  Coin.cpp
//  HoldOffer
//
//  Created by liupengtao on 14-7-23.
//
//

#include "Coin.h"
#include "Box.h"

Coin::Coin(){}
Coin::~Coin(){}

bool Coin::init() {
    if (!Sprite::init()) {
        return false;
    }
    return true;
}

Coin* Coin::create(const std::string &filename,const std::string& text) {
    auto coin = new Coin();
    if (coin && coin->init() && coin->initWithFile(filename)) {
        coin->autorelease();
        auto label = LabelTTF::create(text, "", 18);
        auto size = coin->getContentSize();
        label->setPosition(Vec2(size.width / 2,size.height / 2));
        coin->addChild(label);
        return coin;
    }
    CC_SAFE_DELETE(coin);
    return nullptr;
}

bool Coin::isCollisionBox(Box* box) {
    auto sprite = box->getBoxSprite();
    auto size = sprite->getContentSize();
    auto rect = Rect(0,0,size.width,size.height);
    auto relativePos = sprite->convertToNodeSpace(getPosition());
    if (rect.containsPoint(relativePos)) {
        return true;
    }
    return false;
}