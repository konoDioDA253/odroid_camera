#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <vector>


#define PORT 55000
#define BUFFER_SIZE 100

using namespace std;
using namespace cv;


void showImage(Mat& frame_received, char& keyPressed)
{
    namedWindow("window", WINDOW_AUTOSIZE);
    imshow("window", frame_received);
    return;
}

void receiveUint(SOCKET clientSocket, unsigned int &buffer_size)
{
        // unsigned int buffer_size; 
        unsigned int buffer_size_net; 
        recv(clientSocket, (char*)&buffer_size_net, sizeof(unsigned int),0);
        buffer_size = ntohl((unsigned int)buffer_size_net);
        cout << (unsigned int)buffer_size << endl;
        return;
}

void getImage(SOCKET clientSocket,Mat& frame_received, unsigned int& buffer_size)
{
        // unsigned int buffer_size; 
        // unsigned int buffer_size_net; 
        // recv(clientSocket, (char*)&buffer_size_net, sizeof(unsigned int),0);
        // buffer_size = ntohl((unsigned int)buffer_size_net);
        // cout << (unsigned int)buffer_size << endl;
        receiveUint( clientSocket, buffer_size);
        vector<uchar> buffer;
        // char* new_buffer = reinterpret_cast<char*>(buffer.data());
        buffer.resize((int)buffer_size);
        for	(int i =0; i < buffer_size; i++)
        {
            recv(clientSocket, (char*)buffer.data()/*buffer.data()*/ + i*sizeof(uchar), sizeof(uchar)/*buffer.size()*sizeof(uchar)*/,0);	
            // recv(clientSocket, buffer + i, buffer_size - i/*buffer.size()*sizeof(uchar)*/,0);	
        }
        // vector<uchar> buffer = reinterpret_cast<unsigned char*>(new_buffer);
        // char* pixels = reinterpret_cast<char*>(buffer);
        frame_received = imdecode(buffer,IMREAD_ANYCOLOR,&frame_received);
        return;
}

int main(int argc, char** argv) {
    Mat frame;
    char keyPressed = 0;
    char registreClient = '0';
    unsigned int buffer_size;


    WSADATA wsdata;

    if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
        printf("Fail to initiate ws2_d2.dll \n\n");
        return 0;
    }

    printf("Success to initiate ws_32.dll\n\n");

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN serverAddress;
    ZeroMemory(&serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    // ****************//VirtualBox IP '10.0.2.15'********************
    serverAddress.sin_addr.s_addr = inet_addr("10.0.0.86"); 

    if ((connect(clientSocket, (SOCKADDR*)(&serverAddress), sizeof(serverAddress)) != 0)) {
        printf("Fail to connect.\n\n");
        return 0;
    }

    printf("connected!!! \n\n");

    char toServer[BUFFER_SIZE];

    strcpy(toServer, "Hello Server!!! I'm client!");

    send(clientSocket, toServer, strlen(toServer) + 1, 0);

    printf("Data to server : %s\n\n", toServer);

    char fromServer[BUFFER_SIZE + 1];

    recv(clientSocket, fromServer, BUFFER_SIZE, 0);

    printf("received data from server : %s\n", fromServer);


    while(true)
    {

        Mat frame_received;// = Mat::zeros( 480,640, CV_8UC3); //= imread("E:\\Documents\\Automne_2022\\ELE4205\\mytest\\capture.png",IMREAD_COLOR);
        // int buffer_size  = (int) (frame_received.total()*frame_received.elemSize());
        // char * buffer;
        // buffer = (char *) malloc (sizeof(char) * buffer_size + 1);    
        // unsigned int buffer_size; 
        // unsigned int buffer_size_net; 
        // recv(clientSocket, (char*)&buffer_size_net, sizeof(unsigned int),0);
        // buffer_size = ntohl((unsigned int)buffer_size_net);
        // cout << (unsigned int)buffer_size << endl;

        // vector<uchar> buffer;
        // // char* new_buffer = reinterpret_cast<char*>(buffer.data());
        // buffer.resize((int)buffer_size);
        // for	(int i =0; i < buffer_size; i++)
        // {
        //     recv(clientSocket, (char*)buffer.data()/*buffer.data()*/ + i*sizeof(uchar), sizeof(uchar)/*buffer.size()*sizeof(uchar)*/,0);	
        //     // recv(clientSocket, buffer + i, buffer_size - i/*buffer.size()*sizeof(uchar)*/,0);	
        // }
        // // vector<uchar> buffer = reinterpret_cast<unsigned char*>(new_buffer);
        // // char* pixels = reinterpret_cast<char*>(buffer);
        // frame_received = imdecode(buffer,IMREAD_ANYCOLOR,&frame_received);

        getImage(clientSocket, frame_received, buffer_size);
        imwrite("capture.png",frame_received);


        // imwrite("capture.png", frame_received);

        // namedWindow("window", WINDOW_AUTOSIZE);
        // imshow("window", frame_received);
        // keyPressed = waitKey(1);
        showImage(frame_received, keyPressed);
        keyPressed = waitKey(1);
        send(clientSocket, &registreClient, sizeof(registreClient), 0);
        if (keyPressed == 27)
        {
            registreClient = '1'; //ESC key pressed, quit asap
            break;
        }


    }

    



    closesocket(clientSocket);
    WSACleanup();
    printf("Exit ws_32.dll\n\n");
    return 0;
}