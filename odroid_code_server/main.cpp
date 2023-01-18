#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <opencv2/opencv.hpp>
#include <vector>

#define PORT 55000

using namespace std;
using namespace cv;


void sendUint(int& client_socket, vector<uchar>& compressed_image, unsigned int& buffer_size )
{
    buffer_size = compressed_image.size();
    cout << buffer_size << endl;
    unsigned int size_buf_net = htonl(buffer_size);
    send(client_socket, (const char*)&size_buf_net, sizeof(size_buf_net), 0);
    return;
}

void sendImage(int& client_socket, vector<uchar>& compressed_image, Mat& frame, VideoCapture& capture, unsigned int& buffer_size)
{
    capture >> frame;
    imencode(".png", frame, compressed_image);
    // unsigned int buffer_size = compressed_image.size();
    // cout << buffer_size << endl;
    // unsigned int size_buf_net = htonl(buffer_size);
    // send(client_socket, (const char*)&size_buf_net, sizeof(size_buf_net), 0);
    sendUint(client_socket, compressed_image, buffer_size );
    send(client_socket, compressed_image.data(), compressed_image.size(), 0);
    return;
}

int main(){
	Mat frame, edges;
    char registreServeur = '0';
    unsigned int buffer_size; 
    vector<uchar> compressed_image; 
	VideoCapture capture(0);

    int server_socket;
    int client_socket;

    struct sockaddr_in server_address={0,};
    struct sockaddr_in client_address={0,};

    int client_address_size;

    char toClient[] = "Hello Client!!\n";
    char fromClient[100];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    printf("Server Socket Create!!!\n");

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family=AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    bind(server_socket,(struct sockaddr*)&server_address,
        sizeof(server_address));
    listen(server_socket, 5);

    printf("Wait Client...\n");
    client_address_size = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr*)&client_address,
                            (socklen_t*)&client_address_size);
    printf("Client Connect!!!\n");

    read(client_socket, fromClient, sizeof(fromClient));
    printf("From Client Message: %s\n", fromClient);

    write(client_socket, toClient, sizeof(toClient));
    printf("To Client Message: %s\n", toClient);

    capture.set(CAP_PROP_FRAME_WIDTH, 320);
    capture.set(CAP_PROP_FRAME_HEIGHT, 240);
	if (!capture.isOpened())
	{
		cout << "Failed to connect to the camera." << endl;
	}
    capture >> frame;
    capture >> frame;
    int frames=10;
    for(int i=0; i<frames; i++){
        // nomCapture = "capture" + i + ".png";
        capture >> frame;
        if(frame.empty()){
        cout << "Failed to capture an image" << endl;
        return -1;
        }
        // cvtColor(frame, edges, COLOR_BGR2GRAY);
        // Canny(edges, edges, 0, 30, 3);
        // imwrite("capture.png", frame);
    }

    while(true)
    {
        // capture >> frame;
        // imwrite("capture.png", frame);

        // imencode(".png", frame, compressed_image);
        // unsigned int buffer_size = compressed_image.size();
        // cout << buffer_size << endl;
        // unsigned int size_buf_net = htonl(buffer_size);

        // send(client_socket, (const char*)&size_buf_net, sizeof(size_buf_net), 0);
        // // int retTamImg;   
        // // retTamImg = frame.total() * frame.elemSize();
        // // send( client_socket, (const char *) frame.data, retTamImg, 0 );    
        // send(client_socket, compressed_image.data(), compressed_image.size(), 0);
        sendImage(client_socket, compressed_image, frame, capture, buffer_size);
        imwrite("capture.png", frame);
		recv(client_socket, &registreServeur, sizeof(registreServeur),0);
        if (registreServeur == '1')
        {
            break;			
        }
        
        printf("OK\n");

    }

    printf("QUIT\n");
    capture.release();

    close(client_socket);
    return 0;
}