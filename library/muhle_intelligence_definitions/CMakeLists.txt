cmake_minimum_required(VERSION 3.20)

add_library(muhle_intelligence_definitions INTERFACE "include/muhle_intelligence/definitions.hpp")

target_include_directories(muhle_intelligence_definitions INTERFACE "include")
