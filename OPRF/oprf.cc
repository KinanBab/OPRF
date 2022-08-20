#include "OPRF/oprf.h"

#include <cassert>

namespace OPRF {

using Hash = std::array<unsigned char, crypto_core_ristretto255_HASHBYTES>;
using Scalar = std::array<unsigned char, crypto_core_ristretto255_SCALARBYTES>;
using Point = std::array<unsigned char, crypto_core_ristretto255_BYTES>;

Scalar GenerateKey() {
  Scalar key;
  randombytes_buf(key.data(), sizeof(Scalar));
  return key;
}

Scalar GenerateMask() {
  Scalar mask;
  crypto_core_ristretto255_scalar_random(mask.data());
  return mask;
}

Point HashToPoint(const std::string& keyword) {
  const unsigned char* data =
      reinterpret_cast<const unsigned char*>(keyword.data());

  Hash hash;
  assert(crypto_generichash(hash.data(), sizeof(Hash), data, keyword.size(),
                            NULL, 0) >= 0);
  Point point;
  assert(crypto_core_ristretto255_from_hash(point.data(), hash.data()) >= 0);
  return point;
}

Point MaskPoint(const Point& point, const Scalar& mask) {
  Point output;
  assert(crypto_scalarmult_ristretto255(output.data(), mask.data(),
                                        point.data()) >= 0);
  return output;
}

Point PRF(const Point& point, const Scalar& key) {
  return MaskPoint(point, key);
}

Point UnmaskPoint(const Point& point, const Scalar& mask) {
  Scalar inv;
  assert(crypto_core_ristretto255_scalar_invert(inv.data(), mask.data()) >= 0);
  return MaskPoint(point, inv);
}

}  // namespace OPRF
