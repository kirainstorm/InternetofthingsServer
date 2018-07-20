由于github默认不允许上传dll，所以下载下来的工程无法编译，请做如下操作：

1：安装包 EntityFramework，PagedList，在package manager console中输入

Install-Package EntityFramework
Install-Package PagedList

2：如果碰到错误：签名时加密失败 --“对程序集签名时出错
修改权限为完全控制
打开文件夹C:\Documents and Settings\All Users\Application Data\Microsoft\Crypto\RSA\MachineKeys右键属性-》安全-》everyone 修改权限。