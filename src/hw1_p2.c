#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int MAX_N = 10005;

int par[MAX_N];
int ht[MAX_N];

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

int Same(int a, int b)
{
    return Find(a) == Find(b);
}

int main(int argc, char *argv[])
{
    int t;
    scanf("%d", &t);

    while (t--)
    {
        int n, q;
        scanf("%d%d", &n, &q);

        for(int i=0;i<=n+1;i++){
            par[i] = i;
            ht[i] = 0;
        }

        char oper[6];
        while (q--)
        {
            int a, b;
            scanf("%s", oper);

            switch (oper[0])
            {
            case 'f':
                scanf("%d" ,&a);
                printf("%d",Find(a) );
                break;
            case 'u':
                scanf("%d%d" ,&a,&b);
                Union(a,b);
                break;
            case 's':
                scanf("%d%d" ,&a,&b);
                int res = Same(a,b);
                if( res ){
                    printf("true");
                }
                else{
                    printf("false");
                }
                break;
            default:
                break;
            }

            if( q!=-1 ){
                printf("\n");
            }
        }
    }
    return 0;
}
