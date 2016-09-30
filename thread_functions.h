#pragma once
#ifndef THR_FUNC_
#define THR_FUNC_

#include <WinSock2.h>
#include <mutex>
#include <cstdlib>
#include <WS2tcpip.h>
#include <Windows.h>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>

#define HTTP 80

void make_dos_overthread(HOSTENT remote_host, std::mutex& out_obj, char* message, int num);
void make_dos_underthread(sockaddr_in server, std::mutex& out_obj, char* message);

#endif
