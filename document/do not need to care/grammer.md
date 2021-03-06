# 语言文法

## 文法产生式书写规则

定义的语言的文法的产生式写在SysY.gra文件中，书写该文件的格式如下：  

* 第一行只需要写文法的起始语法变量，而且要保证该语法变量的后面一定要有至少一个空格
* 从第二行开始就开始写文法的产生式。每一行从左往右开始依次按照文法的产生式的三个部分（左部，`->`，右部）书写：
    * 左部放在该行的最前面，不能有任何空白符开头，左部的右边以一个，且只能是一个空格分割，在这个空格之后一定要写`->`
    * `->`之后开始写右部
    * 右部由语法符号（包括语法变量和非终结符）和语义动作组成：
        * 每一个语法符号的左边都需要有且只有一个空格进行分割
        * 每一个语义动作的左边都需要有且只能有两个空格进行分割（一般来说建议语义动作的左边和右边都有三个`_`）
    * 每一行的产生式的最后都需要至少有三个空格符，而这三个空格之后的内容就不会被算进产生式中了，这些内容就被认为是注释
* 不需要在该文件中声明任何终结符，程序会自动读取词法分析器中的token，把这些token作为文法的终结符
* 不需要在该文件中声明除了文法的起始符号之外的任何语法变量，只要是在产生式中出现的不是终结符的符号，都认为是语法变量
* 不需要在该文件中声明非标记性终结符，当你需要使用非标记性终结符的时候，你只需要在相应的位置直接写一个语义动作即可
* 文件中不能有任何的空行，特别注意文件的末尾