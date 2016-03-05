#include <iostream>
#include <vector>

using namespace std;

class FixedSizeFIFOQ {
private:
	vector <int> q;
public:
	FixedSizeFIFOQ(int sz) {
		for(int i=0;i<sz;i++) q.push_back(-1);
	}
	
	~FixedSizeFIFOQ() {
		q.clear();
	}
	
	int push(int value) {
		for (int i=q.size()-1;i>0;i--)  {
			q[i]=q[i-1];
		}
		q[0]=value;
		return q[0];
	}
	
	int last() {
		int res=q.back();
		return(res);
	}
	
	int getElement(int index) {
		if ((index<0)||(index>=q.size())) {
			cout<<"error reading queue element"<<endl;
		}
		return q[index];
	}
	
	void printAll() {
		cout<<"# Queue size: "<<q.size()<<"; {";
		for (int i=0;i<q.size();i++)
			cout<<q[i]<<", ";
		cout<<"}\n";
	}
	
	void clear() {
		q.clear();
	}
};
