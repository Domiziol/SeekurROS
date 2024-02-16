#include "rclcpp/rclcpp.hpp"
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;

class ControlNode : public rclcpp::Node{
    public:
    ControlNode(const char* ip_adressq, int portq) : Node("control_node"){

        sock = socket(AF_INET, SOCK_STREAM, 0);
        
        //	Create a hint structure for the server we're connecting with
        port = portq;
        ipAddress = ip_adressq;

        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

        //	Connect to the server on the socket
        int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
        if (connectRes == -1)
        {
            cerr << "error connecting"<<endl;
            
        }

        char msg = 'C';
        int sbytes = send(sock, (void*)&msg, sizeof(msg), 0);
        if(sbytes == -1){
            cout<<"nie udalo sie wyslac"<<endl;
            
        }

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(1000),
            std::bind(&ControlNode::timerCallback, this));
    }

    private:
    void timerCallback(){
        cout<<"podaj kierunek"<<endl;
        char input;
       
        cin >> input;
        if(input == 'w' || input == 'a' || input == 's' || input == 'd'){
          
            cout<<"wysylam kierunek"<<endl;
            send(sock, (const void*)(sizeof(input)), sizeof(input), 0);
            s = send(sock, (void*)&input, sizeof(input), 0);
            if(s <=0 ){
                cout<<"nie wyslalo sie"<<endl;
            }
            
        }
        else cout<<"niepoprawna komenda"<<endl;
    }
    rclcpp::TimerBase::SharedPtr timer_;
    int sock;
    int port;
    string ipAddress;
    uint32_t size;
    int s;
    char daj[4] = "daj";
    char buf[1];
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControlNode>("10.0.127.32", 54000);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
