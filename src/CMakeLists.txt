aux_source_directory(. NetSrc)

set(NetSrc "${NetSrc};./poll/EpollPoller;./poll/UringPoller;")

message("${NetSrc}")

add_library(libnet ${NetSrc})

target_link_libraries(libnet pthread)

target_link_libraries(libnet glog::glog)

target_link_libraries(libnet uring)