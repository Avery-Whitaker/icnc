#!/bin/bash
set -x
set -e

echo "NO INNER PUTS"
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=1
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=2
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=4
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=8
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=12
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=14
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=16
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=18
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=22
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=26
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=32
multitime -n 10 ./main.app


echo "INNER PUTS"
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=1
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=2
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=4
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=8
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=12
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=14
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=16
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=18
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=22
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=26
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=32
multitime -n 10 ./main.app

