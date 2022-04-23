# LRC（Lion Rock Complier）

## 使用方法：
1. 进入该目录之后执行`make`命令，就会生成一个lrc可执行文件，执行它即可
2. 执行该lrc可执行文件的格式如下：
   
   ```lrc test.sy test```
   
   * 第一个参数`test.sy`是要编译的源文件
   * 第二个参数`test`是要输出的目标文件名
   * 执行完成之后，会生成arm汇编文件`test.s`