//
// Created by Administrator on 2023/2/22.
//
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#define PERSON_NAME_LENGTH 16
#define PERSON_PHONE_LENGTH 11
#define LOG printf

struct Person
{
    char name[PERSON_NAME_LENGTH];
    char phone[PERSON_PHONE_LENGTH];
};

struct Node
{
    struct Node *prev;
    struct Node *next;
    struct Person *node;
};

struct Contacts
{
    struct Node *front;
    int size;
};
int compare_name (char *lhs, char *rhs)
{
    if (!strcmp(lhs, rhs))
        return 0;
    int i = 0;
    while (1)
    {
        if (lhs[i] < rhs[i])
            return -1;
        else if (lhs[i] > rhs[i])
            return 1;
        i++;
    }
}
void list_insert_front (struct Node *item, struct Node **list)
{
    if (*list == NULL)
    {
        *list = item;
        return;
    }
    char *name = item->node->name;
    struct Node *node = NULL;
    for (node = *list; node != NULL; node = node->next)
    {
        char *list_name = node->node->name;
        switch (compare_name(name, list_name))
        {
            case -1:
            case 0:
                item->next = node;
                item->prev = node->prev;
                if (node->prev)
                    node->prev->next = item;
                else
                    *list = item;
                node->prev = item;
                return;
            case 1:
                // 到了最后还是大， 直接插到最后
                if (node->next == NULL)
                {
                    node->next = item;
                    item->prev = node;
                    item->next = NULL;
                    return;
                }
                break;
        }
    }
}
void list_delete (struct Node *item, struct Node **list)
{
    if (item->prev != NULL)
        item->prev->next = item->next;
    if (item->next != NULL)
        item->next->prev = item->prev;

    if (item == *list)
        *list = item->next;

    item->prev = item->next = NULL;
    free(item->node);
    free(item);
}
// 二级指针，将front的地址传进去，解决初始值为NULL时无法添加的情况
int person_insert (struct Person *person, struct Node **list)
{
    if (list == NULL)
        return -1;

    struct Node *node = malloc(sizeof(struct Node));
    node->node = person;
    list_insert_front(node, list);

    return 0;
}
int person_delete (const char *name, struct Node **list)
{
    if (list == NULL)
        return -1;

    struct Node *node = NULL;
    for (node = *list; node != NULL; node = node->next)
    {
        if (!strcmp(name, node->node->name))
            break;
    }

    if (node != NULL)
    {
        list_delete(node, list);
        return 0;
    }
    return -1;
}
struct Person *person_search (const char *name, struct Node *list)
{
    struct Node *node = NULL;
    for (node = list; node != NULL; node = node->next)
    {
        if (!strcmp(name, node->node->name))
            return node->node;
    }

    return NULL;
}
int person_traverse (struct Node *list)
{
    struct Node *node = NULL;
    for (node = list; node != NULL; node = node->next)
        LOG("name: %s, phone: %s\n", node->node->name, node->node->phone);

    return 0;
}

enum Operator
{
    INSERT = 1,
    PRINT,
    DELETE,
    SEARCH,
    SAVE,
    LOAD
};

int insert_entry (struct Contacts *contacts)
{
    if (contacts == NULL)
        return -1;

    struct Person *person = malloc(sizeof(struct Person));
    LOG("请输入你的姓名：");
    scanf("%s", person->name);
    LOG("请输入你的手机号：");
    scanf("%s", person->phone);

    if (person_insert(person, &contacts->front) == 0)
    {
        LOG("添加成功\n");
        contacts->size++;
    }
    else
    {
        LOG("添加失败\n");
        free(person);
    }
}
int delete_entry (struct Contacts *contacts)
{
    if (contacts == NULL)
        return -1;

    if (contacts->size == 0)
    {
        LOG("暂无数据\n");
        return -1;
    }

    char name[PERSON_NAME_LENGTH];
    LOG("请输入要删除的姓名：");
    scanf("%s", name);

    if (person_delete(name, &contacts->front) == 0)
    {
        LOG("删除成功\n");
        contacts->size--;
    }
    else
        LOG("删除失败\n");
}
int print_entry (struct Contacts *contacts)
{
    if (contacts == NULL)
        return -1;

    person_traverse(contacts->front);
}
int search_entry (struct Contacts *contacts)
{
    if (contacts == NULL)
        return -1;

    char name[PERSON_NAME_LENGTH];
    LOG("请输入要删除的姓名：");
    scanf("%s", name);

    struct Person *person = person_search(name, contacts->front);
    if (person != NULL)
        LOG("name: %s, phone: %s\n", person->name, person->phone);
    else
        LOG("查无此人\n");
}
int save_entry (const char *filename, struct Contacts *contacts)
{
    if (contacts == NULL)
        return -1;

    if (contacts->front == NULL)
    {
        LOG("写入文件失败（没有数据）");
        return -1;
    }

    FILE *fd = fopen(filename, "wb");
    if (fd == NULL)
    {
        LOG("写入文件失败（打开文件失败）");
        return -1;
    }

    struct Node *node = NULL;
    int size = sizeof(struct Person);
    for (node = contacts->front; node != NULL; node = node->next)
        // 元素，元素大小，元素个数，文件指针
        fwrite(node->node, size, 1, fd);

    fclose(fd);
    return 0;
}
int load_entry (const char *filename, struct Contacts *contacts)
{
    if (contacts == NULL)
        return -1;

    FILE *fd = fopen(filename, "rb");
    if (fd == NULL)
    {
        LOG("读取文件失败（打开文件失败）");
        return -1;
    }

    struct Person person;
    // 先读一次，解决eof多读一次的问题
    fread(&person, sizeof(person), 1, fd);
    while (!feof(fd))
    {
        struct Person *p = malloc(sizeof(struct Person));
        memcpy(p->name, person.name, PERSON_NAME_LENGTH);
        memcpy(p->phone, person.phone, PERSON_PHONE_LENGTH);
        person_insert(p, &contacts->front);

        fread(&person, sizeof(person), 1, fd);
    }
    LOG("读取成功\n");
    return 0;
}
void menu_info ()
{
    LOG("\n\n*********************************************\n");
    LOG("***** 1.添加人员\t\t2.打印所有人员\t\t3.删除人员 *****\n");
    LOG("***** 4.查找人员\t\t5.保存所有人员\t\t6.读取所有人员 *****\n");
    LOG("***** q to exit *****\n");
}
int main ()
{
    setbuf(stdout, NULL);
    const char *filename = "person";
    struct Contacts *contacts = malloc(sizeof(struct Contacts));
    memset(contacts, 0, sizeof(struct Contacts));
    int select = 0;
    while (1)
    {
        menu_info();
        scanf("%d", &select);

        switch (select)
        {
            case INSERT:
                insert_entry(contacts);
                break;
            case PRINT:
                print_entry(contacts);
                break;
            case DELETE:
                delete_entry(contacts);
                break;
            case SEARCH:
                search_entry(contacts);
                break;
            case SAVE:
                save_entry(filename, contacts);
                break;
            case LOAD:
                load_entry(filename, contacts);
                break;
            default:
                goto exit;
        }
    }
exit:
    free(contacts);
    return 0;
}