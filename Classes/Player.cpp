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
#include "Coin.h"

using namespace std;

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
    scheduleTask();
    return true;
}

void Player::scheduleTask() {
    if (!isPotentialEvaluation) {
        schedule(schedule_selector(Player::generateBall), 0.4f);
        schedule(schedule_selector(Player::doCalculateScore));
    } else {
        
    }
}

void Player::unscheduleTask() {
    unschedule(schedule_selector(Player::doCalculateScore));
    if (!isPotentialEvaluation) {
        unschedule(schedule_selector(Player::generateBall));
    } else {
        
    }
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
    int idx = rand() % 3;
    int score = barrierScore[idx];
    std::string text = StringUtils::format("%d",score);
    if (seg == 1) {
        text = StringUtils::format("-%d",score);
    }
    if (barrier == 1) {
        text += "%";
    } else {
        text += "分";
    }
    
    Vec2 p = positiveChannel[rand() % 3];
    
    if (seg == 1) {
        p = negativeChannel[rand() % 3];
    }
    
    if (isPotentialEvaluation) {
        switch (round) {
            case 1:
                score = firstRoundPotentialScore[idx];
                break;
            case 2:
                score = secondRoundPotentialScore[idx];
                break;
            case 3:
                score = thirdRoundPotentialScore[idx];
                break;
            default:
                break;
        }
        text = StringUtils::format("%d分",score);
        int rewardType = (rand() % redHeartRate) == 4 ? 2 : 1;
        auto coinSprite = Coin::create(rewardType == 2 ? red_heart : coin,text);
        coinSprite->setRewardType(rewardType);
        coinSprite->setScore(score);
        coinSprite->setPosition(p);
        addChild(coinSprite);
        
        coinSprite->runAction(
			Sequence::create(
				MoveTo::create(3, Vec2(p.x,0)),
				CallFuncN::create(CC_CALLBACK_1(Player::ballToDest, this)),
				CallFuncN::create(CC_CALLBACK_1(Player::reducePotentialLife,this)),
				NULL)
			);
        coins.pushBack(coinSprite);
    } else {
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
}

void Player::reset(int barrier,bool potentialEvaluation) {
    this->barrier = barrier;
    this->isPotentialEvaluation = potentialEvaluation;
    this->elliapsedTime = 0;
    this->totalScore = 0;
    this->removeAllChildren();
    _eventDispatcher->removeAllEventListeners();
    balls.clear();
    initComponent();
    scheduleTask();
}

void Player::clearRunningBalls() {
    for(Ball* ball : balls) {
        if (ball) {
            ballToDest(ball);
        }
    }
    balls.clear();
}

PopDialog* Player::createPopDialog() {
	Color4B color = Color4B(153, 153, 153, 127);
	auto pop = PopDialog::create(color);
	return pop;
}

void Player::doCalculateScore(float dt) {
    elliapsedTime += dt;
    auto label = static_cast<Label*>(getChildByTag(TIME_TIP_TAG));
    if (label) {
        label->setString(StringUtils::format("用时:%2.1f",elliapsedTime));
    } else {
        label = Label::createWithSystemFont(StringUtils::format("用时:%2.1f",elliapsedTime), "Arial", 25);
        label->setTag(TIME_TIP_TAG);
        label->setPosition(VisibleRect::center() - Vec2(0,30));
        addChild(label);
    }
    for (Ball* ball : balls) {
        if (ball && ball->getPartial() == LEFT_BOX_TAG && ball->isCollisionBox(leftBox)) {
            totalScore += ball->getScore();
            ballToDest(ball);
            CCLOG("Player::leftBox");
        }
        if (ball && ball->getPartial() == RIGHT_BOX_TAG && ball->isCollisionBox(rightBox)) {
            totalScore -= ball->getScore();
            ballToDest(ball);
            CCLOG("Player::rightBox");
        }
    }
    for (Ball* ball : balls) {
        if (!ball->isRunning()) {
            balls.eraseObject(ball);
        }
    }
    std::string failureTip,failureAction,successTip,successAction;
    char* processFormatTip;
    switch (barrier) {
        case 1:
        {
            failureTip ="简历投递失败,用时超限！";
            failureAction = "重新开始";
            successTip = "简历投递成功";
            successAction = "开始笔试";
            processFormatTip = const_cast<char*>(string("简历完成:%d%%").c_str());
        }
        break;
        case 2:
        {
            failureTip ="笔试未通过,用时超限！";
            failureAction = "潜力测评";
            successTip = "笔试通过";
            successAction = "开始面试";
            processFormatTip = const_cast<char*>(string("笔试分数:%d").c_str());
        }
        break;
		case 3:
		{
			failureTip = "面试未通过，用时超限！";
			failureAction = "潜力测试";
			successTip = "面试通过";
			successAction = "性格面试";
			processFormatTip = const_cast<char*>(string("面试分数：%d").c_str());
		}
			break;
		case 4:
		{
			failureTip = "性格面试未通过，用时超限！";
			failureAction = "潜力测试";
			successTip = "性格面试通过";
			successAction = "谈Offer";
			processFormatTip = const_cast<char*>(string("性格面试分数：%d").c_str());
		}
			break;
        default:
            break;
    }
    
    if (elliapsedTime >= 20.f) {
        unscheduleTask();
        clearRunningBalls();
        
		auto pop = createPopDialog();
        
        auto label = Label::createWithSystemFont(failureTip, "", 28);
        label->setPosition(VisibleRect::center() + Vec2(0,30));
        pop->addChild(label);
        
        auto nextMenu = MenuItemFont::create(failureAction, [=](Ref* node){
            if (barrier > 1) {
                reset(barrier,true);
            } else {
                reset(barrier,false);
            }
        });
        auto menu = Menu::create(nextMenu,nullptr);
        menu->setGlobalZOrder(1);
        menu->setPosition(VisibleRect::center() - Vec2(0,60));
        pop->addChild(menu);
        
        pop->setPosition(Vec2::ZERO);
        addChild(pop,2);
        return;
    }
    if (totalScore >= 20) {
        totalScore = firstBarrierPassScore;
        unscheduleTask();
        
        clearRunningBalls();
        
		auto pop = createPopDialog();
        
        auto label = Label::createWithSystemFont(successTip, "", 28);
        label->setPosition(VisibleRect::center() + Vec2(0,30));
        pop->addChild(label);
        
        auto nextMenu = MenuItemFont::create(successAction, [=](Ref* node){
            reset(++barrier,false);
        });
        auto menu = Menu::create(nextMenu,nullptr);
        menu->setGlobalZOrder(1);
        menu->setPosition(VisibleRect::center() - Vec2(0,60));
        pop->addChild(menu);
        
        pop->setPosition(Vec2::ZERO);
        addChild(pop,2);
    }
    auto processLabel = static_cast<Label*>(getChildByTag(BARRIER_SCORE_TIP_TAG));
    if (processLabel) {
        processLabel->setString(StringUtils::format(processFormatTip,totalScore));
    } else {
        processLabel = Label::createWithSystemFont(StringUtils::format(processFormatTip,totalScore), "Arial", 25);
        processLabel->setTag(BARRIER_SCORE_TIP_TAG);
        processLabel->setPosition(VisibleRect::center());
        addChild(processLabel);
    }
}

void Player::updateLifeTip() {
	auto lifeTip = static_cast<Label*>(getChildByTag(POTENTIAL_LIFE_TIP_TAG));
	if (lifeTip) {
		lifeTip->setString(StringUtils::format("生命值:%d", potentialLife));
	}
	else {
		lifeTip = Label::createWithSystemFont(StringUtils::format("生命值:%d", potentialLife), "", 25);
		lifeTip->setTag(POTENTIAL_LIFE_TIP_TAG);
		lifeTip->setPosition(VisibleRect::center());
		addChild(lifeTip);
	}
}

void Player::updatePotentialScoreTip() {
	auto label = static_cast<Label*>(getChildByTag(POTENTIAL_SCORE_TIP_TAG));
	if (label) {
		label->setString(StringUtils::format("分数:%d", potentialTotalScore));
	}
	else {
		label = Label::createWithSystemFont(StringUtils::format("分数:%d", potentialTotalScore), "", 25);
		label->setTag(POTENTIAL_SCORE_TIP_TAG);
		label->setPosition(VisibleRect::center() - Vec2(0, 30));
		addChild(label);
	}
}

void Player::reducePotentialLife(float dt) {
	potentialLife--;
	updateLifeTip();
	if (potentialLife <= 0) {
		potentialLife = 0;
		auto pop = createPopDialog();

		auto label = Label::createWithSystemFont(StringUtils::format("您的潜力测评分数:%d",potentialTotalScore), "", 28);
		label->setPosition(VisibleRect::center() + Vec2(0, 30));
		pop->addChild(label);

		auto replayMenu = MenuItemFont::create("重新开始", [=](Ref* node){
			reset(1, false);
		});
		auto shareMenu = MenuItemFont::create("分享", [=](Ref* node){
		});
		auto menu = Menu::create(replayMenu, shareMenu,nullptr);

		menu->setGlobalZOrder(1);
		menu->setPosition(VisibleRect::center() - Vec2(0, 60));
		pop->addChild(menu);

		pop->setPosition(Vec2::ZERO);
		addChild(pop, 2);
	}
}

void Player::doPotentialEvaluationTask(float dt) {
    for (Coin* coin : coins) {
        if (coin && (coin->isCollisionBox(leftBox) || coin->isCollisionBox(rightBox))) {
            potentialTotalScore += coin->getScore();
            ballToDest(coin);
			if (coin->getRewardType() == 2) {
				this->potentialLife++;
				this->updateLifeTip();
			}
			updatePotentialScoreTip();
        }
    }
    for (Coin* coin : coins) {
        if (!coin->isRunning()) {
            coins.eraseObject(coin);
        }
    }
}

void Player::ballToDest(Node* node) {
    node->stopAllActions();
    node->removeFromParent();
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
