
gcc -lzmq -L/usr/local/lib/ -ltransport -ljansson -lhisolr -lhiredis -lcurl -I/usr/local/include hwserver_xc.c && ./a.out
