TOPDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

function help
{
  echo "Usage:"
  echo "./build.sh         # help information"
  echo "./build.sh init    # Initialize the dependency"
  echo "./build.sh build   # Build default version (-DCMAKE_BUILD_TYPE=Debug)"
  echo "./build.sh release # Build release version (-DCMAKE_BUILD_TYPE=Release)"

  echo ""
  echo "Examples:"
  echo "# Init."
  echo "./build.sh init"
  echo "# Build Debug."
  echo "./build.sh build"
}

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
    
    mkdir -p build && \
    cd ${TOPDIR}/build && \ 
    cmake .. -G "Unix Makefiles" && \ 
    cmake --build .

    cd $current_dir
}

function do_release
{
    echo "do build"
    current_dir=$PWD
    
    mkdir -p release && \
    cd ${TOPDIR}/release && \ 
    cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release && \ 
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
    release)
      do_release
      ;;
    *)
      help
      ;;
  esac
}

main "$@"