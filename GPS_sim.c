/*===========================================================================
  STC15W204S GPS模拟器 - 内存优化版
  RAM使用减少到 < 100字节
  ===========================================================================*/

/* 自定义8051寄存器定义 */
#ifndef _MY_REG51_H_
#define _MY_REG51_H_

/* 只定义必要的寄存器 */
sfr P1    = 0x90;
sfr P3    = 0xB0;
sfr SP    = 0x81;
sfr TCON  = 0x88;
sfr TMOD  = 0x89;
sfr TL1   = 0x8B;
sfr TH1   = 0x8D;
sfr IE    = 0xA8;
sfr SCON  = 0x98;
sfr SBUF  = 0x99;

/* 只定义必要的位 */
sbit TI   = SCON^1;
sbit RI   = SCON^0;

#endif /* _MY_REG51_H_ */

/* 标准库 - 只包含必要的 */
#include <stdio.h>

/*===========================================================================
  使用 code 存储常量（放在Flash中，不占用RAM）
  ===========================================================================*/
code char GGA_STR[] = "$GPGGA,123456.00,2240.8720,N,11407.4040,E,1,08,1.2,15.5,M,,";
code char GLL_STR[] = "$GPGLL,2240.8720,N,11407.4040,E,123456.00,A,A";
code char RMC_STR[] = "$GPRMC,123456.00,A,2240.8720,N,11407.4040,E,0.00,0.00,260124,0.0,E,A";
code char VTG_STR[] = "$GPVTG,0.00,T,0.00,M,0.00,N,0.00,K,A";
code char ZDA_STR[] = "$GPZDA,123456.00,26,01,2024,00,00";

/*===========================================================================
  使用 xdata 存储大数组（如果支持）或直接发送
  注意：STC15W204S 没有 xdata，我们使用小缓冲区
  ===========================================================================*/
unsigned char small_buf[20];  // 减少到20字节

/*===========================================================================
  全局变量 - 尽量减少
  ===========================================================================*/
unsigned char utc_hour = 12;
unsigned char utc_min = 34;
unsigned char utc_sec = 56;
unsigned char counter = 0;

/*===========================================================================
  函数声明 - 简化版本
  ===========================================================================*/
void UART_Init(void);
void UART_SendByte(unsigned char dat);
void UART_SendString(const char *str);
void UART_SendCodeString(const char code *str);
void DelayMS(unsigned int ms);
void SendGGA(void);
void SendGLL(void);
void SendRMC(void);
void SendVTG(void);
void SendZDA(void);

/*===========================================================================
  串口初始化 - 简化
  ===========================================================================*/
void UART_Init(void) {
    SCON = 0x50;        // 模式1
    TMOD |= 0x20;       // 定时器1模式2
    TH1 = 0xFD;         // 9600波特率
    TL1 = 0xFD;
    TCON |= 0x40;       // TR1=1
    TI = 0;
    RI = 0;
}

/*===========================================================================
  发送一个字节
  ===========================================================================*/
void UART_SendByte(unsigned char dat) {
    SBUF = dat;
    while(!TI);
    TI = 0;
}

/*===========================================================================
  发送RAM中的字符串
  ===========================================================================*/
void UART_SendString(const char *str) {
    while(*str) {
        UART_SendByte(*str++);
    }
}

/*===========================================================================
  发送CODE中的字符串（不占用RAM）
  ===========================================================================*/
void UART_SendCodeString(const char code *str) {
    while(*str) {
        UART_SendByte(*str++);
    }
}

/*===========================================================================
  计算NMEA校验和
  ===========================================================================*/
unsigned char CalculateChecksum(const char *str) {
    unsigned char checksum = 0;
    if(*str == '$') str++;
    while(str && *str != '') {
        checksum ^= *str++;
    }
    return checksum;
}

/*===========================================================================
  计算CODE中字符串的校验和
  ===========================================================================*/
unsigned char CalculateChecksumCode(const char code *str) {
    unsigned char checksum = 0;
    if(*str == '$') str++;
    while(str && *str != '') {
        checksum ^= *str++;
    }
    return checksum;
}

/*===========================================================================
  延时函数 - 优化版本
  ===========================================================================*/
void DelayMS(unsigned int ms) {
    unsigned int i;
    for(i = 0; i < ms; i++) {
        unsigned char j;
        for(j = 0; j < 120; j++) {
            // 空循环
        }
    }
}

/*===========================================================================
  发送GGA语句 - 使用预存的字符串
  ===========================================================================*/
void SendGGA(void) {
    unsigned char checksum;
    
    // 发送固定部分
    UART_SendCodeString(GGA_STR);
    
    // 计算并发送校验和
    checksum = CalculateChecksumCode(GGA_STR);
    UART_SendByte('*');
    
    // 发送校验和（十六进制）
    if((checksum >> 4) < 10) {
        UART_SendByte((checksum >> 4) + '0');
    } else {
        UART_SendByte((checksum >> 4) - 10 + 'A');
    }
    
    if((checksum & 0x0F) < 10) {
        UART_SendByte((checksum & 0x0F) + '0');
    } else {
        UART_SendByte((checksum & 0x0F) - 10 + 'A');
    }
    
    UART_SendByte('\r');
    UART_SendByte('\n');
}

/*===========================================================================
  发送GLL语句
  ===========================================================================*/
void SendGLL(void) {
    unsigned char checksum;
    
    UART_SendCodeString(GLL_STR);
    checksum = CalculateChecksumCode(GLL_STR);
    
    UART_SendByte('*');
    if((checksum >> 4) < 10) {
        UART_SendByte((checksum >> 4) + '0');
    } else {
        UART_SendByte((checksum >> 4) - 10 + 'A');
    }
    
    if((checksum & 0x0F) < 10) {
        UART_SendByte((checksum & 0x0F) + '0');
    } else {
        UART_SendByte((checksum & 0x0F) - 10 + 'A');
    }
    
    UART_SendByte('\r');
    UART_SendByte('\n');
}

/*===========================================================================
  发送RMC语句
  ===========================================================================*/
void SendRMC(void) {
    unsigned char checksum;
    
    UART_SendCodeString(RMC_STR);
    checksum = CalculateChecksumCode(RMC_STR);
    
    UART_SendByte('*');
    if((checksum >> 4) < 10) {
        UART_SendByte((checksum >> 4) + '0');
    } else {
        UART_SendByte((checksum >> 4) - 10 + 'A');
    }
    
    if((checksum & 0x0F) < 10) {
        UART_SendByte((checksum & 0x0F) + '0');
    } else {
        UART_SendByte((checksum & 0x0F) - 10 + 'A');
    }
    
    UART_SendByte('\r');
    UART_SendByte('\n');
}

/*===========================================================================
  发送VTG语句
  ===========================================================================*/
void SendVTG(void) {
    unsigned char checksum;
    
    UART_SendCodeString(VTG_STR);
    checksum = CalculateChecksumCode(VTG_STR);
    
    UART_SendByte('*');
    if((checksum >> 4) < 10) {
        UART_SendByte((checksum >> 4) + '0');
    } else {
        UART_SendByte((checksum >> 4) - 10 + 'A');
    }
    
    if((checksum & 0x0F) < 10) {
        UART_SendByte((checksum & 0x0F) + '0');
    } else {
        UART_SendByte((checksum & 0x0F) - 10 + 'A');
    }
    
    UART_SendByte('\r');
    UART_SendByte('\n');
}

/*===========================================================================
  发送ZDA语句
  ===========================================================================*/
void SendZDA(void) {
    unsigned char checksum;
    
    UART_SendCodeString(ZDA_STR);
    checksum = CalculateChecksumCode(ZDA_STR);
    
    UART_SendByte('*');
    if((checksum >> 4) < 10) {
        UART_SendByte((checksum >> 4) + '0');
    } else {
        UART_SendByte((checksum >> 4) - 10 + 'A');
    }
    
    if((checksum & 0x0F) < 10) {
        UART_SendByte((checksum & 0x0F) + '0');
    } else {
        UART_SendByte((checksum & 0x0F) - 10 + 'A');
    }
    
    UART_SendByte('\r');
    UART_SendByte('\n');
}

/*===========================================================================
  主函数 - 简化版本
  ===========================================================================*/
void main(void) {
    // 初始化串口
    UART_Init();
    DelayMS(1000);
    
    // 发送启动信息
    UART_SendCodeString("\r\nGPS Simulator v1.0\r\n");
    
    // 主循环
    while(1) {
        // 更新时间（简单模拟）
        utc_sec++;
        if(utc_sec >= 60) {
            utc_sec = 0;
            utc_min++;
        }
        if(utc_min >= 60) {
            utc_min = 0;
            utc_hour++;
        }
        if(utc_hour >= 24) {
            utc_hour = 0;
        }
        
        // 发送所有NMEA语句
        SendGGA();
        DelayMS(100);
        SendGLL();
        DelayMS(100);
        SendRMC();
        DelayMS(100);
        SendVTG();
        DelayMS(100);
        SendZDA();
        DelayMS(100);
        
        // 显示状态
        counter++;
        if(counter >= 10) {
            counter = 0;
            UART_SendCodeString("-- Active --\r\n");
        }
        
        // 等待
        DelayMS(700);
    }
}