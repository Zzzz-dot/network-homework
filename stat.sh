# awk '{print $2,$3}' $1|grep KB/s|awk '{print $1}'
awk '{print $1,$2}' $1|grep Count|awk '{print $2}'