#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include "parseXML.h"
#include <vector>


// BAsed on : https://www.youtube.com/watch?v=cNdlrbZSkyQ
// https://stackoverflow.com/questions/1287306/difference-between-string-and-char-types-in-c

using namespace std;

int main() 
{
    // Create socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        cerr << "Can't create a socket!";
        return -1;
    }
    // Bind socket to IP and port

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port=htons(28080);
    inet_pton(AF_INET,"0.0.0.0",&hint.sin_addr); //Convert Strings IP to integer and store to sin_addr
    
    if (bind(listening, (struct sockaddr *)&hint, sizeof(hint)) == -1) {

        cerr << "Can't bind to IP/port";
        return -2;
    }
    // Mark the socket for listening in6_addr

    if (listen(listening, SOMAXCONN) == -1) {

        cerr << "Can't listen!";
        return -3;
    }

    // Create client object side - Accept the call
    sockaddr_in client;
    socklen_t clientSize  = sizeof(client);
    char host[NI_MAXHOST];
    char svc [NI_MAXSERV];

    int clientSocket = accept(listening, (sockaddr *)&client, &clientSize);

    if (clientSocket == -1)
    {
        cerr << "Problem with client connecting!";
        return -4;
    }
    // Close the listening socket
    close(listening);
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    //int result = getnameinfo((sockaddr *)&client,
    getnameinfo((sockaddr *)&client,
                                    sizeof(client),
                                    host,
                                    NI_MAXHOST,
                                    svc,
                                    NI_MAXSERV,
                                    0);

    
    
        cout << host << " is connected on " << svc << endl;
    
    // While receiving display message, echo message
    char buf[4096];
    string buf;
    while (true)
    {
        // Clear the buffer
        memset(buf,0,4096);
        // Wait for message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1)
        {
            cerr << "There was a connection issue" << endl;
            break;
        }
        if (bytesRecv == 0)
        {
            cout << "The client disconnected!" << endl;
            break;
        }
        // Display message        // Display what client sent!
        cout << "Recieved: " << string(buf, 0,bytesRecv) << endl;
        
        // IDEA IS TO CLOSE CONN ONCE DATA Reception is completed
        if (string(buf).find("</MyStudentsData>") != string::npos) 
        {
            cout << "Found closing char: End"<<endl;
            // PROCESS DATA (extract from XML input)
            
            // Convert it to char
            vector<char> buffer(buf, buf + sizeof(buf)/sizeof(*buf));
            parseXML(buffer);
            break;
        
        }        
    }
    // Resend message to client
        //send(clientSocket, buf, bytesRecv + 1, 0);
        send(clientSocket, "Status OK\n",sizeof("Status OK")+1, 0);
    // Close socket
        close(clientSocket);

    

    return 0;
}