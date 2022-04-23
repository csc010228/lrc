# 语义规则

## 相关文件

*   和语义规则相关的文件主要是semantic_rules.h和semantic_rules.cpp两个。
*   在semantic.cpp文件中需要写下各条语义规则的翻译动作，而在semantic_rules.h是所需要的API

## semantic.cpp文件中的语义规则的翻译动作的书写方式

*   现在假设你需要书写一个名为\_\_\_rule\_1\_\_\_的语义动作的翻译动作，那么只需要：

    *   在semantic.cpp文件的末尾的semantic_rule_registers这个map中添加相应数据：

        ```cpp
        //语义动作注册函数表
        map<string,semantic_rule_register> semantic_rule_registers={
            semantic_rule_registers_entry(___rule_1___),
        };
        ```

    *   在semantic.cpp文件的末尾的semantic_rules这个map中添加相应数据：

        ```cpp
        //语义动作注册函数表
        map<string,semantic_rule> semantic_rules={
            semantic_rules_entry(___rule_1___),
        };
        ```

    *   按照如下格式书写相应的翻译动作：

        ```cpp
        define_semantic_rule(___rule_1___)
            ...
        end_define_semantic_rule
        ```

    即可

## 书写注意事项：

* ### 对于标记性非终结符所带的语义动作，比如`A -> B C  ___rule_1___ D E  ___rule_2___`中的\_\_\_rule\_1\_\_\_:

  1. 对语义符号B、C、D设置属性，但是不能对语义符号A和E设置属性
  2. 可以读取B和C的属性，但是不能读取D、E的属性
  3. 如果文法中还有其他的产生式的格式是类似`F -> ...  ___rule_3___ A ...`的话，而且你可以保证产生式`A -> B C  ___rule_1___ D E  ___rule_2___`规约成A之后，这个A一定会是产生式`F -> ...  ___rule_3___ A ...`中的A，那么\_\_\_rule\_1\_\_\_就可以读取A的值；否则的话不行
* ### 对于普通的语义动作，比如`A -> B C  ___rule_1___`中的\_\_\_rule\_1\_\_\_:

  1. 可以设置A的属性
  2. 可以设置B和C的属性，但是它们的属性只会在本次语义动作中有效
  3. 如果文法中还有其他的产生式的格式是类似`D -> ...  ___rule_2___ A ...`的话，而且你可以保证产生式`A -> B C  ___rule_1___`规约成A之后，这个A一定会是产生式`D -> ...  ___rule_2___ A ...`中的A，那么\_\_\_rule\_1\_\_\_就可以读取A的值；否则的话不行