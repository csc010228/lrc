/*
 *
 *  pool.h
 *  全局公共子表达式删除
 *  Creator:Zhengzhou Tu
 *  Create time:2022/8/9
 *
*/
#ifndef __POOL_H
#define __POOL_H
#include"ic_optimizer.h"

// 池元素类型
enum pool_item_type{
    VAR_OR_TMP,
    ARRAY_MEMBER,
    EXPRESSION,
};

enum pool_item_relation{
    SAME,
    DIFFERENT,
    NOT_SURE,
};

// 池元素
typedef class pool_item{
    pool_item_type type;    // 类型
    ic_data *arg1, *arg2;   // 操作数
    pool_item *arg1_pi, *arg2_pi;   // 操作数的池元素指针

    ic_op op;           // 操作符, 即运算符
    class eq_class* my_class; // 所属等价类的指针
    class pool* related_pool; // 所属池的指针

public:

    pool_item(pool* related_pool, ic_data* arg);        // 为 变量 和 数组成员创建 池元素
    pool_item(pool* related_pool, ic_op op, ic_data* arg1, ic_data* arg2);  // 为表达式创建池元素

    void set_class(eq_class* eqc);   // 设置等价类
    void set_arg1_pi(pool_item* pi);
    void set_arg2_pi(pool_item* pi);

    bool is_var();  // 判断池元素是不是 变量、临时变量、数组变量或者常量
    bool is_am();   // 判断池元素是不是数组成员
    bool is_exp();  // 判断池元素是不是表达式

    eq_class* get_class();                  // 返回所属等价类指针
    pool_item_type get_type();              // 返回池元素的类型
    ic_op get_op();                         // 返回池元素的操作符
    ic_data* get_arg1();
    ic_data* get_arg2();
    pool_item* get_arg1_pi();
    pool_item* get_arg2_pi();

}pool_item;

// 等价类
typedef class eq_class{

    list< pool_item* > class_items;
    pool* related_pool;

public:
    eq_class(pool* p);  // 创建一个空的等价类

    bool if_pi_in_eqc(pool_item* pi);
    
    void add_class_item(pool_item* pi);               // 向等价类添加池元素
    void rm_class_item(pool_item* pi);                // 从等价类移除池元素
    int get_class_size();                             // 返回等价类里池元素的个数
    list< pool_item* > get_class_items();             // 返回等价类里的池元素 

    pool_item* get_cheapest_eq_pi();    // 在池里找到代价最小的等价池元素, 没有找到则返回 nullptr
    bool worth_keeping();               // 判断一个等价类是否值得保留

}eq_class;

// 池
typedef class pool{
    bool new_pool;                  // 标记这是一个新的池, 一个新池 与 任何池 P 相交, 结果都是 P

    set< pool_item * > pool_items;
    set< eq_class* > eq_classes;

    // 从 中间代码 到 pool_item 的映射, 数组元素和普通变量均只使用 arg1, 一元表达式使用 arg1, 二元表达式使用 arg1 和 arg2
    // 另外, 数组元素和普通变量的 ic_op 值均为 ic_op::NOP
    map< ic_data*, pool_item* > non_exp_to_pi;  // 非表达式到池元素的映射
    map< tuple<ic_op, ic_data*, ic_data*>, pool_item*> exp_to_pi;    // 表达式到池元素的映射

    map< tuple< ic_data*, eq_class*>, eq_class* > array_member_to_class;      // 数组成员到等价类的映射
    map< tuple< ic_op, eq_class*, eq_class*>, eq_class* > exp_to_class;       // 表达式到等价类的映射

    map< ic_data*, set<pool_item* >* > use_info;    // 使用信息

    ic_basic_block* related_blk;

public:
    pool(ic_basic_block* blk);      // 创建一个池, 池的关联基本块为传入的基本块
    pool(pool* p);                  // 复制一个池, 复制得到的池为旧池, 池的 关联基本块 为 传入池的关联基本块
    ~pool();                        // 销毁一个池
    void set_pool_old();            // 将 pool 的 new_pool 属性置为 false 
    bool is_pool_new();                  // 判断是否是新池(都没有进行过迭代)
    int get_eqc_count();            // 返回池内等价类的个数
    int get_pi_count();             // 返回池内池元素的个数
    bool if_pi_in_pool(pool_item* pi);
    bool if_eqc_in_pool(eq_class* eqc);
    

    void switch_class(pool_item* pi, eq_class* new_eqc);    // 将池元素切换到新的等价类

    pool_item* get_pi_of_non_exp(ic_data* arg); // 得到一个非表达式 ic_data* 对应的池元素
    pool_item* get_pi_of_exp(ic_op op, ic_data* arg1, ic_data* arg2);   // 得到一个表达式 对应的池元素

    eq_class* get_eqc_of_var(ic_data* arg);
    eq_class* get_eqc_of_am_by_ic_data(ic_data* arg);
    eq_class* get_eqc_of_am_by_offset_eqc(ic_data* array_var, eq_class* offset_eqc);
    eq_class* get_eqc_of_non_exp_by_ic_data(ic_data* arg); // 得到一个非表达式对应的等价类
    eq_class* get_eqc_of_exp_by_ic_data(ic_op op, ic_data* arg1, ic_data* arg2);   // 得到一个表达式对应的等价类
    eq_class* get_eqc_of_exp_by_ic_data_eqc(ic_op op, eq_class* arg1_eqc, eq_class* arg2_eqc);   // 得到一个表达式对应的等价类

    int add_use_info(ic_data*, pool_item*);   // 添加使用信息
    int add_use_info(pool_item*);          // 添加使用信息
    int add_use_info(pool_item*, pool_item*);  // 添加使用信息
    void add_pool_item(pool_item* pi, bool print);      // 向池中添加一个池元素. 池元素已经创建完毕, 添加到 池元素集合 和 对应等价类集合 即可

    void clear_map_to_pi(pool_item* pi);                // 去除 在 non_exp_to_pi 或 exp_to_pi 中的映射
    void rm_pi_from_use_info(ic_data*, pool_item* pi);       // 
    void rm_pi_from_use_info(pool_item* pi);            // 移除 pi 时, 将其从 使用了 pi 的 use_info 列表移除
    void rm_pi_from_use_info(pool_item*, pool_item*);
    void rm_pool_item(pool_item* pi, bool print);       // 从池中删去一个池元素. 将池元素从 池元素集合 和 对应等价类集合 移除

    eq_class* create_pool_items_by_ic_data( ic_data* arg );   // 为 ic_data 创建池元素, 并返回 ic_data 所在的等价类
    eq_class* create_pool_item_for_exp(ic_op, ic_data*, ic_data*);      // 为表达式创建一个池元素
    eq_class* create_pool_items_for_right( quaternion_with_info ic );    // 根据一个 ic, 为表达式创建池元素, 并返回表达式所在的等价类
    bool create_pool_item_for_result(eq_class*, ic_data*, bool language_data_type_same);  // 为 result 创建一个池元素

    void add_eq_class(eq_class* eqc);       // 向池中添加一个等价类
    void clear_map_to_eqc(eq_class* eqc);   // 从 array_member_to_class 和 exp_to_class 中删除 到 eqc 的映射
    void rm_eq_class(eq_class* eqc);        // 从池中移除一个等价类


    int rm_use_info(ic_data*, pool_item*);
    int rm_use_infos(ic_data*);                 // 移除一个 ic_data 的所有使用信息
    int add_use_info_by_exp_pi(pool_item* exp_pi);     // 根据 表达式池元素 添加使用信息

    set< pool_item* >* get_use_info(ic_data* arg);     // 获取使用链 或者 数组的使用信息

    void kill_uses_of_arg(ic_data* arg);              // 杀死使用某个 ic_data 的所有数组成员或者表达式
    
    pool_item_relation get_relation(ic_data* arg1, ic_data* arg2);  // 判断两个普通变量的关系

    void deal_call_op(quaternion_with_info ic);         // 处理函数调用指令的影响
    void deal_ret_op(quaternion_with_info ic);          // 处理返回指令的影响
    void deal_assign_op(quaternion_with_info ic); // 处理赋值指令的影响
    void deal_assign_to_array_member(ic_data* result);  // 处理对数组成员赋值的影响

    void single_interation( quaternion_with_info ic );         // 根据一个 iC, 进行单次迭代
    bool pool_same_judge( pool* p2 );       // 判断池与另一个池是否相等
    pool* interation();                     // 为一个基本块, 执行迭代, 并返回得到的出口池

    
    pool* normal_intersection( pool* p2);   // 不进行 数组成员 和 表达式 的扩展
    pool* pool_intersection( pool* p2);     // 执行池的交运算, 并返回交运算的结果

    bool exp_equal_judge(pool_item* exp1, pool_item* exp2);   // 表达式等价判断

    bool is_ic_data_in_pool(ic_data* arg);       // 某个 ic_data 是否在池里
    bool is_ic_data_equal_in_pool(ic_data* arg); // 某个 ic_data 是否在池中有等价存在

    bool is_exp_in_pool(ic_op op, ic_data*, ic_data*);              // 某个 表达式是否在池里
    bool is_exp_equal_in_pool(ic_op op, ic_data*, ic_data*);    // 某个表达式在池里是否有等价存在
    
   // void print_pool();                  // 打印池
    // void pool_item_nullptr_test();      // 检查池元素: 非 nullptr
    // void pool_item_duplicate_test();    // 检查池元素: 无 重复元素
    // void eqc_nullptr_test();            // 检查等价类: 无 重复等价类
    // void pi_shouheng_test();            // 池元素守恒: pool_items 集合元素个数 = 各 等价类 元素 的个数之和
    // void pool_test();                   // 池的检查, 包含上述四个检查
    void pool_simplify();               // 从池里去掉一些不需要的池元素
}pool;


// 返回操作符需要的操作数个数, 仅用于池运算
int operands_need(ic_op op);

// 判断是不是可交换的运算
bool commutative(ic_op op);

int eliminate_common_exp_in_basic_block(ic_basic_block* blk, pool* p_in);

// void print_pi(pool_item* pi);
// void print_eqc(eq_class* eqc);


#endif