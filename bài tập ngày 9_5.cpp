// MultithreadServer.cpp : Tao 1 ung dung chat gom 1 server va 3 client: Client 1 gui hello den server rồi server gui cho 2 client con lai
//



#include <stdio.h>



#include <winsock2.h>



#pragma comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS
DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);



SOCKET clients[64];
int numClients;



int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);



    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);



    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);



    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);



    numClients = 0;



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





        if (strcmp(cmd, "all") == 0)    //chat nhom, neu du lieu nhan duoc la all... thi chuyen sang chat nhom 
        {
            // Gui cho cac client khac
            for (int i = 0; i < numClients; i++)
                if (clients[i] != client) // tranh viec gui lai cho chinh client dang dui len
                {
                    char* msg = buf + strlen(cmd) + 1; // message gui di bat dau tu ki tu tiep theo sau cmd
                    send(clients[i], msg, strlen(msg), 0);
                }
        }
        else // Chat ca nhan
        {
            int id = atoi(cmd); // chuyen chuoi cmd sang so
            // Gui cho client id
            for (int i = 0; i < numClients; i++)
                if (clients[i] == id)
                {
                    char* msg = buf + strlen(cmd) + 1;
                    send(clients[i], msg, strlen(msg), 0);
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