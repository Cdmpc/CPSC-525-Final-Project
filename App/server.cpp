#include "server.hpp"
#include "log.hpp"
#include <cstddef>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

// =============================================== [CONSTRUCTOR AND DESTRUCTOR] ===================================
Server::Server(uint16_t port) : m_serverSocket(-1)
{
    /* Set the port NO */
    m_Port = port;
    if(std::filesystem::create_directories(m_secrets)){
        log_data("file Secrets created");
    }
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
    if(std::filesystem::exists(m_secrets)){
        std::filesystem::remove_all(m_secrets);
    }

}

// =============================================== [MEMBER FUNCTION DEFINITIONS] =================================== 
void Server::run()
{
    /* Calls the private member functions. */
    bind_wrapper();
    if(m_serverSocket == -1)
    {
        log_data("bind error");
        return;
    }
    if(listen(m_serverSocket, 5) == -1)
    {
        log_data("listen error");
        return;
    }
    while(1)
    {
        communication_wrapper();
        if(m_serverSocket == -1)
        { 
            log_data("listen error");
            return;
        }
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


/** Wrapper to parse and handle messages from the client. */
void Server::communication_wrapper()
{
    int status;
    // Reset client socket before accept. 
    m_clientSocket = -1;
    m_clientSocket = accept(m_serverSocket, nullptr, nullptr);
    
    if(m_clientSocket == -1)
    {
        log_data("error accepting connection");
        return;
    }
    printf("\n");
    log_data("handling client");
    
    // connection established
    m_connection = true;
    
    // clear all vars and buffers
    m_bytesRecv = 0;
    m_bytesSent = 0;

    /* Connection loop so that the client can send multiple messages at once. */
    while(true)
    {
        memset(m_send, 0, sizeof(m_send));
        memset(m_recv, 0, sizeof(m_recv));
        memset(m_command, 0, sizeof(m_command));
        memset(m_username, 0, sizeof(m_username));
        memset(m_password, 0, sizeof(m_password));
        memset(m_payload, 0, sizeof(m_payload));

        m_bytesRecv = recv(m_clientSocket, m_recv, sizeof(m_recv)-1, 0);
        if(m_bytesRecv <= 0)
        {
            log_data("Client disconnected or recv error.");
            break;
        }

        m_recv[m_bytesRecv] = '\0';  
        printf("\n");
        log_data("bytes recieved", m_bytesRecv);
        log_data("message", m_recv);

        status = handle_message();

        if(status) {
            memcpy(m_send, m_invalid.c_str(), m_invalid.length());
            m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
            log_data("INVALID");
        }
    }

    close(m_clientSocket);  // close when client disconnects
    m_clientSocket = -1;
    m_connection = false;
}

int Server::handle_message()
{
    try{
        // parse message
        std::stringstream ss(m_recv);
        ss >> m_command;
        ss >> m_username;
        ss >> m_password;

        // get payload after the * marker.
        char* start = strchr(m_recv, '*');
        if(start == NULL){
            return 1;
        }
        memcpy(m_payload, start + 1, MAX_PAYLOAD);
        // store payload size
        m_payloadSize = m_bytesRecv - 11 - strlen(m_username) - strlen(m_password);
    }
    catch(...){
        //message was invalid
        return 1;
    }

    // parsed values
    log_data("command", m_command);
    log_data("username", m_username);
    log_data("password", m_password);
    log_data("payload", m_payload);
    
    // create user
    if(!strcmp(m_command, "CREATE")){
        if(user_exist()){
            memcpy(m_send, m_taken.c_str(), m_taken.length());
            m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
            return 0;
        }
        // user doesn't exist safe to add
        User user;
        memcpy(user.username, m_username, sizeof(m_username));
        memcpy(user.password, m_password, sizeof(m_password));
        
        std::string id = std::to_string(m_numUsers++);
        memcpy(user.handle, m_handlePre, 4);
        memcpy(&user.handle[4], id.c_str(), id.length());
        memcpy(&user.handle[4 + id.length()], m_handlePost, 4);

        m_users.push_back(user);
        memcpy(m_send, m_created.c_str(), m_created.length());
        m_bytesSent = send(m_clientSocket, m_send, (size_t)m_created.length(), 0);
        
        log_data("CREATED");
        log_data("handle", user.handle);
        return 0;
    }

    // verify username and password for all further commands
    if(!verify_user()){
        //invalid username or password
        return 1;
    }
    log_data("password valid");
    
    // exploit - store new username
    if(!strcmp(m_command, "USERNAME")){
        memcpy(m_user.username, m_payload, m_payloadSize);   
        memcpy(m_send, m_stored.c_str(), m_stored.length());
        m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
        log_data("new username", m_user.username);
        log_data("new handle", m_user.handle);
        return 0; 
    }
    
    // store new password
    if(!strcmp(m_command, "PASSWORD")){
        memcpy(m_user.password, m_payload, m_payloadSize); 
        memcpy(m_send, m_stored.c_str(), m_stored.length());
        m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
        log_data("new password", m_user.password);
        return 0;
    }

    // send file contents back
    if(!strcmp(m_command, "LOGIN")){
        get_note();
        m_bytesSent = send(m_clientSocket, m_send, (size_t)m_bytesRecv, 0);
        log_data("contents returned");
        return 0;
    }

    // save payload to file
    if(!strcmp(m_command, "LOGOFF")){
        store_note();
        memcpy(m_send, m_stored.c_str(), m_stored.length());
        m_bytesSent = send(m_clientSocket, m_send, (size_t)m_stored.length(), 0);
        log_data("contents stored");
        return 0;
    }

    // invalid command    
    return 1;
}

bool Server::user_exist()
{
    for(const User & user : m_users){
        if(!strcmp(user.username, m_username)){
            return true;
        }
    }
    return false;
}

// still need to be figured
std::string Server::sha256(std::string& password)
{
    // unsigned char hash[SHA256_DIGEST_LENGTH];
    // SHA256_CTX sha256;
    // SHA256_Init(&sha256);
    // SHA256_Update(&sha256, data.c_str(), data.length());
    // SHA256_Final(hash, &sha256);

    std::string hash;
    return hash;
}

// check username and password
bool Server::verify_user(){
    for(const User & user : m_users){
        if(!strcmp(user.username, m_username)){
            m_user = user;
            log_data("found", m_username);
            if(verify_password()){
                return true;
            }
            return false;
        }
    }
    return false;
} 

// check password
bool Server::verify_password(){
    return !strcmp(m_user.password, m_password);
}

// store payload
void Server::store_note(){
    std::ofstream fp(m_path + m_user.handle, std::ios::binary);
    if (fp.is_open()) {
        fp.write(m_payload, m_payloadSize);
    } else {
        std::cerr << "Error opening file for write: " << m_path + m_user.handle << std::endl;
    }
}

// prep send buffer with file contents 
void Server::get_note(){
    /* Clear buffer before reading */
    memset(m_send, 0, sizeof(m_send));
    std::ifstream fp(m_path + m_user.handle, std::ios::binary);
    if(fp.is_open()){
        fp.read(m_send, MAX_PAYLOAD);
        m_bytesRecv = (int)fp.gcount();
    }
    else{
        std::cerr << "Server::get_note() failed!" << std::endl;
        m_bytesRecv = 0;
    }
}

    


