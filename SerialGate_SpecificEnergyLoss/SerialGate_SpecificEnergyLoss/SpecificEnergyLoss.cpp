#include "SerialGate.h"
#include <future>
#include <ctime>
#include "conio.h"

#define RESPONCE_TIMEOUT 10000 // 10 second

enum TYPE_ENERGY {
	Ap, An, Apn, Rp, Rn, Rpn, mA2h, kV2h
};

struct ENERGY {
	double E_Ap;
	double E_An;
	double E_Apn;
	double E_Rp;
	double E_Rn;
	double E_Rpn;
	double Asqrh;
	double Vsqrh;
};

#define SIZE_ENERGY sizeof(ENERGY)

int StrToInt32(unsigned char * buf) {
	return (((int)*(buf) << 24) & 0xFF000000 | ((int)*(buf + 1)) << 16 & 0x00FF0000 | ((int)*(buf + 2)) << 8 & 0x0000FF00 | ((int)*(buf + 3)) & 0x000000FF);
}

int Recive(char* rx_buff, int count_rx, SerialGate *sg, int resp_timeout_ = RESPONCE_TIMEOUT) {
	if (sg != nullptr) {
		int full_count = 0;
		int count_Rcv = 0;
		bool resp_flg = true;
		clock_t start_rcv = clock();
		do {
			std::future<int> f = std::async(std::launch::async, &SerialGate::Recv, sg, (char*)(rx_buff + full_count), count_rx);
			count_Rcv = f.get();
			full_count += count_Rcv;
			if (full_count == 0) {
				if ((clock() - start_rcv) > resp_timeout_ * CLOCKS_PER_SEC) {
					resp_flg = false;
				}
			}
		} while (*(rx_buff + full_count - 1) != (char)(0x7E) && resp_flg);
		return full_count;
	}
	return -1;
}

int GetEnergy(unsigned char *buff, int type_energy) {
	switch (type_energy)
	{
	case Ap:
		return StrToInt32(buff + 8);
	case An:
		return StrToInt32(buff + 12);
	case Apn:
		return StrToInt32(buff + 16);
	case Rp:
		return StrToInt32(buff + 20);
	case Rn:
		return StrToInt32(buff + 24);
	case Rpn:
		return StrToInt32(buff + 28);
	case mA2h:
		return StrToInt32(buff + 32);
	case kV2h:
		return StrToInt32(buff + 36);
	default:
		return -1;
		break;
	}
}

void setup_communication_option(int &num_com, int &baud, char* file_n) {
	printf("Number COM: ");
	scanf("%d", &num_com);
	printf("\nBitrate: ");
	scanf("%d", &baud);
	printf("\n");
	printf("FILE name for log: ");
	scanf("%s", file_n);
	strncat(file_n, ".txt", 4);
	printf("\n");
}

int SendReceiveCMD_GET_ENERGY(char* tx_buff, int count_tx, unsigned char* rx_buff, int count_rx, SerialGate *sg, ENERGY *energy, int SCL_IMP) {
	sg->Clean();
	int SendCounter = sg->Send(tx_buff, count_tx);

	if (SendCounter != count_tx) {
		sg->Close();
		return -1; // Send error!
	}

	int RcvCounter = Recive((char*)rx_buff, count_rx, sg);

	if (RcvCounter != 0) {
		std::string reciev_buff;

		for (int rcv_it = 0; rcv_it < RcvCounter; rcv_it++) {
			if (rx_buff[rcv_it] == 0x5D) {
				++rcv_it;
				reciev_buff.append(1, (rx_buff[rcv_it] ^ 0x20));
			}
			else {
				reciev_buff.append(1, rx_buff[rcv_it]);
			}
		}

		memcpy(rx_buff, reciev_buff.c_str(), reciev_buff.size());

		energy->E_Ap = (double)(GetEnergy(rx_buff, Ap) * SCL_IMP ) / 1000.0;
		energy->E_An = (double)(GetEnergy(rx_buff, An) * SCL_IMP) / 1000.0;
		energy->E_Apn = (double)(GetEnergy(rx_buff, Apn) * SCL_IMP) / 1000.0;
		energy->E_Rp = (double)(GetEnergy(rx_buff, Rp) * SCL_IMP) / 1000.0;
		energy->E_Rn = (double)(GetEnergy(rx_buff, Rn) * SCL_IMP) / 1000.0;
		energy->E_Rpn = (double)(GetEnergy(rx_buff, Rpn) * SCL_IMP) / 1000.0;
		energy->Asqrh = (double)GetEnergy(rx_buff, mA2h) / 1000.0;
		energy->Vsqrh = (double)GetEnergy(rx_buff, kV2h) * 1000.0;

		memset(rx_buff, 0, count_rx);
	}
	else {
		memset(rx_buff, 0, count_rx);
		sg->Close();
		return -2; // Receive error!
	}
}

int SendReceiveCMD_GET_TYPE(char* tx_buff, int count_tx, unsigned char* rx_buff, int count_rx, SerialGate *sg, int &SCL_IMP) {
	int SendCounter = sg->Send(tx_buff, count_tx);

	if (SendCounter != count_tx) {
		sg->Close();
		return -1; // Send error!
	}
	else {
		int RcvCounter = Recive((char*)rx_buff, count_rx, sg);

		if (RcvCounter != count_rx) {
			memset(rx_buff, 0, count_rx);
			sg->Close();
			return -2; // Receive error!
		}
		else {
			SCL_IMP = StrToInt32((unsigned char*)(rx_buff + 19)) == -126 ? 1000 : StrToInt32((unsigned char*)(rx_buff + 19));
			return 0;
		}
	}

}

void WriteToFile(FILE *file, char* file_name, ENERGY* val_energy, int *time, int time_interval, int frequency, int time_counter, int SCL_IMP) {
	double* error_converge = (double*)calloc(time_counter, sizeof(double));

	double SQRT = 0.0;

	file = fopen(file_name, "w");
	fprintf(file, "Time interval: %d Frequency: %d SCL_IMP: %d\n", time_interval, frequency, SCL_IMP);
	fprintf(file, "Sec:    Error:	\n");
	for (int i = 0; i < time_counter; i++) {
		SQRT = sqrt(val_energy[i].Asqrh * val_energy[i].Vsqrh);
		error_converge[i] = abs(SQRT - val_energy[i].E_Ap);
		fprintf(file, "%d	%f\n", time[i], error_converge[i]);
	}
	fclose(file);
	free(error_converge);
}


int main(int argc, char* argv) {
	SerialGate sg;
	unsigned char buff_receive[256];

	ENERGY *val_energy = NULL;
	int *time = NULL;
	double *error_converge = NULL;
	FILE *file = NULL;

	int num_com, baud;
	bool flag = false;

	char file_name[128];

	char  ResetMetrCMD[] = "\x7E\x00\x00\x00\x01\x03\x20\x22\x7E";
	char  GetEnergyCmD[] = "\x7E\x00\x00\x00\x01\x01\x22\xFF\xDD\x7E";
	char  GetTypeCmD[] = "\x7E\x00\x00\x00\x01\x02\x01\x02\x7E";

	int size_ResetMetrCMD = 9;
	int size_SendGetEnergyCmD = 10, size_ReceiveGetEnergyCmD = 0x2A;
	int size_SendGetTypeCmD = 9, size_ReceiveGetTypeCmD = 0x18;

	double SQRT = 0.0;
	int SCL_IMP = 1000;

	int time_interval = 0, frequency = 0, time_counter = 0;
	int error = 0;

	// Для замеров 
	unsigned int start_time = 0,	// начальное время
		end_time = 0,				// конечное время
		search_time = 0;			// искомое время

	printf("Reading of energy indications and calculation of losses\n\n");

	while (true) {
	lable:
		setup_communication_option(num_com, baud, file_name);

		printf("Enter the time of measurement in sec: ");
		scanf("%d", &time_interval);
		printf("\nEnter the measurement frequency in sec: ");
		scanf("%d", &frequency);
		printf("\n");

		flag = sg.Open(num_com, baud);
		if (flag) {
			printf("COM Open Success!\n");
		}
		else {
			printf("COM Open Error!\n");
			goto lable;
		}

		error = SendReceiveCMD_GET_TYPE(GetTypeCmD, size_SendGetTypeCmD, buff_receive, size_ReceiveGetTypeCmD, &sg, SCL_IMP);

		if (error == -1) {
			printf("Send error CMD_GET_TYPE! Default SCL_IMP = 1000\n");
		}

		if (error == -2) {
			printf("Receive error CMD_GET_TYPE! Default SCL_IMP = 1000\n");
		}

		time_counter = (int)(time_interval / frequency) + 1;
		val_energy = (ENERGY*)calloc(time_counter, SIZE_ENERGY);
		time = (int*)calloc(time_counter, sizeof(int));

		memset(buff_receive, 0, sizeof(buff_receive));

		for (int i = 0; i < time_counter; i++) {
			start_time = clock();

			error = SendReceiveCMD_GET_ENERGY(GetEnergyCmD, size_SendGetEnergyCmD, buff_receive, size_ReceiveGetEnergyCmD, &sg, &val_energy[i], SCL_IMP);
			sg.Clean();

			if (error == -1) {
				printf("Send error CMD_GET_ENERGY! Time iteration %d seconds\n", time[i]);
				WriteToFile(file, file_name, val_energy, time, time_interval, frequency, time_counter, SCL_IMP);
				free(val_energy);
				free(time);
				goto lable;
			}

			if (error == -2) {
				printf("Receive error CMD_GET_ENERGY! Time iteration %d seconds\n", time[i]);
				WriteToFile(file, file_name, val_energy, time, time_interval, frequency, time_counter, SCL_IMP);
				free(val_energy);
				free(time);
				goto lable;
			}

			time[i] = frequency * i;
			printf("It's been %d seconds\n", time[i]);

			end_time = clock();
			search_time = end_time - start_time;
			
			Sleep((int)((double)frequency - (double)search_time / 1000.0) * 1000);
		}

		WriteToFile(file, file_name, val_energy, time, time_interval, frequency, time_counter, SCL_IMP);

		free(val_energy);
		free(time);

		printf("\nConvergence test complete!\n");

		printf("To continue, press any key. To exit press ESC\n\n");

		if (_getch() == 27) {
			sg.Clean();
			sg.Close();
			exit(0);
		}
		sg.Clean();
		sg.Close();
	}



























	//while (true) {
	//lable:
	//	setup_communication_option(num_com, baud, file_name);

	//	flag = sg.Open(num_com, baud);
	//	if (flag) {
	//		printf("COM Open Success!\n");
	//	}
	//	else {
	//		printf("COM Open Error!\n");
	//		goto lable;
	//	}

	//	printf("Enter the time of measurement in sec: ");
	//	scanf("%d", &time_interval);
	//	printf("\nEnter the measurement frequency in sec: ");
	//	scanf("%d", &frequency);
	//	printf("\n");

	//	time_counter = (int)(time_interval / frequency) + 1;

	//	val_energy = (ENERGY*)calloc(time_counter, SIZE_ENERGY);
	//	time = (int*)calloc(time_counter, sizeof(int));

	//	for (int i = 0; i < time_counter; i++) {
	//		start_time = clock();
	//		error = SendReceiveCMD_GET_ENERGY(GetEnergyCmD, size_SendGetEnergyCmD, buff_receive, size_ReceiveGetEnergyCmD, &sg, &val_energy[i]);

	//		time[i] = frequency * i;

	//		if (error == -1) {
	//			printf("Send error CMD_GET_ENERGY! Time iteration %d seconds", time[i]);
	//			free(val_energy);
	//			goto lable;
	//		}

	//		if (error == -2) {
	//			printf("Receive error CMD_GET_ENERGY! Time iteration %d seconds", time[i]);
	//			free(val_energy);
	//			goto lable;
	//		}

	//		time[i] = frequency * i;

	//		end_time = clock();

	//		search_time = end_time - start_time;
	//		printf("It's been %d seconds\n", time[i]);

	//		memset(buff_receive, 0, sizeof(buff_receive));
	//		Sleep((int)((double)frequency - (double)search_time / 1000.0) * 1000);
	//	}


	//	memset(buff_receive, 0, sizeof(buff_receive));
	//	error = SendReceiveCMD_GET_TYPE(GetTypeCmD, size_SendGetTypeCmD, buff_receive, size_ReceiveGetTypeCmD, &sg, SCL_IMP);

	//	if (error == -1) {
	//		printf("Send error CMD_GET_TYPE! Default SCL_IMP = 1000" );
	//	}

	//	if (error == -2) {
	//		printf("Receive error CMD_GET_TYPE! Default SCL_IMP = 1000");
	//	}

	//	error_converge = (double*)calloc(time_counter, sizeof(double));

	//	for (int i = 0; i < time_counter; i++) {
	//		val_energy[i].E_Ap *= (double)SCL_IMP / 1000.0;
	//		val_energy[i].E_An *= (double)SCL_IMP / 1000.0;
	//		val_energy[i].E_Apn *= (double)SCL_IMP / 1000.0;
	//		val_energy[i].E_Rp *= (double)SCL_IMP / 1000.0;
	//		val_energy[i].E_Rn *= (double)SCL_IMP / 1000.0;
	//		val_energy[i].E_Rpn *= (double)SCL_IMP / 1000.0;

	//		SQRT = sqrt(val_energy[i].Asqrh * val_energy[i].Vsqrh);

	//		error_converge[i] = abs(SQRT - val_energy[i].E_Ap);
	//	}

	//	file = fopen("LogСonverge.txt", "w");
	//	file = fopen(file_name, "w");
	//	fprintf(file, "Time interval: %d Frequency: %d SCL_IMP: %d\n", time_interval, frequency, SCL_IMP);
	//	fprintf(file, "Sec:    Error:	\n");
	//	for (int i = 0; i < ((int)time_interval / frequency) + 1; i++) {
	//		fprintf(file, "%d	%f\n", time[i], error_converge[i]);
	//	}
	//	fclose(file);
	//	free(val_energy);
	//	free(error_converge);
	//	free(time);
	//	printf("\nConvergence test complete!\n");

	//	printf("To continue, press any key. To exit press ESC\n\n");

	//	if (_getch() == 27) {
	//		sg.Clean();
	//		sg.Close();
	//		exit(0);
	//	}
	//	sg.Clean();
	//	sg.Close();
	//}
	return 0;
}