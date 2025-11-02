## Plantform Devices

### Platform Device 是什麼？

在 Linux 裡，Platform Device 是一種專門用來描述 SoC（System on Chip）或嵌入式板子上固定存在的硬體裝置 的機制。
這些裝置不像 PCI、USB 那樣能「自動偵測」，
例如：UART、SPI、I²C 控制器、GPIO、NAND、LED、Timer...

👉 Platform Device 就是用來表示「這塊板上有哪些固定硬體」的資料結構。

#### Platform Device / Driver 架構概念

Linux 採用「裝置–驅動」模型：
* Platform Device：描述硬體（誰存在）
* Platform Driver：定義操作硬體的方法（怎麼用）

其中 kernel 是使用 Platform Bus（平台匯流排）來人工登錄與匹配並連結 Platform Device 與 Platform Driver，兩者主要是透過「名字（name）」或者「Device Tree 的 compatible 屬性」來配對。

當 platform_device_register() 和 platform_driver_register() 都被呼叫後，kernel 的 platform bus 會自動比對名稱，找到相同的就呼叫該 driver 的 probe()。

當配對成功時，kernel 會呼叫 driver 的 probe() 進行初始化。

#### Platform Device 結構

```javascript=
#include <linux/platform_device.h>

struct platform_device {
    const char *name;                // 用來和 driver 匹配
    int id;                          // 若有多個相同裝置，用 id 區分
    struct device dev;               // 通用 device 結構（在 sysfs 中代表這個裝置）
    u32 num_resources;               // 資源數量
    struct resource *resource;       // 設備用的硬體資源
};
```

常見資源類型
```javascript=
struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;  // IORESOURCE_MEM, IORESOURCE_IRQ ...
};
```

#### Platform Driver 結構
```javascript=
struct platform_driver {
    int (*probe)(struct platform_device *pdev);
    int (*remove)(struct platform_device *pdev);
    struct device_driver driver;     // 包含名字、匹配表等資訊
};
```

### Platform Driver 註冊流程

1. 註冊 platform device
    * kernel（或 board code / Device Tree）宣告有這個硬體
2. 註冊 platform driver
    * 驅動程式寫好後呼叫 platform_driver_register()
3. kernel 比對 name / compatible
    * 若相符 → 呼叫 driver 的 probe()
    * 若移除 → 呼叫 driver 的 remove()

#### 手動建立 Platform Device 範例（C code 版本）

舊式嵌入式板子常用這種「靜態註冊」方式（不用 Device Tree）
* Device = 
```javascript=
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>

static struct resource joey_resources[] = {
    [0] = {
        .start = 0x10000000,
        .end   = 0x10000FFF,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = 42,
        .end   = 42,
        .flags = IORESOURCE_IRQ,
    },
};

static struct platform_device joey_device = {
    .name = "joey_platform",
    .id = -1,
    .num_resources = ARRAY_SIZE(joey_resources),
    .resource = joey_resources,
};
```

* Driver =

```javascript =
static int joey_probe(struct platform_device *pdev)
{
    struct resource *res;
    void __iomem *base;

    printk(KERN_INFO "joey_platform: probe()\n");

    // 取得記憶體資源
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    base = ioremap(res->start, resource_size(res));
    if (!base)
        return -ENOMEM;

    printk(KERN_INFO "Mapped I/O base = %p\n", base);
    iounmap(base);
    return 0;
}

static int joey_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "joey_platform: remove()\n");
    return 0;
}

static struct platform_driver joey_driver = {
    .probe  = joey_probe,
    .remove = joey_remove,
    .driver = {
        .name = "joey_platform",
        .owner = THIS_MODULE,
    },
};

static int __init joey_init(void)
{
    platform_device_register(&joey_device);
    platform_driver_register(&joey_driver);
    return 0;
}

static void __exit joey_exit(void)
{
    platform_driver_unregister(&joey_driver);
    platform_device_unregister(&joey_device);
}

module_init(joey_init);
module_exit(joey_exit);
MODULE_LICENSE("GPL");
```

### Device Tree 版本（現代用法）

在 ARM、RISC-V 平台上，platform device 通常由 Device Tree (.dts) 自動建立：
```javascript = 
Device Tree 片段：
joey_device@10000000 {
    compatible = "joey,mydevice";
    reg = <0x10000000 0x1000>;
    interrupts = <42>;
    status = "okay";
};

Driver 端：
static const struct of_device_id joey_of_match[] = {
    { .compatible = "joey,mydevice" },
    {},
};
MODULE_DEVICE_TABLE(of, joey_of_match);

static struct platform_driver joey_driver = {
    .probe  = joey_probe,
    .remove = joey_remove,
    .driver = {
        .name = "joey_driver",
        .of_match_table = joey_of_match,
    },
};
```

這樣 kernel 啟動時會：

1. 解析 Device Tree → 產生 platform_device
2. 尋找 matching driver（compatible 一致）
3. 呼叫 probe() 進行初始化

### 總結重點

與其他裝置模型相比常使用於內建裝置

| 類型           | 自動偵測 | 典型用途     | 範例              |
| ------------ | ---- | -------- | --------------- |
| **PCI**      | ✅    | 外掛式裝置    | 網卡、顯卡           |
| **USB**      | ✅    | 可插拔      | USB 隨身碟         |
| **Platform** | ❌    | SoC 內建裝置 | UART、GPIO、Timer |
| **Misc**     | ❌    | 簡單軟體裝置   | watchdog、uinput |


Platform Device 機制被廣泛應用在：
* SoC 內部控制器（I2C, SPI, UART, GPIO）
* 驅動字元裝置（char device）
* Power management（suspend/resume callbacks）
* Device Tree 硬體抽象層

總結:

| 重點              | 說明                                                          |
| --------------- | ----------------------------------------------------------- |
| Platform Device | 描述「板上固定硬體」                                                  |
| Platform Driver | 控制硬體的程式邏輯                                                   |
| 配對方式            | name 或 Device Tree 的 compatible                             |
| 註冊函式            | `platform_device_register()` / `platform_driver_register()` |
| 典型操作            | `ioremap()`、`platform_get_resource()`、`devm_*()` 系列         |

