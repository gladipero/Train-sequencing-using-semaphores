#include<stdio.h>
#include<semaphore.h>
#include<string>
#include<stdlib.h>
#include<pthread.h>
#include<iostream>

using namespace std;

typedef enum{local,mail,special}Type;

class Train
{
	public:	int no,currTrack,nextTrack;
		//string source,dest;
		Type type;
		pthread_t p;
		int tr[10],n1;
}
t[20];
	
sem_t s[20], mutex;	

int n, 
	n1,		
	nt[100],	
	count;

void countTracks();
void search(int[], int, int, int);
void* manageTrain(void*);
bool checkAvailabilityOfTrack(int);
void* runningThread(void*);
int getNextTrack(int,int);
int getTrainForTrackWithTopPriority(int);
int* getTrainsForTrackOfPriority(int,Type);

int main()
{
	int j, k;
	char c[10];
	pthread_t rthread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
    cout<<"enter the number of trains"<<endl;
    cin>>n;
	for(int i=0;i<n;i++)
	{
		//sprintf(c,"%d",i);
		string st(c),type;
		bool f;
		//dest=dest.append(st); source=source.append(st);
        //t[i].dest=dest; t[i].source=source; 
        t[i].no=i+1;
        cout<<"enter the train type for train"<<t[i].no<<endl;
        f=true;
        do
        {
        	cin>>type;
            if(type.compare("mail")==0)
            	{
            		t[i].type=mail; 
            		f=true;
            	}
            	else 
            		if(type.compare("local")==0)
            		{
            			t[i].type=local; 
            			f=true;
            		}
            		else 
            			if(type.compare("special")==0)
            			{
            				t[i].type=special; 
            				f=true;
            			}
            			else
            			{   
            				cout<<"you entered an invalid type, please enter again"<<endl;
			                f=false;
			            }
        }while(!f);

		cout<<"enter the number of tracks for train "<<t[i].no<<" :";
		cin>>k;		t[i].n1=k;
		cout<<"enter the tracks route for train "<<t[i].no<<endl;
		for(j=0;j<k;j++)
			cin>>t[i].tr[j];
		t[i].tr[j]=-1;
		t[i].currTrack=t[i].tr[0];	t[i].nextTrack=t[i].tr[1];
		sem_init(&s[i],0,0);
	}

	sem_init(&mutex,0,1);
	countTracks();
    int ta[n];
	for(int i=0;i<n;i++)
    {   
    	ta[i]=i;
        pthread_create(&(t[i].p),&attr,manageTrain,(void*)&ta[i]);
    }
	pthread_create(&rthread,&attr,runningThread,NULL);	
	for(int i=0;i<n;i++)
		pthread_join(t[i].p,NULL);
	
	return 0;
}

void countTracks()
{
	int d=0;
	bool flag;
	for(int i=0;i<t[0].n1;i++,d++)
	nt[d]=t[0].tr[i];
	for(int i=1;i<n;i++)
	{
		for(int k=0;k<t[i].n1;k++)
		{
			flag=true;
			for(int j=0;j<i;j++)
			{
				count=0;
				search(t[j].tr,0,(t[j].n1)-1,t[i].tr[k]);
				if(count>0)
				{ 	
					flag=false;
					break;
				}
			}
			if(flag)
			{	
				nt[d]=t[i].tr[k];
				d++;
			}
		}
	}
	n1=d;
}

void search(int arr[], int l, int r, int x)
{
    int mid=(l+r)/2;
	if(l<r)
	{	
		search(arr,l,mid,x);
		search(arr,mid+1,r,x);
	}
	else 
		if(arr[r]==x)
			count++;
}

void* manageTrain(void* i)
{
	int q,v,j=*((int*)i);
	if(t[j].currTrack==t[j].tr[0])
    {
        sem_wait(&mutex);
        cout<<"train "<<t[j].no<<" starts from track "<<t[j].currTrack<<endl;
        sem_post(&mutex);
    }
    if(t[j].n1>1)
    {
        q=getNextTrack(j,t[j].currTrack);
        bool flag=checkAvailabilityOfTrack(q);
        if(flag)
        {
            t[j].currTrack=q;
            t[j].nextTrack=-1;
            sem_wait(&mutex);
            cout<<"train "<<t[j].no<<" reaches track "<<t[j].currTrack<<endl;
            sem_post(&mutex);
        }
        else
        {
            t[j].nextTrack=q;
            sem_wait(&mutex);
            cout<<"train "<<t[j].no<<" is waiting for track "<<t[j].nextTrack<<endl;
            sem_post(&mutex);
            sem_wait(&s[j]);
            t[j].currTrack=t[j].nextTrack;
            t[j].nextTrack=-1;
            sem_wait(&mutex);
            cout<<"train "<<t[j].no<<" reaches track "<<t[j].currTrack<<endl;
            sem_post(&mutex);
        }
        v=getNextTrack(j,t[j].currTrack);
        if(v!=-1)
        {
            sem_wait(&mutex);
            cout<<"train "<<t[j].no<<" is heading for track "<<v<<endl;
            sem_post(&mutex);
            manageTrain(&j);
        }
        else
        {
            sem_wait(&mutex);
            cout<<"train "<<t[j].no<<" completes its journey"<<endl;
            sem_post(&mutex);
        }
    }
    else
    {
        sem_wait(&mutex);
        cout<<"train "<<t[j].no<<" has no route to go"<<endl;
        sem_post(&mutex);
    }
}

bool checkAvailabilityOfTrack(int j)
{
	bool flag=true;
	for(int i=0;i<n;i++)
		if(t[i].currTrack==j)
			flag=false;
	return flag;
}

void* runningThread(void* x)
{
    do
    {
        for(int i=0;i<n1;i++)
        {
            int a=getTrainForTrackWithTopPriority(nt[i]);
            if(a!=-1)
            	sem_post(&s[a]);	
        }
    }
    while(true);
}

int getNextTrack(int i,int j)
{
	int w;
	for(int k=0;k<t[i].n1;k++)
	{
		if(t[i].tr[k]==j)
        {   
        	w=t[i].tr[k+1];
            break;
        }
	}
	return w;
}

int getTrainForTrackWithTopPriority(int i)
{
	int tr[40];
	int k=0;	
	Type t;
	for(int j=1;j<=3;j++)
	{

		if(j==1)
			t=special;
		else 
			if(j==2)
				t=local;
			else
				t=mail;
		
		int *train=getTrainsForTrackOfPriority(i,t);
		for(int d=0;train[d]!=-1;d++)
		{	
			tr[k]=train[d];
			k++;
		}
	}
	if(k>0)
		return tr[0];
	else 
		return -1;
}

int* getTrainsForTrackOfPriority(int i,Type type)
{
	int *arr=new int[20];
	int k=0;
	for(int j=0;j<n;j++)
	{
		if(t[j].type==type&&t[j].nextTrack==i)
		{	
			arr[k]=j;
			k++;
		}
	}
	arr[k]=-1;
	return arr;
}
