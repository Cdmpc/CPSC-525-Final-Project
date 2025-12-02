# include "client.hpp"

Client::Client(std::string hostname, uint16_t port)
{
    // hostname must be ip till updated to resolve host 
    m_hostname = hostname;
    m_port = port;
    log("object created");
}

Client::~Client(){

}

void Client::run(){
    std::cout << "Welcome to SUPER DUPER SECURE NOTES ... " << std::endl;
    std::cout << "Store your deepest darkest secrets with us!" << std::endl;
    std::cout << std::endl;
    std::cout << "If you don't already have acccount one will be created for you." << std::endl;
    std::cout << std::endl;
    get_username();
    get_password();
    while(m_editing){
        handle_commands();
    }
}

void Client::get_username()
{
    //loop continues until user enters valid username
    bool invalid = true;
    std::string temp;
    while (invalid){
        std::cout << "Please enter your username: ";
        std::getline(std::cin, temp);
        log(temp);
        if(temp.length() < 1 || temp.length() > 31){
            temp.clear();
            continue;
        }
        else{
            invalid = false;
        }
    }
    m_username = temp;
}

void Client::get_password()
{
        //loop continues until user enters valid username
    bool invalid = true;
    std::string temp;
    while (invalid){
        std::cout << "Please enter your password: ";
        std::getline(std::cin, temp);
        log(temp);
        if(temp.length() < 8 || temp.length() > 31){
            temp.clear();
            continue;
        }
        else{
            invalid = false;
        }
    }
    m_password = temp;
}

void Client::connect_to_server()
{
    // resolve hostname //m_connected should be local????
    while(!m_connected){
        /* Create the socket file descriptor. */
        m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if(m_clientSocket == -1)
        {
            log("falied to create socket");
            sleep(1);
            log("reattemptng...");
            m_connected = false;
            continue;
        }
        /* Configuration of IP and PORT */
        sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(m_port);
        // Insert server_ip as bytes into client_address.sin_addr buffer. 
        inet_pton(AF_INET, m_hostname.c_str(), &server_address.sin_addr);

        /* Connect to the server on the socket. */
        int conn_res = connect(m_clientSocket, (const struct sockaddr*)&server_address, (socklen_t)sizeof(sockaddr_in));
        if(conn_res == -1)
        {
            log("failed to connect to server");
            sleep(1);
            log("reattemptng...");
            m_connected = false;
            continue;
        }
        /* We have successfully connected to the server. */
        m_connected = true;
    }
    m_connected = false;
}

void Client::handle_commands()
{
    log("handling commands");
    // first try get note LOGIN
    get_note();
    m_editing = false;



    // if response is invalid create user

    // save contents to file / create file if created user

    // while m_editing

        // wait for a command from terminal (user can edit textfile here)

        // if command == 1 (LOGOFF)
            // send file contents to server
            // delete file
            // close app (m_editing false)

        // if command == 2 (UPDATE USERNAME)
            // ask for username ensure size is correct

        // if command == 3 (UPDATE PASSWORD)
            // ask for password ensure size is correct

}

void Client::execute_command()
{
    // if ececuting command, last command handled
    m_response.clear();
    // connect and execute
    log(m_command);
    memcpy(m_send, m_command.c_str(), m_command.length());
    connect_to_server();
    m_bytesSent = send(m_clientSocket, m_send, sizeof(m_send), 0);
    log("bytes sent", m_bytesSent);
    m_bytesRecv = recv(m_clientSocket, m_recv, sizeof(m_recv), 0);
    m_response = m_recv;
    log("response", m_response);

    //cleanup
    memset(m_send, 0, sizeof(m_send));
    memset(m_recv, 0, sizeof(m_recv));
}

void Client::get_note(){

    // format command
    m_command.clear();
    m_command.append("LOGIN ");
    m_command.append(m_username);
    m_command.append(" ");
    m_command.append(m_password);
    m_command.append(" *");

    // execute
    execute_command();

    // handle response
    if(m_response == m_invalid){
        create_user();
    }
    //if user didn't exist then empty file, else what was stored
    //go to temp.txt in your current directory
    create_file_to_edit();
}

void Client::create_user()
{
    bool taken = true;
    while(taken){
        // format command
        m_command.clear();
        m_command.append("CREATE ");
        m_command.append(m_username);
        m_command.append(" ");
        m_command.append(m_password);
        m_command.append(" *");

        // try create account
        execute_command();

        // successful
        if(m_response == m_created){
            m_response.clear();
            taken = false;
        }
        if(m_response == m_taken){
            std::cout << "Username taken" << std::endl;
            std::cout << std::endl;
            get_username();
            get_password();
            continue;
        }

    }
}

void Client::store_note()
{
    // execute log off command
}

void Client::update_username()
{
    // execute 
}

void Client::update_password()
{
    // execute
}

void Client::create_file_to_edit()
{
    std::ofstream fp("temp.txt", std::ios::binary);
    fp.write(m_response.data(), m_response.length());
}

int main(int argc, char* argv[]){

    if(argc < 3){
        std::cout << "useage : port hostname" << std::endl;
        return -1;
    }
    if(strlen(argv[2]) != 4){
        std::cout << "port number must be 4 digits" << std::endl;
        return -1;
    }
    std::string port_str = "port";
    int port = atoi(argv[2]);
    std::string hostname = argv[1];

    // create server and run, run loops
    Client client(hostname, (uint16_t)port);
    client.run();
    return 0;
}
