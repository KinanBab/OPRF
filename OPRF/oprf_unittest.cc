#include "OPRF/oprf.h"

#include "gtest/gtest.h"
// NOLINTNEXTLINE
#include "sodium.h"

namespace OPRF {

TEST(OPRF, HashToPoint) {
  std::string str1 = "input1";
  std::string str2 = "input2";
  Point p1 = HashToPoint(str1);
  Point p2 = HashToPoint(str2);
  Point p3 = HashToPoint(str2);
  EXPECT_TRUE(crypto_core_ristretto255_is_valid_point(p1.data()));
  EXPECT_TRUE(crypto_core_ristretto255_is_valid_point(p2.data()));
  EXPECT_TRUE(crypto_core_ristretto255_is_valid_point(p3.data()));
  EXPECT_NE(p1, p2);
  EXPECT_EQ(p2, p3);
}

TEST(OPRF, Randomness) {
  for (size_t i = 0; i < 100; i++) {
    Scalar key1 = GenerateKey();
    Scalar key2 = GenerateKey();
    EXPECT_NE(key1, key2);
    Scalar mask1 = GenerateMask();
    Scalar mask2 = GenerateMask();
    EXPECT_NE(mask1, mask2);
  }
}

TEST(OPRF, Mult) {
  for (size_t i = 1; i <= 100; i++) {
    // Turn i into a scalar of the correct format.
    Scalar scalar;
    scalar.fill(0);
    scalar[0] = i;
    // Get a random point on the curve.
    Point point;
    crypto_core_ristretto255_random(point.data());
    // Multiply point by scalar.
    Point result = MaskPoint(point, scalar);
    // Ensure result = point * scalar.
    Point expected = point;
    for (size_t j = 1; j < i; j++) {
      Point add;
      crypto_core_ristretto255_add(add.data(), expected.data(), point.data());
      expected = add;
    }
    EXPECT_EQ(result, expected);
  }
}

TEST(OPRF, InvMult) {
  // Random point and scalar.
  Point point;
  crypto_core_ristretto255_random(point.data());
  Scalar mask = GenerateMask();
  // Mult by point, then by inverse.
  Scalar inv;
  EXPECT_GE(crypto_core_ristretto255_scalar_invert(inv.data(), mask.data()), 0);
  Point mult = MaskPoint(point, mask);
  EXPECT_NE(point, mult);
  Point result = MaskPoint(mult, inv);
  EXPECT_EQ(point, result);
}

TEST(OPRF, End2End) {
  for (size_t i = 1; i < 100; i++) {
    // The client/party that has the value to PRF needs to hash that value
    // to a point on the elliptic curve.
    std::string str = "test" + std::to_string(i);
    Point point = HashToPoint(str);

    // The server(s) have the key of the oblivious PRF.
    Scalar key = GenerateKey();

    // The OPRF workflow.
    // Client blinds/masks their point using a random mask.
    Scalar mask = GenerateMask();
    Point result = MaskPoint(point, mask);
    // Client sends masked point to server, server applies its key, and sends
    // result to client.
    result = PRF(result, key);
    // Client removes the mask, reconstructing the PRF(str, key).
    // Client does not learn key, server does not learn str or point.
    result = UnmaskPoint(result, mask);

    // Correctness: OPRF must have the same output as the non-oblivious PRF.
    Point expected = PRF(point, key);

    // Check correctness: PRF == OPRF.
    EXPECT_EQ(result, expected);
  }
}

}  // namespace OPRF
