#include "stdlib.h"
#include "conio.h"
#include <string.h>
//#include "stdio.h"
#include <iostream>
#include <stdio.h>

#define Range(a,x,b) (x>=a)&&(x<=b)
#define ItNUMinHEX(x) (	(x >=0x30) && (x <=0x39) || \
						(x >=0x41) && (x <=0x46) || \
						(x >=0x61) && (x <=0x66) )
#define MaskCRC(x) (x == 0x58) || (x == 0x78)

void print_request(unsigned char* buff, int size) {
	printf("Output:");
	for (int i = 0; i < size; ++i) {
		printf(" ");
		if (Range(0x00, buff[i], 0x09))
			printf("%X%X", 0, buff[i]);
		else 
			printf("%X", buff[i]);
	}
	printf("\n");
}

unsigned char two_number_toHEX(char left, char right) {
	return (unsigned char)((left << 4) | right);
}

unsigned char toHEX(char ch) {
	if (Range(0x30, ch, 0x39)) {
		ch -= 0x30;
	}
	else if (Range(0x41, ch, 0x46)) {
		ch -= 0x37;
	}
	else if (Range(0x61, ch, 0x66)) {
		ch -= 0x57;
	}
	else if (MaskCRC(ch)) {
		ch = 0x58;
	}

	return ch;
}

void request_with_CRC(char *str) {
	int size_request = strlen(str);
	int size_request_hex = size_request / 2;

	int i = 0, j = 0;
	unsigned char *requesthex = (unsigned char*)calloc(size_request_hex, sizeof(unsigned char));

	while (size_request - i != 0) {
		requesthex[j] = two_number_toHEX(toHEX(str[i]), toHEX(str[i + 1]));
		i += 2;
		j++;
	}
	unsigned char checksum = 0;
	unsigned char *checksum_buff = (unsigned char*)calloc(size_request_hex, sizeof(unsigned char));
	int size_checksum_buff = size_request_hex;
	j = 1;
	checksum_buff[0] = requesthex[0];
	for (int i = 1; i < size_request_hex - 2; ++i) {
		if (requesthex[i] == 0x7E || requesthex[i] == 0x5D) {
			size_checksum_buff++;
			checksum_buff = (unsigned char*)realloc(checksum_buff, size_checksum_buff);
			checksum_buff[j] = 0x5D;
			j++;
			checksum_buff[j] = requesthex[i] ^ 0x20;
		}
		else {
			checksum_buff[j] = requesthex[i];
		}
		j++;
	}
	checksum_buff[size_checksum_buff - 1] = requesthex[size_request_hex - 1];
	free(requesthex);

	j = 1;
	while (j != size_checksum_buff - 2) {
		checksum ^= checksum_buff[j];
		j++;
	}
	checksum_buff[size_checksum_buff - 2] = checksum;

	print_request(checksum_buff, size_checksum_buff);

	free(checksum_buff);
}

char check_request(char *str) {
	int size = strlen(str);
	if (size >= 7 && size % 2 == 0) {
		if ((size > 6) && two_number_toHEX(toHEX(str[0]), toHEX(str[1])) == 0x7E &&
			two_number_toHEX(toHEX(str[size - 2]), toHEX(str[size - 1])) == 0x7E &&
			toHEX(str[size - 4]) == 0x58 && toHEX(str[size - 3] == 0x58)
			) {
			return 0;
		}
		else {
			return -2;
		}
	}
	else {
		return -1;
	}
}

void delite_space(char* str) {
	int i, j;
	int size = strlen(str);
	char* bufftmp = (char*)calloc(size, sizeof(char));
	// removing spaces 
	for (i = 0, j = 0; i < size; i++)
		if (str[i] != ' ') 
			bufftmp[j++] = str[i];
	bufftmp[j] = '\0';
	memset(str, -1, size);
	memcpy(str, bufftmp, size);

	free(bufftmp);
}

char analysis_request(char* str) {
	bool flag = false;
	char ret = 0;
	for (int i = 0; i < strlen(str); ++i) {
		if (ItNUMinHEX(str[i]) || str[i] == ' ' || MaskCRC(str[i])) {
			flag = true;
		}
		else {
			flag = false;
		}

		if (flag == false) {
			ret = -1;
			break;
		}
	}
	return ret;
}

int main(int argc, char* argv) {
	char request[256];
	int ret = 0;
	printf("Checksum calculation\n\n");
	printf("Input Format: 7E HEAD CRC 7E\nAttention! In request CRC = XX\n\n\n");

	while (true) {
		lable:
		printf("Input: ");
		gets_s(request);

		if (analysis_request(request) != 0) {
			printf("Wrong format!\n");
			goto lable;
		}
	
		delite_space(request);
		ret = check_request(request);

		if (ret != 0) {
			switch (ret)
			{
			case -1:
				printf("Length error!\n");
				goto lable;
			case -2:
				printf("Flag or checksum format error!\n");
				goto lable;
			default:
				break;
			}
		}
		else {
			request_with_CRC(request);
			memset(request, -1, 256);
		}
		
		printf("To continue, press any key. To exit press ESC\n\n");

		if (_getch() == 27)
			exit(0);
	}
	return 0;
}

