# LRC（Lion Rock Complier）

## 使用方法：
1. 进入该目录之后执行`make`命令，就会生成一个lrc可执行文件，执行它即可
2. 执行该lrc可执行文件的格式如下：
   
   ```./lrc -S -o ./output/test.s ./input/test.sy -O2 -debug```
   
   * `test.sy`是要编译的源文件
   * `test.s`是要输出的目标文件名
   * `-S`和`-o`都是必选的，而且顺序不能乱
   * `-O2`是优化标志，是可选的，`-debug`是调试标志，也是可选的，这两个标志的顺序也不能乱
   * 执行完成之后，会生成arm汇编文件`test.s`