#!/bin/sh

if [ $# -eq 0 ]
then
    echo "Usage ./scp.sh filename"
    return
fi

echo $1
sshpass -p raspberry scp $1 pi@192.168.137.67:~

