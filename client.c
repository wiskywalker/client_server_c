/* Jose Wilfredo Aleman Espinoza 
* Netid: ja918
*/ 
#include <stdio.h> /* for perror(), fprintf(), sprintf() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset(), memcpy(), strlen() */
#include <sys/socket.h> /* for sockaddr, socket(), connect(), recv(), send(), htonl(), htons() */
#include <arpa/inet.h> /* for sockaddr_in */
#include <netdb.h> /* for hostent, gethostbyname() */
#include <unistd.h> /* for close() */
#define RCVBUFSIZE 256 /* Size of receive buffer */
#define ERR_EXIT(msg) { perror(msg); exit(1); }

int main(int argc, char *argv[]) {
    int c_sock, port_num, msg_len;
    struct sockaddr_in serv_addr;
    struct hostent *serverIP;
    char buffer[RCVBUFSIZE];
    
    fprintf(stdout, "----- Network Client ----- \n");
    fprintf(stdout, "Commands: get | bounce | exit \n");
    
    while ( 1 ) {
        
        // opening the socket 
        c_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (c_sock < 0) ERR_EXIT("ERROR opening socket");
        //memset((char *) &serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = htons(5100);
        if (connect(c_sock,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) ERR_EXIT("ERROR connecting");
        
        fprintf(stdout, ">");
        
        memset(buffer, 0, RCVBUFSIZE); /* erase */
        fgets(buffer, RCVBUFSIZE, stdin); /* read input */
        
        // variables for the "commands implementation"
        char *input = buffer;
        char *cmd11 = "get";
        char *cmd12 = "GET";
        char *cmd21 = "bounce";
        char *cmd22 = "BOUNCE";
        char *cmd31 = "exit";
        char *cmd32 = "EXIT";
        
        // GET 
        if(strstr(input, cmd11) != NULL || strstr(input, cmd12) != NULL) {
            fprintf(stdout, ">Filename to get: ");
            memset(buffer, 0, RCVBUFSIZE); /* erase */
            fgets(buffer, RCVBUFSIZE, stdin); /* read input */
            
            msg_len = send(c_sock, buffer, strlen(buffer), 0);
            
            if (msg_len < 0) ERR_EXIT("ERROR writing to socket");
            
            memset(buffer, 0, RCVBUFSIZE);
            msg_len = recv(c_sock, buffer, RCVBUFSIZE - 1, 0);
            
            if (msg_len < 0) ERR_EXIT("ERROR reading from socket");
            
            fprintf(stdout, ">Server says:\n%s", buffer);
            
            
            memset(buffer, 0, RCVBUFSIZE);
            
            // Receives the content of the file and print it
            
            while ((msg_len = recv(c_sock, buffer, RCVBUFSIZE - 1, 0) > 0)){
                fprintf(stdout, "%s", buffer);
                memset(buffer, 0, RCVBUFSIZE);
            }
            
            fprintf(stdout, "\n --- End of the file. -- \n");
            close(c_sock);
            continue;
            
        }
        
        // BOUNCE
        if(strstr(input, cmd21) != NULL || strstr(input, cmd22) != NULL) {
            fprintf(stdout, ">Text to bounce: ");
            memset(buffer, 0, RCVBUFSIZE); /* erase */
            fgets(buffer, RCVBUFSIZE, stdin); /* read input */
            
            msg_len = send(c_sock, buffer, strlen(buffer), 0);
            
            if (msg_len < 0) ERR_EXIT("ERROR writing to socket");
            
            memset(buffer, 0, RCVBUFSIZE);
            msg_len = recv(c_sock, buffer, RCVBUFSIZE - 1, 0);
            
            if (msg_len < 0) ERR_EXIT("ERROR reading from socket");
            
            fprintf(stdout, ">Server says: %s\n", buffer);
            
            close(c_sock);
            continue;
            
        }
        
        // Default EXIT or exit
        if(strcmp(input,cmd31) == 0 || strcmp(input,cmd32) == 0) {
        
            msg_len = send(c_sock, buffer, strlen(buffer), 0);
            
            memset(buffer, 0, RCVBUFSIZE);
            msg_len = recv(c_sock, buffer, RCVBUFSIZE - 1, 0);
            
            fprintf(stdout, ">Server says: %s\n", buffer);
            close(c_sock);
            exit(0);
        }
        
        // Exit with code
        if(strstr(input, cmd31) != NULL || strstr(input, cmd32) != NULL) {
           
            msg_len = send(c_sock, buffer, strlen(buffer), 0);
            
            memset(buffer, 0, RCVBUFSIZE);
            msg_len = recv(c_sock, buffer, RCVBUFSIZE - 1, 0);
            
            fprintf(stdout, ">Server says: %s\n", buffer);
            close(c_sock);
            exit(0);
        }else {
            fprintf(stdout, ">Wrong syntax, use GET, BOUNCE or EXIT.\n>\n");
            close(c_sock);
        }
    }
}