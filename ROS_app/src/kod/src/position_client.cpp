#include "rclcpp/rclcpp.hpp"
// #include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/int16.hpp"
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

class PositionNode : public rclcpp::Node{
    public:
    PositionNode(const char* ip_adressq, int portq) : Node("position_node"), count(0) {

        x_pub = this->create_publisher<std_msgs::msg::Int16>("position_x", 100);
        y_pub = this->create_publisher<std_msgs::msg::Int16>("position_y", 100);

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

        char msg = 'P';
        int sbytes = send(sock, (void*)&msg, sizeof(msg), 0);
        if(sbytes == -1){
            cout<<"nie udalo sie wyslac"<<endl;
            
        }

        //	While loop:
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(200),
            std::bind(&PositionNode::timerCallback, this));
    }

    private:
    void timerCallback()
    {
        send(sock, (void*)&daj, sizeof(daj), 0);
        string position;
        memset(buf, 0, 100);
        int sizeReceived = recv(sock, (void*)&size, sizeof(size), 0);
        if (sizeReceived == -1){
            cout << "There was an error getting response from server\r\n";
        }
        else{
            if(size != 0){
                //cout << "SERVER> size of next data: " << size << endl;
                int bytesReceived = recv(sock, buf, size, 0);
                if (bytesReceived == -1){
                    position = "";
                    cout << "There was an error getting response from server\r\n";
                }
                else{
                    position = string(buf, bytesReceived);
                    //cout << "SERVER> message: " << position << endl;
                }
            }
        }

        // trickier but for both laser range and angle of this scan
        auto x_msg = std_msgs::msg::Int16();
        auto y_msg = std_msgs::msg::Int16();

        int x, y;
        istringstream iss(position);
        iss >> x >> y;
        if(x != -1 && y != -1){
            cout<<"x: "<<x<<" y: "<<y<<endl;
            x_msg.data = x;
            x_pub->publish(x_msg);
            y_msg.data = y;
            y_pub->publish(y_msg);
        }
       
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr x_pub;
    rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr y_pub;
    int sock;
    int port;
    string ipAddress;
    char buf[100];
    char daj[4] = "daj";
    uint32_t size;
    size_t count;
    string message;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PositionNode>("10.0.127.32", 54000);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}