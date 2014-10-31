/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     int ticketarray[9];
     int usedticket[9];
     int counter = 0;
     
     srand(time(NULL));
     
     for(counter = 0; counter < 10; counter++)
     {
		 ticketarray[counter] = rand() % 90000 + 10000;
		 usedticket[counter] = 0;
		 printf("Ticket number %i: %i\n",counter, ticketarray[counter]);
	 }
     
     if (argc < 2) 
     {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
     if (sockfd < 0) 
     {
        error("ERROR opening socket");
     }
     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
     {
		error("ERROR on binding");
     }
     
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     
     if (newsockfd < 0)
     {     
		error("ERROR on accept");
	 }
     
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     
     if (n < 0) 
     {
		error("ERROR reading from socket");
	 }
     
	 //printf("Here is the message: %s\n",buffer);
	 //n = write(newsockfd,"I got your message",18);
		
	 if(strstr(buffer, "buy"))
	 {
		 //variables needed
		 char buybuffer[256];
		 int x;
		 int flag = 0;
		 
		 //start the string with something friendly
		 strcat(buybuffer, "Your ticket ID is: ");
		 
		 
		 //look for a valid ticket number
		 //then set it to 0 so we don't use it again
		 for(x = 0; x < 10; x++)
		 {
			 //a valid ticket is defined as not 0, and
			 //not having a corresponding value in the
			 //used ticket array.
			 
			 //if the ticket is used, ticketarray[x] is 0,
			 //and usedticket[x] = whatever the ticket number is
			 if(ticketarray[x] != 0 && usedticket[x] == 0)
			 {
				 //buffer to hold the ticket number
				 char charbuffer[128];
				 
				 //convert it from an int to a char through sprintf()
				 sprintf(charbuffer, "%i", ticketarray[x]);
				 
				 //concatenate it to the buybuffer above
				 strcat(buybuffer, charbuffer);
				 
				 //assign its value to the corresponding usedticket index
				 usedticket[x] = ticketarray[x];
				 
				 //we have a valid ticket, so set the flag to 1
				 flag = 1;
				 break;
			 }
		 }
		 
		 //if we don't have at least one ticket
		 if(flag == 0)
		 {
			 //no more valid tickets
			 n = write(newsockfd, "Error, no more tickets!\n", 25);
		 }
		 
		 //if we have atleast one valid ticket left
		 if(flag == 1)
		 {
			 //we  have a valid ticket left
			 n = write(newsockfd, buybuffer, strlen(buybuffer));
		 }
		 if (n < 0) 
		 {
			error("ERROR writing to socket");
		 }
  
	}
	
	//cancel command
	if(strstr(buffer, "cancel"))
	{
		//necessary variables
		char *cancelbuffer;
		
		int cancelID;
		int counter;
		
		//tokenize the command to get to the ticket
		//number sent to us
		
		//"refund"
		cancelbuffer = strtok(buffer, " ");
		//ticket ID
		cancelbuffer = strtok(NULL, " ");
		
		//cancelID now holds the ticket number
		//so convert it to a integer and store it
		cancelID = atoi(cancelbuffer);
		
		char refundbuffer[256];
		
		//search for the given ticket ID in the usedticket
		//array
		for(counter = 0; counter < 10; counter++)
		{
			//if we found a match
			if(cancelID == usedticket[counter])
			{
			 //format the string we want to send back	
			 strcat(refundbuffer, "We refunded your ticket: ");
			 strcat(refundbuffer, cancelbuffer);
			 
			 //set the usedticket spot to 0, since we don't use
			 //it anymore
			 usedticket[counter] = 0;
			 
			 n = write(newsockfd, refundbuffer, strlen(refundbuffer));
		    }
		    
		    //no match, no ticket to refund
		    else
		    {
			 strcat(refundbuffer, "Error, invalid ticket!");
			 
			 n = write(newsockfd, refundbuffer, strlen(refundbuffer));
			}
	    }
	
    }
	
	//close and be done
	close(newsockfd);
	close(sockfd);
	return 0;
}
