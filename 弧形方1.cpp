#include <bits/stdc++.h>
using namespace std;
int main(){
	int n=49,m=49,R=10;
	int a[105][105]={0},b[105][105]={0};
	// 读入49x49矩阵（正中心为1）
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			cin >>a[i][j];
		}
	}
	// 第一步：49x49结构元素膨胀，变成实心正方形
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			for(int x=-24;x<=24;x++){
				for(int y=-24;y<=24;y++){
					if(i+x>=1 && i+x<=n && j+y>=1 && j+y<=m && a[i+x][j+y]==1){
						b[i][j]=1;
					}
				}
			}
		}
	}
	// 第二步：只腐蚀四个角，四条边保持平直
	// 四个角的圆心分别在(1+R,1+R)、(1+R,m-R)、(n-R,1+R)、(n-R,m-R)
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			if(i<1+R && j<1+R){
				// 左上角：到(1+R,1+R)的距离>R就削掉
				if((i-1-R)*(i-1-R)+(j-1-R)*(j-1-R)>R*R){
					b[i][j]=0;
				}
			}else if(i<1+R && j>m-R){
				// 右上角：到(1+R,m-R)的距离>R就削掉
				if((i-1-R)*(i-1-R)+(j-m+R)*(j-m+R)>R*R){
					b[i][j]=0;
				}
			}else if(i>n-R && j<1+R){
				// 左下角：到(n-R,1+R)的距离>R就削掉
				if((i-n+R)*(i-n+R)+(j-1-R)*(j-1-R)>R*R){
					b[i][j]=0;
				}
			}else if(i>n-R && j>m-R){
				// 右下角：到(n-R,m-R)的距离>R就削掉
				if((i-n+R)*(i-n+R)+(j-m+R)*(j-m+R)>R*R){
					b[i][j]=0;
				}
			}
			// 其他区域（四条边的大部分）：保持1，不动
		}
	}
	// 输出
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			cout <<b[i][j]<<' ';
		}
		cout <<endl;
	}
	return 0;
}
