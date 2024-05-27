#pragma once

struct ChaosToken
{
  enum class Token
  {
    kSkull,
    kCultist,
    kTable,
    kElderThing,
    kElderSign,
    kAutoFail,
    kValue,
    kInvalid
  };

  ChaosToken(const Token pToken = Token::kInvalid, const int8_t pEffect = 0): token{pToken}, effect{pEffect} {}

  Token token;
  int8_t effect;
};