#include "AD5686_5_4R.h"
#include "spi.h"

#define AD568x_OK 0
#define AD568x_ERROR 1

int Write_AD568x(AD5686_5_4R *AD568x, uint8_t Order, uint8_t ADD, uint16_t DATA_Bits);
int AD568x_update_register(AD5686_5_4R *AD568x, uint8_t Aisle);
int AD568x_Renew_LDAC(AD5686_5_4R *AD568x);
int AD568x_write_update_register(AD5686_5_4R *AD568x, uint8_t Aisle, int DATA);
int AD568x_power_mode(AD5686_5_4R *AD568x, uint8_t Aisle, int Mode);
int AD568x_ldac_mask(AD5686_5_4R *AD568x, uint8_t Aisle, int Mode);
int AD568x_set_shift_reg(AD5686_5_4R *AD568x, uint8_t Aisle, int DATA);

/************************用户参数************************/
/**
* @brief  初始化并创建一个AD568x的设备
* @retval 空值
*/
AD5686_5_4R DAC1_AD5686;
int AD5686x_Init(void)
{
    /*用户定义参数*/
    DAC1_AD5686.Name = AD5686;
		DAC1_AD5686.SPI_Aisle=hspi1;
    DAC1_AD5686.EN_Pin=GPIO_PIN_4;
    DAC1_AD5686.EN_Prot=AD5686R_EN_GPIO_Port;
    DAC1_AD5686.LDAC_Pin=GPIO_PIN_3;
    DAC1_AD5686.LDAC_Prot=AD5686R_LDAC_GPIO_Port;
    DAC1_AD5686.RESET_Pin=AD5686R_RESET_Pin;
    DAC1_AD5686.RESET_Prot=AD5686R_RESET_GPIO_Port;
    DAC1_AD5686.Vref = 2.5;
    DAC1_AD5686.GAIN = Double_GAIN;
    DAC1_AD5686.Renew_Mode = asynchronous;

    DAC1_AD5686.POW_MODE_A = normal_Mode;
    DAC1_AD5686.POW_MODE_B = normal_Mode;
    DAC1_AD5686.POW_MODE_C = normal_Mode;
    DAC1_AD5686.POW_MODE_D = normal_Mode;

    DAC1_AD5686.LDAC_MODE_A = LDAC_Turn;
    DAC1_AD5686.LDAC_MODE_B = LDAC_Turn;
    DAC1_AD5686.LDAC_MODE_C = LDAC_Turn;
    DAC1_AD5686.LDAC_MODE_D = LDAC_Turn;

    /*初始化操作*/
    //初始化LDAC引脚
    if (DAC1_AD5686.Renew_Mode == asynchronous)
    {
        HAL_GPIO_WritePin(DAC1_AD5686.LDAC_Prot, DAC1_AD5686.LDAC_Pin, GPIO_PIN_SET);
    }
    else if (DAC1_AD5686.Renew_Mode == Synchronize)
    {
        HAL_GPIO_WritePin(DAC1_AD5686.LDAC_Prot, DAC1_AD5686.LDAC_Pin, GPIO_PIN_RESET);
    }
    //硬件复位AD568X
    HAL_GPIO_WritePin(DAC1_AD5686.RESET_Prot, DAC1_AD5686.RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(DAC1_AD5686.RESET_Prot, DAC1_AD5686.RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(5);
    //检测各通道电源状态
    AD568x_power_mode(&DAC1_AD5686, Aisle_A, DAC1_AD5686.POW_MODE_A);
    AD568x_power_mode(&DAC1_AD5686, Aisle_B, DAC1_AD5686.POW_MODE_B);
    AD568x_power_mode(&DAC1_AD5686, Aisle_C, DAC1_AD5686.POW_MODE_C);
    AD568x_power_mode(&DAC1_AD5686, Aisle_D, DAC1_AD5686.POW_MODE_D);
    //检测各通道是否收LDAC引脚控制
    AD568x_ldac_mask(&DAC1_AD5686, Aisle_A, DAC1_AD5686.LDAC_MODE_A);
    AD568x_ldac_mask(&DAC1_AD5686, Aisle_B, DAC1_AD5686.LDAC_MODE_B);
    AD568x_ldac_mask(&DAC1_AD5686, Aisle_C, DAC1_AD5686.LDAC_MODE_C);
    AD568x_ldac_mask(&DAC1_AD5686, Aisle_D, DAC1_AD5686.LDAC_MODE_D);
}
/************************用户函数************************/
/**
  * @brief 使用V为单位直接写入DAC
  * @param AD568x：写目标设备
  * @param Aisle：选择的通道
  * @param Mode：选择的模式
  * @retval 写操作反馈
  */
int AD568x_write_V(AD5686_5_4R *AD568x, uint8_t Aisle, double DATA_D)
{
		int DATA_I;
		if(DATA_D>((AD568x->Vref)*(AD568x->GAIN)))
		{
			return AD568x_ERROR;
		}
		else
		{
			DATA_I=13107*DATA_D;
		}
		
    if ((AD568x->Name == AD5684) && (DATA_I >= 0 && DATA_I <= 4095))
    {
        if (Write_AD568x(AD568x, Write_Renew_n, Aisle, (uint16_t)DATA_I) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else if ((AD568x->Name == AD5685) && (DATA_I >= 0 && DATA_I <= 16383))
    {
        if (Write_AD568x(AD568x, Write_Renew_n, Aisle, (uint16_t)DATA_I) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else if ((AD568x->Name == AD5686) && (DATA_I >= 0 && DATA_I <= 65535))
    {
        if (Write_AD568x(AD568x, Write_Renew_n, Aisle, (uint16_t)DATA_I) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else
    {
        return AD568x_ERROR;
    }
}

/**
  * @brief 设置LDAC屏蔽
  * @param AD568x：写目标设备
  * @param Aisle：选择的通道
  * @param Mode：选择的模式
  * @retval 写操作反馈
  */
int AD568x_ldac_mask(AD5686_5_4R *AD568x, uint8_t Aisle, int Mode)
{
    uint16_t DATA;
    //逐个枚举各个情况所需要发送的数据
    if (Mode == LDAC_Turn)
    {
        if ((Aisle == Aisle_A) || (Aisle == Aisle_B) || (Aisle == Aisle_C) || (Aisle == Aisle_D))
        {
            DATA = 0x00;
        }
        else
        {
            return AD568x_ERROR;
        }
    }
    else if (Mode == LDAC_Shield)
    {
        if (Aisle == Aisle_A)
        {
            DATA = 0x01;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x02;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x04;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x08;
        }
        else
        {
            return AD568x_ERROR;
        }
    }
    else
    {
        return AD568x_ERROR;
    }
    //发送数据
    if (Write_AD568x(AD568x, Shield_LDAC, 0, DATA) != AD568x_OK)
    {
        return AD568x_ERROR;
    }
    return AD568x_OK;
}
/**
  * @brief 设置掉电模式
  * @param AD568x：写目标设备
  * @param Aisle：选择的通道
  * @param Mode：选择的模式
  * @retval 写操作反馈
  */
int AD568x_power_mode(AD5686_5_4R *AD568x, uint8_t Aisle, int Mode)
{
    uint16_t DATA;
    //逐个枚举各个情况所需要发送的数据
    if (Mode == normal_Mode)
    {
        if ((Aisle == Aisle_A) || (Aisle == Aisle_B) || (Aisle == Aisle_C) || (Aisle == Aisle_D))
        {
            DATA = 0x00;
        }
        else
        {
            return AD568x_ERROR;
        }
    }
    else if (Mode == K1_GND_Mode)
    {
        if (Aisle == Aisle_A)
        {
            DATA = 0x01;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x04;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x10;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x40;
        }
        else
        {
            return AD568x_ERROR;
        }
    }
    else if (Mode == K100_GND_Mode)
    {
        if (Aisle == Aisle_A)
        {
            DATA = 0x02;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x08;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x20;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x80;
        }
        else
        {
            return AD568x_ERROR;
        }
    }
    else if (Mode == High_Impedance_State_Mode)
    {
        if (Aisle == Aisle_A)
        {
            DATA = 0x03;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x0c;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0x30;
        }
        else if (Aisle == Aisle_B)
        {
            DATA = 0xc0;
        }
        else
        {
            return AD568x_ERROR;
        }
    }
    else
    {
        return AD568x_ERROR;
    }
    //发送数据
    if (Write_AD568x(AD568x, DAC_Power_Down_on, 0, DATA) != AD568x_OK)
    {
        return AD568x_ERROR;
    }
    return AD568x_OK;
}
/**
  * @brief 写入并更新n
  * @param AD568x：写目标设备
  * @param Aisle：选择的通道
  * @retval 写操作反馈
  */
int AD568x_write_update_register(AD5686_5_4R *AD568x, uint8_t Aisle, int DATA)
{
    if ((AD568x->Name == AD5684) && (DATA >= 0 && DATA <= 4095))
    {
        if (Write_AD568x(AD568x, Write_Renew_n, Aisle, (uint16_t)DATA) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else if ((AD568x->Name == AD5685) && (DATA >= 0 && DATA <= 16383))
    {
        if (Write_AD568x(AD568x, Write_Renew_n, Aisle, (uint16_t)DATA) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else if ((AD568x->Name == AD5686) && (DATA >= 0 && DATA <= 65535))
    {
        if (Write_AD568x(AD568x, Write_Renew_n, Aisle, (uint16_t)DATA) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else
    {
        return AD568x_ERROR;
    }
}
/**
  * @brief 通过写入寄存器n更新DAC
  * @param AD568x：写目标设备
  * @param Aisle：选择的通道
  * @retval 写操作反馈
  */
int AD568x_update_register(AD5686_5_4R *AD568x, uint8_t Aisle)
{
    if (Write_AD568x(AD568x, Renew_DAC_For_n, Aisle, 0) != AD568x_OK)
    {
        return AD568x_ERROR;
    }
}

/**
  * @brief 写入输入寄存器n
  * @param AD568x：写目标设备
  * @param Aisle：选择的通道
  * @param DATA：数值
  * @retval 写操作反馈
  */
int AD568x_set_shift_reg(AD5686_5_4R *AD568x, uint8_t Aisle, int DATA)
{

    if ((AD568x->Name == AD5684) && (DATA >= 0 && DATA <= 4095))
    {
        if (Write_AD568x(AD568x, Write_n, Aisle, (uint16_t)DATA) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else if ((AD568x->Name == AD5685) && (DATA >= 0 && DATA <= 16383))
    {
        if (Write_AD568x(AD568x, Write_n, Aisle, (uint16_t)DATA) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else if ((AD568x->Name == AD5686) && (DATA >= 0 && DATA <= 65535))
    {
        if (Write_AD568x(AD568x, Write_n, Aisle, (uint16_t)DATA) != AD568x_OK)
        {
            return AD568x_ERROR;
        }
    }
    else
    {
        return AD568x_ERROR;
    }
		/*
    if (AD568x->Renew_Mode == asynchronous)
    {
        AD568x_Renew_LDAC(AD568x);
    }
		*/
    return AD568x_OK;
}

/**
  * @brief 通过LDAC引脚对AD568x的寄存器数据更新
  * @param AD568x：写目标设备
  * @retval 无
  */
int AD568x_Renew_LDAC(AD5686_5_4R *AD568x)
{
    //如果是同步更新，则返回错误
    if (AD568x->Renew_Mode == Synchronize)
    {
        return AD568x_ERROR;
    }
    HAL_GPIO_WritePin(AD568x->LDAC_Prot, AD568x->LDAC_Pin, GPIO_PIN_RESET);
    __nop();
    HAL_GPIO_WritePin(AD568x->LDAC_Prot, AD568x->LDAC_Pin, GPIO_PIN_SET);
    return AD568x_OK;
}
/************************内部函数************************/

/**
  * @brief 对AD568x写操作
  * @param AD568x：写目标设备
  * @param Order：写命令
  * @param ADD：写地址
  * @param Text：写内容
  * @retval 写操作反馈
  */
int Write_AD568x(AD5686_5_4R *AD568x, uint8_t Order, uint8_t ADD, uint16_t DATA_Bits)
{
    uint8_t DATA[3] = {0};
    DATA[0] = (Order<<4) | ADD;
    DATA[1] = DATA_Bits >> 8;
    DATA[2] = DATA_Bits;

    HAL_GPIO_WritePin(AD568x->EN_Prot, AD568x->EN_Pin, GPIO_PIN_RESET);

    if (HAL_SPI_Transmit(&(AD568x->SPI_Aisle), DATA, 3, 1000) != HAL_OK)
    {
        return AD568x_ERROR;
    }

    HAL_GPIO_WritePin(AD568x->EN_Prot, AD568x->EN_Pin, GPIO_PIN_SET);
    return AD568x_OK;
}
