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
void list_insert_front (struct Node *item, struct Node **list)
{
    item->prev = NULL;
    item->next = *list;
    *list = item;
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
// 二级指针，解决初始值为NULL时无法添加的情况
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
int save_entry (struct Contacts *contacts)
{

}
int load_entry (struct Contacts *contacts)
{

}
void menu_info ()
{
    LOG("\n\n*********************************************\n");
    LOG("***** 1.添加人员\t\t2.打印所有人员\t\t3.删除人员 *****\n");
    LOG("***** 4.查找人员\t\t5.保存所有人员\t\t3.读取所有人员 *****\n");
    LOG("***** q to exit *****\n");
}
int main ()
{
    setbuf(stdout, NULL);
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
                break;
            case LOAD:
                break;
            default:
                goto exit;
        }
    }
exit:
    free(contacts);
    return 0;
}