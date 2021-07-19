multiThread: rsa/rsa.c rsa/bignum.c rsa/aes.c rsa/memory_buffer_alloc.c rsa/platform.c rsa/threading.c
	gcc -g -fPIC -o rsa/rsa.o -c rsa/rsa.c
	gcc -g -fPIC -o rsa/bignum.o -c rsa/bignum.c
	gcc -g -fPIC -o rsa/aes.o -c rsa/aes.c
	gcc -g -fPIC -o rsa/memory_buffer_alloc.o -c rsa/memory_buffer_alloc.c
	gcc -g -fPIC -o rsa/platform.o -c rsa/platform.c
	gcc -g -fPIC -o rsa/threading.o -c rsa/threading.c
	gcc -g -fPIC -c multiThread.c
	gcc -g -o multiThread multiThread.o rsa/rsa.o rsa/bignum.o rsa/aes.o rsa/platform.o rsa/memory_buffer_alloc.o rsa/threading.o -lpthread

clean: 
	rm -f *.o *.d rsa/*.o multiThread
