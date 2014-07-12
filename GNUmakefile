CFLAGS=-Wall -O3 -g
CXXFLAGS=-Wall -O3 -g
OBJECTS=main.o gpio.o led-matrix.o thread.o ImageScroller.o
OBJECTS2=main2.o gpio.o led-matrix.o thread.o ImageFlasher.o ImageScroller.o RGBMatrixManipulator.o
OBJECTS3=gpio.o led-matrix.o thread.o ImageFlasher.o ImageScroller.o RGBMatrixManipulator.o
BINARIES=led-matrix led-matrix2 scroll flash 
LDFLAGS=-lrt -lm -lpthread

all : $(BINARIES)

led-matrix.o: led-matrix.cc led-matrix.h

main.o: led-matrix.h

led-matrix : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

led-matrix2 : $(OBJECTS2)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

flash : flash.o $(OBJECTS3)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

scroll : scroll.o $(OBJECTS3)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

install : led-matrix
	cp led-matrix $(HOME)/bin/.

clean:
	rm -f $(OBJECTS) $(BINARIES)
