#include "stdlib.h"
#include "conio.h"
#include <string.h>
//#include "stdio.h"
#include <iostream>
#include <stdio.h>

#define Range(a,x,b) (x>=a)&&(x<=b)

void print_request(unsigned char* buff, int size) {
	printf("Out:");
	for (int i = 0; i < size; ++i) {
		printf(" ");
		if (Range(0x00, buff[i], 0x09))
			printf("%X%X", 0, buff[i]);
		else 
			printf("%X", buff[i]);
	}
	printf("\n");
}

void checksum(unsigned char* buff, int size) {
	//unsigned char checksum = 0;
	//unsigned char *checksum_buff = (unsigned char*)calloc(size, sizeof(unsigned char));
	//int j = 0, size_checksum_buff = size - 3;
	//for (int i = 1; i < size - 3; ++i) {
	//	if (buff[i] == 0x7E || buff[i] == 0x5D) {
	//		size_checksum_buff++;
	//		checksum_buff = (unsigned char*)realloc(checksum_buff, size_checksum_buff);
	//		checksum_buff[j] = 0x5D;
	//		j++;
	//		checksum_buff[j] = buff[i]^0x20;
	//	}
	//	else {
	//		checksum_buff[j] = buff[i];
	//	}
	//	j++;
	//}

	//for (int i = 0; i < size_checksum_buff; ++i) {
	//	printf(" ");
	//	if (Range(0x00, checksum_buff[i], 0x09))
	//		printf("%X%X", 0, buff[i]);
	//	else
	//		printf("%X", checksum_buff[i]);
	//}


	unsigned char checksum = 0;
	unsigned char *checksum_buff = (unsigned char*)calloc(size, sizeof(unsigned char));
	int j = 1, size_checksum_buff = size;
	checksum_buff[0] = buff[0];
	for (int i = 1; i < size - 2; ++i) {
		if (buff[i] == 0x7E || buff[i] == 0x5D) {
			size_checksum_buff++;
			checksum_buff = (unsigned char*)realloc(checksum_buff, size_checksum_buff);
			checksum_buff[j] = 0x5D;
			j++;
			checksum_buff[j] = buff[i] ^ 0x20;
		}
		else {
			checksum_buff[j] = buff[i];
		}
		j++;
	}

	checksum_buff[size_checksum_buff - 1] = buff[size -1];

	j = 1;
	while (j != size_checksum_buff - 2) {
		checksum ^= checksum_buff[j];
		j++;
	}
	checksum_buff[size_checksum_buff - 2] = checksum;

	print_request(checksum_buff, size_checksum_buff);

	free(checksum_buff);
}

char check_reqbuff(unsigned char *reqbuff, int size) {
	if (reqbuff[0] == 0x7E && reqbuff[size - 1] == 0x7E)
		return 0;
	else
		return -1;

	for (int i = 0; i < size; ++i) {
		if ((Range(48, reqbuff[i], 57) || Range(65, reqbuff[i], 70)) == 0) {
			if (i != size -2 )
				return -2;
		}
	}
}


char conversion_request(char *str) {
	int i, j;
	int size = strlen(str),
		num_space = 0;
	int ret = 0;
	// removing spaces and conversion from ASCI to number
	for (i = 0, j = 0; str[i] != '\0'; i++) {
		if (str[i] != ' ') {
			if (Range(48, str[i], 57)) {
				str[j++] = str[i] - 48;
			}
			else if (Range(65, str[i], 70)) {
				str[j++] = str[i] - 55;
			}
			else {
				str[j++] = str[i];
			}
		}
		else {
			num_space++;
		}
	}
	str[j] = '\0';

	size -= num_space;

	int size_reqbuff = size % 2 == 0 ? size / 2 : size / 2 + 1;
	unsigned char *reqbuff = (unsigned char*)calloc(size_reqbuff, sizeof(unsigned char));

	i = 0;
	j = 0;
	while (size - i != 0) {
		reqbuff[j] = (unsigned char)((str[i] << 4) | str[i + 1]);
		i += 2;
		j++;
	}

	ret = check_reqbuff(reqbuff, size_reqbuff);

	if (ret == 0) {
		checksum(reqbuff, size_reqbuff);
	}
	else {
		free(reqbuff);
		return ret;
	}
	return 0;
}

char analysis_request(char* str) {
	bool flag = false;
	for (int i = 0; i < strlen(str); ++i) {
		if (Range(48, str[i], 57)) {
			flag = true;
		} else if (Range(65, str[i], 70)) {
			flag = true;
		} else if (str[i] == ' ' || str[i] == 'X') {
			flag = true;
		}
		else {
			flag = false;
		}

		if (flag == false)
			return -3;
	}
	return 0;
}

int main(int argc, char* argv) {
	char request[128];
	printf("Checksum calculation\n\n");

	while (true) {
		lable:
		printf("Input request(in request CRC = XX): ");
		gets_s(request);

		if (analysis_request(request) != 0)
			goto lable;

		if (conversion_request(request) != 0)
			goto lable;

		printf("To continue, press any key. To exit press ESC\n\n");

		if (_getch() == 27)
			exit(0);
	}
	return 0;
}

