# symbol_table.h

## struct ic_data

*   该结构体表示一个在中间代码中使用的数据，这个数据可以是纯常量，常变量，变量，数组变量，全局变量，函数形参，临时变量，数组取元素等等。

*   定义如下：

    ```c
    //符号表中的变量（广义的）
    //包括定义的变量和临时变量以及纯常数
    //包括基础变量和数组
    struct ic_data
    {
        //用户定义的局部变量的构造函数
        ic_data(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const):type(is_const?ic_data_type::LOCAL_CONST_VAR:ic_data_type::LOCAL_VAR),data_type(data_type),dimensions_len(dimensions_len),const_or_init_value(const_or_init_value),scope(nullptr)
        {
            size_t name_size=name.size();
            var_name=new char[name_size+1];
            strcpy((char *)var_name,name.c_str());
        };
    
        //函数形参的构造函数
        ic_data(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,bool is_const):type(is_const?ic_data_type::CONST_FUNC_F_PARAM:ic_data_type::FUNC_F_PARAM),data_type(data_type),dimensions_len(dimensions_len),scope(nullptr)
        {
            size_t name_size=name.size();
            var_name=new char[name_size+1];
            strcpy((char *)var_name,name.c_str());
        };
    
        //临时变量的构造函数
        ic_data(temporary_variable_index tmp_index,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const):type(is_const?ic_data_type::CONST_TMP_VAR:ic_data_type::TMP_VAR),tmp_index(tmp_index),data_type(data_type),dimensions_len(dimensions_len),const_or_init_value(const_or_init_value),scope(nullptr)
        {
            
        };
    
        //纯常量的构造函数
        ic_data(enum language_data_type data_type,OAA const_or_init_value):var_name(nullptr),type(ic_data_type::PURE_CONST),data_type(data_type),dimensions_len(nullptr),const_or_init_value(const_or_init_value),scope(nullptr)
        {
    
        };
    
        //数组元素的构造函数
        ic_data(struct ic_data * array_var,size_t offset_dimension_lens,struct ic_data * offset):type(ic_data_type::ARRAY_MEMBER),var_name(array_var->var_name),data_type(array_var->data_type),scope(nullptr)
        {
            size_t i=0;
            pair<struct ic_data *,struct ic_data * > * tmp=new pair<struct ic_data *,struct ic_data * >;
            list<struct ic_data * > * array_var_dimensions_len=array_var->dimensions_len;
            if(array_var_dimensions_len->size()>offset_dimension_lens)
            {
                dimensions_len=new list<struct ic_data * >;
                i=array_var_dimensions_len->size()-offset_dimension_lens;
                for(list<struct ic_data * >::reverse_iterator it=array_var_dimensions_len->rbegin();it!=array_var_dimensions_len->rend();it++)
                {
                    if(i--!=0)
                    {
                        dimensions_len->push_front(*it);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                dimensions_len=nullptr;
            }
    
            tmp->first=array_var;
            tmp->second=offset;
            const_or_init_value=OAA((void *)tmp);
        };
    
        //判断该ic_data是不是临时变量
        inline bool is_tmp_var() const
        {
            return (type==ic_data_type::TMP_VAR || type==ic_data_type::CONST_TMP_VAR);
        };
    
        //判断该ic_data是不是数组
        inline bool is_array_var() const
        {
            return dimensions_len!=nullptr;
        };
    
        //判断该ic_data是不是const的，包括const变量和纯const
        inline bool is_const() const
        {
            return (is_const_var() || is_pure_const());
        };
    
        //判断该ic_data是不是const变量
        inline bool is_const_var() const
        {
            return (type==ic_data_type::LOCAL_CONST_VAR || type==ic_data_type::GLOBAL_CONST_VAR || type==ic_data_type::CONST_TMP_VAR);
        };
    
        //判断该ic_data是不是纯const
        inline bool is_pure_const() const
        {
            return type==ic_data_type::PURE_CONST;
        };
    
        //判断该ic_data是不是数组元素
        inline bool is_array_member() const
        {
            return type==ic_data_type::ARRAY_MEMBER;
        };
    
        //判断该ic_data是不是函数形参
        inline bool is_f_param() const
        {
            return (type==ic_data_type::FUNC_F_PARAM || type==ic_data_type::CONST_FUNC_F_PARAM);
        };
    
        //检查该ic_data和另一个ic_data是不是有关联的
        //只有数组和数组取元素会和其他的变量有关联
        bool check_ic_data_related(struct ic_data * var)
        {
            if(is_array_member())
            {
                //对于数组取元素来说
                //先检查其所属的数组在定义的时候的偏移量中有没有var
                for(auto i:(*get_belong_array()->dimensions_len))
                {
                    if(i==var || (i->is_array_member() && i->check_ic_data_related(var)))
                    {
                        return true;
                    }
                }
                //再检查该数组取元素在取偏移地址的时候有没有var
                if(get_offset()==var || (get_offset()->is_array_member() && get_offset()->check_ic_data_related(var)))
                {
                    return true;
                }
                //最后检查其所属数组是不是var
                if(get_belong_array()==var)
                {
                    return true;
                }
            }
            if(is_array_var())
            {
                //对于数组来说，检测var是不是定义数组的时候的某一个维度的偏移量
                for(auto i:*dimensions_len)
                {
                    if(i==var)
                    {
                        return true;
                    }
                }
            }
            return false;
        };
    
        //获取变量的值
        inline OAA get_value() const
        {
            return const_or_init_value;
        };
    
        //获取变量的值
        inline void set_value(OAA new_value)
        {
            const_or_init_value=new_value;
        };
    
        //获取该变量的数据类型
        inline enum language_data_type get_data_type() const
        {
            return data_type;
        };
    
        //为一个数组取元素变量获取它所属的数组
        inline struct ic_data * get_belong_array() const
        {
            if(!is_array_member())
            {
                return nullptr;
            }
            return ((pair<struct ic_data *,struct ic_data * > *)const_or_init_value.pointer_data)->first;
        };
    
        //为一个数组取元素变量获取它的偏移量
        inline struct ic_data * get_offset() const
        {
            if(!is_array_member())
            {
                return nullptr;
            }
            return ((pair<struct ic_data *,struct ic_data * > *)const_or_init_value.pointer_data)->second;
        };
    
        //获取变量的名字
        inline string get_var_name() const
        {
            if(var_name)
            {
                return string(var_name);
            }
            return "";
        };
    
        //获取变量的作用域
        inline struct ic_scope * get_scope() const
        {
            return this->scope;
        };
    
        //判断该变量是不是全局变量
        inline bool is_global() const;
    
        //获取变量所占的空间(以byte为单位)
        inline size_t get_byte_size(bool is_r_param=false) const
        {
            size_t res=0,array_member_num=1;
            pair<struct ic_data *,list<struct ic_data * > * > * tmp;
            if(is_array_var())
            {
                if(is_f_param() || is_r_param)
                {
                    //如果这个数组变量是一个函数形参的话
                    //那么该数组所占空间的大小就是一个指针的大小
                    res=bits_of_data_types.at(language_data_type::POINTER)/8;
                }
                else
                {
                    //如果某一个数组是局部变量或者全局变量（总之不是函数形参）的话
                    //那么该数组所占的空间就是这个数组中所有的元素加起来的空间大小
                    for(auto i:(*dimensions_len))
                    {
                        array_member_num*=i->get_value().int_data;
                    }
                    res=array_member_num*(bits_of_data_types.at(data_type)/8);
                }
            }
            else if(is_array_member())
            {
                //对于数组取元素这个变量来说，需要判断取元素的时候的情况
                tmp=(pair<struct ic_data *,list<struct ic_data * > * > *)const_or_init_value.pointer_data;
                if(tmp->first->dimensions_len->size()==tmp->second->size())
                {
                    res=bits_of_data_types.at(data_type)/8;
                }
                else
                {
                    res=bits_of_data_types.at(language_data_type::POINTER)/8;
                }
            }
            else
            {
                res=bits_of_data_types.at(data_type)/8;
            }
            return res;
        };
    
        //获取变量所占据的内存空间（以4bytes为单位）
        inline size_t get_4_bytes_size(bool is_r_param=false) const
        {
            size_t res=0,byte_size=get_byte_size(is_r_param);
            while(res*4<byte_size)
            {
                res++;
            }
            return res;
        };
    
        //数据类型
        enum ic_data_type type;
        union
        {
            const char * var_name;                      //标识符名称字符串
            temporary_variable_index tmp_index;         //临时变量的编号
        };
        //标识符类型（或者是数组元素类型）
        enum language_data_type data_type;
        //这个成员只有当变量是数组的时候才有效，表示数组的各个维度的长度(如果某一个维度的长度为0，说明这个维度是不限长的),如果变量不是数组，那么这个成员就是nulptr
        //把地址（也就是指针）看成是一维数组
        list<struct ic_data * > * dimensions_len;
        //下面讨论这个const_or_init_value成员的具体含义：
        //if(is_const())
            //if(is_array_var())
                //那么如果该数组被初始化过，那么该成员就是一个vector<OAA> *，该数组存储着数组初始化的值；否则的话就是nullptr
            //else
                //该成员就是该变量的值
        //else if(is_global())
            //if(is_array_var())
                //那么如果该数组被初始化过，那么该成员就是一个vector<OAA> *，该数组存储着数组初始化的值；否则的话就是nullptr
            //else
                //该成员就是一个struct ic_data * ，如果它是nullptr，那么表示该变量没有初始化，否则的话这个指针指向的就是一个pure_const，就是该变量的初始值
        //else if(is_array_member())
            //该成员是一个pair<struct ic_data *,struct ic_data * > *，这个pair的first是该数组元素所属的数组，second是取数组元素的时候的偏移量(在中间代码的时候，把数组取元素全都变成一维的)
        //else
            //if(is_array_var())
                //如果该数组初始化过，那么该成员就是true；否则就是false
            //else
                //该成员是nullptr
        OAA const_or_init_value;
        //作用域
        struct ic_scope * scope;
    };
    ```

    *   type成员指出该变量的类型：

        ```c
        //struct var的类型
        enum class ic_data_type
        {
            LOCAL_VAR,                  //用户定义的局部变量
            LOCAL_CONST_VAR,            //用户定义的局部const变量
            GLOBAL_VAR,                 //用户定义的全局变量
            GLOBAL_CONST_VAR,           //用户定义的全局const变量
            TMP_VAR,                    //临时变量
            CONST_TMP_VAR,              //const临时变量
            PURE_CONST,                 //纯常量
            FUNC_F_PARAM,               //函数形参
            CONST_FUNC_F_PARAM,         //const函数形参
            ARRAY_MEMBER,               //数组成员
        };
        ```

    *   当变量是纯常量的时候，成语var_name和tmp_index都是无意义的；当变量是临时变量的时候，成员tmp_index指明了该临时变量的编号，var_name是无意义的；否则的话，var_name就指明了该变量的名字，tmp_index是无意义的

    *   data_type指明了该变量的数据类型，目前可用的取值只有`language_data_type::INT`和`language_data_type::FLOAT`

    *   dimensions_len成员只有当变量是数组的时候才有效，表示数组的各个维度的长度(如果某一个维度的长度为0，说明这个维度是不限长的),如果变量不是数组，那么这个成员就是nullptr

    *   const_or_init_value成员的含义比较长，但是在代码注释中已经写得较为清楚了

    *   scope是变量的作用域

## struct ic_label

*   该结构体表示中间代码的标签

*   定义如下：

    ```c
    //标签
    struct ic_label
    {
        ic_label(string label_name):name(label_name)
        {
    
        };
    
        string name;                    //标签名
    };
    ```

    *   只有一个name成员来表示该标签的名字

## struct ic_func

*   该结构体代表中间代码中的函数

*   定义如下：

    ```c
    //函数
    struct ic_func
    {
        ic_func(string name,enum language_data_type return_type,list<struct ic_data * > * f_params):name(name),return_type(return_type),f_params(f_params),scope(nullptr)
        {
            
        };
    
        ic_func(string name,enum language_data_type return_type,size_t f_params_num,...):name(name),return_type(return_type),scope(nullptr)
        {
            struct ic_data * f_param;
            f_params=new list<struct ic_data * >;
            va_list argptr;
    	    va_start(argptr,f_params_num);
            for(size_t i=0;i<f_params_num;i++)
            {
                f_param=va_arg(argptr,struct ic_data *);
                f_params->push_back(f_param);
            }
            va_end(argptr);
        };
    
        inline struct ic_data * get_f_param(string f_param_name) const
        {
            struct ic_data * res=nullptr;
            for(auto i:*f_params)
            {
                if(i->get_var_name()==f_param_name)
                {
                    res=i;
                    break;
                }
            }
            return res;
        };
    
        inline list<struct ic_data * > get_f_params() const
        {
            return (*f_params);
        }
    
        inline bool add_f_param(struct ic_data * f_param)
        {
            bool res=true;
            string f_param_name=f_param->get_var_name();
            if(get_f_param(f_param_name)==nullptr)
            {
                f_params->push_back(f_param);
            }
            else
            {
                res=false;
            }
            return res;
        };
    
        inline void set_scope(struct ic_scope * scope)
        {
            this->scope=scope;
        };
    
        inline list<struct ic_data * > get_local_vars() const;
    
        string name;                                        //标识符名称字符串
        enum language_data_type return_type;                //返回值类型
        list<struct ic_data * > * f_params;                 //函数形参
        struct ic_scope * scope;                            //函数作用域
    };
    ```

    *   name指明了函数名称
    *   return_type指明了函数返回值类型
    *   f_params指明了函数形参
    *   scope指明了该函数作用域

## class Symbol_table

*   该类是一个单例类，表示符号表，因为符号表全局只有一个

*   符号表里面存储了所有的函数，常量，以及数组取元素。其他的变量是存储在作用域中的

*   符号表在设计的时候，决定了同一个变量哪怕在代码中多次出现，也只会在内存中保留一份。例如：

    ```c
    int a;
    a=4;
    a=2;
    ```

    此时翻译成中间代码之后，第2行和第3行的a的struct ic_data *是一样的（指针数值一样，意味着地址相同）