# 手持调试器 / Handheld debugger

<div align="center">

[English](#english) | [中文](#中文)

</div>

---

## 中文

### 📖 项目简介



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
