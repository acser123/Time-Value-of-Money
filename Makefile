tvm: tvm.cpp
	g++ -g -Wall -Wshadow -Wconversion -o tvm tvm.cpp

clean:
	rm -fr tvm *.o *.stackdump

