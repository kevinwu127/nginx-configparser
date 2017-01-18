#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "config_parser.h"

// Tests that the NginxConfigStatement handles tokens correctly
TEST(NginxConfigTest, ToString) {
    NginxConfigStatement statement;
    statement.tokens_.push_back("foo");
    statement.tokens_.push_back("bar");

    EXPECT_EQ(statement.ToString(0), "foo bar;\n");
}

// Creates a test fixture to reduce code duplication
class NginxStringConfigTest : public ::testing::Test {
    protected:
        bool ParseString(const std::string config_string) {
            std::stringstream config_stream(config_string);
            return parser_.Parse(&config_stream, &out_config_);
        }
        bool ParseFile(const char* file_name) {
            return parser_.Parse(file_name, &out_config_);
        }
        NginxConfigParser parser_;
        NginxConfig out_config_;
};

// Tests the output of ParseString
TEST_F(NginxStringConfigTest, SimpleConfigStream) {

    EXPECT_TRUE(ParseString("foo bar;"));
    EXPECT_EQ(1, out_config_.statements_.size()) << "Config has one statement";
    EXPECT_EQ("foo", out_config_.statements_.at(0)->tokens_.at(0));
}

// Tests an invalid configuration with no end token
TEST_F(NginxStringConfigTest, InvalidConfig) {
    EXPECT_FALSE(ParseString("foo bar"));
}

// Tests one nested config
TEST_F(NginxStringConfigTest, Nested) {
    EXPECT_TRUE(ParseString("baz { foo bar; }"));
}

// Tests 2+ nested configs
TEST_F(NginxStringConfigTest, DoubleNested) {
  EXPECT_TRUE(ParseString("qux { baz { foo bar; } }"));
}

// Tests missing end brace
TEST_F(NginxStringConfigTest, MissingEndBrace) {
    EXPECT_FALSE(ParseString("qux { baz { foo bar; } "));
}

// Tests missing start brace
TEST_F(NginxStringConfigTest, MissingStartBrace) {
    EXPECT_FALSE(ParseString("qux { baz { foo bar; } } }"));
}

// Empty context considered to be invalid
TEST_F(NginxStringConfigTest, EmptyContext) {
    EXPECT_FALSE(ParseString("foo {}"));
}

// Original test
TEST_F(NginxStringConfigTest, SimpleConfig) {
  EXPECT_TRUE(ParseFile("example_config"));
}

/*
 *   Tests Full Example Configuration from: 
 *   https://www.nginx.com/resources/wiki/start/topics/examples/full/
 */
TEST_F(NginxStringConfigTest, FullConfig) {
  EXPECT_TRUE(ParseFile("nginx.conf"));
}

