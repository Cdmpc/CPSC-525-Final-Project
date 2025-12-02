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
    returning_user();
}

void Client::returning_user()
{
    //loop continues until user enters valid username
    bool invalid = true;
    std::string temp;
    std::cout << "Do you already have and account (Y/N) ? : ";
    while (invalid){
        std::getline(std::cin, temp);
        log(temp);
        if(temp.find('Y') == -1  && temp.find('y') == -1 && temp.find('N') == -1 && temp.find('n') == -1){
            temp.clear();
            std::cout << "(Y/N) ? : ";
            continue;
        }
        else{
            std::transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
            char entered;
            temp.find('Y') < temp.find('N') ? (entered = temp[temp.find('Y')]) : (entered = temp[temp.find('N')]);
            if(entered == 'Y'){
                get_username();
                get_password();
                get_note();
            }
            else{
                get_username();
                get_password();
                create_user();
            }
            invalid = false;
        }
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
            std::cout << "username length must be in range [1,31]" << std::endl;
            std::cout << "Please enter your username: ";
            continue;
        }
        else{
            invalid = false;
        }
    }
    m_username = temp;
}

void Client::get_new_username()
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
            std::cout << "username length must be in range [1,31]" << std::endl;
            std::cout << "Please enter your username: ";
            continue;
        }
        else{
            invalid = false;
        }
    }
    m_new_username = temp;
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
            std::cout << "password length must be in range [8,31]" << std::endl;
            std::cout << "Please enter your password: ";
            continue;
        }
        else{
            invalid = false;
        }
    }
    m_password = temp;
}

void Client::get_new_password()
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
            std::cout << "password length must be in range [8,31]" << std::endl;
            std::cout << "Please enter your password: ";
            continue;
        }
        else{
            invalid = false;
        }
    }
    m_new_password = temp;
}

void Client::get_note(){

    bool invalid = true;
    while(invalid){
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
            std::cout << "INVALID username or password" << std::endl;
            returning_user();
        }
        else{
            invalid = false;
        }
    }
    //go to temp.txt in your current directory
    create_file_to_edit();
    handle_commands();
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
            std::cout << "Username already taken" << std::endl;
            std::cout << std::endl;
            returning_user();
            continue;
        }
    }
    //go to temp.txt in your current directory
    create_file_to_edit();
    handle_commands();
}

void Client::handle_commands()
{
    log("handling commands");
    std::cout << "Your secret can be writen in your temp.txt file, remember to save when done" << std::endl;
    std::cout << std::endl;

    while(m_editing){
        std::cout << "UPLOAD TO SERVER AND EXIT (1)" << std::endl;
        std::cout << "UPDATE USERNAME           (2)" << std::endl;
        std::cout << "UPDATE PASSWORD           (3)" << std::endl;
        std::cout << "(1, 2, 3) ? : ";
        bool invalid = true;
        std::string temp;
        char command;
        while (invalid){
            std::getline(std::cin, temp);
            log(temp);
            if(temp.find('1') == -1  && temp.find('2') == -1 && temp.find('3') == -1){
                temp.clear();
                std::cout << "(1, 2, 3) ? : ";
                continue;
            }
            else{
                int lowest;
                
                temp.find('1') < temp.find('2') ? (lowest = temp.find('1')) : (lowest = temp.find('2'));
                if(temp.find('3') < lowest){
                    command = '3';
                }
                else{
                    command = temp[lowest];
                }
                invalid = false;
            }
        }
        switch (command){
            case '1' : 
                store_note();
                m_editing = false;
                break;
            case '2' :
                update_username();
                break;
            case '3' :
                update_password();
                break;
            default :
                log("How did that happen? Unexpected input in handle_commands switch");
                break;
        }
    }
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

void Client::execute_command()
{
    // if ececuting command, last command handled
    m_response.clear();
    // connect and execute
    log(m_command);
    memcpy(m_send, m_command.c_str(), m_command.length());
    connect_to_server();
    m_bytesSent = send(m_clientSocket, m_send, m_command.length(), 0);
    log("bytes sent", m_bytesSent);
    m_bytesRecv = recv(m_clientSocket, m_recv, sizeof(m_recv), 0);
    m_response = m_recv;
    log("response", m_response);

    //cleanup
    memset(m_send, 0, sizeof(m_send));
    memset(m_recv, 0, sizeof(m_recv));
}

void Client::store_note()
{
    m_command.clear();
    m_command.reserve(4096);
    m_command.append("LOGOFF ");
    m_command.append(m_username);
    m_command.append(" ");
    m_command.append(m_password);
    m_command.append(" *");
    file_to_payload();
    m_command.append(m_payload);
    log("command in store note", m_command);
    execute_command();

    // to do handle response
    log("STORING NOTE...now exiting");
}

void Client::update_username()
{   
    m_command.clear();
    m_command.reserve(4096);
    m_command.append("USERNAME ");
    m_command.append(m_username);
    m_command.append(" ");
    m_command.append(m_password);
    m_command.append(" *");
    get_new_username();
    m_command.append(m_new_username);
    execute_command();

    // to do handle response
    log("UPDATE USERNAME...now returning");
}

void Client::update_password()
{
    m_command.clear();
    m_command.reserve(4096);
    m_command.append("PASSWORD ");
    m_command.append(m_username);
    m_command.append(" ");
    m_command.append(m_password);
    m_command.append(" *");
    get_new_password();
    m_command.append(m_new_password);
    execute_command();

    // to do handle response
    log("UPDATE password...now returning");
}

void Client::create_file_to_edit()
{
    std::ofstream fp("temp.txt", std::ios::binary);
    fp.write(m_response.data(), m_response.length());
}

// prep send buffer with file contents 
void Client::file_to_payload(){
    memset(m_payload, 0, sizeof(m_payload));
    std::ifstream fp("temp.txt", std::ios::binary);
    fp.read(m_payload, MAX_PAYLOAD);
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
