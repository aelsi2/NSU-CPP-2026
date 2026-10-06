#include <gtest/gtest.h>

#include <libregex/regex.hpp>

TEST(LibRegex, BasicString) {
  regex::Regex re = regex::Regex::compile("Hello");

  EXPECT_TRUE(re.match("Hello"));

  EXPECT_FALSE(re.match("hello"));
  EXPECT_FALSE(re.match("Hell"));
  EXPECT_FALSE(re.match("__Hello"));
  EXPECT_FALSE(re.match("Hellolol"));
  EXPECT_FALSE(re.match("Hello "));
  EXPECT_FALSE(re.match(" Hello"));
  EXPECT_FALSE(re.match(""));
}

TEST(LibRegex, AnyChar) {
  regex::Regex re = regex::Regex::compile("Oh.yo.");

  EXPECT_TRUE(re.match("Ohayou"));
  EXPECT_TRUE(re.match("Ohuyou"));
  EXPECT_TRUE(re.match("Oh yo "));
  EXPECT_TRUE(re.match("Oh6yo7"));
  EXPECT_TRUE(re.match("Oh.yo-"));
  EXPECT_TRUE(re.match("Oh!yo$"));

  EXPECT_FALSE(re.match(" hayou"));
  EXPECT_FALSE(re.match("Ohu you"));
  EXPECT_FALSE(re.match("Oh  yo "));
  EXPECT_FALSE(re.match("Oh.yo"));
}

TEST(LibRegex, BasicGroup) {
  regex::Regex re = regex::Regex::compile("00[bc a]m");

  EXPECT_TRUE(re.match("00am"));
  EXPECT_TRUE(re.match("00bm"));
  EXPECT_TRUE(re.match("00cm"));
  EXPECT_TRUE(re.match("00 m"));

  EXPECT_FALSE(re.match("00 mm"));
  EXPECT_FALSE(re.match("00dm"));
  EXPECT_FALSE(re.match("00aam"));
  EXPECT_FALSE(re.match("00m"));
}

TEST(LibRegex, GroupRange) {
  regex::Regex re = regex::Regex::compile("z[d-g]2");

  EXPECT_TRUE(re.match("zd2"));
  EXPECT_TRUE(re.match("ze2"));
  EXPECT_TRUE(re.match("zf2"));
  EXPECT_TRUE(re.match("zg2"));

  EXPECT_FALSE(re.match("zD2"));
  EXPECT_FALSE(re.match("zF2"));
  EXPECT_FALSE(re.match("zg22"));
  EXPECT_FALSE(re.match("zdd2"));
  EXPECT_FALSE(re.match("zc2"));
  EXPECT_FALSE(re.match("zh2"));
  EXPECT_FALSE(re.match("z 2"));
  EXPECT_FALSE(re.match("z2"));
}

TEST(LibRegex, GroupAlphabeticalRanges) {
  regex::Regex re = regex::Regex::compile("[a-zA-Z]");

  EXPECT_TRUE(re.match("a"));
  EXPECT_TRUE(re.match("f"));
  EXPECT_TRUE(re.match("z"));
  EXPECT_TRUE(re.match("B"));
  EXPECT_TRUE(re.match("G"));
  EXPECT_TRUE(re.match("W"));

  EXPECT_FALSE(re.match("0"));
  EXPECT_FALSE(re.match(" "));
  EXPECT_FALSE(re.match("!"));
}

TEST(LibRegex, ComplexGroup) {
  regex::Regex re = regex::Regex::compile("test[a-c0-9zf ]123");

  EXPECT_TRUE(re.match("test 123"));
  EXPECT_TRUE(re.match("testa123"));
  EXPECT_TRUE(re.match("testb123"));
  EXPECT_TRUE(re.match("testc123"));
  EXPECT_TRUE(re.match("testz123"));
  EXPECT_TRUE(re.match("testf123"));
  EXPECT_TRUE(re.match("test0123"));
  EXPECT_TRUE(re.match("test5123"));
  EXPECT_TRUE(re.match("test9123"));

  EXPECT_FALSE(re.match("test_123"));
  EXPECT_FALSE(re.match("testg123"));
  EXPECT_FALSE(re.match("testy123"));
}

TEST(LibRegex, IntersectingGroup) {
  regex::Regex re = regex::Regex::compile("[a-fb-gbg]");

  EXPECT_TRUE(re.match("a"));
  EXPECT_TRUE(re.match("b"));
  EXPECT_TRUE(re.match("c"));
  EXPECT_TRUE(re.match("f"));
  EXPECT_TRUE(re.match("g"));

  EXPECT_FALSE(re.match("h"));
}

TEST(LibRegex, BasicQuestion) {
  regex::Regex re = regex::Regex::compile(" a?aa");

  EXPECT_TRUE(re.match(" aa"));
  EXPECT_TRUE(re.match(" aaa"));

  EXPECT_FALSE(re.match(" a"));
  EXPECT_FALSE(re.match(" aaaa"));
  EXPECT_FALSE(re.match(" baa"));
  EXPECT_FALSE(re.match(" aba"));
}

TEST(LibRegex, MultiQuestion) {
  regex::Regex re = regex::Regex::compile("a?a?b?c?d? hello a?zc?d?");

  EXPECT_TRUE(re.match("aabcd hello azcd"));
  EXPECT_TRUE(re.match("abd hello azcd"));
  EXPECT_TRUE(re.match(" hello azcd"));
  EXPECT_TRUE(re.match("d hello zd"));
  EXPECT_TRUE(re.match("abcd hello azc"));

  EXPECT_FALSE(re.match("abacd hello azcd"));
  EXPECT_FALSE(re.match("aaabcd hello azcd"));
  EXPECT_FALSE(re.match("aabcd hello azccd"));
  EXPECT_FALSE(re.match("aabcd hello acd"));
}

TEST(LibRegex, BasicStar) {
  regex::Regex re = regex::Regex::compile("a*b*");

  EXPECT_TRUE(re.match(""));
  EXPECT_TRUE(re.match("aaaaaaaaa"));
  EXPECT_TRUE(re.match("bbbbbbbbb"));
  EXPECT_TRUE(re.match("aaaaaaaaabbbbbbbbb"));

  EXPECT_FALSE(re.match("abaaaaa"));
  EXPECT_FALSE(re.match("bbbbbbbbbaaaaaaaaa"));
}

TEST(LibRegex, StarPlusChar) {
  regex::Regex re = regex::Regex::compile("a*b+c");

  EXPECT_TRUE(re.match("bc"));
  EXPECT_TRUE(re.match("bbbbbc"));
  EXPECT_TRUE(re.match("aaaaabc"));
  EXPECT_TRUE(re.match("aaaaabbbbbbc"));

  EXPECT_FALSE(re.match("aaaaac"));
  EXPECT_FALSE(re.match("aaaaaaaaabbbbbbbbb"));
}

TEST(LibRegex, GroupPlus) {
  regex::Regex re = regex::Regex::compile("[a cb]+");

  EXPECT_TRUE(re.match("baba baca cabacaca"));
  EXPECT_TRUE(re.match("b"));
  EXPECT_TRUE(re.match("       "));

  EXPECT_FALSE(re.match(""));
  EXPECT_FALSE(re.match("caca_caca"));
}

TEST(LibRegex, StarGreedy) {
  regex::Regex re = regex::Regex::compile("a*a");

  EXPECT_FALSE(re.match(""));
  EXPECT_FALSE(re.match("a"));
  EXPECT_FALSE(re.match("aa"));
  EXPECT_FALSE(re.match("aaa"));
  EXPECT_FALSE(re.match("aaaaaaa"));
}

TEST(LibRegex, PlusGreedy) {
  regex::Regex re = regex::Regex::compile("a+a");

  EXPECT_FALSE(re.match(""));
  EXPECT_FALSE(re.match("a"));
  EXPECT_FALSE(re.match("aa"));
  EXPECT_FALSE(re.match("aaa"));
  EXPECT_FALSE(re.match("aaaaaaa"));
}

TEST(LibRegex, Empty) {
  regex::Regex re = regex::Regex::compile("");

  EXPECT_TRUE(re.match(""));
  EXPECT_FALSE(re.match("abcdef"));
}

TEST(LibRegex, Anything) {
  regex::Regex re = regex::Regex::compile(".*");

  EXPECT_TRUE(re.match(""));
  EXPECT_TRUE(re.match("69420"));
  EXPECT_TRUE(re.match("Agile, scrum? Fuck you, I'm Russian. x. x. - and straight to production."));
}

TEST(LibRegex, AllFeatures) {
  regex::Regex re = regex::Regex::compile("o?k?[1-357-9]+.. *");

  EXPECT_TRUE(re.match("ok13235$$"));
  EXPECT_TRUE(re.match("ok13235$$  "));
  EXPECT_TRUE(re.match("o13235$$  "));
  EXPECT_TRUE(re.match("k13235$$   "));
  EXPECT_TRUE(re.match("ok13235$$"));
  EXPECT_TRUE(re.match("ok5z$"));
  EXPECT_TRUE(re.match("55566"));
  EXPECT_TRUE(re.match("55577966"));
  
  EXPECT_FALSE(re.match("ok$$"));
  EXPECT_FALSE(re.match("ok14235$$"));
  EXPECT_FALSE(re.match("og13235$$"));
  EXPECT_FALSE(re.match("555666"));
}

TEST(LibRegex, InvalidCharacter) {
  ASSERT_THROW(regex::Regex::compile("a_bc123"), regex::RegexCompileError);
}

TEST(LibRegex, GroupNoEnd) {
  ASSERT_THROW(regex::Regex::compile("abc[123"), regex::RegexCompileError);
}

TEST(LibRegex, GroupNoStart) {
  ASSERT_THROW(regex::Regex::compile("abc123]"), regex::RegexCompileError);
}

TEST(LibRegex, RangeNoStart) {
  ASSERT_THROW(regex::Regex::compile("abc[-5]"), regex::RegexCompileError);
}

TEST(LibRegex, RangeNoEnd) {
  ASSERT_THROW(regex::Regex::compile("abc[1-]"), regex::RegexCompileError);
}

TEST(LibRegex, ModifierNoSubject) {
  ASSERT_THROW(regex::Regex::compile("?"), regex::RegexCompileError);
}

TEST(LibRegex, DuplicateModifier) {
  ASSERT_THROW(regex::Regex::compile("[a-z]++"), regex::RegexCompileError);
}

TEST(LibRegex, MultiModifier) {
  ASSERT_THROW(regex::Regex::compile("[a-z]+?"), regex::RegexCompileError);
}
