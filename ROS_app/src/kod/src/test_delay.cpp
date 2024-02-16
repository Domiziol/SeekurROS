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

class TestNode : public rclcpp::Node{
    public:
    TestNode(const char* ip_adressq, int portq) : Node("test_node"){

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

        char msg = 'T';
        int sbytes = send(sock, (void*)&msg, sizeof(msg), 0);
        if(sbytes == -1){
            cout<<"nie udalo sie wyslac"<<endl;
            
        }

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(1000),
            std::bind(&TestNode::timerCallback, this));
    }

    private:
    void timerCallback(){
        const auto now = std::chrono::system_clock::now();
        const auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);
        const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        std::stringstream nowSs;
        nowSs
            << std::put_time(std::localtime(&nowAsTimeT), "%T")
            << '.' << std::setfill('0') << std::setw(3) << nowMs.count();
        string t = nowSs.str();
        memset(sent, 0, 30);
        strcpy(sent, t.c_str());
        size = strlen(sent);
        
        //send(sock, (const void*)&size, sizeof(size), 0);
        s = send(sock, (void*)&sent, strlen(sent), 0);
        if(s <=0 ){
            cout<<"nie wyslalo sie chyba"<<endl;
        }
       
    }
    rclcpp::TimerBase::SharedPtr timer_;
    int sock;
    int port;
    string ipAddress;
    uint32_t size;
    int s;
    char sent[30];
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TestNode>("10.0.127.32", 54000);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
