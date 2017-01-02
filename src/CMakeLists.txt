set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable (1 1.cpp)
add_executable (2 2.cpp)
add_executable (3 3.cpp)
add_executable (4 4.cpp)
add_executable (5 5.cpp)
add_executable (6 6.cpp)
add_executable (7 7.cpp)

# Link the executable to the Reverse library. Since the Reverse library has
# public include directories we will use those link directories when building.
target_link_libraries (5 LINK_PUBLIC crypto)
