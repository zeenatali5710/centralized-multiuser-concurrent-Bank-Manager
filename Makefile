compile : 
	g++ -o server BankAcServer.cpp -lm -lpthread
	g++ -o client BankAcClient.cpp


serverRun :
	./server 8888


clientRun:
	./client 127.0.0.1 8888 0.5 Transactions.txt

 
clean :
	rm server client

