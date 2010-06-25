#include "tuxhistory.h"

#include<ctype.h>
#include<mxml.h>

#include "map.h"

int get_terrain_enum(char *);
void str_upper(char *string);

int map_xml(FILE *fp)
{
    char *text;
    mxml_node_t *tree;
    mxml_node_t *node;
    mxml_node_t *inode;
    mxml_node_t *jnode;

    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);

    for(inode = mxmlFindElement(tree, tree, "row", 
                NULL, NULL, MXML_DESCEND);
            inode != NULL;
            inode = mxmlFindElement(inode, tree, "row",
                NULL, NULL, MXML_DESCEND))
    {
        for(jnode = mxmlFindElement(inode, inode, "tilde",
                    NULL, NULL, MXML_DESCEND);
                jnode != NULL;
                jnode = mxmlFindElement(jnode, inode, "tilde",
                    NULL, NULL, MXML_DESCEND))
        {
            node = mxmlFindElement(jnode, jnode, "terrain",
                    NULL, NULL, MXML_DESCEND);
            printf("%s",node->child->value.text.string);
            
            node = mxmlFindElement(jnode, jnode, "height",
                    NULL, NULL, MXML_DESCEND);
            printf("%d",node->child->value.integer);
        }
        printf("\n");
    }

    str_upper(text);
    
    printf("%s\n",text);
    

    mxmlDelete(jnode);
    mxmlDelete(inode);
    mxmlDelete(node);

    fclose(fp);
}

int get_terrain_enum(char *terrain_string)
{
    if(strcmp(terrain, "GRASSLAND"))
    {
        return 1;
    }
}

void str_upper(char *string)
{
    char c;
    while(*string)
    {
        c = *string;
        *string = toupper(c);
        string++;
    }
}


