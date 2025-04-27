//
// Created by thehao on 2025/4/26.
//

#include <random>
#include "Card.h"

void Card::initCards() {

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
    // 定义均匀分布的整数范围为 100000 - 999999
    std::uniform_int_distribution<> dis(0, m_cards.size());
    // 生成六位随机数
    int randNum = dis(gen);

    // 选取随机卡牌，并存m_cards移除
    auto it = m_cards.begin();
    while (randNum--) ++it;
    m_cards.erase(it);

    return *it;
}

std::string Card::getHandCards() {

    std::string handCards;
    for (int i = 0; i < m_cards.size() - 3; ++i) {
        auto card = takeOneCard();
        handCards += std::to_string(card.first) + "-" + std::to_string(card.second) + "#";
    }
    return handCards;
}

std::string Card::getLastCards() {
    std::string lastCards;
    for (int i = 0; i < m_cards.size(); ++i) {
        auto card = takeOneCard();
        lastCards += std::to_string(card.first) + "-" + std::to_string(card.second) + "#";
    }
    return lastCards;
}
