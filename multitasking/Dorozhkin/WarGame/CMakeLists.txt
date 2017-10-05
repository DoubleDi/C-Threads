cmake_minimum_required(VERSION 3.4)

project(WarGame)

set(SRC_ROOT "${PROJECT_SOURCE_DIR}/src")
set(HEADER_ROOT "${PROJECT_SOURCE_DIR}/include")

include_directories(include)

set(SOURCES
# ${SRC_ROOT}/main.cpp
${SRC_ROOT}/parallel_main.cpp
# ${SRC_ROOT}/army_main.cpp
)

set(SHARED_SOURCES
${SRC_ROOT}/Armor.cpp 
${SRC_ROOT}/Human.cpp 
${SRC_ROOT}/Location.cpp 
${SRC_ROOT}/Weapon.cpp
${SRC_ROOT}/Army.cpp
)

set(HEADERS
${HEADER_ROOT}/Armor.h 
${HEADER_ROOT}/Human.h 
${HEADER_ROOT}/Location.h 
${HEADER_ROOT}/Weapon.h
${HEADER_ROOT}/Army.h
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${SHARED_SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)

#----------- precompiled -----------
set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "${STDAFX_H}")
cotire(${PROJECT_NAME})

#-------------- tests --------------
set(TEST_PROJECT test_${PROJECT_NAME})
add_test(NAME ${TEST_PROJECT} COMMAND ${TEST_PROJECT})

add_subdirectory(tests)

enable_testing()
