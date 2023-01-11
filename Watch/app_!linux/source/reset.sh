#!/bin/bash
while :
do
    process=$(ps -ef | grep "main" | grep -v "grep" | awk '{print $2}')
    if [ $process -eq 0 ];
    then
        echo "not find execute file: main"
    else
        echo "kill -9 main"
        kill -9 $process
        echo "kill over"
        break
    fi
    sleep 1
done
