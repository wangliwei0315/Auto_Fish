# Auto_Fish
基于Arduino语言编写的ESP32喂鱼器代码，可实现Blinker  App远程控制及小爱同学语音控制功能。
1.喂鱼过程实时显示DI采集值
2.喂鱼结束后显示喂鱼器工作时间
3.正在喂鱼，且开始5S后，且连续2次DI采集为高电平，则停止喂鱼（防止DI信号干扰）
