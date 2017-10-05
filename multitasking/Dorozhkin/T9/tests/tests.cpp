#include "stdafx.h"
#include "../include/diler.h"

TEST(WordTestGroup, EasyTests) {
    EXPECT_STREQ("2 2 2", dile("aaa").c_str());
    EXPECT_STREQ("22 22 22", dile("bbb").c_str());
    EXPECT_STREQ("2 22 222", dile("abc").c_str());
    EXPECT_STREQ("2 22 2223", dile("abcd").c_str());
    EXPECT_STREQ("9999 9999", dile("zz").c_str());
    EXPECT_STREQ("000", dile("   ").c_str());
    EXPECT_STREQ("7 77 777 7777", dile("pqrs").c_str());
    EXPECT_STREQ("0202202220222022020", dile(" a b c c b a ").c_str());
}

TEST(WordTestGroup, ComplexTests) {
    EXPECT_STREQ("44339990443399904433999", dile("hey hey hey").c_str());
    EXPECT_STREQ("444055566688833083377778444664", dile("i love testing").c_str());
    EXPECT_STREQ("4433555 555666096667775553", dile("hello world").c_str());
    EXPECT_STREQ("2 22 2223 33 333544 444 4455 5556 66 6667 77 777 77778 88 8889 99 999 9999", 
    dile("abcdefjhihklmnopqrstuvwxyz").c_str());
    EXPECT_STREQ("866444663304447777066 6668084433022337777807777337778884442223303336667770977744484446640777767777", 
    dile("tnine is not the best service for writing sms").c_str());    
}
