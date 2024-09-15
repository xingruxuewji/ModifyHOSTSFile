# 写在前面

该项目是我写的用于可视化修改HOSTS的小工具，基于QT框架。

使用环境：Windows 11
<br>
各个功能已经经过自己电脑的验证

小工具首先通过QSettings库读取软件运行路径下的config.ini文件，读取要修改的HOSTS的路径，如果不存在会自动创建一个config.ini文件，如果使用请先修改config.ini文件。

路径填写对运行exe文件可以将HOSTS文件IP地址与域名映射以列表形式展示

> 注意：如果只想查看HOSTS内容可以直接双击exe，如果想要写入/修改等操作请使用管理员权限打开exe文件。

功能：
- 在上方输入框输入ip地址，域名，点击添加可以添加到HOSTS文件
- 当域名框有值时，会根据域名框内容与文件进行匹配，点击删除按钮可以删除对应条目
- 双击下方列表某一单元，进行修改，修改完按enter对内容进行修改


<br>
打开效果：
<br>

![展示图](https://raw.githubusercontent.com/xingruxuewji/ModifyHOSTSFile/main/images/1.png)




