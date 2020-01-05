[ ! -d "lib/" ] && mkdir lib
g++ --std=c++17 -DHAVE_CONFIG_H=1 -fPIC -Wall -I../include -I../dependencies/c-areslinux/ ../src/Resolver.cpp -c
ar rcs libc-arespp.a Resolver.o
mv libc-arespp.a lib/
rm *.o