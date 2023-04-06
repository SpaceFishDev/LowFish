#ifndef SPEED_TYPES
#define SPEED_TYPES
extern "C"{
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>

    typedef struct list_node
    {
        void* data;
        struct list_node* next;
    } list_node;

    typedef struct
    {
        list_node* head;
        list_node* tail;
        size_t length;
    } linked_list;

    list_node* initialize_node(void* data)
    {
        list_node* n = (list_node*)malloc(sizeof(list_node));
        n->data = data;
        n->next = (void*)0;
        return n;
    }

    void append_node(linked_list* list, list_node* data)
    {
        ++list->length;
        if(list->head == (void*)0)
        {
            list->head = data;
            list->tail = data;
            return;
        }
        list->tail->next = data;
        list->tail = data;
    }
    list_node* linked_list_at(size_t index, linked_list* list)
    {
        if(index > list->length)
        {  
            printf("Index higher than length of list\n");
            exit(-1);
        }
        list_node* n = list->head;
        for(int i = 0; i != index; ++i)
        {
            n = n->next;
        }
        return n;
    }
    void remove_node(linked_list* list, size_t index)
    {
        if(index < list->length)
        {
            int before = index - 1;
            int after = index + 1;
            list_node* node = linked_list_at(before, list);
            list_node* node2 = linked_list_at(after, list);
            node->next = node2; 
            --list->length;
            return;
        }
        if(index == 0)
        {
            list->head = list->head->next;
            --list->length;
            return;
        }
        printf("Index %zu, is out of bounds.", index);
    }
    void insert_node(linked_list* list, list_node* element, size_t index)
    {
        if(index < list->length)
        {
            int before = index - 1;
            int after = index;
            list_node* node = linked_list_at(before, list);
            list_node* node2 = linked_list_at(after, list);
            element->next = node2;
            node->next = element;
            ++list->length;
            return;
        }
        if(index == 0)
        {
            list_node* head = list->head;
            list->head = element;
            element->next = head;
            ++list->length;
            return;
        }
        printf("Index %zu out of bounds." , index);
        exit(-1);
    }

    typedef struct
    {
        void* value;
        char* key;
    } hash_map_element;

    typedef struct
    {
        size_t* element_indices;
        linked_list elements;
        size_t count;
        size_t element_max;
    } hash_map;

    hash_map_element* init_element(void* value, char* key)
    {
        hash_map_element* h = (hash_map_element*)malloc(sizeof(hash_map_element));
        *h = (hash_map_element){value, key};
        return h;
    }

    size_t hash_string(char* string);

    hash_map* init_hash_map()
    {
        hash_map h =  (hash_map){(size_t*)malloc(sizeof(size_t)), (linked_list){0}, 0, 0};
        hash_map* x = (hash_map*)malloc(sizeof(hash_map));
        *x = h;
        return x;
    }

    void add_element(hash_map* hash, hash_map_element* element)
    {
        append_node(&hash->elements, initialize_node(element));
        size_t index_el = hash->count++;
        size_t index = hash_string(element->key);
        if(index > hash->element_max)
        {
            hash->element_indices = (size_t*)realloc(hash->element_indices, index* sizeof(size_t) + sizeof(size_t));
            hash->element_max = index; 
        }
        hash->element_indices[index] = index_el;
    }


    hash_map_element* get_element(hash_map* hash, char* key)
    {
        size_t hash_key = hash_string(key);
        hash_map_element* element = (hash_map_element*)linked_list_at(hash->element_indices[hash_key],&hash->elements)->data;
        return element;
    }
    int hash_contains(hash_map* hash, char* key)
    {
        size_t hash_key = hash_string(key);
        if(hash_key > hash->element_max)
        {
            return 0;
        }
        if(get_element(hash, key) == (void*)0)
        {
            return 0;
        }
        return 1;
    }

    void* get_value(hash_map* hash, char* key)
    {
        return get_element(hash, key)->value;
    }

    size_t hash_string(char* string)
    {
        size_t hash = 0;
        int i = 0;
        for(hash = i = 0; i < strlen(string); ++i)
        {
            hash += string[i] | i;
            hash += (hash >> 10);
            hash ^= (hash >> 6);
            hash |= -strlen(string) * (string[i] * i);
        }
        hash *= string[0];
        hash *= string[strlen(string) - 1];
        hash += (hash >> 4);
        hash ^= (hash >> 11);
        hash += (hash << 16);
        return hash & (1024 * 1024) - 1;
    }

}
#endif
