/*
* ����������
* �˽���ִ�а������������صƲ����������Ҽ���ⲿ�жϣ��ڿ��ƺ��������жϴ�������
*/

#include "adf.h"
#include "sysparams.h"
#include "board.h"
#include "LCD.h"
#include "task_key.h"
//#include "task_disp.h"
#include "key.h"
#include "light.h"
#include "task_super_capacitor.h"
#include "task_report.h"

u8 task_key_dealwith(void);

/*! \brief key status */
static KeyHandler s_key_handler;

/*! \brief
*       key process
*/
void task_key_proc(void)
{
    if(s_key_handler.key_comb_counter>0)
    {
        s_key_handler.key_comb_timer++;
    }
    else
    {
        s_key_handler.key_comb_timer = 0;
        s_key_handler.key_comb_counter = 0;
    }

    if(s_key_handler.key_comb_timer > 7*UNIT_SECOND)
    {
        s_key_handler.key_comb_timer = 0;
        s_key_handler.key_comb_counter = 0;
    }

    switch(s_key_handler.sts)
    {
    case KEY_STS_IDLE:        // ��������״̬
        break;

    case KEY_STS_PUSH:        // �ж��Ƿ�Ϊ�Ϸ�����
        if (TRUE == key_check_valid(s_key_handler.key_val))
        {
            s_key_handler.sts = KEY_STS_VALID;
        }
        else
        {
            s_key_handler.sts = KEY_STS_IDLE;
        }
        break;

    case KEY_STS_VALID:        // ��ⰴ���ͷ�
        // ֻ����û���������ҿ���ʱ����г���
        if(s_report_handler.connected_state == 0 && g_sys_params.light_sts[E_LIGHT_1] == E_LIGHT_ON && g_sys_params.light_sts[E_LIGHT_2] == E_LIGHT_ON)
        {
            if (TRUE == key_release(s_key_handler.key_val))
                s_key_handler.sts = KEY_STS_DEAL;
        }
        else
        {
            s_key_handler.sts = KEY_STS_DEAL;
        }

        break;

    case KEY_STS_DEAL:        // ��������
        #ifdef LIGHT_DEBUG
        printf("dealwith\r\n");
        #endif

        task_key_dealwith();   //����������

        if(s_key_handler.key_val == KEY_IDLE)
            s_key_handler.sts = KEY_STS_IDLE;
        else
            s_key_handler.sts = KEY_STS_FEEDBACK;

        //s_key_handler.feedback_timer = 0;
        //delay_ms(25);
        break;
    case KEY_STS_FEEDBACK:      // �������� ��Ҫ��⿪�ش�֮�����Ƿ� ���ϸ�����,�ʹ洢״̬
//        s_key_handler.feedback_timer++;
//        if (s_key_handler.feedback_timer >= MAX_FEEDBACK_TIMER)
//        {
////            #ifdef LIGHT_DEBUG
////            printf("counter:%d \r\n", s_key_handler.pluse_counter_light);
////            #endif
//
//            //exti_close_by_index(E_LIGHT_1);     // �ر��жϼ��
//
//            // L1��L2���޵Ƹ��ش���
//            if (s_key_handler.pluse_counter_light < MAX_VAILD_PLUSE_COUNT) // 2���ڼ�⵽�жϴ���20����˵���и��أ����С��20����˵��û�и��ص�ȫ���ر�  20
//            {
//                // �ر�L1��L2
//                light_off(E_LIGHT_1);
//                light_off(E_LIGHT_2);
//                //g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_OFF;
//                //g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_OFF;
//
//                // ���ڹصƵ�����¿������
//                //task_super_capacitor_ctrl(1);
//
//            }
//            else
//            {
//                // ���ڿ��Ƶ�����¹رճ��
//                //task_super_capacitor_ctrl(0);
//            }
//
//            // ����Ƶ�״̬
//            //sysparams_write(E_ADDR_OFFSET_LIGHT1_STATE, g_sys_params.light_sts, 2);
//            s_key_handler.sts = KEY_STS_IDLE;
//        }
        reset_delay_recharge_count();//���ó����ʱ
        s_key_handler.sts = KEY_STS_IDLE;
        break;

    default:
        break;
    }
}

/*! \brief
*       �������̳�ʼ�� key task init
*/
void task_key_init(void)
{
//    // �ر�L1��L2
//    light_off(E_LIGHT_1);
//    light_off(E_LIGHT_2);
    //g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_OFF;
    //g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_OFF;
//
//    // ����Ƶ�״̬
//    sysparams_write(E_ADDR_OFFSET_LIGHT1_STATE, g_sys_params.light_sts, 2);

    memset(&s_key_handler, 0, sizeof(s_key_handler));
}

/*! \brief
*       �������̴������� key triggered, start key process
* \param key_code[IN]       - key value
*/
void task_key_triggered(u8 key_code)
{
    s_key_handler.key_val = key_code;
    s_key_handler.sts = KEY_STS_PUSH;
}

/*! \brief
*      ����������ʼ��������  key triggered, start key process
* \param light1_state       - ��һ·�Ƶ�״̬ light1_state
* \param light2_state       - �ڶ�·�Ƶ�״̬ light2_state
*/
void task_key_triggered_manual(u8 light1_state, u8 light2_state)
{
    if (light1_state == E_LIGHT_ON)
    {
        light_on(E_LIGHT_1);
        g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_ON;
    }

    if (light2_state == E_LIGHT_ON)
    {
        light_on(E_LIGHT_2);
        g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_ON;
    }

    //if (light2_state == 1 || light1_state == 1)
    {
        //delay_ms(8);
        //exti_init_by_index(E_LIGHT_1);
        s_key_handler.pluse_counter_light = 0;

        //s_key_handler.feedback_timer = 0;
        s_key_handler.sts = KEY_STS_FEEDBACK;
    }
}

/*! \brief
*       ���°�������״̬
* \return
*       key task run state
*/
u8 task_key_get_state(void)
{
    return s_key_handler.sts;
}

// ����������
u8 task_key_dealwith(void)
{
//    if(g_sys_params.factory_flag > FACTORY_NUM && s_key_handler.key_val == KEY_COMB_1)
//        s_key_handler.key_val = KEY_IDLE;s_report_handler.connected_state==0

    if(s_report_handler.connected_state==1 && s_key_handler.key_val == KEY_COMB_1)
        s_key_handler.key_val = KEY_IDLE;

    switch(s_key_handler.key_val)
    {
    case KEY_1:             // ��������1
        if (g_sys_params.light_sts[E_LIGHT_1] == E_LIGHT_OFF)
        {
            light_on(E_LIGHT_1);
            g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_ON;

            #ifdef LIGHT_DEBUG
            printf("light1 on\r\n");
            #endif
        }
        else
        {
            light_off(E_LIGHT_1);
            g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_OFF;

            if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
                LCD_DisplayMARK(Mark_RH, CLEAR);                                    //ˮ��
            task_super_capacitor_normal_triggered();

            g_run_params.recharge_state = FALSE;
            #ifdef LIGHT_DEBUG
            printf("light1 off\r\n");
            #endif
        }

        //delay_ms(8);
        //exti_init_by_index(E_LIGHT_1);      // ��ʼ����⿪�� �ж�����
        //s_key_handler.pluse_counter_light = 0;
        break;
    case KEY_2:             // ��������2
        if (g_sys_params.light_sts[E_LIGHT_2] == E_LIGHT_OFF)
        {
            light_on(E_LIGHT_2);
            g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_ON;

            #ifdef LIGHT_DEBUG
            printf("light2 on\r\n");
            #endif
        }
        else
        {
            light_off(E_LIGHT_2);
            g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_OFF;

            if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
                LCD_DisplayMARK(Mark_RH, CLEAR);                                    //ˮ��
            task_super_capacitor_normal_triggered();

            g_run_params.recharge_state = FALSE;
            #ifdef LIGHT_DEBUG
            printf("light2 off\r\n");
            #endif
        }

        //delay_ms(8);
        //exti_init_by_index(E_LIGHT_1);      // ��ʼ����⿪�� �ж�����
        //s_key_handler.pluse_counter_light = 0;
        break;
    case KEY_COMB_1:        // ������ϼ�
        //if(g_run_params.key_comb_1)
        //    g_run_params.key_comb_1 = 0;
        //else
        //    g_run_params.key_comb_1 = 1;
        s_key_handler.key_comb_counter++;

        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
        {
            LCD_Display_H(g_run_params.humi, WRITE);//��ʾʪ������
//            // ��ʾ��ѹ
            LCD_DisplayMARK(Mark_PT,CLEAR);                                     // С����
            LCD_DisplayMARK(Mark_T, CLEAR);                                     // �¶ȼ�
//
            LCD_DisplayMARK(Mark_DC, CLEAR);                                    // C ���϶ȱ�־
            LCD_DisplayMARK(Mark_RH, CLEAR);                                    // ˮ��
            LCD_DisplayMARK(Mark_PC, CLEAR);                                    // �ٷֺ�
            LCD_WE_NUM(g_run_params.super_capacitor_voltage/1000,1,WRITE);	    // �¶�ʮλ
            LCD_WE_NUM((g_run_params.super_capacitor_voltage/100)%10,2,WRITE);	// �¶ȸ�λ
            LCD_WE_NUM((g_run_params.super_capacitor_voltage/10)%10,3,WRITE);	// �¶�С��λ
//
        }

        if( g_sys_params.light_sts[E_LIGHT_1] == E_LIGHT_ON && g_sys_params.light_sts[E_LIGHT_2] == E_LIGHT_ON)
        {
            if(s_key_handler.key_comb_counter >= 5 && s_key_handler.key_comb_timer <= 7*UNIT_SECOND)
            {
                LCD_Display_H(g_run_params.humi, WRITE);//��ʾʪ������
                // ��ʾ��ѹ
                LCD_DisplayMARK(Mark_PT,CLEAR);                                     // С����
                LCD_DisplayMARK(Mark_T, CLEAR);                                     // �¶ȼ�

                LCD_DisplayMARK(Mark_DC, CLEAR);                                    // C ���϶ȱ�־
                LCD_DisplayMARK(Mark_RH, CLEAR);                                    // ˮ��
                LCD_DisplayMARK(Mark_PC, CLEAR);                                    // �ٷֺ�
                LCD_WE_NUM(g_run_params.super_capacitor_voltage/1000,1,WRITE);	    // �¶�ʮλ
                LCD_WE_NUM((g_run_params.super_capacitor_voltage/100)%10,2,WRITE);	// �¶ȸ�λ
                LCD_WE_NUM((g_run_params.super_capacitor_voltage/10)%10,3,WRITE);	// �¶�С��λ

                s_key_handler.key_comb_timer = 0;
                s_key_handler.key_comb_counter = 0;
                task_super_capacitor_fast_triggered();
            }
        }

        break;
    default:
#ifdef LIGHT_DEBUG
            printf("comb key\r\n");
#endif
        return 1;
        break;
    }

    return 1;
}

void task_key_set_pluse_counter(void)
{
    //s_key_handler.pluse_counter_light++;
}