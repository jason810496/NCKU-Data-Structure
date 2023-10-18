#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int par[100005];
int ht[100005];

int Find(int x)
{   
    if( par[x] == x ) return x;
    return par[x] = Find(par[x]) ;
}

void Union(int a, int b)
{
    a = Find(a);
    b = Find(b);

    if( a == b ){
        return ;
    }

    if( ht[a] == ht[b] ){
        par[b] = a;
        ht[a]++;
    }
    else if( ht[a] > ht[b] ){
        par[b] = a;
    }
    else{
        par[a] = b;
    }

}


int main(int argc, char *argv[])
{
    int t;
    scanf("%d", &t);
    char oper[6];

    while (t--)
    {
        int n, q;
        scanf("%d %d", &n, &q);

        for(int i=0;i<=n;i++){
            par[i] = i;
            ht[i] = 0;
        }

        
        while (q--)
        {
            int a, b;
            scanf("%s", oper);

            switch (oper[0])
            {
            case 'f':
                scanf("%d" ,&a);
                printf("%d\n",Find(a) );
                break;
            case 'u':
                scanf("%d %d" ,&a,&b);
                Union(a,b);
                break;
            case 's':
                scanf("%d %d" ,&a,&b);
                a = Find(a);
                b = Find(b);
                if( a==b ){
                    printf("true\n");
                }
                else{
                    printf("false\n");
                }
                break;
            default:
                break;
            }

        }
    }
    return 0;
}
