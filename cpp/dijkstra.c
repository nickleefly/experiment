#include <stdio.h>
#define MAX 1000

void dijkstra(int n,int v,int cost[10][10],int dist[10]);

int main()
{
 int n,v,i,j,cost[10][10],dist[10];
 printf("\n Enter the number of Nodes: ");
 scanf("%d",&n);
 printf("\n Enter the Weight Matrix:\n");
 printf("\nEnter 1000 to denote Infinity\n");
 for(i=0;i<n;i++)
 {
   for(j=0;j<n;j++)
   {
     scanf("%d",&cost[i][j]);
   }
 }
 printf("\n Enter the Source Node:");
 scanf("%d",&v);
 dijkstra(n,v-1,cost,dist);
 printf("\n   Shortest Path from Node %d: ",v);
 printf("\n#################################\n\n");
 for(i=0;i<n;i++)
 {
    printf("Distance to Node:%d is %d\n",i+1,dist[i]);
 }
 return 0;
}

void dijkstra(int n,int v,int cost[10][10],int dist[10])
{
  int i,u,count,w,flag[10],min;
  for(i=0;i<n;i++)
  {
    flag[i]=0;
    dist[i]=cost[v][i];
  }
  count=1;
  while(count<n)
  {
    min=MAX;
    for(w=0;w<n;w++)
    {
       if(dist[w]<min && !flag[w])
       {
           min=dist[w];
           u=w;
       }
    }
    flag[u]=1;
    count++;
    for(w=0;w<n;w++)
    {
       if((dist[u]+cost[u][w]<dist[w])&&!flag[w])
       {
          dist[w]=dist[u]+cost[u][w];
       }
    }
 }
}