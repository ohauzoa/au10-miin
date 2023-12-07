file(GLOB_RECURSE SOURCES ./*.c)
add_library(mqtt STATIC ${SOURCES})
