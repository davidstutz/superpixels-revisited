INCDIR = -I./vlib/include -I./
DBG    = -g
OPT    = -O3
CPP    = g++
CFLAGS = $(DBG) $(OPT) $(INCDIR) -fPIC
LINK   = -lm 

all: graph maxflow superpixels 

graph: graph.cpp
	$(CPP) $(CFLAGS) -c graph.cpp -o graph.o $(LINK)

maxflow: maxflow.cpp 
	$(CPP) $(CFLAGS) -c maxflow.cpp -o maxflow.o $(LINK)

superpixels.o: superpixels.cpp graph.cpp maxflow.cpp graph.h energy.h block.h 
	$(CPP) $(CFLAGS) -c superpixels.cpp -o superpixels.o $(LINK)

superpixels: superpixels.o graph.o maxflow.o graph.h energy.h block.h 
	$(CPP) $(CFLAGS) -o superpixels maxflow.o superpixels.o graph.o $(LINK)
clean:
	/bin/rm -f superpixels *.o

clean-all: clean
	/bin/rm -f *~ *.p?m 



