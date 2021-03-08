# TUYA_TEMP
This project is developed using Tuya SDK, which enables you to quickly develop branded apps connecting and controlling smart scenarios of many devices.
For more information, please check Tuya Developer Website.
# 简介
* 从板载的高精度温湿度传感器SHT30获取温湿度信息
* 上传这些信息到涂鸦云平台使我能在涂鸦APP中看到温湿度信息

# 硬件部分
WBR3D WiFi+蓝牙 模块
SHT30 温湿度传感器
CH340USB转串口芯片
STM32L071 MCU

[PCB 链接](https://oshwhub.com/baobaoa/wu-lian-wang-qi-xiang-tai-657332a)
# 软件
涂鸦提供的MCU-SDK
STM32 CubeMX with HAL
MDK

# 说明
* 这是我第一块使用涂鸦模组的板子，所以还存在着一些问题
* 
* MCU价格较为昂贵，制作时可以考虑选择国产的32位MCU
* 
* CH340芯片不是必须的，这次画上PCB板仅仅是因为寒假忘记携带TTL下载器回家
