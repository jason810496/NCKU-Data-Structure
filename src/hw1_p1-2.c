#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// postfix to prefix using stack

const int MAX_N = 10005;
int main(int argc, char *argv[])
{

    char *stk[MAX_N];
    char postfix[MAX_N];
    char prefix[MAX_N];

    int top = 0;
    int postfix_len;

    scanf("%s", postfix);
    postfix_len = strlen(postfix);

    for (int i = 0; i < postfix_len; i++)
    {
        char* c = (char *)malloc(sizeof(char) * 2 );
        c[0] = postfix[i];
        c[1] = '\0';

        if ('A' <= postfix[i] && postfix[i] <= 'Z')
        {
            stk[top] = (char *)malloc(sizeof(char) * 2 );
            strcpy(stk[top++], c);
        }
        else
        { // operator
            int sz1, sz2;
            char *tp1, *tp2;

            sz1 = strlen(stk[top - 1]);
            sz2 = strlen(stk[top - 2]);

            tp1 = (char *)malloc(sizeof(char) * (sz1 + 1) );
            tp2 = (char *)malloc(sizeof(char) * (sz2 + 1) );

            strcpy(tp1,stk[top-1]);
            strcpy(tp2,stk[top-2]);

            free(stk[top-1]);
            free(stk[top-2]);

            top -= 2;

            ++top;
            stk[top] = (char *)malloc(sizeof(char) * (1 + sz2 + sz1 + 1) );

            strcpy(stk[top], c);
            strcat(stk[top], tp2);
            strcat(stk[top], tp1);
        }
    }

    for(int i=top-1;i>=0;i--){
        printf("%s",stk[i]);
    }
    printf("\n");

    return 0;
}
