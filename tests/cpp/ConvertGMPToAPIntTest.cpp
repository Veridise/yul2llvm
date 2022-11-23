#include <gtest/gtest.h>
#include <libyul2llvm/YulASTVisitor/CodegenVisitor.h>

using namespace yul2llvm;

TEST(ConvertGMPToAPIntTest, convertUnsigned) {
  mpz_class x = 10;
  ASSERT_EQ(gmpToAPInt(x, 64).getZExtValue(), 10);
}

TEST(ConvertGMPToAPIntTest, convertSigned) {
  mpz_class x = 0xFFFFFFF6; // -10 as 32-bit
  ASSERT_EQ(gmpToAPInt(x, 16).getSExtValue(), -10);
}

TEST(ConvertGMPToAPIntTest, convertLargerToSmaller) {
  mpz_class x;
  mpz_class two = 2;
  mpz_pow_ui(x.get_mpz_t(), two.get_mpz_t(), 256);
  x = x - 1;
  x = x ^ 0xFF;
  bool clamped;
  ASSERT_EQ(gmpToAPInt(x, 128, &clamped).countPopulation(), 120);
  EXPECT_TRUE(clamped);
}
