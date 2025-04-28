//
// Created by thehao on 2025/4/26.
//

#include <random>
#include "Card.h"
#include "Log.h"

void Card::initCards() {

    Debug("initCards.......");

    // 清空牌组
    m_cards.clear();

    // 加入普通卡牌
    for (int i = 1; i <= 4; ++i) {
    // 花色
        for (int j = 1; j <= 13; ++j) {
        // 点数
            m_cards.insert(std::make_pair(i,j));
        }
    }

    // 加入大小王
    m_cards.insert(std::make_pair(0,14));
    m_cards.insert(std::make_pair(0,15));
}

Card::CardInfo Card::takeOneCard() {

    // 创建随机设备对象，用于获取随机种子
    std::random_device rd;
    // 创建 mt19937 模块（梅森旋转算法），并用随机设备初始化种子
    std::mt19937 gen(rd());
    // 定义均匀分布的整数范围
    std::uniform_int_distribution<> dis(0, m_cards.size()-1);
    // 生成随机数
    int randNum = dis(gen);

    // 获取迭代器并保存返回值
    auto it = m_cards.begin();
    std::advance(it, randNum);  // 或直接使用随机访问迭代器操作
    Card::CardInfo card = *it;

    // 移除元素并返回
    m_cards.erase(it);

//    // 选取随机卡牌，并存m_cards移除
//    auto it = m_cards.begin();
//    while (randNum--) ++it;
//    m_cards.erase(it);

    return card;
}

std::string Card::getHandCards() {

    Debug("getHandCards.......    m_cards.size:%s", std::to_string(m_cards.size()).c_str());
    std::string handCards;
    int handCardsNum = m_cards.size() - 3;
    for (int i = 0; i < handCardsNum; ++i) {
        auto card = takeOneCard();
        handCards += std::to_string(card.first) + "-" + std::to_string(card.second) + "#";
    }
    Debug("getHandCards.......    m_cards.size:%s", std::to_string(m_cards.size()).c_str());
    return handCards;
}

std::string Card::getLastCards() {
    Debug("getLastCards.......    m_cards.size:%s", std::to_string(m_cards.size()).c_str());
    std::string lastCards;
    int lastCardsNum = m_cards.size();
    for (int i = 0; i < lastCardsNum; ++i) {
        auto card = takeOneCard();
        lastCards += std::to_string(card.first) + "-" + std::to_string(card.second) + "#";
    }
    Debug("getLastCards.......    m_cards.size:%s", std::to_string(m_cards.size()).c_str());

    return lastCards;
}
