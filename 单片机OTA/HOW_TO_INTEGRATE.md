# HAL库集成详细步骤

## 方法1: 使用STM32CubeMX（推荐）

### 步骤1: 下载并安装STM32CubeMX

1. **访问ST官网**
   - 网址: https://www.st.com/en/development-tools/stm32cubemx.html
   - 点击 "Get Software"
   - 注册账号（如果需要）
   - 下载STM32CubeMX安装包

2. **安装STM32CubeMX**
   - Windows: 运行安装程序
   - macOS: 打开.dmg文件并安装
   - Linux: 解压并运行安装脚本
   - **注意**: 需要Java运行环境（JRE 8或更高版本）

### 步骤2: 使用STM32CubeMX创建项目

1. **打开STM32CubeMX**
   ```bash
   # macOS/Linux: 运行安装目录下的可执行文件
   # Windows: 从开始菜单启动
   ```

2. **创建新项目**
   ```
   1. 点击 "New Project" 或 "File -> New Project"
   2. 在搜索框输入 "STM32F108T6"
   3. 在搜索结果中双击 "STM32F108T6" MCU
   4. 点击 "Start Project"
   ```

3. **配置UART1（二维码扫描模块）**
   ```
   左侧面板 -> Connectivity -> USART1
   - Mode: 选择 "Asynchronous"
   - Baud Rate: 9600
   - 点击 "GPIO Settings" 标签
   - 确认引脚配置：
     * PA9 -> USART1_TX
     * PA10 -> USART1_RX
   ```

4. **配置UART2（调试输出）**
   ```
   左侧面板 -> Connectivity -> USART2
   - Mode: 选择 "Asynchronous"
   - Baud Rate: 115200
   - GPIO Settings:
     * PA2 -> USART2_TX
     * PA3 -> USART2_RX
   ```

5. **配置UART3（WiFi模块）**
   ```
   左侧面板 -> Connectivity -> USART3
   - Mode: 选择 "Asynchronous"
   - Baud Rate: 115200
   - GPIO Settings:
     * PB10 -> USART3_TX
     * PB11 -> USART3_RX
   ```

6. **配置系统时钟（可选）**
   ```
   左侧面板 -> System Core -> RCC
   - High Speed Clock (HSE): 选择 "Crystal/Ceramic Resonator"（如果使用外部晶振）
   
   点击 "Clock Configuration" 标签
   - 配置系统时钟为72MHz（推荐）
   - 或使用默认配置
   ```

7. **生成项目代码**
   ```
   1. 点击顶部 "Project Manager" 标签
   2. Project Settings:
      - Project Name: STM32_OTA（或任意名称）
      - Project Location: 选择一个临时目录（如 ~/STM32CubeMX/STM32_OTA）
      - Toolchain/IDE: 选择 "Makefile"
   3. Code Generator:
      - 勾选 "Copy only necessary library files"
      - 其他选项使用默认值
   4. 点击 "GENERATE CODE" 按钮
   5. 等待代码生成完成
   ```

### 步骤3: 运行集成脚本

1. **打开终端，进入项目目录**
   ```bash
   cd /Users/zhenghongyu/Desktop/单片机OTA
   ```

2. **运行集成脚本**
   ```bash
   # 替换 <CubeMX项目路径> 为实际路径
   ./scripts/integrate_hal.sh ~/STM32CubeMX/STM32_OTA
   
   # 如果CubeMX项目在其他位置，使用完整路径：
   ./scripts/integrate_hal.sh /path/to/your/CubeMX/project
   ```

3. **查看脚本输出**
   ```
   脚本会显示：
   - 正在复制文件...
   - ✓ HAL驱动库复制完成
   - ✓ CMSIS文件复制完成
   - 验证关键文件...
   - ✓ 所有关键文件已就绪
   ```

### 步骤4: 验证集成

1. **检查HAL目录**
   ```bash
   ls HAL/STM32F1xx_HAL_Driver/Inc/ | head -5
   # 应该看到 stm32f1xx_hal.h 等文件
   
   ls HAL/CMSIS/Include/ | head -5
   # 应该看到 core_cm3.h 等文件
   ```

2. **运行项目设置脚本**
   ```bash
   ./scripts/setup_project.sh
   # 应该显示 "✓ 项目已就绪，可以编译"
   ```

3. **尝试编译（如果已安装工具链）**
   ```bash
   make clean
   make all
   # 如果编译成功，说明集成正确
   ```

---

## 方法2: 手动集成（不使用STM32CubeMX）

### 步骤1: 下载标准外设库

1. **访问ST官网**
   - 搜索 "STM32F10x Standard Peripheral Library"
   - 下载最新版本

2. **解压库文件**
   ```bash
   unzip stm32f10x_stdperiph_lib.zip
   ```

### 步骤2: 手动复制文件

```bash
# 进入项目目录
cd /Users/zhenghongyu/Desktop/单片机OTA

# 复制标准外设库文件（根据实际路径调整）
cp -r <解压路径>/STM32F10x_StdPeriph_Lib/Libraries/STM32F10x_StdPeriph_Driver/* HAL/STM32F10x_StdPeriph_Driver/
cp -r <解压路径>/STM32F10x_StdPeriph_Lib/Libraries/CMSIS/* HAL/CMSIS/
```

### 步骤3: 配置项目

在 `config.h` 中，**保持 `USE_HAL_DRIVER` 注释状态**（默认使用标准外设库）

---

## 方法3: 使用Git（如果可用）

某些HAL库可能托管在Git上，但ST官方通常不提供公开的Git仓库。

---

## 集成后的配置

### 1. 选择库类型

编辑 `config.h`：

```c
// 如果使用STM32CubeMX生成的HAL库，取消注释：
#define USE_HAL_DRIVER

// 如果使用标准外设库，保持注释状态（默认）
// #define USE_HAL_DRIVER
```

### 2. 配置WiFi（可选）

如果需要使用WiFi功能，编辑 `config.h`：

```c
#define WIFI_SSID                "你的WiFi名称"
#define WIFI_PASSWORD            "你的WiFi密码"
```

### 3. 编译项目

```bash
# 清理之前的编译文件
make clean

# 编译所有目标
make all

# 或者分别编译
make bootloader
make application
```

---

## 常见问题

### Q1: 找不到STM32CubeMX项目路径

**A**: 
```bash
# 在CubeMX中，项目路径显示在 Project Manager -> Project Location
# 或者在文件浏览器中查看生成的目录

# 使用完整路径
./scripts/integrate_hal.sh /完整/路径/到/CubeMX项目
```

### Q2: 脚本提示"找不到Drivers目录"

**A**: 确保路径指向CubeMX项目的根目录，应该包含以下结构：
```
CubeMX项目/
├── Drivers/
│   ├── STM32F1xx_HAL_Driver/
│   └── CMSIS/
├── Inc/
├── Src/
└── ...
```

### Q3: 编译时找不到头文件

**A**: 
1. 检查HAL库文件是否已复制
   ```bash
   ls HAL/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h
   # 应该能看到文件
   ```

2. 检查Makefile中的路径配置
   ```bash
   # 查看Makefile中的HAL_DIR设置
   grep HAL_DIR Makefile
   ```

3. 重新运行集成脚本

### Q4: 编译错误 - 未定义的符号

**A**: 
1. 确保所有HAL库源文件都已复制
   ```bash
   ls HAL/STM32F1xx_HAL_Driver/Src/ | wc -l
   # 应该有很多.c文件
   ```

2. 运行 `make clean` 后重新编译

### Q5: macOS/Linux权限问题

**A**: 
```bash
# 确保脚本有执行权限
chmod +x scripts/integrate_hal.sh
chmod +x scripts/setup_project.sh
```

---

## 验证清单

集成完成后，检查以下项目：

- [ ] HAL目录中有文件（不是空的）
- [ ] `HAL/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h` 存在
- [ ] `HAL/CMSIS/Include/core_cm3.h` 存在
- [ ] 运行 `./scripts/setup_project.sh` 显示成功
- [ ] `config.h` 中正确配置了 `USE_HAL_DRIVER`（如果使用HAL库）
- [ ] 项目可以编译（如果已安装工具链）

---

## 下一步

集成完成后，参考以下文档：

- **BUILD_GUIDE.md** - 编译和测试指南
- **USAGE.md** - 使用说明
- **README.md** - 项目概述

