#define F_CPU 8000000UL/*����������� �������� ������� ���������������� 
��� ���������� ������ ������� ��������*/
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "LCD.h"
#include "mq2.h"


int main(void) {
	DDRD = 0x00;
	PORTD = 0b10000000;
	while (1) {
		if (!(PIND & 0b10000000)) {	 //����� �� ������� ������
			void* mq2::inits() {

				Serial.begin(9600);
				adcBits = 1024;//������������ ��� ����������� ���������� ����� ��� ������� �������� ������� MQ
				adcVoltage = 3.3;//������� ���������� ���

			}
			void* mq2::calibrate() {  //������ ������ MQ-2
				float sensor_volt;
				float RS_air; // ����������� � ������ �������

				float sensorValue;
				for (int x = 0; x < 1000; x++)
				{
					sensorValue = sensorValue + readADC(A0);
				}
				sensorValue = sensorValue / 1000.0;
				Serial.println(sensorValue);
				sensor_volt = (sensorValue / adcBits) * adcVoltage;
				RS_air = (adcVoltage - sensor_volt) / sensor_volt;
				_R0 = RS_air / 9.9; // �������� �������� ������� �������� MQ-2
				printData(_R0);
				delay(1000);
			}

			double* mq2::getValue(bool printing, char gasName, double* output) {

				float sensor_volt;
				float RS_gas;
				float ratio;
				float ppm;
				int sensorValue = readADC(A0);
				sensor_volt = ((float)sensorValue / adcBits) * adcVoltage;
				RS_gas = (adcVoltage - sensor_volt) / sensor_volt; // ���������� ����������
				ratio = RS_gas / _R0; // ratio = RS/R0
				switch (gasName) {
				case 'L'://������������ ���������� ����
					_M0 = -0.454838059;
					_B0 = 1.25063406;
					ppm = pow(10, (log(ratio) - _B0) / _M0);
					if (printing) { printData(ppm); }
					output[1] = ppm;
					break;
				case 'P'://������������ �������
					_M0 = -0.461038681;
					_B0 = 1.290828982;
					ppm = pow(10, (log(ratio) - _B0) / _M0);
					if (printing) { printData(ppm); }
					output[1] = ppm;
					break;
				case 'H'://������������ ��������
					_M0 = -0.47305447;
					_B0 = 1.412572126;
					ppm = pow(10, (log(ratio) - _B0) / _M0);
					if (printing) { printData(ppm); }
					output[1] = ppm;
					break;
				case 'M'://������������ ������
					_M0 = -0.372003751;
					_B0 = 1.349158571;
					ppm = pow(10, (log(ratio) - _B0) / _M0);
					if (printing) { printData(ppm); }
					output[1] = ppm;
					break;
				case 'A'://������������ ������
					_M0 = -0.373311285;
					_B0 = 1.310286169;
					ppm = pow(10, (log(ratio) - _B0) / _M0);
					if (printing) { printData(ppm); }
					output[1] = ppm;
					break;
				default://��� ��������
					ppm = pow(10, (log(ratio) - 1.25063406) / -0.454838059);//��������� ���
					output[1] = ppm;
					ppm = pow(10, (log(ratio) - (-0.461038681)) / 1.290828982);//������
					output[2] = ppm;
					ppm = pow(10, (log(ratio) - (-0.47305447)) / -0.454838059);//�������
					output[3] = ppm;
					ppm = pow(10, (log(ratio) - (-0.372003751)) / 1.349158571);//�����
					output[4] = ppm;
					ppm = pow(10, (log(ratio) - (-0.373311285)) / 1.310286169);//�����
					output[5] = ppm;
					break;
				}
				delay(1000);

			}
			bool  mq2::thrValue(char gasName, int threshold) {

				float sensor_volt;
				float RS_gas;
				float ratio;
				float ppm;
				int sensorValue = readADC(A0);
				sensor_volt = ((float)sensorValue / 1024) * 3.3;
				RS_gas = (3.3 - sensor_volt) / sensor_volt; // ������������ ���������


				ratio = RS_gas / _R0; // ����������� = RS/R0
				switch (gasName) {
				case 'L'://��������� ���
					_M0 = -0.454838059;
					_B0 = 1.25063406;
					ppm = pow(10, (log(ratio) - _B0) / _M0);

					break;
				case 'P'://������
					_M0 = -0.461038681;
					_B0 = 1.290828982;
					ppm = pow(10, (log(ratio) - _B0) / _M0);


					break;
				case 'H'://�������
					_M0 = -0.47305447;
					_B0 = 1.412572126;
					ppm = pow(10, (log(ratio) - _B0) / _M0);


					break;
				case 'M'://�����
					_M0 = -0.372003751;
					_B0 = 1.349158571;
					ppm = pow(10, (log(ratio) - _B0) / _M0);


					break;
				case 'A'://�����
					_M0 = -0.373311285;
					_B0 = 1.310286169;
					ppm = pow(10, (log(ratio) - _B0) / _M0);

					break;

				}
				if (ppm >= threshold) {
					return(true);
				}
				else { return(false); }

			}

			bool  mq2::thrNvalue(int threshold) {

				if (readADC(0) >= threshold) {
					return(true);
				}
				else { return(false); }

			}
			int mq2::readADC(int pin) {

				return(analogRead(pin));

			}
			void mq2::printData(float data) { //����� ������ MQ-2(������ ���������� � "data")
				_delay_ms(100);
				lcdInit(); // ������ ������ �������
				lcdClear();
				lcdSetDisplay(LCD_DISPLAY_ON);
				lcdSetCursor(LCD_CURSOR_OFF);

				char text[17];
				strcpy(text, "  pCO2 is ");
				lcdGotoXY(0, 0);
				lcdPuts(text);
				strcpy(text, data); //���������� �������� �  MQ-2
				lcdGotoXY(1, 0);
				lcdPuts(text); //����� ������ � ��������    
			}
			while (1);
		}
		else {        //���� �� ����� �������� ������� ������
			lcdInit(); // ������ ������ �������
			lcdClear();
			lcdSetDisplay(LCD_DISPLAY_ON);
			lcdSetCursor(LCD_CURSOR_OFF);

			char text[17];
			strcpy(text, "  pCO2 is ");
			lcdGotoXY(0, 0);
			lcdPuts(text);
			strcpy(text, "...(waiting)"); //���������� �������� �  MQ-2
			lcdGotoXY(1, 0);
			lcdPuts(text); //����� ������ ����� ��������
		}
	}
}
