#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include "io_extended.h"
#include <cstdlib>
#include <Windows.h>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")

void scan(int from, int to, std::mutex& obj, char* addr);
void socket_connect(sockaddr_in host, std::mutex& obj);

int main(void)
{
#define PORTS_IN_THREAD 100
#define NUMBER_OF_PORTS 65536

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	std::cout << "R-Scan v1.01 by NightGhost" << std::endl
		<< "the port scanner" << std::endl
		<< "supports only IPv4 addresses" << std::endl;

	WSADATA filler;

	if (WSAStartup(0x0202, &filler))
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "WSA was not initialized: " << WSAGetLastError() << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		WSACleanup();

		return EXIT_FAILURE;
	}

	std::cout << "R-Scan is ready. You must enter a remote host name and number of ports (0 to scan all of them)" << std::endl
		<< "or the range of ports, for example: from 49152(exclusively) to 65535" << std::endl
		<< "if you chosen a range, the program will create many of threads, one thread to scan " << PORTS_IN_THREAD << " ports" << std::endl
		<< "if you chosen a number, program will scan ports in range from 0 to number - 1" << std::endl << std::endl;

	char h_name[MAXGETHOSTSTRUCT];

	std::cout << "Enter a host name >> ";
	std::cin.getline(h_name, MAXGETHOSTSTRUCT);

#pragma warning(disable: 4996)
	HOSTENT* remote_host_info = gethostbyname(h_name);

	if (remote_host_info == NULL)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "Can't get an information about remote host " << h_name << ": " << WSAGetLastError() << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		WSACleanup();

		system("pause");
		return EXIT_FAILURE;
	}

	std::cout << "Remote host info: " << std::endl
		<< "Host name: " << remote_host_info->h_name << std::endl;
	switch (remote_host_info->h_addrtype)
	{
	case AF_INET :
		std::cout << "Address type: IPv4" << std::endl;
		break;
	case AF_INET6 :
		std::cout << "Address type: IPv6" << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "Sorry, program can't process IPv6 addresses" << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		WSACleanup();

		system("pause");
		return EXIT_FAILURE;
	}
	if (remote_host_info->h_aliases[0])
	{
		std::cout << "Host aliases:" << std::endl;

		for (int i = 0; remote_host_info->h_aliases[i]; i++)
			std::cout << remote_host_info->h_aliases[i] << std::endl;
	}
	else
		std::cout << "No aliases" << std::endl;
	
	in_addr* temp = new in_addr;
	if (remote_host_info->h_addr_list[1])
	{
		std::cout << "Host addresses:" << std::endl;
		for (int i = 0; remote_host_info->h_addr_list[i]; i++)
		{
			temp->s_addr = *(u_long*)remote_host_info->h_addr_list[i];
			std::cout << inet_ntoa(*temp) << std::endl;
		}
	}
	else
	{
		temp->s_addr = *(u_long*)remote_host_info->h_addr;
		std::cout << "Host address: " << inet_ntoa(*temp) << std::endl;
	}

	delete temp;
		
	std::cout << "What do you choose?" << std::endl
		<< "1) specify range of ports" << std::endl
		<< "2) specify number of ports" << std::endl;

	int i;

	check_error(std::cin, i);
	clear_istream(std::cin);

	switch (i)
	{
	case 1 :
	{
		int from, to;

		std::cout << "Please enter beginning port: ";
		check_error(std::cin, from);
		clear_istream(std::cin);
		std::cout << "Please enter ending port: ";
		check_error(std::cin, to);
		clear_istream(std::cin);

		int number_of_ports = to - from + 1,
			extra_ports = number_of_ports % PORTS_IN_THREAD,
			l_to;
		std::vector<std::thread> threads;
		std::mutex obj;
		
		std::cout << "Scanning begins..." << std::endl;

		if (number_of_ports < 100)
		{
			threads.push_back(std::thread(scan, from + 1, to, ref(obj), remote_host_info->h_addr));
			std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
		}
		else
		{
			for (int from_l = from + 1, to_l = from_l + 99; to_l <= to; to_l += 100, from_l += 100)
			{
				l_to = to_l + 1;
				threads.push_back(std::thread(scan, from_l, to_l, ref(obj), remote_host_info->h_addr));
			}
			if (extra_ports)
				threads.push_back(std::thread(scan, l_to, l_to + extra_ports, ref(obj), remote_host_info->h_addr));

			std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
		}
	}
	break;

	case 2 :
	{
		int number_of_ports;

		std::cout << "Please enter a number of ports: ";
		check_error(std::cin, number_of_ports);
		clear_istream(std::cin);

		if (!number_of_ports)
			number_of_ports = NUMBER_OF_PORTS;

		int extra_ports = number_of_ports % PORTS_IN_THREAD, l_to;
		std::vector<std::thread> threads;
		std::mutex obj;

		std::cout << "Scanning begins..." << std::endl;

		if (number_of_ports < 100)
		{
			threads.push_back(std::thread(scan, 0, number_of_ports - 1, ref(obj), remote_host_info->h_addr));
			std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
		}
		else
		{
			for (int from = 0, to = 99; to < number_of_ports; to += 100, from += 100)
			{
				l_to = to + 1;
				threads.push_back(std::thread(scan, from, to, ref(obj), remote_host_info->h_addr));
			}
			if (extra_ports)
				threads.push_back(std::thread(scan, l_to, l_to + extra_ports - 1, ref(obj), remote_host_info->h_addr));

			std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
		}
	}
	break;

	default :
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "Parameter is not valid" << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		WSACleanup();

		return EXIT_SUCCESS;
	}
			
	std::cout << "Scanning is complete" << std::endl;

	system("pause");
	return EXIT_FAILURE;
}

void scan(int from, int to, std::mutex& obj, char* addr)
{
	std::vector<std::thread> underthreads;
	sockaddr_in host;

	host.sin_family = AF_INET;

	in_addr temp;

	temp.s_addr = *(u_long*)addr;
	inet_pton(AF_INET, (PCSTR)inet_ntoa(temp), &host.sin_addr);

	for (int i = from; i <= to; i++)
	{
		host.sin_port = htons(i);
		underthreads.push_back(std::thread(socket_connect, host, ref(obj)));
	}

	std::for_each(underthreads.begin(), underthreads.end(), std::mem_fn(&std::thread::join));
}

void socket_connect(sockaddr_in host, std::mutex& obj)
{
	WSADATA filler;

	if (WSAStartup(0x0202, &filler))
	{
		obj.lock();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "WSA was not initialized: " << WSAGetLastError() << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		obj.unlock();

		WSACleanup();

		return;
	}

	SOCKET scan_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (!scan_socket)
	{
		obj.lock();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "Invalid socket to scan port " << ntohs(host.sin_port) << ": " << WSAGetLastError() << std::endl;

		if (closesocket(scan_socket))
			std::cout << "Can't close socket: " << WSAGetLastError() << std::endl;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		obj.unlock();

		WSACleanup();

		return;
	}

	if (!connect(scan_socket, (SOCKADDR*)&host, sizeof(host)))
	{
		obj.lock();
		std::cout << "Connection is established on port " << ntohs(host.sin_port) << std::endl;
		obj.unlock();
	}

	if (closesocket(scan_socket))
	{
		obj.lock();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << "Can't close socket: " << WSAGetLastError() << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		obj.unlock();

		return;
	}

	WSACleanup();
}
