
### 中斷（Interrupt）

當硬體（或軟體）發生事件時，CPU 會「打斷」目前的程式流程，，轉去處理更緊急事件 的機制。會跳去執行一段「中斷服務程式（ISR）」的機制。

#### 中斷的流程
```
硬體事件（例如：裝置完成傳輸）
        ↓
CPU 停下目前工作
        ↓
呼叫中斷服務常式（ISR / top half）
        ↓
（可選）安排延遲執行（bottom half，如 tasklet 或 workqueue）
        ↓
CPU 回到原本的工作
```
#### 中斷的功能與特性
| 功能     | 說明                  |
| ------ | ------------------- |
| 即時響應   | 快速反應外部事件（如資料傳送完成）   |
| 提高效率   | CPU 不用忙等（busy wait） |
| 硬體驅動溝通 | 硬體透過中斷告訴系統「我準備好了」   |
| 減少延遲   | 取代輪詢（polling）方式     |

#### 驅動中的中斷範例
```javascript=
irqreturn_t my_isr(int irq, void *dev_id)
{
    printk(KERN_INFO "Interrupt received!\n");
    tasklet_schedule(&my_tasklet);  // 延遲處理
    return IRQ_HANDLED;
}
```



### 延遲執行（Delayed Execution）

延遲執行的意思是：

「把某些不需要立即做的工作，安排在稍後（或背景）再執行。」

它的目的是：

* 減少中斷處理時間（讓 ISR 越快越好）
* 分離「即時事件」與「後續處理」
* 提升系統整體效能與回應速度

常見用途:

在驅動程式中，延遲一段時間後再檢查硬體狀態。

延遲重新嘗試某個動作（例如：重試 I/O）。
延遲某些背景工作（例如清除 buffer、更新狀態）。


#### 延遲執行的幾種常見機制

| 機制                                | 執行上下文           | 可睡眠 | 適用情境         |
| --------------------------------- | --------------- | --- | ------------ |
| **Tasklet**                       | 軟中斷（Softirq）    | ❌   | 中斷下半部，快速處理   |
| **Workqueue**                     | Process context | ✅   | 可以睡眠或呼叫阻塞函式  |
| **Timer**                         | 軟中斷（Softirq）    | ❌   | 定時任務、timeout |
| **Delayed work**                  | Process context | ✅   | 延遲工作，可睡眠     |
| **msleep() / schedule_timeout()** | Process context | ✅   | 主動延遲執行       |


#### 兩者的關係：中斷 + 延遲執行

* 這兩者常常「搭配使用」！
* 常見模式：中斷上半部 + 延遲執行下半部
    | 部分                    | 名稱                  | 功能            | 特性         |
    | --------------------- | ------------------- | ------------- | ---------- |
    | **上半部 (Top Half)**    | ISR（中斷服務常式）         | 立即響應硬體事件、讀取狀態 | 快速、不能睡眠    |
    | **下半部 (Bottom Half)** | tasklet / workqueue | 延遲處理大量或耗時工作   | 可以延遲、部分可睡眠 |

#### 驅動中的延遲執行範例

1. 中斷 + tasklet
    ```javascript=
    #include <linux/module.h>
    #include <linux/interrupt.h>

    static struct tasklet_struct my_tasklet;

    void my_tasklet_func(unsigned long data)
    {
        printk(KERN_INFO "Tasklet: process deferred work\n");
    }

    irqreturn_t my_isr(int irq, void *dev_id)
    {
        printk(KERN_INFO "ISR: interrupt occurred\n");
        tasklet_schedule(&my_tasklet); // 延遲執行
        return IRQ_HANDLED;
    }

    static int __init my_init(void)
    {
        tasklet_init(&my_tasklet, my_tasklet_func, 0);
        request_irq(10, my_isr, IRQF_SHARED, "my_irq", &my_tasklet);
        return 0;
    }

    static void __exit my_exit(void)
    {
        tasklet_kill(&my_tasklet);
        free_irq(10, &my_tasklet);
    }

    module_init(my_init);
    module_exit(my_exit);
    MODULE_LICENSE("GPL");
    ```

    🧠 運作流程：

    * 硬體觸發中斷 IRQ 10
    * my_isr() 執行（上半部），排程 tasklet
    * 核心稍後執行 my_tasklet_func()（下半部）

2. Busy Wait
```javascript=
for( n1=0; n1<10; n1++ )
{
    printk(KERN_ALERT "n1=%d \r\n", n1 );  // 1Sec

    j = jiffies + 1 * HZ;
    while (jiffzies < j) ;   // BusyWait
}
```
* jiffies 是一個不斷增長的全域變數，代表系統開機以來的「時脈 tick 數」。
* Busy Wait會讓CPU一直持續在等待，使用msleep()或者schedule_timeout()比較好

3. schedule()
```javascript=
for( n1=0; n1<10; n1++ )
{
    printk(KERN_ALERT "n1=%d \r\n", n1 );

    j = jiffies + 1 * HZ;
    while (jiffies < j)
        schedule();  // OS Ready/Running/Wait&Block
}
```
* schedule()執行會讓出 CPU 的使用權，讓其他可執行的 process 有機會被排程執行。

4. schedule_timeout

```javascript=
for( n1=0; n1<10; n1++ )
{
    printk(KERN_ALERT "n1=%d \r\n", n1 );

    set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout( 1*HZ );
}
```
* schedule_timeout() 會自動計算超時，時間到即喚醒，不需要其他事件。
* 1*HZ: 一秒後自動喚醒

4. 延遲工作隊列（Workqueue）
```javascript=
DECLARE_WAIT_QUEUE_HEAD(joey_wait_queue_head);
for( n1=0; n1<10; n1++ )
{
    printk(KERN_ALERT "n1=%d \r\n", n1 );
    wait_event_timeout( joey_wait_queue_head , 0, 1 * HZ );

}
```
* 使用queue進行Delay，用來管理「哪些 process 正在等待」
* 1 秒後自動被 scheduler 喚醒，程式繼續執行。

CPU 釋放：在這 1 秒睡眠期間，CPU 可以去執行其他 process，不會 busy wait。

4. mdelay（短暫延遲）

```javascript=
for( n1=0; n1<10; n1++ )
{
    printk(KERN_ALERT "n1=%d \r\n", n1 );
    mdelay( 1000 );
}// 1 秒後執行
```
* mdelay() 會完全占用 CPU，在延遲期間不會切換到其他 process，不能在長時間延遲使用。
* 適用於短暫延遲（通常在微秒到幾毫秒範圍）

---
### Tasklet

Tasklet 是 Linux Kernel 的 底半部 (Bottom Half) 機制之一，用來延後執行中斷處理的工作，減少中斷上下文的執行時間。

特點：

* 運行在 軟體中斷 (soft interrupt) 上下文。
* 可以被中斷觸發，但比硬體中斷 (Top Half) 執行慢。
    * 等硬體中斷結束後就會被呼叫
* 不能睡眠（sleep）。
* 適合做「簡單、快速」的延遲處理，例如：
    * 累計資料
    * 發送事件給使用者空間
    * 設定旗標、喚醒等待隊列


#### Tasklet 使用範例

假設我們要在中斷服務例程 (ISR) 中延後處理：
```javascript=
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static struct tasklet_struct my_tasklet;

void my_tasklet_func(unsigned long data)
{
    printk(KERN_ALERT "Tasklet running! data=%ld\n", data);
}

irqreturn_t my_ISR(int irq, void *dev_id)
{
    printk(KERN_ALERT "ISR triggered\n");
    tasklet_schedule(&my_tasklet); // 延後處理
    return IRQ_HANDLED;
}

static int __init my_module_init(void)
{
    printk(KERN_ALERT "Init module\n");
    tasklet_init(&my_tasklet, my_tasklet_func, 123);
    // 這裡通常會 request_irq()
    return 0;
}

static void __exit my_module_exit(void)
{
    tasklet_kill(&my_tasklet);
    printk(KERN_ALERT "Exit module\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
```

執行順序：
1. 中斷發生 → my_ISR 立即執行。
2. my_ISR 呼叫 tasklet_schedule() → Tasklet 加入隊列。
3. 中斷完成 → 軟中斷上下文執行 Tasklet。
4. Tasklet 執行 my_tasklet_func()。

### 中斷處理

接下來會詳細介紹中斷處理如何進行的，中斷是一種 **異步事件通知機制**。
當外部裝置（例如鍵盤、網卡、UART、GPIO）發生事件時，會透過 IRQ 線
 (Interrupt ReQuest) 向 CPU 發送訊號。
CPU 收到後會：
* 暫停目前正在執行的工作；
* 保存現場 (context)；
* 轉去執行一段特定的程式：中斷服務例程 (ISR, Interrupt Service Routine)；
* 處理完畢後回到原本的程式繼續執行。

中斷處理的一個主要問題就是如何處理長時間的任務，中斷處理需要很快速地完成，且不能讓中斷阻塞太久。

Linux 為了解決這個問題把中斷處理分為兩個階段:
| 階段      | 名稱                  | 執行位置      | 特性       | 範例            |
| ------- | ------------------- | --------- | -------- | ------------- |
| **上半部** | ISR (Top Half)      | 中斷發生時立即執行 | 速度快、不能睡眠 | 讀取硬體暫存器、清中斷旗標 |
| **下半部** | Tasklet / Workqueue | 稍後再執行     | 可處理較長任務  | 資料處理、通知使用者空間  |


如果 ISR 處理花太多時間，系統即會發生：

* CPU 佔用太久；
* 其他中斷被延遲；
* 整體延遲上升。

👉 因此：

上半部只做必要的工作（如：清除硬體旗標）；

下半部才做耗時的動作（如：拷貝資料、通知應用程式）。
  * Tasklet / Workqueue為先前所介紹過的延遲執行功能

#### ISR (上半部) 的特性
```javascript
irqreturn_t my_isr(int irq, void *dev_id);
```
特性：

* 不可睡眠（不能呼叫可能阻塞的函式，例如 copy_to_user() 或 msleep()）
* 執行時間要短
* 通常會排程一個下半部

#### 註冊與釋放中斷
註冊中斷
```javascript
int request_irq(unsigned int irq, irq_handler_t handler,
                unsigned long flags, const char *name, void *dev);
```
| 參數        | 說明                              |
| --------- | ------------------------------- |
| `irq`     | 中斷號碼 (由硬體定義，如 GPIO IRQ、PCI IRQ) |
| `handler` | 安裝的處理 IRQ 函數指標                     |
| `flags`   | 例如 `IRQF_SHARED` (可共用 IRQ)      |
| `name`    | 出現在 `/proc/interrupts`          |
| `dev`     | 用來識別中斷來源的指標 (通常是裝置結構體)          |
* 當中斷發生時會將 (void*)dev 指標傳給 IRQ 處理函數，通常是一個結構。
* 報告的中斷顯示在 `/proc/interrupts`

釋放中斷
```javascript
void free_irq(unsigned int irq, void *dev);
```

常見中斷旗標 (flags)
| 旗標                     | 意義                     |
| ---------------------- | ---------------------- |
| `IRQF_SHARED`          | 可共用同一 IRQ (常見於 PCI 裝置) |
| `IRQF_TRIGGER_RISING`  | 上升沿觸發                  |
| `IRQF_TRIGGER_FALLING` | 下降沿觸發                  |
| `IRQF_TRIGGER_HIGH`    | 高電位觸發                  |
| `IRQF_TRIGGER_LOW`     | 低電位觸發                  |

* 若是 GPIO 中斷，需搭配 gpio_to_irq(pin) 轉換。

#### 實際 IRQ 流程圖
```
硬體中斷發生
     │
     ▼
  CPU 暫停現有程序
     │
     ▼
  進入 ISR (Top Half)
     │
     ├─ 清除硬體旗標
     ├─ 排程 tasklet/workqueue
     ▼
  結束 ISR → 回到原程式
     │
     ▼
 tasklet/workqueue 執行 (Bottom Half)

```

#### 以 GPIO 為例所建立的範例
```javascript =
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joey");
MODULE_DESCRIPTION("GPIO IRQ Example");

#define GPIO_NUM 17  // 假設使用 GPIO17 (依實際板子修改)

static int irq_num;
static struct tasklet_struct my_tasklet;

// Tasklet 底半部
static void my_tasklet_func(unsigned long data)
{
    printk(KERN_INFO "Tasklet: 處理 GPIO IRQ 底半部, message=%s\n", (char *)data);
}

// ISR 上半部
static irqreturn_t my_gpio_isr(int irq, void *dev_id)
{
    printk(KERN_INFO "ISR: GPIO 中斷觸發! IRQ=%d\n", irq);
    tasklet_schedule(&my_tasklet); // 排程底半部
    return IRQ_HANDLED;
}

static int __init my_gpio_irq_init(void)
{
    int ret;
    char *msg = kmalloc(32, GFP_KERNEL);
    strcpy(msg, "Hello from GPIO IRQ");

    // 1. 申請 GPIO
    ret = gpio_request(GPIO_NUM, "my_gpio_irq");
    if (ret) {
        printk(KERN_ERR "gpio_request 失敗\n");
        return ret;
    }

    // 2. 設為輸入模式
    gpio_direction_input(GPIO_NUM);

    // 3. 轉換成 IRQ 號碼
    irq_num = gpio_to_irq(GPIO_NUM);
    if (irq_num < 0) {
        printk(KERN_ERR "gpio_to_irq 失敗\n");
        gpio_free(GPIO_NUM);
        return irq_num;
    }

    // 4. 初始化 Tasklet
    tasklet_init(&my_tasklet, my_tasklet_func, (unsigned long)msg);

    // 5. 註冊中斷 (上升沿觸發)
    ret = request_irq(irq_num, my_gpio_isr, IRQF_TRIGGER_RISING, "gpio_irq_handler", NULL);
    if (ret) {
        printk(KERN_ERR "request_irq 失敗\n");
        gpio_free(GPIO_NUM);
        kfree(msg);
        return ret;
    }

    printk(KERN_INFO "GPIO IRQ 模組載入成功: GPIO=%d, IRQ=%d\n", GPIO_NUM, irq_num);
    return 0;
}

static void __exit my_gpio_irq_exit(void)
{
    free_irq(irq_num, NULL);
    tasklet_kill(&my_tasklet);
    gpio_free(GPIO_NUM);
    printk(KERN_INFO "GPIO IRQ 模組卸載完成\n");
}

module_init(my_gpio_irq_init);
module_exit(my_gpio_irq_exit);
```
