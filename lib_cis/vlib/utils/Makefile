INCDIR = -I../include
DBG    = -g
OPT    = -O3
CPP    = g++
CFLAGS = $(DBG) $(OPT) $(INCDIR) -fpermissive
LINK   = -lm 

all: pyramid smooth grad canny dt dilate

pyramid: pyramid.cpp 
	$(CPP) $(CFLAGS) -o pyramid pyramid.cpp $(LINK)

smooth: smooth.cpp 
	$(CPP) $(CFLAGS) -o smooth smooth.cpp $(LINK)

grad: grad.cpp 
	$(CPP) $(CFLAGS) -o grad grad.cpp $(LINK)

canny: canny.cpp 
	$(CPP) $(CFLAGS) -o canny canny.cpp $(LINK)

dt: dt.cpp 
	$(CPP) $(CFLAGS) -o dt dt.cpp $(LINK)

dilate: dilate.cpp 
	$(CPP) $(CFLAGS) -o dilate dilate.cpp $(LINK)

clean:
	/bin/rm -f pyramid smooth grad canny dt dilate *.o

clean-all: clean
	/bin/rm -f *~ *.p?m 



