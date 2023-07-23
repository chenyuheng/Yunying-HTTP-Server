#include "../src/conf.hpp"

#include <gtest/gtest.h>

// fail test should be the first test, because Conf is a singleton
TEST(ConfTest, FailLoading) {
  // test default values
  EXPECT_EQ(8080, yunying::Conf::getInstance().get_port());
  EXPECT_EQ(5, yunying::Conf::getInstance().get_working_threads_num());
  EXPECT_EQ(1 << 25, yunying::Conf::getInstance().get_cache_size_bytes());
  EXPECT_EQ(300, yunying::Conf::getInstance().get_cache_default_max_age());
  EXPECT_EQ(10, yunying::Conf::getInstance().get_cache_clean_interval());
  EXPECT_EQ(yunying::OriginType::StaticFile,
            yunying::Conf::getInstance().get_origin_type());
  EXPECT_EQ(".", yunying::Conf::getInstance().get_root_dir());
  EXPECT_EQ("upstream.host", yunying::Conf::getInstance().get_upstream_host());
  EXPECT_EQ("185.199.110.153",
            yunying::Conf::getInstance().get_upstream_domain());
  EXPECT_EQ(0, yunying::Conf::getInstance().get_upstream_port());

  // load test_fail.lua and test values, should not change
  yunying::Conf::getInstance().set_lua_config_path(
      "../test/config/test_fail.lua");
  yunying::Conf::getInstance().parseLua();
  EXPECT_EQ(8080, yunying::Conf::getInstance().get_port());
  EXPECT_EQ(5, yunying::Conf::getInstance().get_working_threads_num());
  EXPECT_EQ(1 << 25, yunying::Conf::getInstance().get_cache_size_bytes());
  EXPECT_EQ(300, yunying::Conf::getInstance().get_cache_default_max_age());
  EXPECT_EQ(10, yunying::Conf::getInstance().get_cache_clean_interval());
  EXPECT_EQ(yunying::OriginType::StaticFile,
            yunying::Conf::getInstance().get_origin_type());
  EXPECT_EQ(".", yunying::Conf::getInstance().get_root_dir());
  EXPECT_EQ("upstream.host", yunying::Conf::getInstance().get_upstream_host());
  EXPECT_EQ("185.199.110.153",
            yunying::Conf::getInstance().get_upstream_domain());
  EXPECT_EQ(0, yunying::Conf::getInstance().get_upstream_port());
}

TEST(ConfTest, SuccessfulLoading) {
  // test default values
  EXPECT_EQ(8080, yunying::Conf::getInstance().get_port());
  EXPECT_EQ(5, yunying::Conf::getInstance().get_working_threads_num());
  EXPECT_EQ(1 << 25, yunying::Conf::getInstance().get_cache_size_bytes());
  EXPECT_EQ(300, yunying::Conf::getInstance().get_cache_default_max_age());
  EXPECT_EQ(10, yunying::Conf::getInstance().get_cache_clean_interval());
  EXPECT_EQ(yunying::OriginType::StaticFile,
            yunying::Conf::getInstance().get_origin_type());
  EXPECT_EQ(".", yunying::Conf::getInstance().get_root_dir());
  EXPECT_EQ("upstream.host", yunying::Conf::getInstance().get_upstream_host());
  EXPECT_EQ("185.199.110.153",
            yunying::Conf::getInstance().get_upstream_domain());
  EXPECT_EQ(0, yunying::Conf::getInstance().get_upstream_port());

  // load test_success.lua and test values
  yunying::Conf::getInstance().set_lua_config_path(
      "../test/config/test_success.lua");
  yunying::Conf::getInstance().parseLua();
  EXPECT_EQ(808, yunying::Conf::getInstance().get_port());
  EXPECT_EQ(0, yunying::Conf::getInstance().get_working_threads_num());
  EXPECT_EQ(233, yunying::Conf::getInstance().get_cache_size_bytes());
  EXPECT_EQ(344, yunying::Conf::getInstance().get_cache_default_max_age());
  EXPECT_EQ(455, yunying::Conf::getInstance().get_cache_clean_interval());
  EXPECT_EQ(yunying::OriginType::Upstream,
            yunying::Conf::getInstance().get_origin_type());
  EXPECT_EQ("test_root", yunying::Conf::getInstance().get_root_dir());
  EXPECT_EQ("test.host", yunying::Conf::getInstance().get_upstream_host());
  EXPECT_EQ("1.2.3.4", yunying::Conf::getInstance().get_upstream_domain());
  EXPECT_EQ("https", yunying::Conf::getInstance().get_upstream_protocol());
  EXPECT_EQ(23, yunying::Conf::getInstance().get_upstream_port());
}
