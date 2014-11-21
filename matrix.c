#include<stdio.h>
#include<limits.h>
int n=0;

    void print_optimal(int s[][n] , int i,int j){
        if(i==j)
            printf("a%d",i);
          
        else
        {
            printf("(");
            print_optimal(s,i,s[i][j]);
            print_optimal(s,s[i][j] +1 , j);
            printf(")");
        }
           
    }
    // Matrix Ai has dimension p[i-1] x p[i] for i = 1..n
    int MatrixChainOrder(int p[])
    {
     
        /* For simplicity of the program, one extra row and one extra column are
           allocated in m[][].  0th row and 0th column of m[][] are not used */
        int m[n][n],s[n][n];
     
        int i, j, k, L, q,x,y;
     
        /* m[i,j] = Minimum number of scalar multiplications needed to compute
           the matrix A[i]A[i+1]...A[j] = A[i..j] where dimention of A[i] is
           p[i-1] x p[i] */
     
        // cost is zero when multiplying one matrix.
        for (i = 1; i < n; i++)
            m[i][i] = 0;
        for (i = 1; i < n; i++)
            s[i][i] = 0;
        // L is chain length.  
        for (L=2; L<=n; L++)   
        {
            for (i=1; i<=n-L+1; i++)
            {
                j = i+L-1;
                m[i][j] = INT_MAX;
                for (k=i; k<=j-1; k++)
                {
                    // q = cost/scalar multiplications
                    q = m[i][k] + m[k+1][j] + p[i-1]*p[k]*p[j];
                    if (q < m[i][j]){
                        m[i][j] = q;
                        s[i][j] = k;
                    }
                        
                }
            }
        }
        
        for( x =1;x<=n;x++){
            printf("\n");
            for ( y = x; y < n; ++y)
            {
                printf("%d  ", s[x][y]);
            }
        }
        print_optimal(s,1,n-1);
        return m[1][n-1];
    }
 
int main()
{
    //int arr[] = {30,35,15,5,10,20,25};
    int arr[] = {20,50,30,10,10};
    n = sizeof(arr)/sizeof(arr[0]);
 
    printf("Minimum number of multiplications is %d ",MatrixChainOrder(arr));
 
    getchar();
    return 0;
}

int final_result[n],inter_result[n];

for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
        int temp_i =i;
        while(temp_i != 0){
            printf("a\n");
            temp_i++;
        }
        printf("(" );
    }
}