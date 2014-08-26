// ----------------------------------------------------------------------------
//  Description      : String component tests.
// ----------------------------------------------------------------------------
//  (c) Copyright 1998 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <iostream>
#include <ixlib_string.hh>
#include <vector>
#include "test.hh"




BEGIN_TEST
  vector<xString> vec;
  vec.push_back("A"); vec.push_back("B"); vec.push_back("C");
  TEST(concat(vec.begin(),vec.end(),"-") == "A-B-C")
  
  xString weenie = "Gosh. I'm a weenie.";
  weenie.find_replace(xString("Gosh."),xString("Josh."));
  TEST(weenie == "Josh. I'm a weenie.") 

  weenie = "I'm a helpless helping weenie.";
  weenie.find_replace(xString("help"),xString("rest"));
  TEST(weenie == "I'm a restless resting weenie.") 

  weenie = "I'm a helpless helping weenie.";
  weenie.find_replace("weenie.","woonie.");
  TEST(weenie == "I'm a helpless helping woonie.")

  weenie = "I'm a helpless helping weenie.";
  weenie.find_replace('e','o');
  TEST(weenie == "I'm a holploss holping woonio.") 
  
  // next ist test 8
  weenie = "       gimme a four-letter word";
  TEST(strRemoveLeading(weenie) == "gimme a four-letter word")
  TEST(strRemoveTrailing(weenie) == "       gimme a four-letter word")
  TEST(strRemoveLeadingTrailing(weenie) == "gimme a four-letter word")
  
  weenie = "gimme a four-letter word       ";
  TEST(strRemoveLeading(weenie) == "gimme a four-letter word       ")
  TEST(strRemoveTrailing(weenie) == "gimme a four-letter word")
  TEST(strRemoveLeadingTrailing(weenie) == "gimme a four-letter word")

  weenie = "       gimme a four-letter word       ";
  TEST(strRemoveLeading(weenie) == "gimme a four-letter word       ")
  TEST(strRemoveTrailing(weenie) == "       gimme a four-letter word")
  TEST(strRemoveLeadingTrailing(weenie) == "gimme a four-letter word")
  
  weenie = "      ";
  TEST(strRemoveLeading(weenie) == "")
  TEST(strRemoveTrailing(weenie) == "")
  TEST(strRemoveLeadingTrailing(weenie) == "")
  
END_TEST
