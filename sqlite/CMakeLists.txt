file(GLOB_RECURSE SOURCES ./*.c)
add_library(sqlite STATIC ${SOURCES})
