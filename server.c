/* Jose Wilfredo Aleman Espinoza 
* Netid: ja918
*/ 
#include <stdio.h> /* perror(), fprintf(), sprintf() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <sys/socket.h> /* socket(), bind(), listen(), accept(), recv(), send(), htonl(), htons() */
#include <sys/types.h>
#include <sys/stat.h> /* for open() */
#include <fcntl.h>    /* for open() */

#include <netinet/in.h> // for struct sockaddr_in
#include <arpa/inet.h> /* for sockaddr_in */
#include <unistd.h> /* for close() */
#include <time.h> /* for time */

#include <ctype.h> /* for isspace */

#define MAXPENDING 5 /* Max outstanding connection requests */
#define RCVBUFSIZE 256 /* Size of receive buffer */
#define ERR_EXIT(msg) { perror(msg); exit(1); }


// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}



int main(int argc, char *argv[]) {
    int rv_sock, s_sock, port_num, msg_len;
    char addr[10] = "127.0.0.1"; // IP Address
    port_num = 5100;             // Port number
    char buffer[RCVBUFSIZE];
    struct sockaddr_in serv_addr;
    time_t ticks;
    
    rv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (rv_sock < 0) ERR_EXIT("ERROR opening socket");
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(addr);
    serv_addr.sin_port = htons(port_num);
    
    if (bind(rv_sock,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        ERR_EXIT("ERROR on binding");
        exit(1);
    }
    
    if (listen(rv_sock, MAXPENDING) < 0){
        ERR_EXIT("ERROR on listen");
        exit(1);
    }
    
    ticks = time(NULL);
    fprintf(stdout, "----- Network Server ----- \n");
    fprintf(stdout, " -%.24s\r\n",ctime(&ticks));
    printf("Server <address:port> : %s:%d\n", addr,port_num);
    
    while ( 1 ) { /* Server runs forever */
        
        fprintf(stdout, "\nWaiting for client to connect...\n");
        
        s_sock = accept(rv_sock, NULL, NULL);
        
        if (s_sock < 0) {
            ERR_EXIT("ERROR on accept new client");
        } 
        
        memset(buffer, 0, RCVBUFSIZE);
        msg_len = recv(s_sock, buffer, RCVBUFSIZE, 0);
        
        if (msg_len < 0){
            ERR_EXIT("ERROR reading from socket");
        } 
        
        // the client will ask either for a file or to bounce or to exit
        char *msgType = ".txt";
        char *cmd31 = "exit";
        char *cmd32 = "EXIT";
        
        // open the file
        int fd;
        char *input = buffer;
        char *response = "";
        
        // so far just txt files
        if(strstr(input, msgType) != NULL) {       // User asks for a file
            
            // open the file, trim the input to get the filename
            fd = open(trimwhitespace(input), O_RDONLY, 0600);
                        
            fprintf(stdout, "Client requested file: %s\n", input);
            
            if (fd < 0) {
                response = "ERROR: no such file";
                msg_len = send(s_sock, response, strlen(response), 0);
                fprintf(stdout, "\n%s\n", response);
            } else {
                // Print the content of the file and send it to the client
                ssize_t read_bytes = 0;
                memset(buffer, 0, RCVBUFSIZE);
                while ((read_bytes = read(fd, buffer,RCVBUFSIZE)) > 0){
                    msg_len = send(s_sock, buffer, RCVBUFSIZE, 0);
                    fprintf(stdout, "%s",buffer);
                    memset(buffer, 0, RCVBUFSIZE);
                }
            fprintf(stdout, "\n --- End of the file. -- \n");
            close(s_sock);
            continue;
            }
        } 
        
        // Plain EXIT/exit
        if(strcmp(trimwhitespace(input),cmd31) == 0 || strcmp(trimwhitespace(input),cmd32) == 0) {
            response = "Client disconected.";
            msg_len = send(s_sock, response, strlen(response), 0);
            fprintf(stdout, "\n%s\n", response);
            close(s_sock);
            continue;
        }
        
        // Exit with code
        if(strstr(input, cmd31) != NULL || strstr(input, cmd32) != NULL) {       // User asks for a file
            
            const char separator = ' ';
            char * const sep_at = strchr(input, separator);
            
            if(sep_at != NULL){
                *sep_at = '\0'; /* overwrite first separator, creating two strings. */
                response = "Client disconected with code: ";
                char *str = malloc(strlen(sep_at + 1) + strlen(response));
                strcat(str, response);
                strcat(str, sep_at + 1);
                
                fprintf(stdout,"%s\n", str);
                msg_len = send(s_sock, str, strlen(str), 0);
                str = ""; 
            }
              
            close(s_sock);
            continue;
        } 
        
        // The user is sending wrong syntax msgs just ignore
        if (strlen(buffer) == 0){
            close(s_sock);
            continue;
        }
        else {      
            // User sent a bounce
            response = "BOUNCE! - Message: ";
            char *str = malloc(strlen(buffer) + strlen(response));
            strcat(str, response);
            strcat(str, buffer);
            msg_len = send(s_sock, str, strlen(str), 0);
            fprintf(stdout, "\n%s\n",str);
            memset(buffer, 0, RCVBUFSIZE);
            str = "";            
        }
        
        if (msg_len < 0) ERR_EXIT("ERROR writing to socket");
        close(s_sock);
    }
/* NOT REACHED, because the server runs forever */
}
