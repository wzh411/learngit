#include <bits/stdc++.h>
using namespace std;
int main(){
	int n,m,a[110][110]={0},b[110][110]={0};
	cin >>n>>m;
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			cin >>a[i][j];
		}
	}
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			for(int x=-24;x<=24;x++){
				for(int y=-24;y<=24;y++){
					if(i+x>=0 && i+x<=n && j+y>=0 && j+y<=m && a[i+x][j+y]==1){
					  }	
						b[i][j]=1;
					}
					int e=(i-24),f=(j-24),k;
	                if((e*f)<0){k=(0-(e*f));}
	                if((e*f)>=0){k=(e*f);}
	                if(k<=576){b[i][j]=1;
	                  else if(k>576){b[i][j]=0;
	                    }
	                }	
				}
			}
		}
	}
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			cout <<b[i][j]<<' ';
		}
		cout <<endl;
	}
	return 0;
}
