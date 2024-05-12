cmake_minimum_required(VERSION 3.16)
project(lab-14_linq)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror -Wextra")
set(SRC src)
set(INCLUDE include)
set(SANITIZE_COMPILE -fsanitize=address,undefined -g)
set(SANITIZE_LINKER -fsanitize=address,undefined)

include_directories(${INCLUDE})

add_executable(lab-14_linq ${SRC}/test.cpp)

option(ENABLE_SANITIZERS "Enable Sanitizers" OFF)
option(ENABLE_MINIMAL_SMOKE_TEST "Include assembly with /src/minimal_smoke_test.cpp" OFF)
option(ENABLE_SMOKE_TEST "Include assembly with /src/smoke_test.cpp" OFF)

if(ENABLE_SANITIZERS)
    target_compile_options(lab-14_linq PRIVATE ${SANITIZE_COMPILE})
    target_link_options(lab-14_linq PRIVATE ${SANITIZE_LINKER})
endif()

if (ENABLE_MINIMAL_SMOKE_TEST AND ENABLE_SANITIZERS)
    add_executable(lab-14_linq_minimal_smoke_test ${SRC}/minimal_smoke_test.cpp)
    target_compile_options(lab-14_linq_minimal_smoke_test PRIVATE ${SANITIZE_COMPILE})
    target_link_options(lab-14_linq_minimal_smoke_test PRIVATE ${SANITIZE_LINKER})
endif()

if (ENABLE_SMOKE_TEST AND ENABLE_SANITIZERS)
    add_executable(lab-14_linq_smoke_test ${SRC}/smoke_test.cpp)
    target_compile_options(lab-14_linq_smoke_test PRIVATE ${SANITIZE_COMPILE})
    target_link_options(lab-14_linq_smoke_test PRIVATE ${SANITIZE_LINKER})
endif()
