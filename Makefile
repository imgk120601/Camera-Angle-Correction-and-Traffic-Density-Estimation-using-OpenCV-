CC=g++

##here CC represent which compilar u are using

CFLAGS=-c -Wall #defining variables for modularity
 ## here CFLAGS denotes which compilare flags or function/propery u are using in this case it is (-Wall)--Warning

all: hello
	#here all is the first Target and its dependency is on hello
		
hello: blob.o method4.o 
	$(CC) method4.cpp blob.cpp  -o hello  -std=c++11 -pthread `pkg-config --cflags --libs opencv` 
	
	#here hello is the target and its dependency is on a1.o( object of file a.cpp ) and mouseHandler.o ( object of file mouseHandler.cpp)
	#here this is the command of our hello Target and it will execute only after hello satisfies its dependencies( in our case they are a1.o and  mouseHandler.o)
	
	

blob.o: blob.cpp 
	$(CC)  $(CFLAGS) blob.cpp  -std=c++11 -pthread `pkg-config --cflags --libs opencv`
	
	#mouseHandler.o (an object of mouseHandler.cpp) has dependency on mouseHandler.cpp
	 #this will compile our mouseHandler.cpp, it is also command of our mouseHandler.o Target	
method4.o: method4.cpp 
	$(CC) $(CFLAGS) method4.cpp  -std=c++11 -pthread `pkg-config --cflags --libs opencv`
	#a1.o(object of a1.cpp) has dependency on a1.cpp
	 #this will compile our a1.cpp file,also this is command of our a1 Target



	
clean:
	rm -rf *o hello   # this will remove all object files and executable in our folder
