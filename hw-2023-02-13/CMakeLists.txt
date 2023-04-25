cmake_minimum_required(VERSION 3.25)

project(copy_paste)

file(GLOB copy_paste_files "./*.cpp")
add_executable(copy_paste ${copy_paste_files})
