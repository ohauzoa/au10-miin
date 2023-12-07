cmake_minimum_required(VERSION 3.1.0)
project(tt)

#SET(CMAKE_SYSTEM_NAME Linux)
#SET(CMAKE_SYSTEM_PROCESSOR arm)
#SET(COMPILER_ROOT ~/Tina-Linux/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-)

#SET(CMAKE_C_COMPILER ${COMPILER_ROOT}gcc)
#SET(CMAKE_CXX_COMPILER ${COMPILER_ROOT}g++)
#SET(CMAKE_LINKER ${COMPILER_ROOT}ld)
#SET(CMAKE_NM ${COMPILER_ROOT}nm)
#SET(CMAKE_OBJCOPY ${COMPILER_ROOT}objcopy)
#SET(CMAKE_OBJDUMP ${COMPILER_ROOT}objdump)
#SET(CMAKE_RANLIB ${COMPILER_ROOT}ranlib)

#add_definitions (-DFOO -DBAR = xyz)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../../libs)

SET(STAGING_DIR ~/Tina-Linux/out/t113-au_10/staging_dir/target/usr)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${STAGING_DIR}/include)
INCLUDE_DIRECTORIES(${STAGING_DIR}/include/freetype2)
LINK_DIRECTORIES(${STAGING_DIR}/lib)
#INCLUDE_DIRECTORIES(/$ENV{HOME}${STAGING_DIR}/include)
#LINK_DIRECTORIES(/$ENV{HOME}/Dev/C++/ThirdParty/boost_1_60_0/stage/gcc/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/mqtt)

file(GLOB_RECURSE INCLUDES "lv_ui/*.h" "sqlite/*.h" "mqtt/*.h" "src/*.h" "./lv_drivers/*.h" "./lvgl/*.h"  "./*.h" )
file(GLOB_RECURSE SOURCES  "lv_ui/*.c" "sqlite/*.c" "mqtt/*.c" "src/*.c" "./lv_drivers/*.c" "./lvgl/*.c"  )

SET(CMAKE_CXX_FLAGS "-O3 -g0 -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare")
# -D_GNU_SOURCE=1 -D_REENTRANT \
#")

# Realtime Timer사용시 cmake에 추가 -D_POSIX_C_SOURCE -lrt 
add_definitions(-D_BSD_SOURCE -DLV_CONF_INCLUDE_SIMPLE -DUSE_SUNXIFB_G2D -DUSE_SUNXIFB_CACHE -DUSE_SUNXIFB_DOUBLE_BUFFER -DUSE_SUNXIFB_G2D_ROTATE)

add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})

#target_link_libraries(${PROJECT_NAME} asound uapi SDL2 SDL2_mixer mpg123 samplerate ts freetype m)
target_link_libraries(${PROJECT_NAME} pthread uapi ts mosquitto freetype m rt)

add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})

#CFLAGS+=-DLV_USE_SUNXIFB_G2D_FILL \
#				-DLV_USE_SUNXIFB_G2D_BLEND \
#				-DLV_USE_SUNXIFB_G2D_BLIT \
#				-DLV_USE_SUNXIFB_G2D_SCALE

#EXTRA_CFLAGS = -DUSE_SUNXIFB_G2D
#EXTRA_CFLAGS += -DUSE_SUNXIFB_CACHE
#EXTRA_CFLAGS += -DUSE_SUNXIFB_DOUBLE_BUFFER
#EXTRA_CFLAGS += -DUSE_SUNXIFB_G2D_ROTATE


#Collect the files to compile
#Collect the files to compile



