# 手持调试器 / Handheld debugger

<div align="center">

[English](#english) | [中文](#中文)

</div>

---

## 中文


### 📖 项目版本 
v0.0.1 ：开始正式理想
v0.1.0 ：硬件测试完毕，基础功能正常，有简单的串口检测命令

### 📖 项目简介

（妮蔻蹦跶着从树梢倒挂下来，尾巴卷成问号，瞳孔里闪着小闪电⚡）

爱你～师傅傅把“妮蔻”印在小盒子身上惹！NK001-TSZS，念起来像“妮蔻——偷偷在说悄悄话”，嘿嘿，妮蔻听见惹！

别看它巴掌大，里面可藏着妮蔻的“千变口袋”：

1. 会呼吸的PWM

   旋钮轻轻一扭，就能让马达像小蜂鸟翅膀一样“嗡嗡～”快慢可调，也能让LED从“月牙”睡到“满月”。妮蔻变成小灯，陪它一起亮晶晶✨。

2. 魔法复读机

   把串口命令喂给它，它就乖乖“妮蔻妮蔻”地重复喊，一秒一次、一分一次，随师傅傅心情。妮蔻偷偷在里面放了一颗小石子，跳一下喊一次，绝不偷懒！

3. 电鼻子ADC

   0～3.3 V 的味道都能嗅出来，超灵敏！如果电压偷跑，它就“哔哔——”叫，像妮蔻发现糖果不见时那么着急🚨。

4. 悄悄话采集器

   任何偷偷溜过的串口字节，都会被它按顺序排好，贴在一张小纸条上，等师傅傅检阅。妮蔻最喜欢偷看纸条，然后学人家说话大舌头～

5. 双向传声筒

   既能当听话的小耳朵（RX），也能当爱唠叨的小嘴巴（TX）。波特率？数据位？校验？统统跳个舞就能换，像妮蔻换皮肤一样快！

6. 低电量“哭哭”报警

   电池饿惹，它就“滴——滴——”哭给师傅傅听；充满时，又“叮✨”一声破涕为笑。妮蔻也在旁边跟着哭、跟着笑，情绪价值满满！

7. 充放电小保姆

   插上USB，它先自己吃饱，再把多余的能量分给外设，像妮蔻把最后一颗糖分给好朋友。有过充、过放、过流小护盾，师傅傅放心去冒险！

（妮蔻一个后空翻，变成小盒子模样，又“噗”地恢复原型）

“NK001-TSZS”——妮蔻的首号分身，陪师傅傅闯实验室、跑现场、爬屋顶。以后它还会学更多把戏：逻辑分析、波形生成都排队等妮蔻教呢！

（眨着星星眼，递上一朵刚变出来的小铁线莲）

带妮蔻一起去调试世界吧～谁让师傅傅把名字刻进惹妮蔻的小心脏！

### ✨ 功能特点

- 动态内存分配创建FreeRTOS任务


### 🔧 硬件要求

- 芯片：STM32F103C8T6


### 🛠️ 开发环境

- IDE：Keil MDK 5.x / 6.x
- 编译器：ARM Compiler 5/6
- 库：
  - STM32标准外设库 (StdPeriph)
  - FreeRTOS V10.0.1

### 📂 项目结构

```
Despacito003/
├── Start/               # 启动文件
├── Library/             # 标准库文件
├── User/                # 用户代码
├── FreeRTOS/            # FreeRTOS源码
│   ├── inc/             # FreeRTOS头文件
│   ├── src/             # FreeRTOS源文件
│   └── port/            # 移植文件
├── Objects/             # 编译生成的目标文件
└── Listings/            # 编译生成的列表文件
```

### 🚀 使用方法

1. 打开项目：使用Keil MDK打开项目文件
2. 编译项目：点击"Build"按钮
3. 下载程序：连接ST-Link，点击"Download"按钮将程序下载到开发板
4. 观察结果：

### 📝 代码示例

```c
// 创建定时按键触发任务
```

### 📚 相关文档

- [FreeRTOS动态任务创建](../../docs/zh/003-FreeRTOS动态任务创建.md)

---

### English translation of the above Chinese section

### 📖 Project Description

(Blank in original)

### ✨ Features

- Create FreeRTOS tasks using dynamic memory allocation

### 🔧 Hardware Requirements

- MCU: STM32F103C8T6

### 🛠️ Development Environment

- IDE: Keil MDK 5.x / 6.x
- Compiler: ARM Compiler 5/6
- Libraries:
  - STM32 Standard Peripheral Library (StdPeriph)
  - FreeRTOS V10.0.1

### 📂 Project Structure

```
Despacito003/
├── Start/               # Startup files
├── Library/             # Standard library files
├── User/                # User code
├── FreeRTOS/            # FreeRTOS source code
│   ├── inc/             # FreeRTOS header files
│   ├── src/             # FreeRTOS source files
│   └── port/            # Port files
├── Objects/             # Compiled object files
└── Listings/            # Compiled listing files
```

### 🚀 Usage

1. Open project: Use Keil MDK to open the project file
2. Compile project: Click the "Build" button
3. Download program: Connect ST-Link and click "Download" to flash the board
4. Observe result

### 📝 Code Example

```c
// Create a timed button-triggered task
```

### 📚 Related Documentation

- [FreeRTOS Dynamic Task Creation](../../docs/zh/003-FreeRTOS动态任务创建.md)
