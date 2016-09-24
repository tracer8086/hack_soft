#ifndef THR_FUNC_
#define THR_FUNC_

#include <WinSock2.h>
#include <mutex>

#define HTTP 80

void make_dos_overthread(HOSTENT remote_host, std::mutex& out_obj, char* message, int num);
void make_dos_underthread(sockaddr_in server, std::mutex& out_obj, char* message);

#endif
