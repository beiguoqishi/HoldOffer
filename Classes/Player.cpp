//
//  Player.cpp
//  HoldOffer
//
//  Created by liupengtao on 14-7-19.
//
//
#include "Player.h"
#include "VisibleRect.h"
#include "Box.h"
#include "Ball.h"
#include "PopDialog.h"

Player::Player() {
    
}

Player::~Player(){
    
}

Scene* Player::createScene() {
    auto scene = Scene::create();
    auto layer = Player::create();
    scene->addChild(layer);
    return scene;
}

bool Player::init() {
    if (!Layer::init()) {
        return false;
    }
    initComponent();
    schedule(schedule_selector(Player::generateBall), 0.4f);
    schedule(schedule_selector(Player::doCalculateScore));
    return true;
}

void Player::initComponent() {
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
        listener->onTouchBegan = [=](Touch* touch,Event* event) {
            CCLOG("Player::onTouchBegain");
            auto node = event->getCurrentTarget();
            int tag = node->getTag();
            if (tag == NodeTag::LEFT_BOX_TAG || tag == NodeTag::RIGHT_BOX_TAG) {
                auto box = static_cast<Box*>(node);
                auto sprite = box->getBoxSprite();
                CCLOG("Player::NodeTag:%d",tag);
                auto locationInNode = sprite->convertToNodeSpace(touch->getLocation());
                auto s = sprite->getContentSize();
                auto r = Rect(0,0,s.width,s.height);
                CCLOG("Player::target x:%f,target y:%f",locationInNode.x,locationInNode.y);
                CCLOG("Player::target size,width:%f,height:%f",s.width,s.height);
                if (r.containsPoint(locationInNode)) {
                    initBoxPos = box->getPosition();
                    return true;
                }
            }
            return false;
        };
    
        listener->onTouchMoved = [=](Touch* touch,Event* event) {
            CCLOG("Player::onTouchMoved");
            auto node = event->getCurrentTarget();
            int tag = node->getTag();
            CCLOG("Player::moved::tag::%d",tag);
            if (tag == NodeTag::LEFT_BOX_TAG || tag == NodeTag::RIGHT_BOX_TAG) {
                CCLOG("Player::NodeTag:%d",tag);
                auto box = static_cast<Box*>(event->getCurrentTarget());
                auto boxWidth = box->getBoxSprite()->getContentSize().width;
                auto halfWidth = (VisibleRect::getVisibleWidth() - 5) /2;
                
                auto pos = Vec2((node->getPosition() + touch->getDelta()).x,initBoxPos.y);
                
                if (tag == NodeTag::LEFT_BOX_TAG) {
                    if (pos.x < boxWidth / 2) {
                        pos.x = boxWidth / 2;
                    }
                    if (pos.x > halfWidth - boxWidth / 2) {
                        pos.x = halfWidth - boxWidth / 2;
                    }
                }
                
                if (tag == NodeTag::RIGHT_BOX_TAG) {
                    if (pos.x < halfWidth + 5 + boxWidth / 2) {
                        pos.x = halfWidth + 5 + boxWidth / 2;
                    }
                    if (pos.x > halfWidth * 2 + 5 - boxWidth / 2) {
                        pos.x = halfWidth * 2 + 5 - boxWidth / 2;
                    }
                }
                
                node->setPosition(pos);
            }
        };

    CCLOG("Player::initComponent");
    leftBox = Box::create();
    leftBox->setPosition(Vec2(VisibleRect::getVisibleRect().origin.x + VisibleRect::getVisibleWidth() / 4,VisibleRect::getVisibleRect().origin.y + 20));
    leftBox->setTag(NodeTag::LEFT_BOX_TAG);
    
    addChild(leftBox,1);
    
    rightBox = Box::create();
    rightBox->setPosition(Vec2((VisibleRect::getVisibleWidth() / 4) * 3,20));
    
    rightBox->setTag(NodeTag::RIGHT_BOX_TAG);
    addChild(rightBox);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(),leftBox);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), rightBox);

}

void Player::generateBall(float dt) {
    int seg = rand() % 2;
    std::string text = "";
    int score = 0;
    if (barrier == 1) {
        score =firstBarrierScore[rand() % 3];
        text = StringUtils::format("%d",score);
        if (seg == 1) {
            text = StringUtils::format("-%d",score);
        }
    }
    
    Vec2 p = positiveChannel[rand() % 3];
    
    if (seg == 1) {
        p = negativeChannel[rand() % 3];
    }
    auto ball = Ball::create(Color4F(CCRANDOM_0_1(),CCRANDOM_0_1(),CCRANDOM_0_1(),1), Color4F(CCRANDOM_0_1(),CCRANDOM_0_1(),CCRANDOM_0_1(),1), text);
    ball->setScore(score);
    ball->setPartial(LEFT_BOX_TAG);
    if (seg == 1) {
        ball->setPartial(RIGHT_BOX_TAG);
    }
    
    ball->setPosition(p);
    addChild(ball);
    
    ball->runAction(Sequence::create(MoveTo::create(3, Vec2(p.x,0)),CallFuncN::create(CC_CALLBACK_1(Player::ballToDest, this)),NULL));
    balls.pushBack(ball);
}

void Player::doCalculateScore(float dt) {
    for (Ball* ball : balls) {
        if (ball && ball->getPartial() == LEFT_BOX_TAG && ball->isCollisionBox(leftBox)) {
            totalScore += ball->getScore();
            ballToDest(ball);
            ball = nullptr;
            CCLOG("Player::leftBox");
        }
        if (ball && ball->getPartial() == RIGHT_BOX_TAG && ball->isCollisionBox(rightBox)) {
            totalScore -= ball->getScore();
            ballToDest(ball);
            ball = nullptr;
            CCLOG("Player::rightBox");
        }
    }
    
    switch (barrier) {
        case 1:
        {
            if (totalScore >= 5) {
                totalScore = firstBarrierPassScore;
                unschedule(schedule_selector(Player::doCalculateScore));
                unschedule(schedule_selector(Player::generateBall));
                for(Ball* ball : balls) {
                    if (ball) {
                        ball->stopAllActions();
//                        ballToDest(ball);
//                        ball = nullptr;
                    }
                }
                Color4B color = Color4B(153,153,153,127);
                auto pop = PopDialog::create(color);
                
                auto label = Label::createWithSystemFont("简历投递成功", "", 28);
                label->setPosition(VisibleRect::center() + Vec2(0,30));
                pop->addChild(label);
                
                auto nextMenu = MenuItemFont::create("开始笔试", [=](Ref* node){
                    CCLOG("Player::Click 开始笔试");
                });
                auto menu = Menu::create(nextMenu,nullptr);
                menu->setGlobalZOrder(1);
                menu->setPosition(VisibleRect::center() - Vec2(0,60));
                pop->addChild(menu);
                
                pop->setPosition(Vec2::ZERO);
                addChild(pop,2);
            }
            auto label = static_cast<Label*>(getChildByTag(FIRST_BARRIER_SCORE_TIP_TAG));
            if (label) {
                label->setString(StringUtils::format("简历完成:%d%%",totalScore));
            } else {
                label = Label::createWithSystemFont(StringUtils::format("简历完成:%d%%",totalScore), "Arial", 25);
                label->setTag(FIRST_BARRIER_SCORE_TIP_TAG);
                label->setPosition(VisibleRect::center());
                addChild(label);
            }
        }
        break;
        default:
            break;
    }
}

void Player::ballToDest(Node* node) {
    node->removeFromParent();
    Ball* ball = static_cast<Ball*>(node);
    balls.eraseObject(ball);
}

void Player::onEnter() {
    Layer::onEnter();
    CCLOG("Player::onEnter");
}

void Player::onDraw(const Mat4 &transform, bool transformUpdated) {
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
    glLineWidth(5);
    
    DrawPrimitives::setDrawColor4B(255, 0, 0, 255);
	DrawPrimitives::drawLine(VisibleRect::top(),VisibleRect::bottom());
    
    
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void Player::draw(Renderer *renderer, const Mat4 &transform, bool transformUpdated) {
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(Player::onDraw, this, transform, transformUpdated);
	renderer->addCommand(&_customCommand);
}
