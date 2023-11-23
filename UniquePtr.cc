#include <iostream>
#include <utility>
#include <format>
#include <span>
#include <memory>
#include <vector>

template<typename T>
class DefaultDeleter{
public:
	void operator()(const T*p)const{
		//puts(__PRETTY_FUNCTION__);
		delete p;
	}
};
template<typename T>
class DefaultDeleter<T[]>{
public:
	void operator()(const T*p)const{
		//puts(__PRETTY_FUNCTION__);
		delete[] p;
	}
};

template<typename T,typename Deleter=DefaultDeleter<T>>requires std::invocable<Deleter,T*>
class UniquePtr{
	public:

	T*m_p=nullptr;
	// template<typename U,typename UDeleter>
	// friend class UniquePtr<U,UDeleter>;
	explicit UniquePtr(T*p){
		//puts(__PRETTY_FUNCTION__);
		m_p=p;
	}
	~UniquePtr(){
		if(m_p)Deleter{}(m_p);
		//puts(__PRETTY_FUNCTION__);
	}
	UniquePtr(UniquePtr const&)=delete;
	UniquePtr& operator=(UniquePtr const&)=delete;
	UniquePtr(UniquePtr&&rhs) noexcept{
		m_p=std::exchange(rhs.m_p,nullptr);
		//puts(__PRETTY_FUNCTION__);
	}
	UniquePtr&operator=(UniquePtr&&rhs) noexcept{
		if(this!=&rhs)[[likely]]{
			if(m_p)
				Deleter{}(m_p);
			m_p=std::exchange(rhs.m_p,nullptr);
		}
		//puts(__PRETTY_FUNCTION__);
		return *this;
	}
	template<typename U,typename UDeleter>requires std::convertible_to<U*,T*>
	explicit UniquePtr(UniquePtr<U,UDeleter>&&rhs){
		m_p=std::exchange(rhs.m_p,nullptr);
	}
	T* release(){
		return std::exchange(m_p,nullptr);
	}

	void reset(){
		if(m_p)
			Deleter{}(m_p);
		m_p=nullptr;
	}
	T* operator->(){
		return m_p;
	}
	T& operator*()noexcept{
		return *m_p;
	}
	[[nodiscard]]T*get()const{
		return m_p;
	}
};

template<typename T,class ...Args>
UniquePtr<T> MakeUnique(Args&& ...args){
	return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
template<typename T>
UniquePtr<T> MakeUniqueForOverwrite(){
	return UniquePtr<T>(new T);
}
struct S{
	int a;
	int b;
};
struct Animal{
	virtual void speak()=0;
	virtual ~Animal()=default;
};
struct Dog final:Animal{
	int age;
	void speak() override{
		printf("Bark!I'm %d years old\n",age);
	}

	explicit Dog(int a){age=a;}
	~Dog() override =default;
};
struct Cat final :Animal{
	int & age;
	void speak() override{
		printf("Meow!I'm %d years old\n",age);
	}

	explicit Cat(int&a):age(a){  }
	~Cat() override =default;
};