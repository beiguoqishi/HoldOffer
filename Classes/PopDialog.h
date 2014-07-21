//
//  PopDialog.h
//  HoldOffer
//
//  Created by liupengtao on 14-7-21.
//
//

#ifndef __HoldOffer__PopDialog__
#define __HoldOffer__PopDialog__

#include "cocos2d.h"
USING_NS_CC;

class PopDialog : public LayerColor {
public:
    static PopDialog* create(const Color4B& color);
    virtual bool init();
    PopDialog();
    virtual ~PopDialog();
};

#endif /* defined(__HoldOffer__PopDialog__) */
