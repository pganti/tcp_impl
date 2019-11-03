#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <errno.h>
#include <string>

typedef string String;

void sighandler(int);

int
main(int argc, char *argv[])
{
	int	i;
	int	sockfd; int setval = 1;
	struct	sockaddr_in servaddr;
	struct	hostent *hp; 
	char	request[1000];
	char	response[1000000];
	ssize_t n;

	//added variables
	String ret;
	char *recBuf;
	recBuf = new char[1024];
	int nRet = 0;


	if (argc != 3) {
		fprintf(stderr, "usage: %s <hostname> <path>\n", argv[0]);
		exit(1);
	}

	signal(SIGPIPE, sighandler);

	//hostent retrieval from hostname
	if ( (hp = gethostbyname(argv[1])) == 0) {
		fprintf(stderr, "%s: unknown host\n", argv[0]);
		exit(1);
	}

	//get socket descriptor
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "socket error\n");
		exit(2);
	}

	bzero(&servaddr, sizeof(servaddr));
	bcopy(hp->h_addr, &servaddr.sin_addr, hp->h_length);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(80);

	//connect to the server
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Cannot connect to stream socket\n");
		exit(3);
	}

	//prepare a GET request
	sprintf(request, "GET %s HTTP/1.1\r\nhost: %s\r\n\r\n", argv[2], argv[1]);

	//send the request by breaking it into two
	if ( (n = write(sockfd, request, 1)) < 0) {
		fprintf(stderr, "Cannot send first request\nError: %s\n", strerror(errno));
		exit(4);
	}

	if ( (n = write(sockfd, request + 1, strlen(request) - 1)) < 0) {
		fprintf(stderr, "Cannot send second request\nError: %s\n", strerror(errno));
		exit(4);
	}

/*
	if ( (n = read(sockfd, response, 1460 + 186)) > 0) {
		response[n] = '\0';
		printf("%s\n", response);
		fprintf(stderr, ">>> n = %d\n\n", n);
	}
*/
		//Receive the response until "\r\n\r\n" is encountered
		while(true) 
		{
			//recv for the request 
			if ((nRet=recv(sockfd, recBuf, 1024, 0)) < 0) 
			{
				fprintf(stderr, "recv failed... %s\n", strerror(errno));
				close(sockfd);		
				delete recBuf;
				recBuf=NULL;
				exit(4);		
			}
			
			if(nRet>0)
				ret.append(recBuf,nRet);

			if (nRet == 0) {
			//cout << "\nBytes received: " << ret.length() << " Header length: " << ret.find("\r\n\r\n")+4 <<endl;
			//cout << ret.substr(0,ret.find("\r\n\r\n")+4) << endl;
			break;
			}
			
	
			if(ret.find("\r\n\r\n")  != String::npos) {
			//cout << "\nBytes received: " << ret.length() << "Header length: " << ret.find("\r\n\r\n")+4 <<endl;
			//cout << ret.substr(0,ret.find("\r\n\r\n")+4) << endl;
			//cout<<"end of header found-- retreating"<<endl;
			break;
			}

		}
		
		//delete the buffer
		delete recBuf;
		recBuf=NULL;
	
		//print the response
		cout<<"Response data:"<<endl<<ret<<endl;
	
		//start sleeping for 20 seconds, to let server timeout
		fprintf(stdout, "Start sleeping ...\n");
		sleep(20);
	
 
	//attempt the first write
	if ( (n = write(sockfd, request, 5)) < 0) {
		fprintf(stderr, "Cannot send first request\nError: %s\n", strerror(errno));
		exit(4);
	}

	//sleep between the two writes
#ifdef SLEEP
	sleep(1);
#endif	
	
	//attempt the second write
	if ( (n = write(sockfd, request + 5, strlen(request) - 5)) < 0) {
		fprintf(stderr, "Cannot send second request\nError: %s\n", strerror(errno));
		exit(4);
	}

	fprintf(stdout, "Written %d characters\n", n);

}

void sighandler(int signo)
{
	signal(SIGPIPE, SIG_IGN);
	fprintf(stderr, "SIGPIPE caught\n");
	return;
}
