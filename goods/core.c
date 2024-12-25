
///*当 fscanf 成功读取并赋值给所有指定的参数时，它返回成功读取的项数。例如，如果格式字符串包含三个格式说明符，并且所有三个参数都成功读取并赋值，则返回值为3。*/
///*如果在读取任何数据之前遇到文件结束符（EOF），则返回 EOF，通常为 -1
//如果在读取过程中发生错误（例如，文件格式与格式字符串不匹配），也会返回 EOF*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_GOODS 1000

// 定义商品结构体                                    ------>商品结构体和树节点结构体的嵌套
typedef struct Product {
    char product_id[10];       // 商品ID
    char product_name[50];     // 商品名称
    char product_type[20];     // 商品类型
    int price;                 // 商品价格
    char description[200];     // 商品描述
} Product;
Product products[150];


// 定义平衡二叉树节点
typedef struct TreeNode {
    Product product;           // 商品数据
    struct TreeNode* left;     // 左子树
    struct TreeNode* right;    // 右子树
    int height;                // 节点高度
} TreeNode;




TreeNode* search_array[10];
int count_search_array;
char user_type[20];
char user_name[50];
TreeNode* priceTree;
TreeNode* nameTree;
TreeNode* typeTree;


// 获取节点高度
int height(TreeNode* node) {
    return node ? node->height : 0;
} 

// 更新节点高度
int Max(int a, int b){
    return a > b ? a : b;
} 

// 右旋操作
TreeNode* rightRotate(TreeNode* y) {
    TreeNode* x = y->left;
    TreeNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = Max(height(y->left), height(y->right)) + 1;
    x->height = Max(height(x->left), height(x->right)) + 1;

    return x;
}

// 左旋操作
TreeNode* leftRotate(TreeNode* x) {
    TreeNode* y = x->right;
    TreeNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = Max(height(x->left), height(x->right)) + 1;
    y->height = Max(height(y->left), height(y->right)) + 1;

    return y;
}

// 获取平衡因子
int getBalance(TreeNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

// 插入商品到平衡二叉树，按价格排序
TreeNode* insert_price(TreeNode* node, Product product) {
    if (!node) {
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->product = product;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }

    if (product.price < node->product.price)
        node->left = insert_price(node->left, product);
    else if (product.price > node->product.price)
        node->right = insert_price(node->right, product);
    else
        return node;

    node->height = Max(height(node->left), height(node->right)) + 1;

    int balance = getBalance(node);

    // 左左情况
    if (balance > 1 && product.price < node->left->product.price)
        return rightRotate(node);

    // 右右情况
    if (balance < -1 && product.price > node->right->product.price)
        return leftRotate(node);

    // 左右情况
    if (balance > 1 && product.price > node->left->product.price) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // 右左情况
    if (balance < -1 && product.price < node->right->product.price) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// 插入商品到平衡二叉树，按名称排序
TreeNode* insert_name(TreeNode* node, Product product) {
    if (!node) {
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->product = product;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }

    if (strcmp(product.product_name, node->product.product_name) < 0)//(product.price < node->product.price)
        node->left = insert_name(node->left, product);
    else if (strcmp(product.product_name, node->product.product_name) > 0)//(product.price > node->product.price)
        node->right = insert_name(node->right, product);
    else
        return node;

    node->height = Max(height(node->left), height(node->right)) + 1;

    int balance = getBalance(node);

    // 左左情况
    if (balance > 1 && strcmp(product.product_name, node->product.product_name) < 0)
        return rightRotate(node);

    // 右右情况
    if (balance < -1 && strcmp(product.product_name, node->product.product_name) > 0)
        return leftRotate(node);

    // 左右情况
    if (balance > 1 && strcmp(product.product_name, node->product.product_name) > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // 右左情况
    if (balance < -1 && strcmp(product.product_name, node->product.product_name) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// 中序遍历，打印商品信息
void inOrder(TreeNode* root) {
    if (root) {
        inOrder(root->left);
        printf("名称: %s,\t价格: %d\n",
            root->product.product_name,
            root->product.price);
        inOrder(root->right);
    }
}

#define Max_PRODUCTS 1000

// 从文件中读取商品数据
// 注意fscanf在循环中的读取方式，和返回值
// 注意含有空格的文件很难读，要有特殊的以换行为结束的字符串
int loadProducts() {
    FILE* file = fopen("F:/aligned_2.txt", "r");
    if (!file) {
        printf("无法打开文件。\n");
        return -1;
    }

    int count = 0;
    while (fscanf(file, "%s %s %s %d %[^\n]",
        products[count].product_id,
        products[count].product_name,
        products[count].product_type,
        &products[count].price,
        products[count].description) != EOF) {
        count++;
    }

    fclose(file);
    return count;

}

 //为了调节平衡因子找到最小值节点
TreeNode* minValueNode(TreeNode* node) {
    TreeNode* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// 删除二叉树节点
TreeNode* deleteNode(TreeNode* root, int price) {
    if (!root) return root;

    if (price < root->product.price)
        root->left = deleteNode(root->left, price);
    else if (price > root->product.price)
        root->right = deleteNode(root->right, price);
    else {
        if (!root->left || !root->right) {
            TreeNode* temp = root->left ? root->left : root->right;
            if (!temp) {
                temp = root;
                root = NULL;
            }
            else
                *root = *temp;
            free(temp);
        }
        else {
            TreeNode* temp = minValueNode(root->right);
            root->product = temp->product;
            root->right = deleteNode(root->right, temp->product.price);
        }
    }

    if (!root) return root;

    root->height = 1 + Max(height(root->left), height(root->right));
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}


//递归搜索的模块
TreeNode* find_with_name(TreeNode* cur) {
    if (cur == NULL) {
        return NULL;
    }
    if (strcmp(cur->product.product_name,user_name) == 0) {
        return cur;
    }//递归停止



    if (strcmp(cur->product.product_name,user_name) > 0) {
        return find_with_name(cur->left);
    }
    else {
        return find_with_name(cur->right);
    }
}

//遍历整棵树
void search_all_tree(TreeNode* cur) {
    if (cur == NULL) {
        return; 
    }
    search_all_tree(cur->left);

    if (  strcmp(cur->product.product_type,  user_type) == 0 ){
        search_array[count_search_array] = cur;
        count_search_array++;
        //printf("%d\n", count_search_array);
    }
    search_all_tree(cur->right);
}
/*
缓冲区大小检查：
scanf 的 %9s：仅限制了读取的字符数不超过9个，但 不会检查缓冲区 buffer 的大小。如果缓冲区 buffer 的大小小于10个字符（例如，只有5个字符的空间），仍然会导致缓冲区溢出。
scanf_s 的 %9s：不仅限制了读取的字符数不超过9个，还 要求提供缓冲区的大小参数，以确保不会超过缓冲区的大小。
*/


/*在C语言中，我们两个scanf之间可能有回车的残留*/
void find_with_key() {

    printf("\n查询名称（主键）：输入P\t,查询类型（非主键）：输入S\n");
    char key;
    //getchar();
    scanf_s(" %c", &key, sizeof(char));
    if (key == 'P') {
        //查询主键
        printf("type:");
        scanf(" %s", user_type);
        TreeNode* cur = priceTree;
        count_search_array = 0;
        search_all_tree(cur);
        printf("\t\t\t<一共查找到%d条结果>",count_search_array);
        for (int i = 0; i < count_search_array; i++) {
            printf("\n--------------------------------------------------------------------------begin");
            printf("\ninfomation:\n 编号\t\t%s\n 商品名\t\t\"%s\"\n 商品类型\t<%s>\n 价格\t\t$%d\n 商品描述\t%s\n", search_array[i]->product.product_id, search_array[i]->product.product_name, search_array[i]->product.product_type, search_array[i]->product.price, search_array[i]->product.description);
            printf("--------------------------------------------------------------------------end\n");
        }
    }
    else if (key == 'S') {
        //查询并排序附键
        TreeNode* cur = nameTree;
        printf("输入商品名称：");
        scanf(" %s",user_name);
        TreeNode* find = find_with_name(cur);
        if (find == NULL) {
            printf("查询失败不存在");
        }
        else {
            printf("--------------------------------------------------------------------------begin");
            printf("\ninfomation:\n 编号\t\t%s\n 商品名\t\t\"%s\"\n 商品类型\t<%s>\n 价格\t\t$%d\n 商品描述\t%s\n", find->product.product_id, find->product.product_name, find->product.product_type, find->product.price, find->product.description);
            printf("--------------------------------------------------------------------------end");
        }
        return;
    }
}



void Find_good_detail(TreeNode* ask) {

    printf("输入要查询的商品的名称(长度超过49自动截断):");
    scanf_s(" %49s",user_name, sizeof(user_name));
    
    //通过迭代查询
    TreeNode* find = find_with_name(ask);
    if (find == NULL) {
        printf("查询失败不存在");
    }
    else {
        printf("\n--------------------------------------------------------------------------begin");
        printf("\ninfomation:\n 编号\t\t%s\n 商品名\t\t\"%s\"\n 商品类型\t<%s>\n 价格\t\t$%d\n 商品描述\t%s\n", find->product.product_id, find->product.product_name, find->product.product_type, find->product.price, find->product.description);
        printf("--------------------------------------------------------------------------end\n");
    }
    return;
}



int main() {
    int product_count = loadProducts();
    if (product_count <= 0) {
        printf("未加载任何商品。\n");
        return -1;
    }

    priceTree = NULL;
    nameTree = NULL;
    typeTree = NULL;

    // 构建按价格排序的平衡二叉树
    for (int i = 0; i < product_count; i++) {
            priceTree = insert_price(priceTree, products[i]);
        
    }
    // 打印按价格排序的商品
    printf("按价格排序的商品信息：\n");
    inOrder(priceTree);



    // 构建按名称排序的平衡二叉树
    for (int i = 0; i < product_count; i++) {
            nameTree = insert_name(nameTree, products[i]);        
    }
    // 打印按价格排序的商品
    printf("按名称排序的商品信息：\n");
    inOrder(nameTree);





   // // 删除随机价格商品
   // printf("\n删除部分商品后：\n");
   // for (int i = 0; i < 5; i++) { // 删除5个商品示例
   //     int price = products[rand() % product_count].price;
   //     priceTree = deleteNode(priceTree, price);
   // }
   // inOrder(priceTree);



   //直接检索某个节点，目前只实现了里面的按名称查询的模块
    Find_good_detail(nameTree);


    //区分主键和附键进行查询，主键可以直接定位单个，附键会有查询结果的排序呈现
    if (priceTree == NULL) {
        printf("en");
    }

    find_with_key();


    return 0;
}