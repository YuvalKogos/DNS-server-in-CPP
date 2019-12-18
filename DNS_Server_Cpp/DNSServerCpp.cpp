// DNSServerCpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <WS2tcpip.h>
#include <winsock.h>
#include <vector>
#include <math.h>
#include <iomanip>

#pragma comment (lib,"ws2_32.lib")
using namespace std;

char SetBitInByte(char byte, int idx, bool one)
{
	if (one)
	{
		char ret = (char)(int(byte) | (1 << idx));
		return ret;
		
	}
	else
		return (char)(int(byte) & ~(1 << idx));
}

int BinrayToInt(string num)
{
	int result = 0;
	int pownum = 7;
	for (int i = 0; i < num.size(); i++)
	{
		if (num[i] == '1')
			result += pow(2, pownum);
		pownum--;
	}

	return result;
}

string GetFlags(char* data)
{
	char org_flag_1 = data[2];
	char org_flag_2 = data[3];

	string binary_num_1 = "";

	//QR
	binary_num_1 += "1";
	for (int i = 1; i < 5; i++)
		binary_num_1 += data[i];
	//AA
	binary_num_1 += '1';
	//TC
	binary_num_1 += '0';
	//QR
	binary_num_1 += '0';

	//Byte 2
	string binary_num_2 = "00000000";

	/*char ret[2];
	ret[0] = org_flag_1;
	ret[1] = org_flag_2;*/
	string ret;
	ret += char(BinrayToInt(binary_num_1)) + char(BinrayToInt(binary_num_2));
	return ret;

}



bool CheckCharOfDomain(char a,char split)
{
	int i = int(a);
	if ((i < 97 || i > 122) && (i != 46) && i != int(split))
		return false;
	return true;
}

vector<string> GetDomainName(char* data)
{
	bool state = false;
	int curr_length = 0;
	vector<string> domain_parts;
	string domain_str = "";
	int x = 0;
	char split = data[12];
	//loop over the question query from byte 12 (after the header)
	for (int i = 12; i < 100; i++)
	{
		if (state)
		{
			if (int(data[i]) == 0) //byte != 0
			{
				domain_parts.push_back(domain_str);
				break;
			}
			domain_str += data[i];
			x++;
			if (x == curr_length)
			{
				domain_parts.push_back(domain_str);
				domain_str = "";
				state = false;
				x = 0;
			}


		}
		else
		{
			state = true;
			curr_length = (int)(data[i]);
		}
	}
	/*cout << "printing domain parts :\n";
	for (string part : domain_parts)
		cout << "part " << part << endl;*/
	return domain_parts;
}

void PrintQuery(char* data);
void PrintQuery(const char* data);

unsigned char* BuildResponse(char* data, vector<int> IP)
{
	unsigned char response[512];
	int idx = 0;
	////////////Header section////////////
	string header = "";
	//Transaction ID
	response[idx++] = data[0];
	response[idx++] = data[1];
	
	//Get the flags
	//header += GetFlags(data);
	response[idx++] = (0x84);
	response[idx++] = (0x00);

	//Question count (1 in 2 bytes)
	response[idx++] = (0);
	response[idx++] = (1);

	//Answer count (1 in 2 bytes)
	response[idx++] = (0);
	response[idx++] = (1);

	//Nameserver count (0 in 2 bytes)
	response[idx++] = (0);
	response[idx++] = (0);

	//Aditional count (0 in 2 bytes)
	response[idx++] = (0);
	response[idx++] = (0);


	//////////Question Query///////////
	vector<string> domain_parts = GetDomainName(data);
	string question = "";
	//Append each part of the domain name
	for (string part : domain_parts)
	{
		int length = part.size();
		response[idx++] = char(length);
		//Append each char in domain part
		for (int i = 0; i < length; i++)
		{
			response[idx++] = part[i];
		}
	}
	//QType (1 in 2 bytes)
	response[idx++] = (0);
	response[idx++] = (1);
	//QClass (1 in 2 bytes)
	response[idx++] = (0);
	response[idx++] = (1);
	 

	//////////Answer section///////////
	string answer = "";
	
	//Domain name (compression - c0\0c)
	response[idx++] = (0xc0);
	response[idx++] = (0x0c);
	
	//Type
	response[idx++] = (0);
	response[idx++] = (0x01);
	
	//Class
	response[idx++] = (0);
	response[idx++] = (0x01);

	//TTL (200) in 4 bytes
	response[idx++] = (0);
	response[idx++] = (0);
	response[idx++] = (0);
	response[idx++] = (0x10);

	//Return IP adress (192.3.2.1)
	//length
	response[idx++] = (0x00);
	response[idx++] = (0x04);
	response[idx++] = (0x04);
	response[idx++] = (0x03);
	response[idx++] = (0x02);
	response[idx++] = (0x01);


	//Response query
	cout << "Query answer : " << idx <<  endl;
	//PrintQuery(response);
	for (int i = 0; i < 100; i++)
	{
		if (response[i] < 97 || response[i] > 122)
			cout << int(response[i]) << " ";
		else
			cout << response[i] << " ";
	}


	return response;
}

void PrintQuery(char* data)
{
	for (int i = 0; i < 50; i++)
	{
		if (int(data[i]) >= 97 && int(data[i]) <= 122)
			cout << data[i] << " ";
		else
			cout << int(data[i]) << " ";
	}
	cout << endl;
		
}

void PrintQuery(const char* data)
{
	for (int i = 0; i < 50; i++)
	{
		if (int(data[i]) >= 97 && int(data[i]) <= 122)
			cout << data[i] << " ";
		else
			cout << int(data[i]) << " ";
	}
	cout << endl;

}

void PrintQuery(unsigned char* data)
{
	for (int i = 0; i < 50; i++)
	{
		if (int(data[i]) >= 97 && int(data[i]) <= 122)
			cout << data[i] << " ";
		else
			cout << int(data[i]) << " ";
	}
	cout << endl;

}

char* createQuestion()
{
	char response[512];
	int idx = 0;
	////////////Header section////////////
	string header = "";
	//Transaction ID
	response[idx++] = char(0x00);
	response[idx++] = char(0x01);

	//Get the flags
	//header += GetFlags(data);
	response[idx++] = char(0x81);
	response[idx++] = char(0x80);

	//Question count (1 in 2 bytes)
	response[idx++] = char(0);
	response[idx++] = char(1);

	//Answer count (1 in 2 bytes)
	response[idx++] = char(0);
	response[idx++] = char(1);

	//Nameserver count (0 in 2 bytes)
	response[idx++] = char(0);
	response[idx++] = char(0);

	//Aditional count (0 in 2 bytes)
	response[idx++] = char(0);
	response[idx++] = char(0);


	//////////Question Query///////////
	vector<string> domain_parts = {"www","google","com"};
	string question = "";
	//Append each part of the domain name
	for (string part : domain_parts)
	{
		int length = part.size();
		response[idx++] = char(length);
		//Append each char in domain part
		for (int i = 0; i < length; i++)
		{
			response[idx++] = part[i];
		}
	}
	//QType (1 in 2 bytes)
	response[idx++] = char(0);
	response[idx++] = char(1);
	//QClass (1 in 2 bytes)
	response[idx++] = char(0);
	response[idx++] = char(1);

	return response;
}


int maxRectInHist(int* arr,int idx,int length)
{
	if (idx == length - 1)
		return arr[idx];
	int seq = 1;
	for (int i = idx+1; i < length; i++)
	{
		if (arr[i] < arr[idx])
			break;
			seq++;
	}

	return max(seq * arr[idx], maxRectInHist(arr, idx + 1, length));
}

void findKSmalletsInArray(int* arr,int length, int k)
{
	int* ret = new int[k];
	int ret_idx = 0;
	while (ret_idx < k)
	{
		//Find the min of the array
		int min = INT32_MAX;
		int min_idx = 0;
		for (int i = 0; i < length; i++)
		{
			if (arr[i] < min)
			{
				min = arr[i];
				min_idx = i;
			}
		}
		
		ret[ret_idx] = arr[min_idx];
		arr[min_idx] = INT32_MAX;
		ret_idx++;
	}

	for (int i = 0; i < k; i++)
	{
		cout << ret[i] << " ";
	}
	cout << endl;
}



int main()
{
	int arr[12] = {9,2,-1,1,2,3,4,5,0,3,3,2};
	findKSmalletsInArray(arr, 12, 3);
	cout << maxRectInHist(arr,0, 12) << endl;
	cout << sizeof(arr) << endl;
	cin.get();
	vector<int> IP = { 10,23,212,69 };
	
	char* flags = new char[2];
	flags[0] = char(1);
	flags[1] = char(0);
	cout << int(GetFlags(flags)[0]) << "," << int(GetFlags(flags)[1]) << endl;
	cout << int(SetBitInByte('a',1,true)) << endl;

	//Initialize socket
	WSADATA WSAData;
	SOCKET server;
	SOCKADDR_IN addr;

	//Initialize the socket
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	server = socket(AF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(53);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;


	int sizeofAddr = sizeof(addr);


	//Start binding
	bind(server, (sockaddr*)&addr, sizeof(addr));

	//Listen for queries
	char buffer[512];
	while (true)
	{
		
		
		if (recvfrom(server, buffer, 512, 0, (sockaddr*)&addr, (int*)&sizeofAddr) == SOCKET_ERROR)
			cout << "Couldn't recieve DNS query...\n";
		else
		{
			cout << "Query recieved : " << endl;
			cout << string(buffer) << endl;
			//cout << "SPLASPDFLAPSF: " << int(buffer[12]) << " , " << buffer[13] << endl;
			PrintQuery(buffer);
			//cout << buffer<< endl;
		}
		
		unsigned char* DNSResponse = BuildResponse(buffer,IP);
		cout << "Response created." << endl;
		//PrintQuery(DNSResponse);
		cout << endl;
		char DNS_response[512];
		for (int i = 0; i < 512; i++)
			DNS_response[i] = DNSResponse[i];	
	
		if (sendto(server, DNS_response, 512, 0, (sockaddr*)&addr, (int)&sizeofAddr) == SOCKET_ERROR)
			cout << "Couldn't send DNS answer... \n";
	}

	system("Pause");
    return 0;
}

