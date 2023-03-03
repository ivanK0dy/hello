#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include "parsexml.h"
#include "mydebug.h"
// for non-blocking I/O
#include <fcntl.h>
// for GetNameInfo ResolveName function
#include <netdb.h>
// To read program config file 
#include "rapidxml-1.13/rapidxml.hpp"
// git clone https://github.com/brofield/simpleini.git
#include "simpleini/SimpleIni.h"
#include "common.h"
// For Header in ProcessData
#include <map>
#include <unordered_map>
// All functions in one place
#include "functions.h"




unsigned int PORT = 28080;
unsigned int WAITINGQUEUE = 15;  // No of new clients in active queue
const int MAXCLIENTS = 30;
unsigned int client_sock[MAXCLIENTS] {0};
bool DEBUG = false;
#define HEADER_MAX 8192
#define CRLF "\r\n"

using namespace std;






void ParseConfig() 
{
        using namespace rapidxml;
      //Parse Config file
        CSimpleIniA ini;
        ini.SetUnicode();
        ini.SetMultiKey();
        ini.SetAllowKeyOnly();
        SI_Error rc = ini.LoadFile("config.ini");
        //if (rc < 0) debugOUT("Unable to open configuration file: config.ini");
        guard(rc,"Unable to open configuration file: config.ini");
        debugOUT("Configuration file confi.ini loaded");
        // Check Debug settings
        const char* cD;
        cD = ini.GetValue("General", "debug", "0", 0);
            DEBUG = static_cast<int>(*cD) - 48;
            if (DEBUG) debugOUT("DEBUG Enabled");
        // Check listenng port
        cD = ini.GetValue("General", "port", "28080", 0);
        PORT = atoi(cD);
        debugOUT("Listening on PORT:", PORT);
        //cD = ini.GetAllValues("General","SupportedType", )
        // Get Supported Types
}


void ProcessData(char *buffer, int bytesRecv, int &clientfd)
{
    // BAsed on: https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/index.html
    // Display message        // Display what client sent!
    //cout << "Recieved: " << string(buffer, 0,bytesRecv) << endl;
    //debugOUT("Data Recieved:\n", string(buffer, 0,bytesRecv) );
 
 // Common aarguments
    
    size_t strBegin, srchEnd;
    std::string key, value;
    // QueryString MAP
    std::unordered_map<std::string, std::string> queryMAP;
    // Header Values MAP
    std::map<std::string, std::string> headerMAP;
//TODO    Create FTD Data MAP hollding decoded parameters OR Create object holding all data about device
     std::unordered_map<std::string, std::string> FTD2;


 // PART 1 - Process Header Data
    /* Look for the end-of-header (eoh) CRLF CRLF substring; */
    char *eoh;
    eoh = strstr (buffer, "\r\n\r\n");
    const char * query;
    //cout << buffer;
    
    if(eoh != NULL) 
    {
        //eoh point to double new line \r\n\r\n
        /* Replace the blank line of CRLF CRLF with \0 to split the  header and body */
        eoh[2] = '\0';

        char *line = buffer;
        char *eol = strstr (line, "\r\n");
        size_t body_length = 0;
        
        std::string::size_type index;
        std::string hline;
        while (eol != NULL)
        {
                /* Null-terminate the line by replacing CRLF with \0 */
                *eol = '\0';
                hline = line;
                index = hline.find(':', 0);
                // check if next char after : is space so it doesnt include POST containing :
                //if ((index != std::string::npos) && (index + 1 == ' '))
                if ((index != std::string::npos) && (hline[index+1] == ' '))
                {
                    //if (hline[index+1] == ' ') cout << "'"<< hline[index] << hline[index+1] << "'" << endl;
                    std::string prvi = trim(hline.substr(0, index));
                    std::string drugi = trim(hline.substr(index + 1));
                    //cout << "Prvi,Drugi: [" << prvi << "] , [" << drugi << "]" << endl;
                    headerMAP.insert(std::make_pair(prvi, drugi));
                }
                // Check if line containing POST if yes, parse it, null if not find.
                
                else if ( (hline.find("POST") || hline.find("GET") ) != std::string::npos )
                {
// PART 1 - Process POST Header Query
                   // cout << line;                  
                    debugOUT("Parsing QUERY Pharameters from", hline);
                    processPOSTquery(queryMAP,line);
                    if (DEBUG) printMAP(queryMAP);
                }

                /* Move the line pointer to the next line */
                line = eol + 2;
                eol = strstr (line, "\r\n");
        }
// PART 1 - Process Header Data
        debugOUT("Parsing received headers");
        if (DEBUG) printMAP(headerMAP);
            
        if(headerMAP.find("Content-Length") != headerMAP.end())
        {
            body_length = stoi( headerMAP["Content-Length"] );
            debugOUT("Header lenght reported", body_length);

            char *body = strdup (eoh + 4);
            debugOUT("Body lenght",strlen (body));
            
            if (body_length > strlen (body)) // if false, all data received
            {
            debugOUT("Not all data recieved, extending buffer", strlen(body));
                /* Increase the body size and read additional data from the
            socket; the number of bytes to request is the Content-Length
            field minus the number of bytes already received */
            body = (char*) realloc(body, body_length);
            bytesRecv = read (clientfd, body + strlen (body), body_length - strlen (body));
            }

            const static std::map<std::string, int> str2case
            {
                {"application/json",1},
                {"application/xml", 2}
            };
        
            // Process Payload Data // if nothing is found, revert to default case 0
            switch( str2case.count(headerMAP["Content-Type"]) ? str2case.at(headerMAP["Content-Type"]) : 0 )
            {
                case 1:
                {
                    //cout << "Recieved: " << string(buffer, 0,bytesRecv) << endl;
                    debugOUT("JSON Payload selected");
                    //Send REPLY
                    ConnSendReply(clientfd);
                    debugOUT("Successfull respond sent!, Closing connection, no JSON parser!!!");
                    shutdown(clientfd, SHUT_RDWR);   
                    break;
                }
                case 2:
                    //cout << "Recieved: " << string(buffer, 0,bytesRecv) << endl;
                    debugOUT("XML Payload selected");
                    ConnSendReply(clientfd);
                    debugOUT("Successfull respond sent!, Closing connection, processing data");

                    debugOUT("Data (Body) part is", body);
                    
                    if (body != nullptr ) 
                    {
                        parseXML(body, body_length, FTD2);
                        // Parse XML and put data in MAP
                        if (DEBUG) printMAP(FTD2);
                    }
                    // cout << body << endl;
                    shutdown(clientfd, SHUT_RDWR);

                    break;

                case 0:
                    if((send(clientfd,"Unnsupported Content-type\n", 27, 0)) == 27)
                    
                        debugOUT("Send unnsuported client type successfully");
                    break;
                    
            }

            // FILL THE XLM, put values in MAP, fill the DB
            // 1 GET QUERY PARMS

            // 2. DECODE BODY DATA Packet
            // debugOUT("Body\n", body);




            free(body);
        }
    }
    else debugOUT("Nullptr - Input doesn't contain right http headers.");
}


string ResolveName(sockaddr_in &client, int choice = 99)
{
    char host[NI_MAXHOST];
    char port [NI_MAXSERV];
    string r_str;
    //memset(host, 0, NI_MAXHOST);
    memset(port, 0, NI_MAXSERV);
    getnameinfo((sockaddr *)&client,
                                    sizeof(client),
                                    host,
                                    NI_MAXHOST,
                                    port,
                                    NI_MAXSERV,
                                    NI_NUMERICSERV);

    
    switch (choice) 
    {    
    case 1: //return host;
        r_str.append(" [");
        r_str.append(host);
        r_str.append("]");
        return r_str;
        break;
    
    case 2: // return port
        r_str.append(" [");
        r_str.append(host);
        r_str.append("]");
        return r_str;
        break;

     case 99: // return host : port
        r_str.append(" [");
        r_str.append(host);
        r_str.append("]");
        r_str.append(":");
        r_str.append(port);
        return r_str;
        break;
    
    default:
        return "none";
    }
}



// BAsed on : https://www.youtube.com/watch?v=cNdlrbZSkyQ
// https://stackoverflow.com/questions/1287306/difference-between-string-and-char-types-in-c
// 
//setsockopt options 
// https://linux.die.net/man/7/socket




int main() 
{
    // ParseConfig file
    ParseConfig();
    // inizialize to ZERO
    int master_SOCK, new_SOCK, client_SOCK[MAXCLIENTS] {0}, activity, i, sock_descriptor, max_sock_descriptor;
    ssize_t bytes;
    struct sockaddr_in adr;
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    // Supported Content Types init.

   
    
    /* The calloc() function returns:

    a pointer to the start of the memory block allocated by the function.
    null pointer if allocation fails.
    Allocate a buffer to handle initial responses up to 8 KB
    */
    char *buff;   // OLD: char buff[4096]; // Data buffer of 4K
    buff = (char *)calloc(HEADER_MAX + 1, sizeof (char));
     if (!buff) {
      cout << "Memory Allocation Failed";
      exit(1);
   }

    fd_set readfds;     //Collection of socket descriptors for which we are ready to READ over network

// 1st to create a MASTER SOCKET (Listening) - socket to detect arrival of new connections req from new clients
    
    // Create socket it opens a file desctriptor
    // SOCK_STREAM (TCP) vs SOCK_DGRAM (UDP)
    // init socket () if -1 then print error
    // non-Blocking Linux "SOCK_STREAM | SOCK_NONBLOCK"  or blocking "SOCK_STREAM" directly!
    //int nSocket = guard(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0), "Can't create a socket!");
    master_SOCK = guard(socket(AF_INET, SOCK_STREAM , 0), "Can't create a socket!");

    // in addtition get current flags, and set additional "O_NONBLOCK" sockoption
    int flags = guard(fcntl(master_SOCK, F_GETFL), "Could not get current flags on TCP listening socket");
    guard(fcntl(master_SOCK,F_SETFL, flags | SO_REUSEADDR | O_NONBLOCK ), " couldn't set requested flag(s) to TCP listening socket");
    
    debugOUT("Master Socket initialized as", master_SOCK);
    adr.sin_family = AF_INET;
    //srv.sin_addr.s_addr = inet_addr("0.0.0.0");
    //inet_pton(AF_INET,"0.0.0.0",&hint.sin_addr); //Convert Strings IP to integer and store to sin_addr
    adr.sin_addr.s_addr = INADDR_ANY;
    // host to network byte order conversion, ntohs - reverse
    adr.sin_port = htons(PORT);

    guard(bind(master_SOCK,(struct sockaddr *)&adr,sizeof(adr)),"Binding to master sock failed...");
    int addrlen = sizeof(adr);
    // Mark the socket master_SOCK as socket that will be used to accept incoming connection requests using accept.
    if(listen(master_SOCK, WAITINGQUEUE) < 0) debugOUT("Listen issue");
    cout << "Application started. Listening for connections on port: " << PORT << endl;

    

    while(1) 
    {
        // Initializing set to ZERO and adding the master socket
        FD_ZERO(&readfds);
        FD_SET(master_SOCK, &readfds);
        max_sock_descriptor = master_SOCK;

        // Add child sockets to set
        for (i = 0; i < MAXCLIENTS; i++)
        {
            // check current list for still valid sockets and re-add them to FD
            if (client_SOCK[i] > 0) FD_SET(client_SOCK[i], &readfds);
            // Highest File Descriptor Number which is needed for the select function!!
            if (client_SOCK[i] > max_sock_descriptor) max_sock_descriptor = client_SOCK[i];
        }

        // Waiting for something to happen on the master socket As the wait is NULL wait is indefinite.
        // Every time "select" is called, once done it clears all FD selectors in sets!!!!!!!!!!
        // So we need to SET them over again. Slect return -1 on error
        activity = select(max_sock_descriptor + 1, &readfds, nullptr, nullptr, &timeout);
        if (activity < 0) debugOUT("Select call failed, returned: ", activity);
        
        // Check if there is any activity on master socket, if yes this is threated as new connection.
        
        if(FD_ISSET(master_SOCK, &readfds))
        {
            if ((new_SOCK = accept(master_SOCK, (struct sockaddr *)&adr, (socklen_t *)&addrlen)) < 0)
            {
                // if ZERO or a anything negative
                debugOUT("New accept call failed", new_SOCK);
                exit(EXIT_FAILURE);
            }

            //Printing out the socket number which will be used to send and receive messages
            debugOUT("New Connection established ", ResolveName(adr));
            debugOUT("New Connection FD", new_SOCK);
            //return "Connection established to client" on -1 or false return issue
            //if (send(new_SOCK,"Connection established\n", 24, 0) != 24) debugOUT("Connection confirmation send error");

            // Add new Socket to FD Set Array
            for(i = 0; i < MAXCLIENTS; i++) 
            {
                if(client_SOCK[i] == 0) 
                {
                    client_SOCK[i] = new_SOCK;
                    debugOUT("New socket added to FD array As", new_SOCK);
                    break;
                }
            }
        }

        //If not the master socket then it checks is some i/o activity on some other socket
        for (i = 0; i < max_sock_descriptor; i++)
        {
            sock_descriptor = client_SOCK[i];
            
            if(FD_ISSET(sock_descriptor, &readfds))
            {
                //Checking if the activity was for closing OR reading the incoming message
                if ((bytes = read(sock_descriptor, buff, HEADER_MAX)) == 0)
                {
                    // Return info regarding client which disconnected
                    getpeername(sock_descriptor, (struct sockaddr *)&adr, (socklen_t *)&addrlen);
                        // ntons - host to network byte order conversion, ntohs - reverse
                    //cout << "Closing connection by Host: " << inet_ntoa(adr.sin_addr) << " : " << ntohs(adr.sin_port) << endl;
                    debugOUT("Connection closed to host", ResolveName(adr));
                

                    close(sock_descriptor);
                    // clear FD set position
                    client_SOCK[i] = 0;
                }
                
                else
                {
                //PROCESS DATA, add it to class
                    //cout << "Echoing back the message that came in the socket!" << endl;
                        // send(sock_descriptor,"GOT IT\n", 8, 0);

                //Read the packet header and check the length of the payload if
                // lenght is complete, send reply OK, and 
                    buff[bytes] = '\0';
                    ProcessData(buff, bytes, sock_descriptor);
                    //int total_recv = recv_timeout(sock_descriptor, 4);
                    //send (sock_descriptor,buff, strlen(buff), 0);
                }
            }
        }
    }

    free(buff);
    return EXIT_SUCCESS;
}