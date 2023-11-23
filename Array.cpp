#include <iostream>
#include <array>
#include <format>
#include <vector>
#include <utility>
#ifdef _MSC_VER
#define _ST1W_UNREACHABLE() __assume(0)
#elifdef __GNUC__
#define _ST1W_UNREACHABLE() __builtin_unreachable()
#else
#define _ST1W_UNREACHABLE() do{}while(1)
#endif
template<typename _Tp,size_t _N>
struct Array{
	_Tp _M_element[_N];
	_Tp& operator[](size_t _idx)noexcept{
		return _M_element[_idx];
	}
	const _Tp& operator[](size_t _idx)const noexcept{
		return const_cast<const _Tp&>(const_cast<Array*>(this)->operator[](_idx));
	}
	_Tp& at(const size_t _idx){
		//.cold
		if(_idx > _N)[[unlikely]]throw std::runtime_error(std::format("index {} out of bound", _idx));
		return _M_element[_idx];
	}
	[[nodiscard]] const _Tp& at(const size_t idx)const{
		return const_cast<const _Tp&>(const_cast<Array*>(this)->at(idx));
	}
	[[nodiscard]] static constexpr size_t size()noexcept{
		return _N;
	}
	_Tp* data()noexcept{
		return _M_element;
	}
	[[nodiscard]] const _Tp* data()const noexcept{
		return const_cast<Array*>(this)->data();
	}
	using iterator=_Tp*;
	using const_iterator=const _Tp*;
	[[nodiscard]]iterator begin()noexcept{
		return _M_element;
	}
	[[nodiscard]]iterator end()noexcept{
		return _M_element + _N;
	}
	[[nodiscard]]const_iterator begin()const noexcept{
		return const_cast<Array*const>(this)->begin();
	}
	[[nodiscard]]const_iterator end()const noexcept{
		return const_cast<Array*const>(this)->end();
	}
	_Tp& front()noexcept{
		return operator[](0);
	}
	_Tp& back()noexcept{
		return operator[](_N - 1);
	}

	void fill(const _Tp&val)noexcept(std::is_nothrow_copy_assignable_v<_Tp>){
//	void fill(const _Tp&val)noexcept(noexcept(std::declval<_Tp&>()=val)){
		for(auto&e:*this){
			e=val;
		}
	}
	void swap(Array<_Tp,_N>&rhs)noexcept(std::is_nothrow_swappable_v<_Tp>){
		for(size_t i=0;i<_N;++i){
			std::swap((*this)[i],rhs[i]);
		}
	}
};
//模板推导指引（Deduction Guide）
//
//这个推导指引是为 Array 类模板定义的。它指导编译器如何从构造函数参数推导出类模板的模板参数。
//这里 T 和 Ts... 代表一系列类型。
//当使用 Array 的构造函数，并传递至少一个参数时，第一个参数的类型用于确定 Array 的元素类型 T。
//sizeof...(Ts) 计算可变参数包 Ts... 中的元素数量。因此，1 + sizeof...(Ts) 计算出数组的总大小，包括第一个参数 T 和后续的 Ts... 参数。
//因此，这个推导指引允许你从如 Array(1, 2, 3) 这样的构造函数调用中推导出 Array<int, 3> 类型。
template<typename T,typename ...Ts>
Array(T,Ts...) -> Array<T,1+sizeof...(Ts)>;

template<typename T,size_t N>
constexpr Array<std::decay_t<T>,N> to_array(T (&a)[N])noexcept(std::is_nothrow_constructible_v<T>){
	return Array<std::decay_t<T>,N>{a};
}

void fn(Array<int,10> const&arr ){
	//std::remove_reference<decltype(arr)>::type::size()
	for(int i=0;i< std::decay_t<decltype(arr)>::size();++i){

	}
}
int main(){
//	std::function<int(int&,double)>func;
//	std::cout<<typeid(std::decay_t<decltype(func)>).name()<<std::endl;
//	std::cout<<typeid(decltype(func)).name()<<std::endl;
	auto arr= Array{1,3,4};
	for(auto &e:arr){
		std::cout<<e<<" ";
	}
	Array{2,2,2}.swap(arr);
	for(auto &e:arr){
		std::cout<<e<<" ";
	}
//	std::cout<<"sadfa";
//	const auto arr=Array<int,10>{};
//	fn(arr);
//	for(int i=0;i<arr.size();++i){
//
//	}
//	for(auto&e:arr){
//		std::cout<<e<<std::endl;
//	}
//	auto arr=Array<int,10>{};
//	auto &rarr=arr;
//	std::cout<<typeid(arr).name()<<std::endl;
//	std::cout<<typeid(rarr).name()<<std::endl;
//	std::cout<<std::is_same_v<decltype(arr),decltype(rarr)><<std::endl;
//	std::cout<<std::is_same_v<decltype(arr),std::decay_t<decltype(rarr)>><<std::endl;
//	std::cout<<std::is_same_v<decltype(arr),std::remove_reference_t<decltype(rarr)>><<std::endl;
}