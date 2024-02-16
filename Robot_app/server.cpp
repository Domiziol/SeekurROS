#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <stdexcept>
#include <map>
#include <chrono>
#include <iomanip>

using namespace std;
struct Message { 
    long type; 
    char text[100]; 
}; 

class Server{
    int my_socket;
    int fd_max;
    int newfd;
    int nbytes;
    fd_set my_master;
    fd_set read_fds;
    map<char, int> clients_ids;
    key_t key, key2, key3;
    int laser_queue_id, pose_queue_id, controls_queue_id; 
    char client_id[1];
    uint32_t size;
    int info_l, info_p, info_c;
    struct msqid_ds laser_q, pose_q, controls_q;
    char daj[4];

    public:
    Server(const char* ip_adress, int port, int key_l, int key_p, int key_c){
        my_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(my_socket == -1){
            cerr << "can't create a socket, exiting" <<endl;
            exiting();
        }

        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, ip_adress, &hint.sin_addr);

        if(bind(my_socket, (sockaddr*)&hint, sizeof(hint)) == -1){
            cerr << "can't bind, exiting" <<endl;
            exiting();
        }

        if(listen(my_socket, SOMAXCONN) == -1){
            cerr << "can't listen, exiting" <<endl;
            exiting();
        }

        laser_queue_id = msgget(key_l, 0666 | IPC_CREAT);
        pose_queue_id = msgget(key_p, 0666 | IPC_CREAT);
        controls_queue_id = msgget(key_c, 0666 | IPC_CREAT);

        msgctl(laser_queue_id, IPC_STAT, &laser_q);
        msgctl(pose_queue_id, IPC_STAT, &pose_q);
        msgctl(controls_queue_id, IPC_STAT, &controls_q);
        info_l = laser_q.msg_qnum;
        info_p = pose_q.msg_qnum;
        info_c = controls_q.msg_qnum;

        FD_ZERO(&my_master);
        FD_ZERO(&read_fds);
        FD_SET(my_socket, &my_master);
        fd_max = my_socket;
        cout<<"Accepting connections on port: "<<port<<endl;
    }

    void handle_new_connection(int i){
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);

        if((newfd = accept(my_socket, (sockaddr*)&client, &clientSize)) == -1){
            cout<<"Server - accept error!"<<endl;

        }else{
            cout<<"Server - accept is OK"<<endl;
            FD_SET(newfd, &my_master);
            if(newfd > fd_max){ fd_max = newfd; }

            nbytes = recv(newfd, client_id, sizeof(client_id), 0);
            if(nbytes <= 0){
                close(i);
                FD_CLR(i, &my_master);
            }

            char id = client_id[0];
            cout<<id<<endl;
            clients_ids[id] = newfd;
        }        
    }

    int exiting(){
        return 0;
    }

    void handle_existing_connection(int i){
       
        if(clients_ids.find('L') != clients_ids.end()){
            msgctl(laser_queue_id, IPC_STAT, &laser_q);
            info_l = laser_q.msg_qnum;

            if(i == clients_ids.at('L')){
                nbytes = recv(i, daj, sizeof(daj), 0);
                int clientSocket = i;

                    if(info_l > 0){       
                        Message message;
                        message.type = 1;
                        
                        memset(message.text, 0, 100);
                        msgrcv(laser_queue_id, &message, sizeof(message), message.type, 0);
                        size = strlen(message.text);
                        send(clientSocket, (const void*)&size, sizeof(size), 0);
                        send(clientSocket, message.text, strlen(message.text), 0);
                    }
                    else{
                        Message message;
                        message.type = 1;
                        
                        memset(message.text, 0, 100);
                        string nl = "-1 -1";
                        strcpy(message.text, nl.c_str());
                        size = strlen(message.text);
                        send(clientSocket, (const void*)&size, sizeof(size), 0);
                        send(clientSocket, message.text, strlen(message.text), 0);

                    }
            }
        }
        
        if(clients_ids.find('P') != clients_ids.end()){
            msgctl(pose_queue_id, IPC_STAT, &pose_q);
            info_p = pose_q.msg_qnum;

            if(i == clients_ids.at('P')){
                nbytes = recv(i, daj, sizeof(daj), 0);
                int clientSocket = i;
            
                if(info_p > 0){       //position "x y"
                    Message message;
                    message.type = 1;

                    memset(message.text, 0, 100);
                    msgrcv(pose_queue_id, &message, sizeof(message), message.type, 0);
                    size = strlen(message.text);
                    send(clientSocket, (const void*)&size, sizeof(size), 0);
                    send(clientSocket, message.text, strlen(message.text), 0);
                }
                else{
                    Message message;
                    message.type = 1;
                    
                    memset(message.text, 0, 100);
                    string nl = "-1 -1";
                    strcpy(message.text, nl.c_str());
                    size = strlen(message.text);
                    send(clientSocket, (const void*)&size, sizeof(size), 0);
                    send(clientSocket, message.text, strlen(message.text), 0);
                }
            }
        }
        

        if(clients_ids.find('C') != clients_ids.end()){
            msgctl(controls_queue_id, IPC_STAT, &controls_q);
            
            if(i == clients_ids.at('C')){       //control commands "direction" (wasd)
                Message message;
                message.type = 1;
                int clientSocket = i;
                uint32_t size;
                memset(message.text, 0, 100);
                char dir[10];
                memset(dir, 0, 10);
                
                int sizeReceived = recv(my_socket, (void*)&size, sizeof(size), 0);
                nbytes = recv(clientSocket, dir, size, 0);
                if(nbytes <= 0){
                    cout<<"error receiving data about motion direction"<<endl;
                }
                else{ 
                    strcpy(message.text, (const char*)&dir[0]);
                    msgsnd(controls_queue_id, &message, sizeof(message.text), 0);
                    cout<<"SERVER: otrzymalem od kontrolera: "<<message.text[0]<<endl;
                }
            }
        }
    }

    void run(){

        while(true){
            read_fds = my_master;
            if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1){
                cout<<"Server-select() error!"<<endl;
            }

            for(int i=0; i<=fd_max; i++){

                if(FD_ISSET(i, &read_fds)){
                    std::cout << "Handling clinet: " << i << std::endl;

                    if(i == my_socket){                                 
                        handle_new_connection(i);
                    }else{  
                        handle_existing_connection(i);                     
                    }
                }   
            }
        }
    }

};

int main(){
                //      IP,      port,  klucz_l,  klucz_p,  klucz_c
    Server server("10.0.127.32", 54000, 16938621, 33715846, 50493063);
    server.run();
   
    return 0;
}