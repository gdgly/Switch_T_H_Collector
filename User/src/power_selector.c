#include "adf.h"
#include "power_selector.h"

/*
    ��Դ���緽ʽ�л�ģʽ�������л�5Vģ�鹩�绹�ǵ���������
*/

//��Դѡ�����ų�ʼ��
void power_selector_init(void)
{
    gpio_init(power_selector_cfg.port, power_selector_cfg.pin, GPIO_Mode_Out_PP_High_Fast);
}

//// Ӳ��V1.1
////���緽ʽ�л�Ϊ������ȡ5V��ģ��
//void power_selector_module(void)
//{
//    gpio_set_high(power_selector_cfg.port, power_selector_cfg.pin);
//}
//
////���緽ʽ�л�Ϊ��������
//void power_selector_capacitor(void)
//{
//    gpio_set_low(power_selector_cfg.port, power_selector_cfg.pin);
//}


// Ӳ��V1.2  �����ϵ糬������û�е�ѹ��ѡ���Դģ�鹩�����ṩ��Ƭ����磬���������ݳ�����֮��ѡ�񳬼����ݹ��硣
//���緽ʽ�л�Ϊ������ȡ5V��ģ��
void power_selector_module(void)
{
    gpio_set_low(power_selector_cfg.port, power_selector_cfg.pin);
}

//���緽ʽ�л�Ϊ��������
void power_selector_capacitor(void)
{
    gpio_set_high(power_selector_cfg.port, power_selector_cfg.pin);
}
