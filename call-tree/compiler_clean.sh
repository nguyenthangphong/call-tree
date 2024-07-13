OPTIONS=$1

rm -rf build/*.0

if [ "$OPTIONS" = "-fstack-usage" ]; then
    rm -rf build/*.su
fi

if [ "$OPTIONS" = "-fdump-rtl-expand" ]; then
    rm -rf build/*.expand
fi
