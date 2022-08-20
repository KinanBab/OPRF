#ifndef OPRF_OPRF_H_
#define OPRF_OPRF_H_

#include <array>
#include <ostream>
#include <string>

// NOLINTNEXTLINE
#include "sodium.h"

namespace OPRF {

using Hash = std::array<unsigned char, crypto_core_ristretto255_HASHBYTES>;
using Scalar = std::array<unsigned char, crypto_core_ristretto255_SCALARBYTES>;
using Point = std::array<unsigned char, crypto_core_ristretto255_BYTES>;

Scalar GenerateKey();

Scalar GenerateMask();

Point HashToPoint(const std::string& keyword);

Point MaskPoint(const Point& point, const Scalar& mask);

Point PRF(const Point& point, const Scalar& key);

Point UnmaskPoint(const Point& point, const Scalar& mask);

}  // namespace OPRF

template <size_t N>
std::ostream& operator<<(std::ostream& os,
                         const std::array<unsigned char, N>& e) {
  os << "[ ";
  for (size_t i = 0; i < N; i++) {
    os << static_cast<unsigned int>(e[i]);
    if (i + 1 < N) {
      os << ", ";
    }
  }
  os << " ]";
  return os;
}

template <size_t N>
bool operator==(const std::array<unsigned char, N>& l,
                const std::array<unsigned char, N>& r) {
  for (size_t i = 0; i < N; i++) {
    if (l[i] != r[i]) {
      return false;
    }
  }
  return true;
}

template <size_t N>
bool operator!=(const std::array<unsigned char, N>& l,
                const std::array<unsigned char, N>& r) {
  return !(l == r);
}

#endif  // OPRF_OPRF_H_
