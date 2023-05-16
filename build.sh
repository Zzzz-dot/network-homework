TOPDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

function do_init
{
    echo "do init"
    git submodule init
    git submodule update
    current_dir=$PWD

    # build libevent
    cd ${TOPDIR}/deps/liburing && \ 
    ./configure
    make -j4 && \
    make install

    cd $current_dir
}

function do_build
{
    echo "do build"
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