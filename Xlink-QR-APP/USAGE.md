# 使用说明

## 快速开始

### 1. 环境准备

- **开发工具**：STM32CubeIDE 或 Keil MDK
- **编译器**：ARM GCC工具链
- **烧录工具**：ST-Link或J-Link
- **硬件**：STM32F108T6开发板

### 2. 项目配置

#### 2.1 配置Flash分区

根据实际Flash大小调整`flash_manager.h`中的分区配置：

```c
#define FLASH_SIZE               (64 * 1024)  // 根据实际Flash大小修改
#define BOOTLOADER_SIZE          (8 * 1024)   // Bootloader大小
#define PARTITION_SIZE           (28 * 1024)  // 每个应用分区大小
```

#### 2.2 配置网络接口

如果使用WiFi模块（如ESP8266），需要配置UART接口：

```c
// 在firmware_download.c中配置
#define WIFI_UART           USART2
#define WIFI_BAUDRATE       115200
```

#### 2.3 配置二维码扫描接口

配置用于接收二维码数据的UART：

```c
// 在qr_scanner.c中配置
#define QR_UART              USART1
#define QR_BAUDRATE          9600
```

### 3. 编译项目

#### 3.1 编译Bootloader

```bash
make bootloader
```

生成的文件：`build/bootloader.bin`

#### 3.2 编译应用程序

```bash
make application
```

生成的文件：`build/application.bin`

### 4. 烧录固件

#### 4.1 烧录Bootloader

使用ST-Link Utility或OpenOCD：

```bash
# 使用OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
        -c "program build/bootloader.bin verify reset exit 0x08000000"
```

#### 4.2 烧录应用程序

```bash
# 烧录到App A分区
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
        -c "program build/application.bin verify reset exit 0x08002000"
```

### 5. 使用OTA升级

#### 5.1 准备固件文件

1. 编译新版本的应用程序
2. 确保固件文件包含版本信息（在偏移0x200处）
3. 将固件文件上传到HTTP服务器

#### 5.2 生成二维码

使用在线工具或Python脚本生成包含固件URL的二维码：

```python
import qrcode

url = "http://example.com/firmware_v1.2.3.bin"
qr = qrcode.QRCode(version=1, box_size=10, border=5)
qr.add_data(url)
qr.make(fit=True)
img = qr.make_image(fill_color="black", back_color="white")
img.save("firmware_qr.png")
```

#### 5.3 执行升级

1. 触发OTA升级（通过按键或UART命令）
2. 扫描包含固件URL的二维码
3. 系统自动下载、验证并更新固件
4. 设备自动重启并加载新固件

## 固件版本格式

固件版本存储在固件文件的偏移0x200处，格式为：

```c
typedef struct {
    uint8_t major;      // 主版本号
    uint8_t minor;      // 次版本号
    uint8_t revision;   // 修订版本号
    uint8_t build;      // 构建号
} firmware_version_t;
```

示例：版本1.2.3.4
- major = 1
- minor = 2
- revision = 3
- build = 4

## 调试方法

### 1. UART调试输出

配置UART用于调试信息输出：

```c
// 在main.c中
void debug_printf(const char *fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    uart_send_string(UART_DEBUG, buffer);
}
```

### 2. 查看Flash内容

使用ST-Link Utility查看Flash内容，验证：
- 分区信息是否正确写入
- 版本号是否正确
- CRC32值是否正确

### 3. 测试Bootloader

1. 将BOOT0引脚拉高，复位设备
2. 设备应进入Bootloader模式
3. 通过UART发送测试命令

## 常见问题

### Q1: 升级后设备无法启动

**A**: 检查以下几点：
1. 新固件的向量表是否正确
2. 分区信息是否正确写入
3. CRC32校验是否通过
4. 如果新分区无效，Bootloader应自动回滚

### Q2: 二维码扫描失败

**A**: 检查：
1. UART配置是否正确
2. 二维码数据格式是否正确
3. URL格式是否有效（必须以http://或https://开头）

### Q3: 固件下载失败

**A**: 检查：
1. 网络连接是否正常
2. URL是否可访问
3. RAM缓冲区是否足够
4. 网络协议栈是否正确配置

### Q4: Flash写入失败

**A**: 检查：
1. Flash是否已解锁
2. 写入地址是否对齐（4字节对齐）
3. Flash是否已擦除
4. 写入数据是否超出分区大小

## 安全建议

1. **固件签名**：建议对固件进行数字签名，Bootloader验证签名后再启动
2. **加密传输**：使用HTTPS保护固件传输
3. **访问控制**：限制固件服务器的访问权限
4. **版本验证**：确保只允许升级到更高版本
5. **回滚保护**：防止恶意回滚到旧版本

## 性能优化

1. **压缩传输**：使用gzip压缩固件，减少下载时间
2. **差分升级**：只下载差异部分
3. **后台下载**：在后台下载，不影响正常功能
4. **断点续传**：支持下载中断后继续

