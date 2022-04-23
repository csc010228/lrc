# LRC（Lion Rock Complier）

## 使用方法：
1. 进入该目录之后执行make命令，就会生成一个lrc可执行文件，执行它即可
2. 执行该lrc可执行文件的格式如下：
   
   ```lrc test.txt test```
   
   * 第一个参数`test.sy`是要编译的源文件
   * 第二个参数`test`是要输出的目标文件名
   * 执行完成之后，总共会生成四个文件：
     * 预处理之后的文件`test.pre`
     * 词法分析出来的token文件`test.token`
     * 中间代码文件`test.ic`
     * 汇编文件`test.s`

**注意**：  
目前该目录下的lrc可执行文件是在Mac OS下生成的，如果在Windows上需要重新进行make编译