//
//  PopDialog.cpp
//  HoldOffer
//
//  Created by liupengtao on 14-7-21.
//
//

#include "PopDialog.h"

PopDialog::PopDialog(){}
PopDialog::~PopDialog(){}

bool PopDialog::init() {
    if (!LayerColor::init()) {
        return false;
    }
    CCLOG("Player::PopDialog::init");
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [=](Touch* touch,Event* event) {
        CCLOG("Player::PopDialog::TouchBegain");
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
    return true;
}

PopDialog* PopDialog::create(const Color4B& color) {
    auto ret = new PopDialog();
    if (ret && ret->init() && ret->initWithColor(color)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    ret = nullptr;
}