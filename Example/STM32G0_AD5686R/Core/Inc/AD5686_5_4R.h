
#ifndef __AD5686_5_4R_H
#define __AD5686_5_4R_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"




/*设备型号*/
#define AD5684 0x84
#define AD5685 0x85
#define AD5686 0x86
/*设备参数*/
#define Single_GAIN 1  //单倍增益
#define Double_GAIN 2  //双倍增益
#define Synchronize 1  //同步更新
#define asynchronous 2 //异步更新
/*寄存器地址*/
#define NO_Action 0x00         //无操作
#define Write_n 0x01           //写入输入寄存器n
#define Renew_DAC_For_n 0x02   //以输入寄存器n的内容更新DAC寄存器n
#define Write_Renew_n 0x03     //写入并更新DAC通道n
#define DAC_Power_Down_on 0x04 //DAC掉电\上电
#define Shield_LDAC 0x05       //硬件LDAC屏蔽寄存器
#define Software_Reset 0x06    //软件复位
#define Reference_Voltage 0x07 //内部基准电压源设置寄存器
#define Set_DACEN 0x08         //设置DCEN寄存器
#define Read_Back 0x09         //设置回读寄存器
/*通道地址*/
#define Aisle_A 0x01
#define Aisle_B 0X02
#define Aisle_C 0X04
#define Aisle_D 0X08
/*掉电模式*/
#define normal_Mode 0x00               //正常模式
#define K1_GND_Mode 0x01               //1K电阻接地
#define K100_GND_Mode 0x10             //100K电阻接地
#define High_Impedance_State_Mode 0x11 //三态模式
/*LDAC屏蔽模式*/
#define LDAC_Turn 0   //不屏蔽
#define LDAC_Shield 1 //屏蔽

    typedef struct __AD5686_5_4R
    {
        uint16_t Name;

        //SPI使能引脚
        uint16_t EN_Pin;
        GPIO_TypeDef *EN_Prot;
        //LDAC引脚
        uint16_t LDAC_Pin;
        GPIO_TypeDef *LDAC_Prot;
        //硬件复位引脚
        uint16_t RESET_Pin;
        GPIO_TypeDef *RESET_Prot;
        //所使用的SPI
        SPI_HandleTypeDef SPI_Aisle;

        /********硬件参数********/
        //基准电压值
        float Vref;
        //增益倍数选择
        int GAIN;
        //数据更新方式
        int Renew_Mode;

        /********通道参数********/
        //掉电模式
        int POW_MODE_A;
        int POW_MODE_B;
        int POW_MODE_C;
        int POW_MODE_D;
        //LDAC屏蔽模式
        int LDAC_MODE_A;
        int LDAC_MODE_B;
        int LDAC_MODE_C;
        int LDAC_MODE_D;
    } AD5686_5_4R;


int AD5686x_Init(void);
int Write_AD568x(AD5686_5_4R *AD568x, uint8_t Order, uint8_t ADD, uint16_t DATA_Bits);
int AD568x_update_register(AD5686_5_4R *AD568x, uint8_t Aisle);
int AD568x_Renew_LDAC(AD5686_5_4R *AD568x);
int AD568x_write_update_register(AD5686_5_4R *AD568x, uint8_t Aisle, int DATA);
int AD568x_power_mode(AD5686_5_4R *AD568x, uint8_t Aisle, int Mode);
int AD568x_ldac_mask(AD5686_5_4R *AD568x, uint8_t Aisle, int Mode);
int AD568x_set_shift_reg(AD5686_5_4R *AD568x, uint8_t Aisle, int DATA);
int AD568x_write_V(AD5686_5_4R *AD568x, uint8_t Aisle, double DATA_D);
extern AD5686_5_4R DAC1_AD5686;

#ifdef __cplusplus
}
#endif

#endif /* __AD5686_5_4R_H */
