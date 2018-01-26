all:
	g++ -Wall -std=gnu++11 server.cpp -o myftpserver
	g++ -Wall -std=gnu++11 client.cpp -o myftp

clean:
	rm myftpserver
	rm myftp
