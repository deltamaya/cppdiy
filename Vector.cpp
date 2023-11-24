#include <cmath>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <vector>
namespace maya{
	template<typename T,typename Alloc=std::allocator<T>>
	class Vector{
		static const size_t npos=-1;
		using value_type=T;
		using iterator=value_type*;
		using reference=value_type&;
		using const_iterator=const value_type*;
		using const_reference=const value_type&;

	private:
		iterator _start;
		iterator _finish;
		iterator _end_of_storage;
		[[no_unique_address]]Alloc _alloc;
		void insert_aux(iterator pos,const value_type& x){
			if(_finish!=_end_of_storage){
				std::construct_at(_finish,x);
				++_finish;
				auto temp=x;
				copy_backward(pos,_finish-2,_finish-1);
				*pos=temp;
			}else{
				const size_t old_size=size();
				const size_t len=old_size?2*old_size:1;
				iterator new_start= _alloc.allocate(len);
				iterator new_finish=new_start;
				try{
					new_finish= uninitialized_copy(begin(),pos,new_start);
					construct(new_finish,x);
					++new_finish;
					new_finish= uninitialized_copy(pos,end(),new_finish);
				}catch(...){
					deallocate(new_start);
					throw;
				}
				deallocate(_start);
				_start=new_start;
				_finish=new_finish;
				_end_of_storage=_start+len;
			}
		}
		void realloc(){
			size_t sz=size();
			auto new_size=sz*2;
			auto new_start=_alloc.allocate(sizeof(T)*new_size);
			for(size_t i=0;i<sz;++i) {
				std::construct_at(new_start+i,std::move(*(_start+i)));
			}
			_alloc.deallocate(_start,sz);
			_start=new_start;
			_finish=_start+sz;
			_end_of_storage=_start+new_size;
		}
	public:
		[[nodiscard]]size_t size()const{return _finish-_start;}
		[[nodiscard]]size_t capacity()const{return _end_of_storage-_start;}
		iterator begin()noexcept{return _start;}
		iterator end()noexcept{return _finish;}
		const_iterator begin()const noexcept{return _start;}
		const_iterator end()const noexcept{return _finish;}


		reference operator[](size_t i)noexcept{
			return _start[i];
		}
		const_reference operator[](size_t i)const noexcept{
			return const_cast<Vector*>(this)->operator[](i);
		}
		bool operator==(const Vector<value_type>&rhs)noexcept{
			if(size()!=rhs.size())return false;
			for(int i=0;i<rhs.size();++i){
				if((*this)[i]!=rhs[i])return false;
			}
			return true;
		}
		bool operator!=(const Vector<value_type>&rhs){
			return !operator==(rhs);
		}
		Vector(){
			_start= _alloc.allocate(10);
			_finish=_start;
			_end_of_storage=_start+10;
		}
		template<std::random_access_iterator InputIterator>requires std::is_same_v<decltype(*InputIterator{}),T>
		Vector(InputIterator begin,InputIterator end)noexcept(std::is_nothrow_constructible_v<T>){
			_start= _alloc.allocate(end-begin);
			_finish=_start;
			_end_of_storage=_finish+(end-begin);
			while(begin<end){
				std::construct_at(_finish,*begin);
				++_finish;++begin;
			}

		}
		void swap(Vector<value_type>&v)noexcept{
			std::swap(_start,v._start);
			std::swap(_finish,v._finish);
			std::swap(_end_of_storage,v._end_of_storage);
			
		}
		Vector<value_type>& operator=(Vector<value_type>v){
			swap(v);
			return *this;
		}
		Vector(size_t n,const value_type& value=T()){
			_start= _alloc.allocate(n);
			_finish=_start+n;
			_end_of_storage=_start+n;
			for(auto it=_start;it!=_finish;++it) {
				std::construct_at(it,value);
			}
		}
		~Vector(){
			for(auto iter=_start;iter!=_finish;++iter) {
				std::destroy_at(iter);
			}
			_alloc.deallocate(_start,sizeof(T)*size());
		}
		Vector(const Vector<value_type>&rhs):
		_start(nullptr),
		_finish(nullptr),
		_end_of_storage(nullptr){
			reserve(rhs.capacity());
			for(const auto&e:rhs){
				push_back(e);
			}
		}

		void resize(size_t len,const value_type&value=value_type()){
			if(len==size())return;
			if(len<size()){
				erase(_start+len,_finish);
			}
			else{
				auto sz=size();
				for(int i=0;i<len-sz;++i){
					push_back(value);
				}
			}
		}
		void reserve(size_t len){
			if(len<=capacity())return;
			size_t sz=size();
			auto new_start= _alloc.allocate(len);
			for(size_t i=0;i<sz;++i) {
				std::construct_at(new_start+i,std::move(*(_start+i)));
			}
			_alloc.deallocate(_start,sz);
			_start=new_start;
			_finish=_start+sz;
			_end_of_storage=_start+len;
		}
		bool empty(){
			return _finish==_start;
		}
		T& back(){
			return *(_finish-1);
		}
		T& front(){
			return *_start;
		}
		iterator erase(iterator pos){
			return erase(pos,pos+1);
		}
		iterator erase(iterator first,iterator last){
			iterator iter=first;
			while(last!=_finish) {
				std::construct_at(iter++,std::move(*last++));
			}
			while(iter!=_finish) {
				std::destroy_at(iter++);
			}
		}
//		void fill_insert(iterator pos,size_t len,const value_type&value){
//
//		}
//		void insert(iterator pos,size_t len,const value_type&value){
//			fill_insert(pos,len,value);
//		}
		// iterator insert(iterator pos,const value_type&value){
		// 	size_t n=pos-begin();
		// 	if(_finish!=_end_of_storage&&pos==_finish)[[unlikely]]{
		// 		std::construct_at(_finish,value);
		// 		++_finish;
		// 	}else{
		// 		insert_aux(pos,value);
		// 	}
		// 	return begin()+n;
		// }
		void push_back(const value_type&value)noexcept(std::is_nothrow_constructible_v<value_type>){
			if(_finish==_end_of_storage)[[unlikely]]{
				realloc();
			}
			std::construct_at(_finish++,value);
		}
		void push_back(value_type&&value)noexcept(std::is_nothrow_constructible_v<value_type>){
			if(_finish==_end_of_storage)[[unlikely]]{
				realloc();
			}
			std::construct_at(_finish++,std::move(value));
		}
		void pop_back(){
			std::destroy_at(--_finish);
		}
		template<typename ...Args>
		iterator emplace_back(Args&&...args){
			if(_finish==_end_of_storage)[[unlikely]]{
				realloc();
			}
			std::construct_at(_finish++,std::forward<Args>(args)...);
			return _finish;
		}
	};
}
struct TestClass{
	int i;
	TestClass():TestClass(0){  }
	TestClass(int v):i(v){ std::cout<<"constructing "<<i<<std::endl; }
	~TestClass(){
		if(i>=0)std::cout<<"destructing "<<i<<std::endl;
	}
	TestClass(const TestClass&rhs){
		i=rhs.i;
		std::cout<<"copy constructing "<<i<<std::endl;

	}
	TestClass(TestClass&&xval){
		i=xval.i;
		xval.i=-1;

		std::cout<<"moving "<<i<<std::endl;
	}
};
int main(){
	maya::Vector<TestClass>vec;
	for(int i=0;i<3;++i) {
		vec.emplace_back(i);
	}
	vec.pop_back();
	vec.push_back(3);
	vec.reserve(20);
	vec.push_back(9);
	vec.resize(10);
}