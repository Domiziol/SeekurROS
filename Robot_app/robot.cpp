#include "Aria.h"
#include "ariaUtil.h"

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>
#include <string.h>

#define sleeptime 400

using namespace std;

struct Message { 
    long type; 
    char text[100]; 
}; 


int main(int argc, char **argv){
    
    //*inicjalizacja robotyczna*
    Aria::init();
    ArRobot robot;
    ArArgumentParser parser(&argc, argv);
    parser.loadDefaultArguments();
    ArRobotConnector robotConnector(&parser, &robot);
    ArLaserConnector laserConnector(&parser, &robot, &robotConnector);
    parser.addDefaultArgument("-connectLaser");

    
    if(!robotConnector.connectRobot())
    {
        ArLog::log(ArLog::Terse, "Nie udało się nawiązać połączenia z robotem.");
        if(parser.checkHelpAndWarnUnparsed())
        {
            Aria::logOptions();
            Aria::exit(1);
        }
    }

    robot.runAsync(true);
    laserConnector.connectLasers();
    
    
    robot.lock(); // Zamkniecie mutexu i otwarcie wprowadzania komend ruchu
    robot.comInt(ArCommands::ENABLE, 1); // Wlaczenie napedow
    robot.stop(); //zatrzymanie robota
    robot.moveTo(ArPose(0,0,0)); //wyzerowanie położenia
    robot.setTransVelMax(200); // ustawienia max predkosci 
    robot.setRotVelMax(10);
    robot.enableMotors();
    robot.unlock(); // Otwarcie mutexu i zamkniecie petli na komendy
    int id_l = msgget(16938621, 0666 | IPC_CREAT);
    int id_p = msgget(33715846, 0666 | IPC_CREAT);
    int id_c = msgget(50493063, 0666 | IPC_CREAT);
    int info_c;

    struct msqid_ds laser_q, pose_q, controls_q;

    msgctl(id_c, IPC_STAT, &controls_q);

    info_c = controls_q.msg_qnum;

    Message lasers, poses, controls;

    int range;
    double angle;
    double x, y;
    int vel = 100, rotvel = 500;
    int distance = 400;
    int rot = 30;

    while(true){
        msgctl(id_c, IPC_STAT, &controls_q);
        info_c = controls_q.msg_qnum;
        if(info_c){
            memset(controls.text, 0, 100);
            memset(lasers.text, 0, 100);
            memset(poses.text, 0, 100);

            controls.type = 1;
            msgrcv(id_c, &controls, sizeof(controls), controls.type, 0);
            char dir = controls.text[0];
            
            //std::cout<<dir<<endl;
            if(dir == 'w'){
                std::cout<<"jade do przodu"<<endl;
                robot.lock();
                robot.setVel(vel);
                robot.move(distance);
                robot.unlock();

                while(true){
                    if (robot.isMoveDone(10)) // Jeżeli polecenie wykonane
                    {
                        break;
                    }
                        range = robot.checkRangeDevicesCumulativePolar(-100, 100, &angle);
                        string msg = to_string(range)+" "+to_string(angle);
                        strcpy(lasers.text, msg.c_str());
                        lasers.type = 1;
                        msgsnd(id_l, &lasers, sizeof(lasers.text), 0);
                        std::cout<<"range: "<<range<<" angle: "<<angle<<endl;
                        x = robot.getX();
                        y = robot.getY();
                        string msg_p = to_string(x)+" "+to_string(y);
                        strcpy(poses.text, msg_p.c_str());
                        poses.type = 1;
                        msgsnd(id_p, &poses, sizeof(poses.text), 0);
                        std::cout<<"x: "<<x<<" y: "<<y<<endl;
                        
                        ArUtil::sleep(sleeptime);
                }
            }

            if(dir == 's'){
                std::cout<<"jade do tylu"<<endl;
                robot.lock();
                robot.setVel(vel);
                robot.move(-distance);
                robot.unlock();

               while(true){
                    if (robot.isMoveDone(10)) // Jeżeli polecenie wykonane
                    {
                        break;
                    }
                        range = robot.checkRangeDevicesCumulativePolar(-100, 100, &angle);
                        string msg = to_string(range)+" "+to_string(angle);
                        strcpy(lasers.text, msg.c_str());
                        lasers.type = 1;
                        msgsnd(id_l, &lasers, sizeof(lasers.text), 0);
                        std::cout<<"range: "<<range<<" angle: "<<angle<<endl;
                        x = robot.getX();
                        y = robot.getY();
                        string msg_p = to_string(x)+" "+to_string(y);
                        strcpy(poses.text, msg_p.c_str());
                        poses.type = 1;
                        msgsnd(id_p, &poses, sizeof(poses.text), 0);
                        std::cout<<"x: "<<x<<" y: "<<y<<endl;
                        
                        ArUtil::sleep(sleeptime);
                }
            }

            if(dir == 'a'){
                std::cout<<"skrecam w lewo"<<endl;
                robot.lock();
                robot.setRotVel(rotvel);
                robot.setDeltaHeading(rot);
                robot.unlock();

                while(true){
                    if (robot.isHeadingDone(10)) // Jeżeli polecenie wykonane
                    {
                        break;
                    }
                        range = robot.checkRangeDevicesCumulativePolar(-100, 100, &angle);
                        string msg = to_string(range)+" "+to_string(angle);
                        strcpy(lasers.text, msg.c_str());
                        lasers.type = 1;
                        msgsnd(id_l, &lasers, sizeof(lasers.text), 0);
                        std::cout<<"range: "<<range<<" angle: "<<angle<<endl;
                        x = robot.getX();
                        y = robot.getY();
                        string msg_p = to_string(x)+" "+to_string(y);
                        strcpy(poses.text, msg_p.c_str());
                        poses.type = 1;
                        msgsnd(id_p, &poses, sizeof(poses.text), 0);
                        std::cout<<"x: "<<x<<" y: "<<y<<endl;
                        
                        ArUtil::sleep(sleeptime);
                }
            }

            if(dir == 'd'){
                std::cout<<"skrecam w prawo"<<endl;
                robot.lock();
                robot.setRotVel(rotvel);
                robot.setDeltaHeading(-rot);
                robot.unlock();

                while(true){
                    if (robot.isHeadingDone(10)) // Jeżeli polecenie wykonane
                    {
                        break;
                    }
                        range = robot.checkRangeDevicesCumulativePolar(-100, 100, &angle);
                        string msg = to_string(range)+" "+to_string(angle);
                        strcpy(lasers.text, msg.c_str());
                        lasers.type = 1;
                        msgsnd(id_l, &lasers, sizeof(lasers.text), 0);
                        std::cout<<"range: "<<range<<" angle: "<<angle<<endl;
                        x = robot.getX();
                        y = robot.getY();
                        string msg_p = to_string(x)+" "+to_string(y);
                        strcpy(poses.text, msg_p.c_str());
                        poses.type = 1;
                        msgsnd(id_p, &poses, sizeof(poses.text), 0);
                        std::cout<<"x: "<<x<<" y: "<<y<<endl;
                        
                        ArUtil::sleep(sleeptime);
                }
            }   
        }
    }

    Aria::exit(0); //Koniec
    return 0;

}