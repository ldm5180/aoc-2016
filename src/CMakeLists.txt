set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable (01 01.cpp)
add_executable (02 02.cpp)
add_executable (03 03.cpp)
add_executable (04 04.cpp)
add_executable (05 05.cpp)
add_executable (06 06.cpp)
add_executable (07 07.cpp)
add_executable (08 08.cpp)
add_executable (09 09.cpp)

# Link the executable to the Reverse library. Since the Reverse library has
# public include directories we will use those link directories when building.
target_link_libraries (05 LINK_PUBLIC crypto)
