// MultithreadServer.cpp : Tao 1 ung dung chat gom 1 server va 3 client: Client 1 gui hello den server rồi server gui cho 2 client con lai
//



#include <stdio.h>



#include <winsock2.h>
#include <string>
#include <iostream>
using namespace std;



#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS
DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);



SOCKET clients[64];
int numClients = 0;
string arrNameClient[1000];
string nameId[3] = { "kien","nam","phong" };


int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);



    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);



    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);



    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);

    
    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);
        printf("New client accepted: %d\n", client);

       

        // Them client vao mang
        clients[numClients] = client;
        numClients++;



        CreateThread(0, 0, ClientThread, &client, 0, 0);
    }



    closesocket(listener);
    WSACleanup();
}



DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET client = *(SOCKET*)lpParam;
    char buf[256], cmd[32];
    int ret;
    char askNameClient[] = "client_id: ";
    char resPon[16];
    BOOL flat = TRUE; //dùng để thoát vòng while khi nhập client_id
    while (flat) {
        send(client, askNameClient, strlen(askNameClient), 0);
        int ret = recv(client, resPon, sizeof(resPon), 0);
        if (ret < sizeof(resPon))
            resPon[ret - 1] = 0;
        for (int i = 0; i < 3; i++) {
            const char* str = nameId[i].c_str();
            if (strcmp(resPon, str) == 0) {
                arrNameClient[client] = nameId[i];
                flat = FALSE;
                cout << "da nhap dung ten \n";
                break;
            }
        }
    }
    flat = TRUE;
    while (1)
    {
        // Nhan du lieu tu client
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)// neu ma co loi thi xoa client do luon
        {
            RemoveClient(client);
            break;
        }


        // In ra man hinh
        buf[ret] = 0;
        printf("Received: %s\n", buf);



        // Xu ly du lieu
        // Doc tu dau tien va luu vao cmd
        sscanf(buf, "%s", cmd); //doc cac ky tu lien tiep nhau den khi xuat hien space 


        char sendBuf[256];
        strcpy(sendBuf, arrNameClient[client].c_str()); // coppy tên người gửi vào sendBuf
        int pos = arrNameClient[client].length();
        memcpy(sendBuf + pos, ": ",2);
        pos = pos + 2;

        char* msg = buf + strlen(cmd) + 1; // message gui di bat dau tu ki tu tiep theo sau cmd

        memcpy(sendBuf + pos, msg, 256);//chỗ này vẫn dc nhưng nó lạ lắm
        pos = pos + ret - strlen(cmd) - 1;
        sendBuf[pos + 1] = 0;

        if (strcmp(cmd, "all") == 0)    //chat nhom, neu du lieu nhan duoc la all... thi chuyen sang chat nhom 
        {
            
            // Gui cho cac client khac
            for (int i = 0; i < numClients; i++)
                if (clients[i] != client) // tranh viec gui lai cho chinh client dang dui len
                {
                    send(clients[i], sendBuf, strlen(sendBuf), 0);
                }
        }
        else // Chat ca nhan
        {
            int id = atoi(cmd); // chuyen chuoi cmd sang so
            // Gui cho client id
            for (int i = 0; i < numClients; i++)
                if (clients[i] == id)
                {
                    
                    send(clients[i], sendBuf, strlen(sendBuf), 0);
                    
                }
        }
    }
    closesocket(client);
}



void RemoveClient(SOCKET client)
{
    // Tim vi tri can xoa
    int i = 0;
    while (clients[i] != client) i++;



    // Xoa socket client khoi mang
    if (i < numClients - 1)
        clients[i] = clients[numClients - 1];



    // Giam so phan tu di 1
    numClients--;
}