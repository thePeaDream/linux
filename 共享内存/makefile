.PHONY:all
all:shmClient shmServer
shmClient:shmClient.cxx
	g++ -o $@ $^ -std=c++11
shmServer:shmServer.cxx
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -f shmClient shmServer