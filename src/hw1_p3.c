#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int par[1000005];
int sz[1000005];

typedef struct e {
    int a, b , w;
} edge;

edge edges[2000005];

int Find(int x)
{   
    if( par[x] == x ) return x;
    return par[x] = Find(par[x]) ;
}

int Union(int a, int b)
{
    a = Find(a);
    b = Find(b);

    if( a == b ){
        return 0;
    }

    if( sz[a] < sz[b] ){
        par[a] = b;
        sz[b] += sz[a];
    }
    else{
        par[b] = a;
        sz[a] += sz[b];
    }

    return 1;
}

int cmp(const void *a, const void *b)
{
    return ((edge *)a)->w - ((edge *)b)->w;
}


int main(int argc, char *argv[])
{
    int n, m;
    scanf("%d %d", &n, &m);

    for(int i=0; i<n; i++){
        par[i] = i;
        sz[i] = 1;
    }

    for(int i=0; i<m; i++){
        scanf("%d %d %d", &edges[i].a, &edges[i].b, &edges[i].w);
    }

    long long mst = 0;

    qsort(edges, m, sizeof(edge), cmp);

    for(int i=0; i<m; i++){
        if( Union(edges[i].a, edges[i].b) ){
            mst += edges[i].w;
        }
    }

    printf("%lld\n", mst);
    
    return 0;
}
