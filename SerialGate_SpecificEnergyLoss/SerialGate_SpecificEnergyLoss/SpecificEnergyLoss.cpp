#include "SerialGate.h"
#include <future>
#include <ctime>
#include "conio.h"

#define RESPONCE_TIMEOUT 10000 // 10 second

enum TYPE_ENERGY {
	Ap, An, Apn, Rp, Rn, Rpn, mA2h, kV2h
};

int StrToInt32(unsigned char * buf) {
	return (((int)*(buf) << 24) & 0xFF000000 | ((int)*(buf + 1)) << 16 & 0x00FF0000 | ((int)*(buf + 2)) << 8 & 0x0000FF00 | ((int)*(buf + 3)) & 0x000000FF);
}

int Recive(char* rx_buff, int count_rx, SerialGate *sg, int resp_timeout_ = RESPONCE_TIMEOUT) {
	if (sg != nullptr) {
		int full_count = 0;
		int count_Rcv = 0;
		bool resp_flg = true;
		clock_t start_rcv = clock();
		short count_pack = -1;
		do {
			std::future<int> f = std::async(std::launch::async, &SerialGate::Recv, sg, (char*)(rx_buff + full_count), count_rx);
			count_Rcv = f.get();
			full_count += count_Rcv;
			if (full_count == 0) {
				if ((clock() - start_rcv) > resp_timeout_ * CLOCKS_PER_SEC) {
					resp_flg = false;
				}
			}
			if (full_count >= 3 && count_pack == -1) {
				count_pack = ((unsigned short)(*(rx_buff + 1) & 0x07) << 8 | (unsigned short)*(rx_buff + 2) & 0xFF) + 2;
			}
		} while ((*(rx_buff + full_count - 1) != (char)(0x7E) || full_count < 10 ) /*full_count != count_pack && resp_flg*/);
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


int main(int argc, char* argv) {
	SerialGate sg;
	unsigned char buff_receive[256];

	char  ResetMetrCMD[] = "\x7E\x00\x00\x00\x01\x03\x20\x22\x7E";
	char  GetEnergyCmD[] = "\x7E\x00\x00\x00\x01\x01\x22\xFF\xDD\x7E";

	int size_ResetMetrCMD = 9;
	int size_GetEnergyCmD = 10;

	double	E_Ap = 0,
		E_An = 0,
		E_Apn = 0,
		E_Rp = 0,
		E_Rn = 0,
		E_Rpn = 0,
		Asqrh = 0,
		Vsqrh = 0,
		SQRT = 0,
		delta = 2.5;

	bool b = sg.Open(11, 9600);
	if (b == true)
		printf("COM Open Success!");
	else
		printf("COM Open Error!");

	int SendCounter = sg.Send(GetEnergyCmD, size_GetEnergyCmD);

	if (SendCounter != size_GetEnergyCmD)
		printf("Error Send!");

	int RcvCounter = Recive((char*)buff_receive, sizeof(buff_receive), &sg);

	if (RcvCounter != sizeof(buff_receive))
		printf("Error Recive!");

	E_Ap = (double)GetEnergy(buff_receive, Ap) / 1000.0;
	E_An = (double)GetEnergy(buff_receive, An) / 1000.0;
	E_Apn = (double)GetEnergy(buff_receive, Apn) / 1000.0;
	E_Rp = (double)GetEnergy(buff_receive, Rp) / 1000.0;
	E_Rn = (double)GetEnergy(buff_receive, Rn) / 1000.0;
	E_Rpn = (double)GetEnergy(buff_receive, Rpn) / 1000.0;
	Asqrh = (double)GetEnergy(buff_receive, mA2h);
	Vsqrh = (double)GetEnergy(buff_receive, kV2h) * 1000.0;

	SQRT = sqrt(Asqrh * Vsqrh);

	if (abs(SQRT - E_Ap) > delta)
		printf("Error Calculation!");

	_getch();
	return 0;
}