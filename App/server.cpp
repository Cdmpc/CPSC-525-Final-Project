#include "server.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cstdint>
#include <cstring>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// =============================================== [CONSTRUCTOR AND DESTRUCTOR] ===================================
Server::Server(uint16_t port) : m_serverSocket(-1)
{
    /* Set the port NO */
    m_Port = port;
}
Server::~Server()
{
    /* Close off the server socket. */
    if(m_serverSocket == -1)
    {
        std:: cerr << "Something went wrong, serverSocket fd was never set!\n";
    }
    else
    {
        std::cout << "\nAll done, closing Server()...\n";
    }
    close(m_serverSocket);
}

// =============================================== [MEMBER FUNCTION DEFINITIONS] =================================== 
void Server::run()
{
    /* Calls the private member functions. */
    bind_wrapper();
    if(m_serverSocket == -1)
    {
        std::cerr << "bind_wrapper() failed!" << std::endl;
        return;
    }
    listen_wrapper();
    if(m_serverSocket == -1)
    {
        std::cerr << "listen_wrapper() failed!" << std::endl;
        return;
    }

    while(1)
    {
        handle_message();
    }
}

/** Creates network TCP socket over IPv4 Addresses and binds it to the port specified by the Server class.  */
void Server::bind_wrapper()
{
    // socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket == -1) 
    { 
        std::cerr << "bind_wrapper() --> socket(2) failed because of: " << strerror(errno) << std::endl;
        return;
    }

    /* SO_REUSEADDR option so that the "address in not in use" error is avoided if connection is temrinated. */
    int optval = 1;
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        std::cerr << "setsockopt(SO_REUSEADDR) failed because of: " << strerror(errno) << std::endl;
    }
    
    /* Assemble IP and PORT configurations. */
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // IPv4 adresss
    serverAddress.sin_port = htons((uint16_t) m_Port); // supplied port
    serverAddress.sin_addr.s_addr = INADDR_ANY; // accept all connections

    /* Bind the socket to the IP/Port duo. The socket is the file descriptor. */
    int bind_status = bind(serverSocket, (struct sockaddr * ) &serverAddress, sizeof(serverAddress));
    if(bind_status == -1)
    {
        std::cerr << "bind_wrapper() --> bind(2) failed because of: " << strerror(errno) << std::endl;
        return;
    }
    /* Save the serverSocket fd into the equivalent member variable for use in the listen wrapper. */
    m_serverSocket = serverSocket;
}


/** Wrapper listen function to mark socket file descriptor as a passive socket to listen and comminucate with. */
void Server::listen_wrapper()
{
    /* Call listen(2), handle error if listen(2) fails. */
    int listen_status = listen(m_serverSocket,  SOMAXCONN);
    if(listen_status == -1)
    {
        std::cerr << "listen_wrapper() --> listen(2) failed because of: " << strerror(errno) << std::endl;
        return;
    }   
}


/* Accept the connection and store the client metadata on the server. */
void Server::handle_message()
{
    /* Define the client network metadata */
    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);

    /* Host device information buffer */
    char host[NI_MAXHOST];
    std::string host_str;

    /* Service requests from clients bufffer */
    char serv[NI_MAXSERV];
    std::string serv_str;

    /* Accept the incoming connection */
    int client_socket = accept(m_serverSocket, (struct sockaddr *) &client, &client_size);
    if(client_socket == -1)
    {
        std::cerr << "Server::handleMessage() --> accept(2) failed due to:" << strerror(errno) << std::endl;
        return;
    }
    else
    {
        std::cout << "Connection from client was successful!\n";
    }

    /* Nullify the C-string buffers */
    memset(host, 0, sizeof(host));
    memset(serv, 0, sizeof(serv));

    /* TODO: Change this if necessary if we are adding more functionality. */

    /* Extract metadata about the client into a readable format. */
    int metadata = getnameinfo((struct sockaddr *)&client, client_size, 
                                (char *) host, NI_MAXHOST, (char *) serv, NI_MAXSERV, 0);

    /* getnameinfo returns 0 on success. */
    if(metadata == 0)
    {
        host_str = host;
        serv_str = serv;
        std::cout << host_str << " connected on " << serv_str << std::endl;
    }
    else
    {
        /* Connect manually if getnameinfo fails somehow. */
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        host_str = host;
        std::cout << host_str << " connected on " << ntohs(client.sin_port) << std::endl;
    }

    /* Prepare the message */
    char msg_buf[4096];
    std::string msg_buf_string;
    msg_buf_string.reserve(4096);

    /* Loop to communicate from the server to the client listening on the other side. */
    ssize_t bytes_recv = 0;
    ssize_t bytes_sent = 0;
    while(1)
    {
        /* Clear the buffer */
        memset(msg_buf, 0, sizeof(msg_buf));
        /* Wait for the message */
        bytes_recv = recv(client_socket, msg_buf, sizeof(msg_buf), 0);

        // Something went wrong...
        if(bytes_recv == -1)
        {
            std::cerr << "Server::handleMessage() --> recv() failed due to: " << strerror(errno) << std::endl; 
            break;
        }
        if(bytes_recv == 0)
        {
            std::cout << "Server::handleMessage() --> The client disconnected, bytesRecv = 0\n";
            break;
        }

        /* Read or display what the client sent. */
        msg_buf_string = msg_buf;
        std::cout << "Received from client to server: " << msg_buf_string << std::endl;

        /* Send the message back to the Client */
        bytes_sent = send(client_socket, msg_buf, bytes_recv, 0);
        if(bytes_sent == -1)
        {
            std::cerr << "Server::handleMessage() --> send() failed due to: " << strerror(errno) << std::endl;
            break;
        }
        if(bytes_sent == 0)
        {
            std::cout << "Server::handleMessage() --> The server disconnected, bytes_sent = 0\n";
            break;
        }
    }

    // Close the client socket. 
    close(client_socket);
}
