在接下來的範例將說明關於 Interrupt 以及 Plantform Driver 的開發範例，兩者在 Linux Driver 開發上都有著重要的步驟。

## Linux Interrupt
在 Linux kernel 中，中斷（Interrupt） 是一種由 硬體主動通知 CPU 的事件，
用來「打斷」目前的 CPU 執行流程，讓系統能立即處理緊急或重要的事件。

常見情境

* 網卡接收到封包（NIC interrupt）
* 按鍵被按下（GPIO interrupt）
* 磁碟傳輸完成（DMA interrupt）
* 計時器到期（Timer interrupt）

## Plantform Driver
在 Linux 中，「Platform Device」指的是一種不透過自動匯流排探測（如 PCI、USB）
。
* Platform Device

    在 Linux 裡，Platform Device 是一種專門用來描述 SoC（System on Chip）或嵌入式板子上固定存在的硬體裝置 的機制。
    這些裝置不像 PCI、USB 那樣能「自動偵測」，而是由系統手動登錄的裝置。

    例如：UART、SPI、I²C 控制器、GPIO、NAND、LED、Timer...

* Platform Driver

    接著去定義控制此 Platform Device 操作硬體的方法的 Platform Driver，並且皆由 Platform Bus 來進行匹配，兩者主要是透過「名字（name）」或者「Device Tree 的 compatible 屬性」來配對。


