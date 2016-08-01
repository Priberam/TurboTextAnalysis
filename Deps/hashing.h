/**
* @file Hashing.h
* @brief Methods for hashing handling.
*
* @author  dan
* @date  20/05/2016
* @copyright Copyright PRIBERAM Informa'tica, SA.
* @par  Company
*  Priberam, http://www.priberam.pt/
*/
#ifndef HASHING_H
#define HASHING_H
#include <stdint.h>
#include <functional>

template <typename TFirst, typename TSecond>
struct HashablePair : public std::pair<TFirst, TSecond> {
  typedef std::pair<TFirst, TSecond> Base;
public:
  HashablePair() : Base() {};
  HashablePair(const Base& a) : Base(a) {};
  HashablePair(Base&& a) : Base(std::move(a)) {};
  size_t operator()(const HashablePair& p) const {
    size_t seed = std::hash<TFirst>()(p.first);
    HashCombine<size_t>(std::hash<TSecond>()(p.second), &seed);
    return seed;
  };
  bool operator()(const HashablePair &lhs, const HashablePair &rhs) const {
    return lhs.first == rhs.first && lhs.second == rhs.second;
  };

private:
  template <typename TSeed>
  inline void HashCombine(TSeed value, TSeed *seed) const {
    *seed ^= value + 0x9e3779b9 + (*seed << 6) + (*seed >> 2);
  };
};

#endif
