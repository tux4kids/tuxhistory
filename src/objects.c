#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<mxml.h>

#include "fileops.h"
#include "objects.h"

static int init_obj_hash(void);

static int init_obj_hash(void)
{

    obj_table_hash = make_hashtable(hashtable_default_hash, NUM_OBJECTS+NUM_OF_TYPES);

    if(obj_table_hash == NULL)
        return 1;

    hashtable_add(obj_table_hash, "FOREST_MIXED", FOREST_MIXED);
    hashtable_add(obj_table_hash, "FOREST_TROPICAL", FOREST_TROPICAL);
    hashtable_add(obj_table_hash, "FOREST_CONIFER", FOREST_CONIFER);
    hashtable_add(obj_table_hash, "FOREST_SCRUB", FOREST_SCRUB);
    hashtable_add(obj_table_hash, "FOREST_BOREAL", FOREST_BOREAL);
    hashtable_add(obj_table_hash, "FOREST_WETLAND", FOREST_WETLAND);
    hashtable_add(obj_table_hash, "FOREST_RAIN", FOREST_RAIN);

    hashtable_add(obj_table_hash, "FOREST", FOREST);
    hashtable_add(obj_table_hash, "GOLD", GOLD);
    hashtable_add(obj_table_hash, "STONE", STONE);
    hashtable_add(obj_table_hash, "BUILDING", BUILDING);
    hashtable_add(obj_table_hash, "UNIT", UNIT);

    return 0;
}

int objects_xml(FILE *fp)
{
    int value;
    mxml_node_t *tree;
    mxml_node_t *node;
    mxml_node_t *inode;

    th_obj object;

    tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
    if(init_obj_hash())
        return 1;

    for(inode = mxmlFindElement(tree, tree, "object", 
                NULL, NULL, MXML_DESCEND);
            inode != NULL;
            inode = mxmlFindElement(inode, tree, "object",
                NULL, NULL, MXML_DESCEND))
    {
        node = mxmlFindElement(inode, inode, "type",
                               NULL, NULL, MXML_DESCEND);
        if(node != NULL)
        {
            if(value != -1)
            {
                value = (int)hashtable_lookup(obj_table_hash, node->child->value.opaque);
                object.type = value;
            }
        }
        else
        {
            object.type = -1;
            printf("objects_xml: Error loading objects description file");
            return 1;
        }

        node = mxmlFindElement(inode, inode, "name",
                               NULL, NULL, MXML_DESCEND);
        if(node != NULL)
        {
            value = (int)hashtable_lookup(obj_table_hash, node->child->value.opaque);
            if(value != -1)
            {
                object.name_enum = value;
                strcpy(object.name, node->child->value.opaque);    
            }
        }
        else
        {
            object.name_enum = -1;
            printf("objects_xml: Error loading objects description file");
            return 1;
        }

        node = mxmlFindElement(inode, inode, "rname",
                               NULL, NULL, MXML_DESCEND);
        if(node != NULL)
        {
            strcpy(object.rname, node->child->value.opaque);
        }
        else
        {
            strcpy(object.rname, "");
            printf("objects_xml: Error loading objects description file");
            return 1;
        }
        node = mxmlFindElement(inode, inode, "description",
                               NULL, NULL, MXML_DESCEND);
        if(node != NULL)
        {
            strcpy(object.description, node->child->value.opaque);
            printf(" string: %s\n", node->child->value.opaque);
        }
        else
        {
            strcpy(object.rname, "");
            printf("objects_xml: Error loading objects description file");
            return 1;
        }

        node = mxmlFindElement(inode, inode, "live",
                    NULL, NULL, MXML_DESCEND);
            
        if(atoi(node->child->value.opaque) >= 0)
        {
            object.live = atoi(node->child->value.opaque);
        }
        else
        {
            object.live = -1;
            printf("objects_xml: Error loading objects description file");
            return 1;
        }

        /* Debug: print the values of current object */
        printf("%d %s:%d(%s) %s lives: %d\n", 
                object.type,
                object.name,
                object.name_enum,
                object.rname,
                object.description,
                object.live);
    }

        
    free_hashtable(obj_table_hash);

    mxmlDelete(inode);
    mxmlDelete(node);

    fclose(fp);

    return 0;
}

