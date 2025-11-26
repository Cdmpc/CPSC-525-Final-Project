#include "client.hpp"
#include "log.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

void fancy_print(const std::string& message) {
    size_t length = message.length();
    std::string separator(length + 20, '-');

    std::cout << separator << "\n";
    std::cout << "|         " << message << "         |\n";
    std::cout << separator << "\n";
};

/* ============================================ [CONSTRUCTORS AND DESTRUCTORS] =====================================================  */
Client::Client(const std::string & server_ip, int server_port) : socket_fd(-1), is_connected(0), was_sent(0), quit_flag(0)
{
    /* Set up the IP and Port number. */
    this->server_ip = server_ip;
    this->server_port = server_port;
    
    // std::string buffer initialization.
    this->send_buff = "0";
    this->send_buff.reserve(4096);
    
    // but recv_buff will be a std::vector<char>
    this->recv_buff = {'0'};
    this->recv_buff.resize(4096);
};

Client::~Client()
{
    /* Close off the server socket. */
    if(socket_fd == -1 && this->quit_flag == 0)
    {
        std:: cerr << "Something went wrong, serverSocket fd was never set!\n";
    }
    else
    {
        fancy_print("DISCONNECTED, GOODBYE!...");
    }
    close(socket_fd);
};

/* ============================================ [RUN() FUNCTION] =====================================================  */
void Client::run()
{
    /* Attempt connection. */
    bool conn_success = connect_to_server();
    if(!conn_success)
    {
        log_data("Client connection error");
        return;
    }
    else
    {
        /* Connection successful, try sending data. */
        fancy_print("WELCOME TO NOTEE-FY! (Type QUIT or EXIT to disconnect...)");
        bool send_status = 0;
        std::string recv_buff_string = "";
        while (get_conn_flag()) 
        {
            /* Do not close the program if you could not send data and did not QUIT. Try again. */
            send_status = send_data();
            if(!send_status && this->quit_flag == 0)
            {
                log_data("Client could not sent data to server");
                continue;
            }
            recv_buff_string = recieve_data();
            if(recv_buff_string.empty() && get_conn_flag() == 0 && this->quit_flag == 0)
            {
                log_data("Client did not recieve anything from server.");
                break;
            }
        }
    }

    /* Clean up */
    if(this->socket_fd != -1) {
        close(this->socket_fd);
        this->socket_fd = -1;
    }
}

/* ============================================ [CLASS MEMBER FUNCTIONS] =====================================================  */

/** Connects to the server, returns 1 if connection was successful, and 0 otherwise. */
bool Client::connect_to_server()
{
    /* Create the socket file descriptor. */
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(this->socket_fd == -1)
    {
        std::cerr << "Client::connect_to_server() --> socket(2) called failed due to: " << strerror(errno) << std::endl;
        this->is_connected = 0;
        return this->is_connected;
    }
    /* Configuration of IP and PORT */
    sockaddr_in client_address;
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons((uint16_t)this->server_port);
    // Insert server_ip as bytes into client_address.sin_addr buffer. 
    inet_pton(AF_INET, this->server_ip.c_str(), &client_address.sin_addr);

    /* Connect to the server on the socket. */
    int conn_res = connect(this->socket_fd, (const struct sockaddr*)&client_address, (socklen_t)sizeof(sockaddr_in));
    if(conn_res == -1)
    {
        std::cerr << "Client::connect_to_server() --> connect(2) failed due to: " << strerror(errno) << std::endl;
        return 0;
    }
    /* We have successfully connected to the server. */
    this->is_connected = 1;
    return this->is_connected;
};


bool Client::send_data()
{
    /* Only send data if the client managed to connect to the server. */
    if(get_conn_flag())
    {
        /* Arrow prompt, pipe user command into send_buff */
        std::cout << "[CLIENT] ENTER COMMAND: ";
        std::getline(std::cin, this->send_buff);
        std::transform(this->send_buff.begin(), this->send_buff.end(), this->send_buff.begin(), ::toupper);

        /* QUIT or EXIT command to shut off client communication. */
        if (send_buff == "QUIT" || send_buff == "EXIT") {
            this->was_sent = 0; // no further send/recv
            std::cout << "Shutting down client...\n";
            close(this->socket_fd);
            this->socket_fd = -1;
            this->quit_flag = 1;
            this->is_connected = 0;
            return false;
        }

        /* Send the populated string to the server. */
        ssize_t bytes_sent = send(this->socket_fd, this->send_buff.c_str(), this->send_buff.size() + 1, 0);
        if(bytes_sent == -1)
        {
            std::cerr << "Client::send_data() --> send(2) failed due to: " << strerror(errno) << std::endl;
            this->was_sent = 0;
            return this->was_sent;
        }
        /* Message can be sent successfully. */
        this->was_sent = 1;
        return this->was_sent;
    }
    else
    {
        std::cerr << "Client::send_data() --> Client::connect_to_server() did not connect!" << std::endl;
        this->was_sent = 0;
        return this->was_sent;
    }
}

std::string Client::recieve_data()
{
    /* Only recieve data from the server if the client sent data to it. */
    std::string recv_buff_string = "";
    if(this->was_sent)
    {
        /* Wait for the server's response and display it on the client's terminal. Return the buffer. */
        ssize_t bytes_recv = recv(this->socket_fd, this->recv_buff.data(), this->recv_buff.size() + 1, 0);
        if(bytes_recv == 0)
        {
            std::cout << "\nSERVER HAS CLOSED THE CONNECTION...\n";
            this->is_connected = 0;
        }
        else if (bytes_recv == -1) 
        {
            std::cerr << "Client::recieve_data() --> recv(2) failed due to: " << strerror(errno) << std::endl;
            recv_buff_string.clear();
            return recv_buff_string;
        }
        log_data("bytes recieved from server", (int)bytes_recv);
        std::string recv_buff_string(this->recv_buff.begin(), this->recv_buff.begin() + bytes_recv);
        std::cout << "\n[SERVER]: " << recv_buff_string << std::endl;
        return recv_buff_string;
    }
    else
    {
        if(this->quit_flag == 0)
            std::cerr << "Client::recieve_data() --> Client::send_to_data() did not send data!" << std::endl;
        recv_buff_string.clear();
        return recv_buff_string;
    }
}