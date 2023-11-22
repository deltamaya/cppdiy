#include <iostream>
#include <utility>

template<typename T,typename Deleter>requires std::invocable<Deleter>
class UniquePtr{
	T*m_p=nullptr;
public:
	explicit UniquePtr(const T*p){
		puts(__PRETTY_FUNCTION__);
		m_p=p;
	}
	~UniquePtr(){
		fclose(m_p);
		puts(__PRETTY_FUNCTION__);
	}
	UniquePtr(UniquePtr const&)=delete;
	UniquePtr& operator=(UniquePtr const&)=delete;
	UniquePtr(UniquePtr&&rhs) noexcept{
		m_p=std::exchange(rhs.m_p,nullptr);
		puts(__PRETTY_FUNCTION__);
	}
	UniquePtr&operator=(UniquePtr&&rhs) noexcept{
		if(this!=&rhs)[[likely]]{
			free(m_p);
			m_p=std::exchange(rhs.m_p,nullptr);
		}
		puts(__PRETTY_FUNCTION__);
		return *this;
	}
	[[nodiscard]]FILE*get()const{
		return m_p;
	}
};
int main(){
	auto a=UniquePtr<FILE>{"a.txt"};
	for(auto i=0;i<3;++i) {
		std::cout<<(char)fgetc(a.get())<<std::endl;
	}
	UniquePtr b=std::move(a);
	for(auto i=0;i<2;++i) {
		std::cout<<(char)fgetc(b.get())<<std::endl;
	}
}