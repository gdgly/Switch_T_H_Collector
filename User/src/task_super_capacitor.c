/*
* ����������
*     �˽��̴�������̣�ʹ�������緽ʽ�� ��ⳬ�����ݵ�ѹ����ѹ���㡣�����л�
* ���緽ʽ��
*/
#include "adf.h"
#include "sysparams.h"
//#include "disp.h"
//#include "rtc.h"
#include "LCD.h"
#include "board.h"

// ����ͷ�ļ�����
#include "task_report.h"
#include "task_sample.h"
#include "task_disp.h"
#include "task_key.h"
#include "task_super_capacitor.h"

typedef enum _SuperCapacitorState{
    E_SC_STATE_IDLE = 0,            // ����״̬
    E_SC_STATE_COLLECT,              // ���״̬
    E_SC_STATE_FAST_RECHARGE            // ���״̬
}SuperCapacitorState;

//���ģʽ
typedef enum _SuperCapacitorRechargeMode{
    E_SC_RECHARGE_50MS = 0,//��
    E_SC_RECHARGE_125MS,
    E_SC_RECHARGE_250MS,
    E_SC_RECHARGE_375MS,
    E_SC_RECHARGE_500MS
}_SuperCapacitorRechargeMode;

//���ģʽ
typedef enum _SuperCapacitorRechargeSpeed{
    E_SC_RECHARGE_SPEED_10MS = 0,//��
    E_SC_RECHARGE_SPEED_20MS,
    E_SC_RECHARGE_SPEED_30MS,
    E_SC_RECHARGE_SPEED_40MS,
    E_SC_RECHARGE_SPEED_50MS
}_SuperCapacitorRechargeSpeed;



SuperCapacitorHandler s_super_capacitor_handler;

//void task_super_capacitor_init(u8 have_lights_on)
//{
//    s_super_capacitor_handler.recharge_switch = 0;
//    //    if (have_lights_on == 1)
//    //    {
//    s_super_capacitor_handler.recharge_max_counter = 2;
//    //    }
//    //    else
//    //    {
//    //        s_super_capacitor_handler.recharge_max_counter = SUPER_CAPACITOR_RECHARGE_IDLE_COUNTER;
//    //    }
//}


// ϵͳ�ж��Ƿ���Գ��
static s8 recharge_state(void)
{
    // �л���ƽΪ�ߣ�������硣ֻ���ڽ��̿��е�ʱ��ſ�ʼ��磬�����κ�һ������ִ�����֮�����1s����ʱȻ���ٿ�ʼ���
    if(  task_report_get_state() == E_REPORT_IDLE    && \
         task_sample_get_state() == E_COLLECT_IDLE   && \
         task_key_get_state()    == KEY_STS_IDLE     && \
         task_disp_get_state()   == E_TASK_DISP_IDLE && \
         s_super_capacitor_handler.delay_recharge_count <= 0 )      // ��ʾ����
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

// ���ó����ʱ
void reset_delay_recharge_count(void)
{
    s_super_capacitor_handler.delay_recharge_count = UNIT_SECOND;//�κ�����ֻҪִ�����������һ�γ����ʱ
}

// �������ݳ���ʼ��
void task_super_capacitor_init(void)
{
    s_super_capacitor_handler.recharge_count = E_SC_RECHARGE_250MS;
    s_super_capacitor_handler.recharge_mode = E_SUPERCAPACITOR_MODE_NORMAL;     //�ϵ�Ĭ��������ģʽ
    reset_delay_recharge_count();//���ó����ʱ

    return;
}



// ����������
void task_super_capacitor_proc(void)
{
    u16 super_capacitor_v_time = 0;     // �����ݵ�ѹ��ʱ��ʱ��
    u16 before_super_capacitor_voltage;                 // ����ϴγ������ݵ�ѹ

    // ��������
    s_super_capacitor_handler.recharge_counter++;

    if(s_super_capacitor_handler.delay_recharge_count > 0)
    {
        s_super_capacitor_handler.delay_recharge_count--;
    }


        if(s_super_capacitor_handler.recharge_counter >= s_super_capacitor_handler.recharge_count)
        {
            s_super_capacitor_handler.recharge_counter = 0;

            if(recharge_state() == 0)
            {
                GPIO_SetBits(GPIOB, GPIO_Pin_6);
                if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
                {
                    switch(s_super_capacitor_handler.recharge_speed)
                    {
                    case E_SC_RECHARGE_SPEED_10MS:
                        delay_ms(10);
                        break;

                    #if 0
                    case E_SC_RECHARGE_SPEED_20MS:
                        delay_ms(20);
                        break;
                    #endif

                    #if 0
                    case E_SC_RECHARGE_SPEED_30MS:
                        delay_ms(30);
                        break;
                    #endif

                    #if 0
                    case E_SC_RECHARGE_SPEED_40MS:
                        delay_ms(40);
                        break;
                    #endif

                    case E_SC_RECHARGE_SPEED_50MS:
                        delay_ms(50);
                        break;
                    default:
                        break;
                    }
                }
                GPIO_ResetBits(GPIOB, GPIO_Pin_6);
            }
        }

    switch(s_super_capacitor_handler.state)
    {
    case E_SC_STATE_IDLE:        // ����״̬
        if(task_report_get_state() > E_REPORT_READY || s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
            super_capacitor_v_time = NB_SUPER_CAPACITOR_COLLECT_COUNTER;        //���豸���������ϴ����ݵ�ʱ��5s���һ�ε��ݵ���
        else
            super_capacitor_v_time = NORMO_SUPER_CAPACITOR_COLLECT_COUNTER;     //���豸���������ϴ����ݵ�ʱ��15s���һ�ε��ݵ���

        s_super_capacitor_handler.sample_counter++;
        if (s_super_capacitor_handler.sample_counter >= super_capacitor_v_time)
        {
            // �ָ�������
            s_super_capacitor_handler.sample_counter = 0;
            // �л�״̬
            s_super_capacitor_handler.state = E_SC_STATE_COLLECT;
        }
        break;
    case E_SC_STATE_COLLECT:        //���ݵ�ѹ�ɼ�

        // �ɼ��������ݵ�ѹ
        before_super_capacitor_voltage = g_run_params.super_capacitor_voltage;     // ���³���������ʷ��ѹֵ
        g_run_params.super_capacitor_voltage = task_super_capacitor_read_voltage();             // ��λmV

        // �صƵ���������ˮ����ʾ
        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
        {
            if(g_sys_params.light_sts[E_LIGHT_1] == E_LIGHT_OFF || g_sys_params.light_sts[E_LIGHT_2] == E_LIGHT_OFF)
                LCD_DisplayMARK(Mark_RH, CLEAR);                                    //ˮ��
        }

        // �ж��Ƿ��ڳ��,ˮ����˸��ʾ��
        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
        {
            if((g_run_params.super_capacitor_voltage-before_super_capacitor_voltage)>0 || g_run_params.super_capacitor_voltage == 0)
                g_run_params.recharge_state = TRUE;
            else
                g_run_params.recharge_state = FALSE;
        }
        else
        {
           g_run_params.recharge_state = FALSE;
        }

        // �л���幩�緽ʽ
        if(g_run_params.super_capacitor_voltage >= SUPER_CAPACITOR_FULL)     // >=4900 �жϼ���ѹ�Ƿ���� ��������򹩵��Դ�л�Ϊ���ݹ���
            power_selector_capacitor();            // �л�Ϊ�������ݹ���
        if(g_run_params.super_capacitor_voltage <= SUPER_CAPACITOR_LOSS)     // <=3600 �жϼ���ѹ�Ƿ���� ��������򹩵��Դ�л�Ϊģ�鹩��
            power_selector_module();               //�л�Ϊ������ȡ��ģ�鹩��

        // �رտ��ģʽ
        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
        {
            if(g_run_params.super_capacitor_voltage >= 5000)
            {
                s_super_capacitor_handler.hold_recharge_count++;
                if(s_super_capacitor_handler.hold_recharge_count>=10)
                {
                    s_super_capacitor_handler.hold_recharge_count = 0;
                    task_super_capacitor_normal_triggered();                        // �л�Ϊ����ģʽ

                    g_run_params.recharge_state = FALSE;                            // �ر�ˮ����˸
                    //if(g_sys_params.factory_flag <= FACTORY_NUM){
                            LCD_DisplayMARK(Mark_RH, CLEAR);                        // ���ˮ��
                    //}
                    task_key_init();                                                // ����ģʽ�³�����ص�
                }
            }
        }

        // �л����Ƶ�� �жϵ��ݵ�ѹ
        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_NORMAL)
        {
            if(g_run_params.super_capacitor_voltage >= 3600)
            {
                s_super_capacitor_handler.recharge_count = E_SC_RECHARGE_125MS;
            }
            else if(g_run_params.super_capacitor_voltage < 3600)
            {
                s_super_capacitor_handler.recharge_count = E_SC_RECHARGE_250MS;
            }
        }
        else if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)//���ʹ��125msģʽ
        {
            s_super_capacitor_handler.recharge_count = E_SC_RECHARGE_125MS;
        }

#if 1
        if(g_run_params.super_capacitor_voltage<=3000)
            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_10MS;

//        else if(g_run_params.super_capacitor_voltage>3000 && g_run_params.super_capacitor_voltage<4000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_50MS;

        else if(g_run_params.super_capacitor_voltage>3000 )
            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_50MS;
#endif

//#if 0
//        if(g_run_params.super_capacitor_voltage<=2000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_10MS;
//        else if(g_run_params.super_capacitor_voltage>2000 && g_run_params.super_capacitor_voltage<3000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_20MS;
//        else if(g_run_params.super_capacitor_voltage>3000 && g_run_params.super_capacitor_voltage<4000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_30MS;
//        else if(g_run_params.super_capacitor_voltage>4000 )
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_50MS;
//#endif

//#if 0
//        if(g_run_params.super_capacitor_voltage<=1000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_10MS;
//        else if(g_run_params.super_capacitor_voltage>1000 && g_run_params.super_capacitor_voltage<2000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_20MS;
//        else if(g_run_params.super_capacitor_voltage>2000 && g_run_params.super_capacitor_voltage<3000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_30MS;
//        else if(g_run_params.super_capacitor_voltage>3000 && g_run_params.super_capacitor_voltage<4000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_40MS;
//        else if(g_run_params.super_capacitor_voltage>4000 )
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_50MS;
//#endif

        //printf("g_run_params.super_capacitor_voltage = %d\r\n", g_run_params.super_capacitor_voltage);

        // �л�״̬
        s_super_capacitor_handler.state = E_SC_STATE_IDLE;
        break;
//    case E_SC_STATE_FAST_RECHARGE:
//        if(s_super_capacitor_handler.exit_fast_recharge_timer >= 1000)
//            GPIO_ResetBits(GPIOB, GPIO_Pin_6);
//        else
//            GPIO_SetBits(GPIOB, GPIO_Pin_6);
//
//        break;
    default:
        s_super_capacitor_handler.state = E_SC_STATE_IDLE;
        break;
    }
}

// ��ȡ���״̬
u8 task_super_capacitor_get_mode(void)
{
    return s_super_capacitor_handler.state;
}

//void task_super_capacitor_ctrl(u8 enable)
//{
//    s_super_capacitor_handler.recharge_switch = enable;
//    s_super_capacitor_handler.recharge_counter = 0;
//}

// ���ٳ��ģʽ
void task_super_capacitor_fast_triggered(void)
{
    s_super_capacitor_handler.recharge_mode = E_SUPERCAPACITOR_MODE_FAST;
}

// �������ģʽ
void task_super_capacitor_normal_triggered(void)
{
    s_super_capacitor_handler.recharge_mode = E_SUPERCAPACITOR_MODE_NORMAL;
}


// �ɼ��������еĵ�ѹֵ
u16 task_super_capacitor_read_voltage(void)
{
//    u16 super_capacitor_voltage = 0;
//
//    // ��ʼ���������ݲ���
//    adc_power_on(SUPER_CAPACITOR_RECHARGE);
//    adc_init(SUPER_CAPACITOR_RECHARGE);
//
//    delay_ms(5);
//
//    // �Գ������ݲ���
//    super_capacitor_voltage = adc_sample(SUPER_CAPACITOR_RECHARGE);
//    //super_capacitor_voltage = 3038;
//
//    // ��ѹת��
//    super_capacitor_voltage = super_capacitor_voltage / 62;
//
//    // �ر��ͷų������ݲ�����·
//    adc_close(SUPER_CAPACITOR_RECHARGE);
//    adc_power_off(SUPER_CAPACITOR_RECHARGE);

    float adc_value = 0;

    // ��ʼ���������ݲ���
    adc_power_on(SUPER_CAPACITOR_RECHARGE);
    adc_init(SUPER_CAPACITOR_RECHARGE);
    // �Գ������ݵ�ѹ����
    adc_value = adc_sample(SUPER_CAPACITOR_RECHARGE);
    //printf("adc_value = %f\r\n",adc_value);

    adc_close(SUPER_CAPACITOR_RECHARGE);            // �ر��ͷų������ݲ�������
    adc_power_off(SUPER_CAPACITOR_RECHARGE);        // �رյ��ݵ�ѹ�ɼ�ADC

    // ��ѹ����
    //adc_value = adc_value*6600/4096;
    adc_value = adc_value*6200/4096;
    //g_run_params.super_capacitor_voltage = (u16)adc_value;  //��λmV
    //printf("g_run_params.super_capacitor_voltage = %d\r\n",g_run_params.super_capacitor_voltage);

    return (u16)adc_value;
}

u8 super_capacitor_electric_quantity(u16 voltage)
{
    u8 voltage_percent = 0;

    voltage_percent = voltage/50;       //�������ݵ����ٷֱ�

    if(voltage_percent>100)
    {
        voltage_percent = 100;
    }

    return voltage_percent;
}




