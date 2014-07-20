//
//  Ball.cpp
//  HoldOffer
//
//  Created by liupengtao on 14-7-20.
//
//

#include "Ball.h"
#include "Box.h"

Ball::Ball(){}

Ball::~Ball(){}

Ball* Ball::create(const Color4F& bColor, const Color4F& iColor,const std::string& text) {
    Ball* ball = new Ball();
    ball->borderColor = bColor;
    ball->innerColor = iColor;
    ball->text = text;
    if (ball && ball->init()) {
        ball->autorelease();
        return ball;
    } else {
        delete ball;
        ball = NULL;
        return NULL;
    }
}

bool Ball::init() {
    if (!Node::init()) {
        return false;
    }
    auto draw = DrawNode::create();
    addChild(draw);
    draw->drawDot(Vec2::ZERO,outerRadius,borderColor);
    draw->drawDot(Vec2::ZERO, innerRadius, innerColor);
    
    auto label = LabelTTF::create(text, "Arial", 24);
    label->setColor(Color3B(0,255,0));
    addChild(label);
    
    return true;
}

bool Ball::isCollisionBox(Box* box) {
    auto sprite = box->getBoxSprite();
    auto size = sprite->getContentSize();
    auto rect = Rect(0,0,size.width,size.height);
    auto relativePos = sprite->convertToNodeSpace(getPosition());
    if (rect.containsPoint(relativePos)) {
        return true;
    }
    return false;
}

//void Ball::onDraw(const Mat4 &transform, bool transformUpdated) {
//	Director* director = Director::getInstance();
//	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
//	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
////    glLineWidth(8);
////    
////    DrawPrimitives::setDrawColor4B(255, 0, 0, 255);
////	DrawPrimitives::drawCircle(Vec2::ZERO, 20,CC_DEGREES_TO_RADIANS(90),50,false);
////    
////    DrawPrimitives::setDrawColor4B(255, 255, 0, 255);
////    DrawPrimitives::drawSolidCircle(Vec2::ZERO, 20, CC_DEGREES_TO_RADIANS(90), 50);
//    
//    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
//}
//
//void Ball::draw(Renderer *renderer, const Mat4 &transform, bool transformUpdated) {
//	_customCommand.init(_globalZOrder);
//	_customCommand.func = CC_CALLBACK_0(Ball::onDraw, this, transform, transformUpdated);
//	renderer->addCommand(&_customCommand);
//}

