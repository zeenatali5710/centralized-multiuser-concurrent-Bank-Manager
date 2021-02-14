//Include libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define MAXFILE 500

int main(int argc, char **argv)
{

    clock_t timeTaken = clock(); // set clock timer
	if(argc < 4)
	{
		fprintf(stderr, "Enter hostname, port, timestamp, filename\n"); // to connect to specified server
		exit(1);
	}

	char buffer[MAXFILE];
	int x=0;
	char ch;
	char line[200];
	char *clientfilename;
	clientfilename=argv[4];

	//to open transaction file
	FILE *myfile = fopen(clientfilename, "r");
	if(myfile==NULL)
	{
		printf("Unable to open file\n");
	}

	struct hostent *server;
	int portno;
	double sec;

	//fetch server details entered by user
	portno = atoi(argv[2]);
	sec = atof(argv[3]);
	server = gethostbyname(argv[1]);
	if(server == NULL)
	{
		fprintf(stderr, "host does not exists\n");
		exit(1);
	}

	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET; //server protocol family
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,server->h_length);
	server_addr.sin_port = htons(portno);


	int socketfd = socket(AF_INET, SOCK_STREAM, 0); // create socket

	if(socketfd < 0)
	{
		fprintf(stderr, "Socket not created \n");
		exit(0);
	}

    double time;
    double initial;
    //connect to server socket
	if(connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "Connection failed \n");
	}

	x++;
	memset(buffer, 0, MAXFILE);
	int size=0;
	char string[MAXFILE];
    int transactions=0;

    //if client connects with server, fetch each transaction and send it line by line
    while (fgets(buffer, MAXFILE, myfile) != NULL)
    {
    	transactions++;
    	initial=initial+1;
    	char *p;
    	p=strtok(buffer,"\n");
    	if(p)
    	{
    		strcpy(buffer,p);
    	}

    	size+=strlen(buffer);
    	char record[256];
    	strcpy(record,buffer);

    	//compare timestamp before sending
    	p=strtok(record," ");
    	if(p)
    	{
    		time=atoi(p);
    	}

    	//checking race condition: request will get delayed based on timestamp attached in transaction file and the time already spent
    	while(initial!=time)
    	{
    		if(initial>time)
    		{
    			sleep((initial-time)*sec);
    			initial=initial+1;
    		}
    		else
    		{
    			sleep((time-initial)*sec);
    			initial=initial+1;
    		}
    	}

    	int n;
    	if(initial==time)
    	{
    		printf("-----request sending----->> %s\n\n",buffer);
			n = write(socketfd, buffer, sizeof(buffer));
			if(n < 0)
			{
				fprintf(stderr, "writing to socket failed\n");
				exit(1);
			}
			bzero(buffer, MAXFILE);
			//read acknowledgement signal received from server
			n = read(socketfd, buffer, MAXFILE);
			printf("Server has signalled that message is received : %s\n",buffer);
			timeTaken = clock() - timeTaken;
            printf("\n Time taken to execute transaction is : %lf seconds \n",(float)timeTaken/CLOCKS_PER_SEC);
	    }
    }

	close(socketfd); // socket closed
	exit(0);
	return 0;
}
