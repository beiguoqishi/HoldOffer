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
    reset(1, false);
    return true;
}

void Player::scheduleTask() {
    if (!isPotentialEvaluation) {
        schedule(schedule_selector(Player::generateBall), 0.4f);
        schedule(schedule_selector(Player::doCalculateScore));
    } else {
        schedule(schedule_selector(Player::generateBall), 0.6f);
        schedule(schedule_selector(Player::doPotentialEvaluationTask));
    }
}

void Player::unscheduleTask() {
    unschedule(schedule_selector(Player::generateBall));
    if (!isPotentialEvaluation) {
        unschedule(schedule_selector(Player::doCalculateScore));
    } else {
        unschedule(schedule_selector(Player::doPotentialEvaluationTask));
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
    } else if(barrier == 4) {
        if (seg == 1) {
            text = "阴暗/" + text + "分";
        } else {
            text = "阳光/" + text + "分";
        }
    }
    else {
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
        auto coinSprite = Coin::create(rewardType == 2 ? red_heart : coin,"");
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
        if (barrier == 5) {
            int idx = rand() % 3;
            int unit = roundOfferUnit[round - 1][idx];
            auto piece = Coin::create(coin, StringUtils::format("%dK",unit));
            piece->setScore(unit);
            piece->setPosition(p);
            addChild(piece);
            
            piece->runAction(
                                  Sequence::create(
                                                   MoveTo::create(3, Vec2(p.x,0)),
                                                   CallFuncN::create(CC_CALLBACK_1(Player::ballToDest, this)),
                                                   NULL)
                                  );
            offerPieces.pushBack(piece);
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
}

void Player::nextRound(int round) {
    this->round = round;
    this->offerTotalScore = 0;
    reset(2, false);
}

void Player::reset(int barrier,bool potentialEvaluation) {
    this->barrier = barrier;
    this->isPotentialEvaluation = potentialEvaluation;
    this->elliapsedTime = 0;
    this->totalScore = 0;
    this->potentialTotalScore = 0;
    this->removeAllChildren();
    _eventDispatcher->removeAllEventListeners();
    clearRunningBalls();
    initComponent();
    updateTipByBarrier(barrier);
    scheduleTask();
}

void Player::clearRunningBalls() {
    for(Ball* ball : balls) {
        if (ball) {
            ballToDest(ball);
        }
    }
    balls.clear();
    for (Coin* coin : coins) {
        if (coin) {
            ballToDest(coin);
        }
    }
    coins.clear();
    for(Coin* coin : offerPieces) {
        if (coin) {
            ballToDest(coin);
        }
    }
    offerPieces.clear();
}

PopDialog* Player::createPopDialog() {
	Color4B color = Color4B(153, 153, 153, 127);
	auto pop = PopDialog::create(color);
	return pop;
}

void Player::updateSpriteStatus() {
    for (Ball* ball : balls) {
        if (ball && ball->getPartial() == LEFT_BOX_TAG && ball->isCollisionBox(leftBox)) {
            totalScore += ball->getScore();
            ballToDest(ball);
        }
        if (ball && ball->getPartial() == RIGHT_BOX_TAG && ball->isCollisionBox(rightBox)) {
            totalScore -= ball->getScore();
            ballToDest(ball);
        }
    }
    for (Ball* ball : balls) {
        if (ball && !ball->isRunning()) {
            balls.eraseObject(ball);
        }
    }
    for (Coin* coin : offerPieces) {
        if (coin && (coin->isCollisionBox(leftBox) || coin->isCollisionBox(rightBox))) {
            offerTotalScore += coin->getScore();
            ballToDest(coin);
        }
    }
    for (Coin* coin : offerPieces) {
        if (coin && !coin->isRunning()) {
            offerPieces.eraseObject(coin);
        }
    }
}

void Player::updateTipByBarrier(int barrier) {
    switch (barrier) {
        case 1:
        {
            failureTip ="简历投递失败,用时超限！";
            failureAction = "重新开始";
            successTip = "简历投递成功";
            successAction = "开始笔试";
            processFormatTip = "简历完成:%.0f%%";
        }
            break;
        case 2:
        {
            failureTip ="笔试未通过,用时超限！";
            failureAction = "潜力测评";
            successTip = "笔试通过";
            successAction = "开始面试";
            processFormatTip = "笔试分数:%.0f";
        }
            break;
		case 3:
		{
			failureTip = "面试未通过，用时超限！";
			failureAction = "潜力测试";
			successTip = "面试通过";
			successAction = "性格面试";
			processFormatTip = "面试分数：%.0f";
		}
			break;
		case 4:
		{
			failureTip = "性格面试未通过，用时超限！";
			failureAction = "潜力测试";
			successTip = "性格面试通过";
			successAction = "谈Offer";
			processFormatTip = "性格面试分数：%.0f";
		}
			break;
        default:
            break;
    }
}

void Player::doCalculateScore(float dt) {
    elliapsedTime += dt;
    updateScoreTip(TIME_TIP_TAG, "用时:%2.1f",elliapsedTime, VisibleRect::center() - Vec2(0,30));

    updateSpriteStatus();

    if (barrier == 5) {
        updateScoreTip(OFFER_SCORE_TIP_TAG,"工资:%.0fK", offerTotalScore, VisibleRect::center());
    } else {
        updateScoreTip(BARRIER_SCORE_TIP_TAG, (processFormatTip), totalScore, VisibleRect::center());
    }

    if (elliapsedTime >= 20.f) {
        barrierTimeout(failureTip,failureAction);
        return;
    }
    if (barrier != 5 && totalScore >= 20) {
        barrierSuccess(successTip, successAction);
    }
}

void Player::barrierTimeout(const string& failureTip,const string& failureAction) {
    unscheduleTask();
    clearRunningBalls();
    removeTipOnPop();
    auto pop = createPopDialog();
    Menu* menu = nullptr;
    if (barrier < 5) {
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
        menu = Menu::create(nextMenu,nullptr);
    } else {
        auto label = Label::createWithSystemFont(StringUtils::format("您获取的第 %d 个offer是 %d K",round,offerTotalScore), "", 28);
        label->setPosition(VisibleRect::center() + Vec2(0,30));
        pop->addChild(label);
        MenuItemFont* nextOffer = nullptr;
        auto shareMenu = share();
        if (round < 3) {
            nextOffer =MenuItemFont::create("寻找下一个offer", [=](Ref* node){
                nextRound(++round);
            });
        }
        
        if (nextOffer) {
            menu = Menu::create(nextOffer,shareMenu,nullptr);
        } else {
            menu = Menu::create(shareMenu,nullptr);
        }
    }
    
	menu->alignItemsVerticallyWithPadding(10.f);
    menu->setGlobalZOrder(1);
    menu->setPosition(VisibleRect::center() - Vec2(0,60));
    pop->addChild(menu);
    pop->setPosition(Vec2::ZERO);
    addChild(pop,2);
}

void Player::removeTipOnPop() {
    removeTipOnPopByTag(BARRIER_SCORE_TIP_TAG);
    removeTipOnPopByTag(TIME_TIP_TAG);
    removeTipOnPopByTag(POTENTIAL_SCORE_TIP_TAG);
    removeTipOnPopByTag(POTENTIAL_LIFE_TIP_TAG);
    removeTipOnPopByTag(OFFER_SCORE_TIP_TAG);
}

void Player::removeTipOnPopByTag(int tag) {
    auto node = getChildByTag(tag);
    if (node) {
        node->removeFromParent();
    }
}

void Player::barrierSuccess(const string& successTip,const string& successAction) {
    totalScore = firstBarrierPassScore;
    unscheduleTask();
    
    clearRunningBalls();
    
    removeTipOnPop();
    
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

void Player::updateScoreTip(int tag,const string& format,float score,const Vec2& p) {
    auto label = static_cast<Label*>(getChildByTag(tag));
	if (label) {
		label->setString(StringUtils::format(const_cast<char*>(format.c_str()), score));
	}
	else {
		label = Label::createWithSystemFont(StringUtils::format(const_cast<char*>(format.c_str()), score), "Arial", 25);
		label->setTag(tag);
		label->setPosition(p);
		addChild(label);
	}
}

MenuItemFont* Player::share() {
    auto shareMenu = MenuItemFont::create("分享", [=](Ref* node){
    });
    return shareMenu;
}

void Player::reducePotentialLife(Node* node) {
	Coin* coin = static_cast<Coin*>(node);
	if (coin->getRewardType() != 2) {
		potentialLife--;
	}
	
    updateScoreTip(POTENTIAL_LIFE_TIP_TAG, "生命值:%.0f", potentialLife, VisibleRect::center());
	if (potentialLife <= 0) {
        clearRunningBalls();
        removeTipOnPop();
        unscheduleTask();
		potentialLife = 0;
		auto pop = createPopDialog();

		auto label = Label::createWithSystemFont(StringUtils::format("您的潜力测评分数:%d",potentialTotalScore), "", 28);
		label->setPosition(VisibleRect::center() + Vec2(0, 30));
		pop->addChild(label);

		auto replayMenu = MenuItemFont::create("重新开始", [=](Ref* node){
			reset(barrier, false);
		});
        replayMenu->setPosition(Vec2::ZERO);
		auto shareMenu = share();
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
				if (this->potentialLife >= 3) {
					this->potentialLife = 3;
				}
				updateScoreTip(POTENTIAL_LIFE_TIP_TAG, "生命值:%.0f", potentialLife, VisibleRect::center());
			}
            updateScoreTip(POTENTIAL_SCORE_TIP_TAG, "分数:%.0f", potentialTotalScore, VisibleRect::center() - Vec2(0, 30));
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
