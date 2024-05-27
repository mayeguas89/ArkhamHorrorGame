#pragma once
#include "card.h"

#include <algorithm>
#include <memory>
#include <queue>
#include <random>

class DeckShuffler
{
public:
  DeckShuffler()
  {
    std::random_device rd;
    mGen = std::mt19937(rd());
  }

  template<typename T>
  void operator()(T& theCards)
  {
    std::shuffle(theCards.begin(), theCards.end(), mGen);
  }

  template<>
  void operator()(std::queue<std::shared_ptr<Card>>& theCards)
  {
    std::vector<std::shared_ptr<Card>> cards;
    while (!theCards.empty())
    {
      auto& card = theCards.front();
      cards.push_back(std::move(card));
      theCards.pop();
    }
    std::shuffle(cards.begin(), cards.end(), mGen);
    std::for_each(cards.begin(), cards.end(), [&](auto& card) { theCards.push(std::move(card)); });
  }

private:
  std::mt19937 mGen;
};

template<typename T>
class Deck
{
public:
  virtual ~Deck() = default;
  virtual void AddCard(std::shared_ptr<Card> card) = 0;
  virtual bool Empty() const = 0;
  virtual size_t Size() const = 0;
  virtual void Shuffle() = 0;
  virtual std::shared_ptr<Card> DrawCard() = 0;
};

template<typename T>
class DeckImpl: public Deck<T>
{
public:
  DeckImpl() {}

  virtual void AddCard(std::shared_ptr<Card> card) override;

  virtual bool Empty() const override
  {
    return mCards.empty();
  }

  virtual size_t Size() const override
  {
    return mCards.size();
  }

  virtual void Shuffle() override
  {
    static DeckShuffler deckShuffler;
    deckShuffler.operator()<T>(mCards);
  }

  virtual std::shared_ptr<Card> DrawCard() override;

private:
  T mCards;
};

template<typename T>
inline void DeckImpl<T>::AddCard(std::shared_ptr<Card> card)
{
  mCards.push_back(std::move(card));
}

template<typename T>
inline std::shared_ptr<Card> DeckImpl<T>::DrawCard()
{
  auto card = mCards.front();
  mCards.erase(mCards.begin());
  return card;
}

template<>
inline void DeckImpl<std::queue<std::shared_ptr<Card>>>::AddCard(std::shared_ptr<Card> card)
{
  mCards.push(std::move(card));
}

template<>
inline std::shared_ptr<Card> DeckImpl<std::queue<std::shared_ptr<Card>>>::DrawCard()
{
  auto card = mCards.front();
  mCards.pop();
  return card;
}
