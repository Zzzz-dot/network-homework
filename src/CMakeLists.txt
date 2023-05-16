aux_source_directory(. NetSrc)

set(NetSrc "${NetSrc};./poll/EpollPoller")

message("${NetSrc}")

add_library(libnet ${NetSrc})

target_link_libraries(libnet pthread)
