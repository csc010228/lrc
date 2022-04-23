/*
 *
 *  asm_generator_component.cpp
 *  汇编代码生成器组件
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#include "asm_generator_component.h"
#include "asm_generator.h"

/*
通知中介者有事件发生

Parameters
----------
event:发生的事件

Return
------
返回处理完事件之后返回的响应数据
*/
struct event Asm_generator_component::notify(struct event event)
{
    return mediator_->notify(this,event);
}

/*
设置中介者

Parameters
----------
mediator:中介者
*/
void Asm_generator_component::set_mediator(Asm_generator *mediator)
{
    mediator_=mediator;
}