# Install script for directory: /Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/gtest/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/tkachenko/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Bespalov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/00/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Zaitcev/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Garanyan/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/trempoltsev/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Kupriakov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Popkova/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/zuev/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/sheviakov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/konyukhov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/islentyev/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/valinurov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Pronin/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Grubov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Odzhaev/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Ovsepyan/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Ivanova/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Ermolaev/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Petukhov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Spiridonova/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/pushkin/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Dorozhkin/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/seliverstov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Davydov/cmake_install.cmake")
  include("/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/Bezsudnova/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/d.dorogkin/Documents/cmc/cpp_spring_2017/msu_cpp_spring_2017/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
