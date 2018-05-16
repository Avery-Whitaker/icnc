#!/bin/bash
set -x
set -e

echo "NO INNER PUTS"
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=155
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=160
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=165
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=170
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=175
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=180
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=185
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=190
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=195
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -DNO_INNER_PUTS -DCHUNK_SIZE=200

echo "INNER PUTS"
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=155
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=160
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=165
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=170
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=175
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=180
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=185
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=190
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=195
multitime -n 10 ./main.app
g++ -std=c++11 -pthread -O3 main.cpp -o main.app -lcnc -lrt -ltbb -ltbbmalloc -UNO_INNER_PUTS -DCHUNK_SIZE=200
multitime -n 10 ./main.app
