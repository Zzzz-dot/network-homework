add_executable(echo echo.cpp)
target_link_libraries(echo libnet)

add_executable(pingpong pingpong.cpp)
target_link_libraries(pingpong pthread)

add_executable(fileop fileop.cpp)
target_link_libraries(fileop libnet)