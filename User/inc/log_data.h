#ifndef _LOG_DATA_H
#define _LOG_DATA_H

#include "adf.h"

#define LOG_BASE_ADDR           85      //eeprom�洢��ʼλ��
#define LOG_MAX_RECORDS_COUNT   67      //72      //�洢���ݵ������  //20���Ӵ洢һ������ ���洢һ���������

//STM8L152C6  1024 eeprom  ��ʼ�洢λ����85 ��1024-85=939  ���Ի�ʣ��939���ֽڵ����ݣ�ÿ��log_data �� 14���ֽ�  ��939/14=67  �������洢���ݸ�����67
//
typedef struct _LogData{
    s16         temp;                                           // �¶�
    s16         humi;                                           // ʪ��
    u8          rssi;                                           // �ź�ǿ��
    u8          st;                                             // ������
    u8          battery;                                        // ��ص���
    u8          sample_time[7];                                 // �ɼ�ʱ��
}LogData;

void log_write(LogData *log, u32 records_wr);

void log_read(LogData *log, u32 records_rd);

void log_clear_data(void);

extern LogData   g_last_log_data;

#endif