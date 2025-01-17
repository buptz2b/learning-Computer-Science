﻿// 读取文档内容： 读取给定的 txt 文件，统计其中各个字符（包括英文字母、标点符号等）的频率。

// 构建哈夫曼树： 使用字符的频率构建哈夫曼树。每个叶子节点代表一个字符，节点的权重为该字符的频率。

// 生成哈夫曼编码： 根据哈夫曼树，生成每个字符的哈夫曼编码。

// 压缩文件： 使用哈夫曼编码将文本文件内容转化为二进制的编码。

// 计算压缩率： 计算压缩后的文件大小与原始文件大小的比例，得到压缩率。

// 解压缩文件： 利用哈夫曼树，将压缩文件还原为原始的文本文件。
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#define MAX_LEAF 50000
#define MAX_NODE (2 * MAX_LEAF - 1)
#define MAX_BIT 100
// 别加分号

typedef struct node
{
    int weight;
    int parent;
    int l_kid;
    int r_kid;
} Node;

Node huffman_tree[MAX_NODE + 10];           // 哈夫曼树,非叶子节点也是意义的，只是不会用到
// 我们一开始得到的只是叶子的权值
char huffman_codes[MAX_LEAF + 10][MAX_BIT]; // 每个字符的哈夫曼编码
int freq[MAX_LEAF + 10];                    // 字符频率数组
int code_length[MAX_LEAF + 10];             // 每个字符的哈夫曼编码长度，注意这里没有未出现的字符
wchar_t huf_index_map_char[MAX_LEAF];

int cur_count_num = 0; // 包括了非叶节点的实边界
int ture_leaf_num = 0; // 只有叶子的部分
void inihuffman()
{
    for (int i = 0; i < MAX_NODE; i++)
    {
        huffman_tree[i].weight = 0;
        huffman_tree[i].l_kid = -1;
        huffman_tree[i].r_kid = -1;
        huffman_tree[i].parent = -1;
    }
    printf("初始化完毕\n");
}

int all_ch_infile_num = 0;
void characterCount(FILE* input)
{
    wchar_t ch = 0;
    for (int i = 0; i <= MAX_LEAF; i++)
    {
        freq[i] = 0;
    }
    short bom;
    fread(&bom, sizeof(short), 1, input);

    while ((fread(&ch, sizeof(short), 1, input)) >= 1)
    {
        freq[ch]++;          // 增加对应字符的频率
        all_ch_infile_num++; // 统计所有的字符总数
        ch = 0;
    }
    printf("--------%d\n", all_ch_infile_num);
}

void build_huffman_tree()
{

    cur_count_num = 0;                 // 设置为0，后面会变
    for (int i = 0; i < MAX_LEAF; i++) // 统计字符频率，这个的范围比较大，拿到了所有可能的字符
    {
        if (freq[i] > 0)
        {
            huffman_tree[cur_count_num].weight = freq[i]; // 统计字符到分布式的数组里面
            huf_index_map_char[cur_count_num] = i;        // 记录这是什么字符
            cur_count_num++;                              // 大数组前几个都只压入了出现过的的叶子
        }
        // 哈哈，最后一个哈夫曼大数组的cur_count_num的位置没有东西,放第一个非叶子合成节点
    }

    setlocale(LC_ALL, "zh-CN");  // 设置中文环境
    // 在这里处理需要本地化的部分


    for (wchar_t i = 0; i <= MAX_LEAF; i++)
    {
        if (freq[i] != 0)
        {
            //wprintf(L"\'%lc\' 出现了 %d次\n", i,freq[i]);----------------------------------->！！！！！！！！！！！
            // cur_count_num++; // 统计出现个数
        }
    } // test

    setlocale(LC_ALL, "C");  // 恢复到默认的C环境（POSIX标准）
    // 在这里处理不需要本地化的部分

    ture_leaf_num = cur_count_num; // 这才是正宗的基于不为零的频率表而进行生成的过程

    for (int k = 1; k <= ture_leaf_num - 1; k++) // 必须进行 num_leaf -1 轮合并
    {
        // 找出权重最小的两个节点
        int min1 = -1, min2 = -1; // 用于存储最小和第2小滴节点的索引
        for (int i = 0; i < cur_count_num; i++)
        {
            if (huffman_tree[i].parent == -1)
            { // 找到父节点为 -1 的有效节点
                // 如果min1还没有值或当前节点的权重更小，更新min1和min2
                if (min1 == -1 || huffman_tree[i].weight < huffman_tree[min1].weight)
                {
                    min2 = min1; // 将min1的值赋给min2
                    min1 = i;    // 更新min1为当前节点
                }
                // 否则，如果min2没有值或当前节点的权重大于min1但小于min2，更新min2
                else if (min2 == -1 || huffman_tree[i].weight < huffman_tree[min2].weight)
                {
                    min2 = i; // 更新min2为当前节点
                }
            }
        }
        //printf("找到的最小：%d，权重：%d, 此小：%d,权重：%d\n", min1, huffman_tree[min1].weight, min2, huffman_tree[min2].weight);

        // 创建父节点,在我们之前统计的位置的后面继续塞入实际有意义的哈夫曼节点
        huffman_tree[cur_count_num].weight = huffman_tree[min1].weight + huffman_tree[min2].weight;
        huffman_tree[cur_count_num].parent = -1;
        huffman_tree[cur_count_num].l_kid = min1;
        huffman_tree[cur_count_num].r_kid = min2;
        // 更新父节点
        huffman_tree[min1].parent = cur_count_num;
        huffman_tree[min2].parent = cur_count_num;
        // 更新大数组的末尾
        cur_count_num++;
    }
    // 整个树建立成功
}

int temp_code[MAX_LEAF + 10][MAX_BIT];
void generate_huffman_codes()
{
    for (int i = 0; i < ture_leaf_num; i++)
    {
        code_length[i] = 0;
    }
    // char huffman_codes[MAX_LEAF + 10][MAX_BIT]; // 每个字符的哈夫曼编码
    // int code_length[MAX_LEAF + 10]; // 每个字符的哈夫曼编码长度，注意这里没有未出现的字符
    // 树已经建立好了，所以其实只是对cur_count_num才有建立的意义，其余的频率为0的节点我就没有去建立了

    for (int i = 0; i < cur_count_num; i++) // 麻烦所有的节点，无论叶子，只要在树上就要一视同仁
    {
        // printf("此时是i = %d节点\n", i);
        int temp_parent;
        int temp_kid = i;
        if (huffman_tree[i].parent != -1)
        {
            for (temp_parent = huffman_tree[i].parent; temp_parent != -1; temp_kid = temp_parent, temp_parent = huffman_tree[temp_parent].parent)
            {
                // printf("temp_parent = %d\n", temp_parent);
                if (temp_kid == huffman_tree[temp_parent].l_kid)
                {
                    temp_code[i][code_length[i]++] = 0;
                    // printf("是左子\n");
                }
                else if (temp_kid == huffman_tree[temp_parent].r_kid)
                {
                    temp_code[i][code_length[i]++] = 1;
                    // printf("是右子\n");
                }
            }
        }
        for (int j = 1; j <= code_length[i]; j++) // 有多少个编码
        {
            huffman_codes[i][code_length[i] - j] = '0' + temp_code[i][j - 1]; // 逆向结束, 用'0'偏移，形成字符
        }
        huffman_codes[i][code_length[i]] = '\0'; // C字符串结尾
    }

     for (int i = 0; i < ture_leaf_num; i++)
     {
         setlocale(LC_ALL, "zh-CN");
         wprintf(L"'%lc'", huf_index_map_char[i]);
         setlocale(LC_ALL, "C");
         printf("哈夫曼编码为%s\n", huffman_codes[i]);
     } // test
}

void compress_in_file(FILE* compress, FILE* input)
{
    // 进行文件的读入之前，必须把原来的文件表的pos改为开头0
    fseek(input, 0, SEEK_SET);

    wchar_t ch = 0;
    unsigned long bit_buffer = 0;
    int bit_count = 0; // 用于所有字符的二进制数据的缓冲区
    short bom;
    fread(&bom, sizeof(short), 1, input);
    while ((fread(&ch, sizeof(short), 1, input)) >= 1)
    {
        int ch_lenth = 0;
        int ch_index; // 这个不能做循环变量
        int ch_temp_index;
        for (ch_temp_index = 0; ch_temp_index < ture_leaf_num; ch_temp_index++)
        {
            if (huf_index_map_char[ch_temp_index] == ch) // 得知此时读入的字符，处于哈夫曼编码的哪个下标的位置
            {
                ch_lenth = code_length[ch_temp_index]; // 重点是每一种字符所在的哈夫曼大数组的index下标要找到
                ch_index = ch_temp_index;
                // printf("%c下标为%d:%s\n", ch, ch_temp_index, huffman_codes[ch_temp_index]);
            }
        } // 得到读入的某一个字符的长度，也可以知道编码

        // 看好了，告诉你如何把字符中的01，经过层层转化，一个一个当做二进制输入到缓冲区中!
        // printf("这里是不断的注入%c:\n", ch);
        // printf("开始%d个有效bit\n", bit_count);
        // printf(":%s\n", huffman_codes[ch_index]);
        for (int i = 0; i < ch_lenth; i++)
        {
            // 不断把哈夫曼表01字符串以某种形式，变成逐步变长的01流，看好了，，，，，，，，，，，，，，，，，，，，，，，，这里其实是和上面一样，默认假设哈夫曼编码不会太长
            // printf("bit_buffer  =  %u\n", (bit_buffer));
            // printf("(huffman_codes[%d][%d])  =   %u\n", ch_index, i, (huffman_codes[ch_index][i]));
            // printf("(huffman_codes[%d][%d] - '0')  =   %u\n", ch_index, i, (huffman_codes[ch_index][i] - '0'));
            bit_buffer = (bit_buffer << 1) | (huffman_codes[ch_index][i] - '0'); // 通过|和 <<移位，实现了二进制位串的改变
            bit_count++;
            if (bit_count == 8)
            {                                                   // 积累捕获到了8个位就直接把32位中的低8位写入
                fwrite(&bit_buffer, sizeof(char), 1, compress); // 这个从小端序机器写入底层二进制的方法，你以前估计搞不出
                // fputc(bit_buffer, compress);  //也是一种策略，但是会有自动截断低八位
                bit_buffer = 0;
                bit_count = 0;
            }
        }
        ch = 0;
    }
    // 你在缓冲区的使用中，前头刚刷新，又要从最低一位开始，可能有部分的数据没有凑齐8bits，没法顶到8bits的高位，失去连续，则人工补齐后面0000
    if (bit_count != 0)
    {
        bit_buffer = (bit_buffer << (8 - bit_count));
        fwrite(&bit_buffer, sizeof(char), 1, compress);
    }

    printf("----------------------------------------");
    // FILE *compressed_2 = fopen("D:/huffman_input.txt", "r");
    fseek(input, 0, SEEK_END);
    printf("\ninput->%ld bytes\n", ftell(input));
    fseek(compress, 0, SEEK_END);
    printf("compress->%ld bytes\n", ftell(compress));

    printf("\nso...压缩率为%.2lf\n", ftell(compress) * 1.0 / ftell(input));
    printf("----------------------------------------\n");
}

//  FILE *compressed_2 = fopen("D:/compressed.bin", "r");
//  FILE *input_2 = fopen("D:/huffman_input.txt", "r");
// 重新打开后进行统计大小,直接移动到文件末尾，用文件指针的位置来统计；
// 拜托别问.bin中，总bit可能不被8整➗，想多了，我们之前用缓冲区就是8个8个一存，并且计算机基本单位从来不是位，而是字节，字.bin中也是一样

/*"rb" 模式：无论文件扩展名是 .txt 还是 .bin，"rb" 模式的核心作用都是以 逐字节 的方式读取文件。（前提是utf-8编码）
在这个模式下，每次读取操作返回的都是一个 完整的字节（即 8 位），
没有任何字符转换或特殊处理。这意味着文件的每个字节（无论是文本还是二进制数据）都将按原样读取。
逐位读取：如果你想要逐位操作每个字节（即检查文件中的每个比特位），你需要读取字节并手动执行
位运算（比如使用 >> 和 &）来逐位处理。*/
//void test()
//
//{
//    setlocale(LC_ALL, "zh-CN");  // 设置中文环境
//    // 在这里处理需要本地化的部分
//
//
//    for (wchar_t i = 0; i < ture_leaf_num; i++)
//    {
//        setlocale(LC_ALL, "zh-CN");  // 设置中文环境
//        wprintf(L"%lc : ", huf_index_map_char[i]); 
//        setlocale(LC_ALL, "C");  // 恢复到默认的C环境（POSIX标准）
//
//        puts(huffman_codes[i]);
//    }    
//
//
//
//}
int get_root_index()
{
    int i;
    for (i = 0; huffman_tree[i].parent != -1; i = huffman_tree[i].parent)
    {
    }
    return i;
}
void decompress_to_file()
{
    /*从文件中读取压缩数据（按比特逐个处理）。
    根据哈夫曼树的结构，向下遍历树并解码比特流。
    每次到达叶子节点时，输出一个字符。
    重复这个过程直到文件内容完全解压。*/
    // 逐位操作必须依赖于缓冲区
    FILE* compressed = fopen("D:/compressed.bin", "rb");
    FILE* decompressed = fopen("D:/decompressed.txt", "w");
    unsigned bit_buffer = 0;
    int bit_count = 0;
    int root;
    root = get_root_index(); // 找到哈夫曼树的树根
    int cur_huffnode = root;
/*   fseek(compressed, 0, SEEK_END);
    printf("compressed->%ld bytes\n", ftell(compressed))*/;
    fseek(compressed, 0, SEEK_SET);

    int now_decode_num = 0;
    // 告诉你什么叫做遍历比特流，我们始终以每一个01为处理单位
    while (now_decode_num < all_ch_infile_num)
    {
        // buf的消耗补充，知道读取结束
        if (bit_count == 0)
        {
            // printf("here1\n");
            unsigned num = fread(&bit_buffer, sizeof(char), 1, compressed);
            bit_count = 8; // 有效位置满血
            if (num == 0)
            {
                break;
            } // 跳出
        }

        // 在缓冲区中捕获每一个有效的位，并从哈夫曼树的root逐步下移，到达叶子节点，则更新，并且写入，路径是实时更新的
        int now = (bit_buffer >> (bit_count - 1)) & 1; //&是对位的处理的必要步骤
        bit_count--;                                   // 有效的位的个数减少
        // printf("here %d\n", now);

        // 用拿到的位移动
        if (now == 1)
        {
             //printf("now == 1\n");
            cur_huffnode = huffman_tree[cur_huffnode].r_kid;
        }
        else if (now == 0)
        {
             //printf("now == 0\n");
            cur_huffnode = huffman_tree[cur_huffnode].l_kid;
        }

        // 检查移动结果
        if (huffman_tree[cur_huffnode].r_kid == -1 && huffman_tree[cur_huffnode].l_kid == -1)
        {
            wchar_t ch = huf_index_map_char[cur_huffnode];
             //printf("%c\n", ch);
            fwrite(&ch, sizeof(short), 1, decompressed);
            //fputc(ch, decompressed); // 写入新字符
            now_decode_num++;        // 目前还没有复刻完原文件中所有的字符
            //p/*rintf("\n复刻第%d个字符\n", now_decode_num);*/
            setlocale(LC_ALL, "zh-CN");
             //wprintf(L"写入第%d个字符%lc\n", now_decode_num, ch);------------------------------------->写入的检查单元测试
             setlocale(LC_ALL, "C");

            cur_huffnode = root; // 重新从头开始移动
        }
    }
}
int user_trace[MAX_LEAF];
wchar_t user_str[MAX_LEAF];  // 存储输入的宽字符字符串
wchar_t file_str[MAX_LEAF];  // 接受查询的字符串
void search_in_binfile()
{
    printf("请输入要搜索的中英文字符\n");

    wchar_t user = 0;
    int user_cnt = 0;
    setlocale(LC_ALL, "zh-CN");  // 设置中文环境
    // 在这里处理需要本地化的部分
    wscanf(L"%ls", user_str);  // 读取一个宽字符

    setlocale(LC_ALL, "C");  // 恢复到默认的C环境（POSIX标准）
    // 在这里处理不需要本地化的部分


    // 正式开始搜索user,user_index已有, 控制次数遍历01bits总数,制造模式串
    //for (int i = 0; i < MAX_NODE; i++)
    //{
    //    user_trace[i] = -1; // 用户的可能查询位置的标记
    //}
    // 用户的字符串查询位置的标记
   /* int user_trace;*/

    FILE* compressed = fopen("D:/compressed.bin", "rb");
    fseek(compressed, 0, SEEK_SET);
    unsigned int bit_buffer = 0;
    int bit_count = 0;
    int root;
    root = get_root_index(); // 找到哈夫曼树的树根
    int cur_huffnode = root;

    // 改改，基本和解压缩出来一样
    int now_decode_num = 0;
    //printf("--------------%d\n", all_ch_infile_num);
    printf("----------------------------------------");
    while (now_decode_num < all_ch_infile_num)
    {
        // buf的消耗补充，知道读取结束
        if (bit_count == 0)
        {
            // printf("here1\n");
            unsigned num = fread(&bit_buffer, sizeof(char), 1, compressed);
            bit_count = 8; // 有效位置满血
            if (num == 0)
            {
                break;
            } // 跳出
        }

        // 在缓冲区中捕获每一个有效的位，并从哈夫曼树的root逐步下移，到达叶子节点，则更新，并且写入，路径是实时更新的
        int now = (bit_buffer >> (bit_count - 1)) & 1; //&是对位的处理的必要步骤
        bit_count--;                                   // 有效的位的个数减少
        // printf("here %d\n", now);

        // 用拿到的位移动
        if (now == 1)
        {
             //printf("now == 1\n");
            cur_huffnode = huffman_tree[cur_huffnode].r_kid;
        }
        else if (now == 0)
        {
             //printf("now == 0\n");
            cur_huffnode = huffman_tree[cur_huffnode].l_kid;
        }

        // 检查移动结果,这里才要改一下
        if (huffman_tree[cur_huffnode].r_kid == -1 && huffman_tree[cur_huffnode].l_kid == -1)
        {
            // printf("here2\n");
            // printf("here2\n");
            setlocale(LC_ALL, "zh-CN");  // 设置中文环境
            // 在这里处理需要本地化的部分
            wchar_t ch = huf_index_map_char[cur_huffnode];
            //wprintf(L"%lc\n", ch);
            file_str[now_decode_num] = ch;
            setlocale(LC_ALL, "C");  // 恢复到默认的C环境（POSIX标准

            // 在这里处理不需要本地化的部分
            
            now_decode_num++; // 目前还没有复刻完原文件中所有的字符
            //printf("%d\n", now_decode_num);
            //printf("写入第%d个字符\n", now_decode_num);--------------------------->有用
            cur_huffnode = root; // 重新从头开始移动
        }
    }//此时已经完全解析完了file中的字符串，并转移动内存中

    setlocale(LC_ALL, "zh-CN");  // 设置中文环境
    file_str[now_decode_num] = L'\0';
    //wprintf(L"你输入的字符串是: %ls\n", user_str);-------------------------------->有用
    wprintf(L"\n\n从压缩文件提取出的字符串是: %ls\n", file_str);



    //开始正式的查询
    int user_str_len = 0;
    int file_str_len = now_decode_num;
    for (int i = 0; user_str[i] != 0; i++) {
        user_str_len++;
    }
    //设置查找变量
    int file_i = 0;
    int user_i = 0;
    int find_cnt = 0;
    
    while (file_i < file_str_len) {
        if (user_str[user_i] == file_str[file_i]) 
        {
            user_i++;
            file_i++;
            if (user_i == user_str_len) {
                user_i= 0;
                user_trace[find_cnt] = file_i - user_str_len;
                find_cnt++;         
            }
        }
        else if(user_str[user_i] != file_str[file_i])
        {
            file_i = file_i - (user_i - 1);
            user_i = 0;
        }
    }
    
    if (find_cnt > 0) {
        wprintf(L"\n一共已找到 %d 处，位置为",find_cnt);
        for (int i = 0; i < find_cnt; i++) {
            wprintf(L"%d  ", user_trace[i] + 1);
        }
        putwchar(L'\n');

    }
    else {
        wprintf(L"该文档未找到");
    }





    setlocale(LC_ALL, "C");  // 恢复到默认的C环境（POSIX标准）

}
int main()
{
    // 初始化基本的哈夫曼树
    inihuffman();

    // 打开文档,在不同的函数中，基本上都要反复操作文件，所以可以在局部函数中，创建一个行的文件描述符
    FILE* input = fopen("D:/huffman_input.txt", "r");
    FILE* compressed = fopen("D:/compressed.bin", "wb");
    FILE* decompressed = fopen("D:/decompressed.txt", "w");

    // 得到初始的叶子节点，统计字符频率
    characterCount(input);

    // 构建哈夫曼树
    build_huffman_tree();

    // 生成哈夫曼编码
    generate_huffman_codes();

    // 根据写出的哈夫曼编码，进行对文件的压缩，写入二进制文件,输出压缩率
    compress_in_file(compressed, input);

    // 根据文件进行解压操作
    decompress_to_file();

    // 搜索字符
    search_in_binfile();

    fclose(input);
    fclose(compressed);
    fclose(decompressed);
    return 0;
}
