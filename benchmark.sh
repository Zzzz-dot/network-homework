# 1 10 50 100 并发客户端
declare i=1
while [ $i -le $1 ];do
    Release/examples/pingpong 127.0.0.1:1234 >> result_$1 &
    let i=$i+1
done