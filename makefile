
CC=g++
OBJS=oratestor.o
LIB=-L/opt/aspire/product/misc20/MISCKERNEL20/app/oracle/product/11.2/lib 
INCLUDE=-I/opt/aspire/product/misc20/MISCKERNEL20/app/oracle/product/11.2/rdbms/public -I/usr/include/libxml2

oratestor:$(OBJS)
	$(CC) -g -o oratestor $(OBJS) $(LIB) -locci -lclntsh  -lxml2 -lz -lm
oratestor.o: oratestor.cpp
	$(CC) -g  -c oratestor.cpp $(INCLUDE)
clean:
	rm -rf *.o & rm oratestor
