//
// Created by thehao on 2025/4/26.
//

#ifndef DDZ_SERVER_CARD_H
#define DDZ_SERVER_CARD_H
#include <map>

class Card {

public:

    // 初始化牌组信息
    void initCards();

    // 随机获取一张卡牌
    using CardInfo = std::pair<int, int>;
    CardInfo takeOneCard();

    // 返回手牌信息
    std::string getHandCards();

    // 返回底牌信息
    std::string getLastCards();

private:
    // 牌组信息
    std::multimap<int, int> m_cards;

};


#endif //DDZ_SERVER_CARD_H
