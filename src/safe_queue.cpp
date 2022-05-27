#include <vector>
#include <iterator>
#include <thread>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <functional>
#include <queue>
#include <condition_variable>
#include <time.h>
#include <stdexcept>


using namespace std;


class SafeQueue{
	queue<int> q;
	condition_variable c;
	mutable mutex m;
	
	mutable mutex vector_m;
	vector<bool> everybody_works;
	
	public:
		SafeQueue(int thread_num)
		{
			everybody_works.assign(thread_num,false);
		}
		
		SafeQueue(SafeQueue const& other)
		{
			lock_guard<mutex> g(other.m);
			lock_guard<mutex> gv(other.vector_m);
			q = other.q;
			everybody_works = other.everybody_works;
		}
		
		SafeQueue(queue<int> const& default_queue, int thread_num)
		{
			q = default_queue;
			everybody_works.assign(thread_num,false);
		}

		void push(int val)
		{
			lock_guard<mutex> g(m);
			q.push(val);
			c.notify_one();
		}
		
		int size()
		{
			lock_guard<mutex> g(m);
			return q.size();
		}
		
		void set_me_working(int th, bool val)
		{
			lock_guard<mutex> g(vector_m);
			everybody_works[th] = val;
			c.notify_one();
		}
		
		bool is_everybody_working()
		{
			lock_guard<mutex> g(vector_m);
			return accumulate(everybody_works.begin(), everybody_works.end(), false);
		}
		
		int just_pop()
		{
			lock_guard<mutex> lk(m);
			if(q.empty())
				throw "No elems";
			int a = q.front();
			q.pop();
			return a;
		}
		
		bool wait_pop(int& a, int& b)
		{
			unique_lock<mutex> lk(m);
			c.wait(lk, [this]{ return q.size()>1 || !is_everybody_working();});
			if(q.empty()) 
			{
				throw "Ooops! Smth has gone wrong!";
			}
			if(q.size()==1) 
				return false; 
			a = q.front();
			q.pop();
			b = q.front();
			q.pop();
			return true;
		}
	
};


void thread_work(SafeQueue& q, int my_num)
{
	while(true)
	{
		int a, b;
		bool go = q.wait_pop(a,b);
		
		if(!go) 
			break;
		
		q.set_me_working(my_num, true);
		int m = max(a,b);
		this_thread::sleep_for(chrono::milliseconds(100)); 
		q.push(m);
		q.set_me_working(my_num, false);
	}

	q.set_me_working(my_num, false);
}



int find_max_multi_thread(vector<int> v, int req_num_treads)
{
	queue<int> default_queue;
	for(auto& a: v)
		default_queue.push(a);
	
	int max_threads = thread::hardware_concurrency();
	int num_threads = min(max_threads,req_num_treads);
	vector<thread> threads(num_threads - 1);
	
	SafeQueue q(default_queue, num_threads);

	try
	 {
		int tres, tstart, tend;
		auto start = std::chrono::steady_clock::now();
		
		 for(int i = 0; i < num_threads-1;i++)
			  threads[i] = thread(thread_work, ref(q), i);
		thread_work(ref(q), num_threads - 1);
		
		for_each(threads.begin(), threads.end(), mem_fn(&thread::join));
		
		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		
		cout<< "Done by "<< num_threads <<" thread(s) in "<<elapsed.count()<<" milisecs."<<endl;
		
		return q.just_pop();
		
	}
	catch(...)
	{
		for_each(threads.begin(), threads.end(), mem_fn(&thread::join));
		throw;
	}
		
}


int main()
{
	vector<int> v;
	int N;
	cin>>N;
	for(int i = 0; i<N;i++)
		v.push_back(i);
	
	try
	{
		
		int m = find_max_multi_thread(v,100);
		cout<<"Max is "<< m << endl; 
		

		
		m = find_max_multi_thread(v,1);
		cout<<"Max is "<< m << endl; 
	}
	catch(...)
	{
		cout<<"Ooops!"<<endl;
		return 0;
	}
	
	return 0;
}

