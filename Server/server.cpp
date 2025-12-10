#include "server.hpp"

// =============================================== [CONSTRUCTOR AND DESTRUCTOR] ===================================
Server::Server(uint16_t port) : m_serverSocket(-1)
{
    /* Set the port NO */
    m_Port = port;
    
    // master account to use to close
    const char* username = "master";
    const char* password = "shutdown";
    User user;
    memcpy(user.username, username, 7);
    memcpy(user.password, password, 9);
    m_users.push_back(user);

    // remove any old secret files
    if(std::filesystem::exists(m_secrets)){
        std::filesystem::remove_all(m_secrets);
    }
    // create secrets folder
    if(!std::filesystem::create_directories(m_secrets)){
        srv_log("ERROR CREATING SECRETS FOLDER");
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
        srv_log("bind error");
        return;
    }
    while(m_running){
        listen_wrapper();
        if(m_serverSocket == -1)
        { 
            srv_log("listen error");
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


/** Wrapper listen function to mark socket file descriptor as a passive socket to listen and comminucate with. */
void Server::listen_wrapper()
{
    int status;
    status = listen(m_serverSocket,  5);
    if(status == -1)
    {
        srv_log("error calling listen");
        return;
    }

    m_clientSocket = accept(m_serverSocket, nullptr, nullptr);
    if(m_clientSocket == -1)
    {
        srv_log("error accepting connection");
        return;
    }
    std::cout << std::endl;
    srv_log("--HANDLING CLIENT--");
    
    // clear all vars and buffers
    m_bytesRecv = 0;
    m_bytesSent = 0;

    memset(m_send, 0, sizeof(m_send));
    memset(m_recv, 0, sizeof(m_recv));
    memset(m_command, 0, sizeof(m_command));
    memset(m_username, 0, sizeof(m_username));
    memset(m_password, 0, sizeof(m_password));
    memset(m_payload, 0, sizeof(m_payload));
   
    
    /* Wait for the message */
    m_bytesRecv = recv(m_clientSocket, m_recv, sizeof(m_recv), 0);
    
    status = handle_message();

    // 1 means message was invalid
    if(status){
        memcpy(m_send, m_invalid.c_str(), m_invalid.length());
        m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
        srv_log("MESSAGE FORMAT INVALID");
        std::cout << std::endl;
    }else{
        srv_log("--CLIENT HANDLED--");
        std::cout << std::endl;
    }
    close(m_clientSocket);   
}

int Server::handle_message()
{
    try{
        // parse message
        std::stringstream ss(m_recv);
        ss >> m_command;
        ss >> m_username;
        ss >> m_password;

        // get payload
        char* start = strchr(m_recv, '*');
        if(start == NULL){
            return 1;
        }
        memcpy(m_payload, start + 1, MAX_PAYLOAD);
        // store payload size
        m_payloadSize = m_bytesRecv - 10 - strlen(m_username) - strlen(m_password);
    }
    catch(...){
        //message was invalid
        return 1;
    }

    // parsed values
    srv_log("----------------------");
    srv_log("COMMAND", m_command);
    srv_log("username", m_username);
    srv_log("password", m_password);
    srv_log("payload", m_payload);
    srv_log("----------------------");
    
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
        m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
        
        srv_log("RESPONSE","CREATED");
        return 0;
    }

    // verify username and password for all further commands
    if(!verify_user()){
        srv_log("USERNAME OR PASSWORD INVALID");
        //invalid username or password no hints give about which
        return 1;
    }
    srv_log("USER VALIDATED");

    if(!strcmp(m_command, "EXIT")){
        m_running = false;
        return 0;
    }
    
    // EXPLOIT HERE - username buffer can be overflow to change the file handle associated with account
    if(!strcmp(m_command, "USERNAME")){
        srv_log("index", m_user_index);
        memcpy(m_users[m_user_index].username, m_payload, m_payloadSize);   
        memcpy(m_send, m_stored.c_str(), m_stored.length());
        m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
        srv_log("NEW USERNAME STORED");
        return 0; 
    }
    
    // store new password
    if(!strcmp(m_command, "PASSWORD")){
        memcpy(m_users[m_user_index].password, m_payload, m_payloadSize); 
        memcpy(m_send, m_stored.c_str(), m_stored.length());
        m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
        srv_log("NEW PASSWORD STORED");
        return 0;
    }

    // send file contents back
    if(!strcmp(m_command, "LOGIN")){
        get_note();
        m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
        srv_log("CONTENTS OF SECRET RETURNED");
        return 0;
    }

    // save payload to file
    if(!strcmp(m_command, "LOGOFF")){
        store_note();
        memcpy(m_send, m_stored.c_str(), m_stored.length());
        m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
        srv_log("CONTENTS OF SECRET STORED");
        return 0;
    }
    // invalid command    
    return 1;
}

// seen if there is alreday a user with same username
bool Server::user_exist()
{
    for(User user : m_users){
        if(!strcmp(user.username, m_username)){
            srv_log("USER ALREADY EXISTS");
            return true;
        }
    }
    return false;
}


// check username and password
bool Server::verify_user(){
    int index = 0;
    for(User user : m_users){
        if(!strcmp(user.username, m_username)){
            m_user = user;
            m_user_index = index;
            if(verify_password()){
                return true;
            }
            return false;
        }
        index++;
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
    fp.write(m_payload, m_payloadSize);
}

// prep send buffer with file contents 
void Server::get_note(){
    std::ifstream fp(m_path + m_user.handle, std::ios::binary);
    fp.read(m_send, MAX_PAYLOAD);
}


int main(int argc, char* argv[]){

    if(argc < 2){
        std::cout << "please define port number" << std::endl;
        return -1;
    }
    if(strlen(argv[1]) != 4){
        std::cout << "port number must be 4 digits" << std::endl;
        return -1;
    }
    std::string port_str = "port";
    int port = atoi(argv[1]);

    srv_log(port_str, port);

    // create server and run, run loops
    Server server(port);
    server.run();

    return 0;
}


