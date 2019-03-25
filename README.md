# WM Libraries
## 1、介绍
本软件包是Winner Micro W600系列嵌入式WiFi芯片软件支持包。通过这个软件包可以在RT-Thread嵌入式系统中很方便的使用W600系列芯片进行开发。

### 1.1 WM Libraries软件包目录结构如下所示：
``` 
wm_libraries
├───Demo                            // SDK原生的一些例子
├───Doc                             // 参考文档
├───Include                         // 头文件
├───Lib                             // Wlan库				
├───oneshot                         // oneshot配网库
├───Platform
|   ├───Boot                        // .S启动文件
|   ├───Common                      // SDK公共文件
|   ├───Drivers                     // 芯片驱动
|   ├───Inc                         // 平台相关头文件
│   └───Sys                         // 系统相关文件
├───rtthread                        // RT-Thread patch
├───Tools                           // img生成工具
├───README.md                       // 软件包使用说明
└───SConscript                      // RT-Thread 默认的构建脚本
```
### 1.2 依赖

- RT_Thread 3.0+

## 2、获取方式
使用 WM Libraries 软件包需要在 RT-Thread 的包管理中选中它，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers  --->
         [*] wm_libraries:a librariy package for WinnerMicro devices.
            Version (latest)  --->
```
配置完成后让 RT-Thread 的包管理器自动更新，或者使用 pkgs --update 命令更新包到 BSP 中。
## 3、注意事项
- 使用W601芯片时,请注意芯片Flash的大小，区分2M Flash和1M Flash.详细查看[WM_W60X_2M_Flash布局说明](/Doc/WM_W60X_2M_Flash布局说明_V1.0.pdf)、[WM_W60X_2M_Flash参数区使用说明](/Doc/WM_W60X_2M_Flash参数区使用说明_V1.0.pdf)、[WM_W60X_2M_Flash固件生成说明](WM_W60X_2M_Flash固件生成说明_V1.0.pdf).
## 4、更多资料
- [W600芯片](http://www.winnermicro.com/html/1/156/158/497.html)
- [W601芯片](http://www.winnermicro.com/html/1/156/158/531.html)
- [参考设计](http://www.winnermicro.com/html/1//162/163/index.html)
- [解决方案](http://www.winnermicro.com/html/1//162/164/index.html)
## 5、联系方式
- [WinnerMicro](http://www.winnermicro.com/)
- [info@winnermicro.com](mailto:info@winnermicro)

