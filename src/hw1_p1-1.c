#include <stdio.h>
#include <string.h>
// Infix to postfix

const int MAX_N = 100005;

int main(int argc, char* argv[]) {

    char stk[MAX_N];
    char infix[MAX_N];
    char postfix[MAX_N];

    // Read infix expression
    scanf("%s", infix);
    

    int top = 0;
    int idx = 0;
    int infix_len = strlen(infix);

    // Convert infix to postfix

    
    for(int i=0;i<infix_len;i++){
        char c = infix[i];

        if(c == '('){
            stk[top++] = c;
        }else if(c == ')'){
            while(stk[top-1] != '('){
                postfix[idx++] = stk[top-1];
                top--;
            }
            top--;
        }else if(c == '+' || c == '-'){
            while(top > 0 && stk[top-1] != '('){
                postfix[idx++] = stk[top-1];
                top--;
            }
            stk[top++] = c;
        }else if(c == '*' || c == '/'){
            while(top > 0 && (stk[top-1] == '*' || stk[top-1] == '/')){
                postfix[idx++] = stk[top-1];
                top--;
            }
            stk[top++] = c;
        }else{
            postfix[idx++] = c;
        }
    }

    while(top > 0){
        postfix[idx++] = stk[top-1];
        top--;
    }

    printf("%s\n", postfix);


    return 0;
}
/*

(A+B)*(C+D)

AB+CD+*

A+B*C/D+E*F/G+H

ABC*D/+EF*G/+H+

ABC
+*




*/