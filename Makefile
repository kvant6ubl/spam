CC = g++
CFLAGS = -g -Wall -Os -I./ -Imodels/
LDFLAGS = -Wall 
BLAS_LIBS = -std=c++11 -lcblas -lblas -liomp5 -lpthread 
LIBS = -lboost_regex -lboost_filesystem -lpng -lm

spam: src/spam.cpp src/tspam12.o  src/auxiliary.o  ${LIBS}
	g++ -o spam ${CFLAGS} ${LDFLAGS} src/spam.cpp src/tspam12.o src/auxiliary.o ${LIBS}

.cpp.o:
	g++ -c -o $@ $< ${CFLAGS}

clean:
	rm -f *.o *~ 
