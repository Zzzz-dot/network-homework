TOPDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

function do_init
{
    echo "do init"
    git submodule init
    git submodule update
    current_dir=$PWD

    # build uring
    cd ${TOPDIR}/deps/liburing && \ 
    ./configure && \
    make -j4 && \
    make install

    # build glog
    cd ${TOPDIR}/deps/glog && \ 
    cmake -S . -B build -G "Unix Makefiles" && \
    # cmake --build build && \
    cmake --build build --target install

    cd $current_dir
}

function do_build
{
    echo "do build"
    current_dir=$PWD
    
    cd ${TOPDIR}/build && \ 
    cmake .. -G "Unix Makefiles" && \ 
    cmake --build .

    cd $current_dir
}

function main
{
  case "$1" in
    init)
      do_init
      ;;
    build)
      do_build
      ;;
  esac
}

main "$@"