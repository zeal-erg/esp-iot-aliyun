/**
 * @file iot_esp_list.h
 * @author zenggong@tuya.com 
 * @brief 
 * @version 0.1
 * @date 2020-04-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef __IOT_ESP_LIST_H__
#define __IOT_ESP_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copied from include/linux/...
 */

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) ); })

struct iot_esp_list_head {
    struct iot_esp_list_head *next, *prev;
};
#define WRITE_ONCE(var, val) \
(*((volatile typeof(val) *)(&(var))) = (val))

#define LIST_HEAD_INIT(name) \
    { &(name), &(name) }

#define LIST_HEAD(name) \
    struct iot_esp_list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct iot_esp_list_head *list) {
    WRITE_ONCE(list->next, list);
    list->prev = list;
}

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)                 \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head);                               \
         pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)          \
    for (pos = list_entry((head)->next, typeof(*pos), member),  \
        n = list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head);                                \
         pos = n, n = list_entry(n->member.next, typeof(*n), member))

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int iot_esp_list_empty(const struct iot_esp_list_head *head) {
    return head->next == head;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __iot_esp_list_add(struct iot_esp_list_head *_new,
                              struct iot_esp_list_head *prev,
                              struct iot_esp_list_head *next) {
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void iot_esp_list_add_tail(struct iot_esp_list_head *_new, struct iot_esp_list_head *head) {
    __iot_esp_list_add(_new, head->prev, head);
}

/**
 * list_add_head - add a new entry
 * @new: new entry to be added
 * @head: list head to add it behind
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void iot_esp_list_add_head(struct iot_esp_list_head *_new, struct iot_esp_list_head *head) {
    __iot_esp_list_add(_new, head, head->next);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __iot_esp_list_del(struct iot_esp_list_head *prev, struct iot_esp_list_head *next) {
    next->prev = prev;
    prev->next = next;
}

#define LIST_POISON1 ((void *)0x00100100)
#define LIST_POISON2 ((void *)0x00200200)
/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void iot_esp_list_del(struct iot_esp_list_head *entry) {
    __iot_esp_list_del(entry->prev, entry->next);
    entry->next = (struct iot_esp_list_head *)LIST_POISON1;
    entry->prev = (struct iot_esp_list_head *)LIST_POISON2;
}

#ifdef __cplusplus
}
#endif

#endif
