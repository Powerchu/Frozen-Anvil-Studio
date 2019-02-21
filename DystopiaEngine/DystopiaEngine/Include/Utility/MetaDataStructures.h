/* HEADER *********************************************************************************/
/*!
\file	MetaDataStructures.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains useful metafunction "data structures" 
	that are also used by meta algorithms.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _META_DATASTRUCTS_H_
#define _META_DATASTRUCTS_H_



namespace Ut
{
	struct NULL_TYPE;

	template <unsigned label, typename T>
	struct Indexer
	{
		using type = T;
		static constexpr unsigned value = label;
	};

	template <unsigned _label, typename T>
	using Indexer_t = typename Indexer<_label, T>::type;

	// Use this when instantiation is unwanted
	template <typename ... Ty>
	struct Collection;

	// Use this when you need to pass types into functions
	template <typename ... Ty>
	struct TypeList
	{};

	template <typename T, T ...>
	struct IntegralList;

	template <typename L, typename R>
	struct Duplex
	{
		using lhs = L;
		using rhs = R;
	};




	
	// ============================================== META FUNCTORS ============================================= //



	template <typename T, T lhs, T rhs>
	struct MetaLessThanV
	{
		static constexpr bool value = lhs < rhs;
	};

	template <typename T, typename U>
	struct MetaLessThan;

	template <template <unsigned, typename ...> typename Set, unsigned lhs, unsigned rhs, typename ... L, typename ... R>
	struct MetaLessThan<Set<lhs, L...>, Set<rhs, R...>>
	{
		static constexpr bool value = lhs < rhs;
	};





	// ========================================= CERTAIN USEFUL STRUCTS ======================================== //


	// Size of
	// ========= ================================

	template <typename T>
	struct SizeofList;

	template <template <typename...> typename Set, typename ... T>
	struct SizeofList<Set<T...>>
	{
		static constexpr auto value = sizeof...(T);
	};

	template <typename Ty, template <typename, Ty...> typename Set, Ty ... vals>
	struct SizeofList<Set<Ty, vals...>>
	{
		static constexpr auto value = sizeof...(vals);
	};


	// Concat
	// ========= ================================

	template <typename ...Tys>
	struct MetaConcat;

	template <template <typename ...> class Set, typename ... Tys>
	struct MetaConcat<Set<Tys...>>
	{
		using type = Set<Tys...>;
	};

	template <template <typename ...> class Set, typename ... lhs, typename ... rhs>
	struct MetaConcat<Set<lhs...>, Set<rhs...>>
	{
		using type = Set<lhs..., rhs...>;
	};

	template <template <typename ...> class Set, typename ... lhs, typename ... rhs, typename ... Rest>
	struct MetaConcat<Set<lhs...>, Set<rhs...>, Rest...>
	{
		using type = typename MetaConcat<
			Set<lhs..., rhs...>,
			typename MetaConcat<Rest...>::type
		>::type;
	};

	template <typename T, template <typename, T ...> class Set, T ... vals>
	struct MetaConcat<Set<T, vals...>>
	{
		using type = Set<T, vals...>;
	};

	template <typename T, template <typename, T ...> class Set, T ... LHS, T ... RHS>
	struct MetaConcat<Set<T, LHS...>, Set<T, RHS...>>
	{
		using type = Set<T, LHS..., RHS...>;
	};

	template <typename T, template <typename, T ...> class Set, T ... LHS, T ... RHS, typename ... Rest>
	struct MetaConcat<Set<T, LHS...>, Set<T, RHS...>, Rest...>
	{
		using type = typename MetaConcat<
			Set<T, LHS..., RHS...>,
			typename MetaConcat<Rest...>::type
		>::type;
	};

	template <template <unsigned ...> class Set, unsigned ... LHS, unsigned ... RHS, typename ... Rest>
	struct MetaConcat<Set<LHS...>, Set<RHS...>, Rest...>
	{
		using type = typename MetaConcat<
			Set<LHS..., RHS...>,
			typename MetaConcat<Rest...>::type
		>::type;
	};

	template <template <unsigned ...> class Set, unsigned ... LHS, unsigned ... RHS>
	struct MetaConcat<Set<LHS...>, Set<RHS...>>
	{
		using type = Set<LHS..., RHS...>;
	};

	template <template <unsigned ...> class Set, unsigned ... Vs>
	struct MetaConcat<Set<Vs...>>
	{
		using type = Set<Vs...>;
	};

	template <typename ... T>
	using MetaConcat_t = typename MetaConcat<T...>::type;


	// MakeRange
	// =========== ===============================

	template <auto limit, auto start = 0>
	struct MetaMakeRange
	{
	private:
		template <auto ... Vals>
		struct Range;

		template <auto min, auto max, auto range>
		struct RangeBuilder
		{
		private:
			static constexpr auto midpt = (max + min) >> 1;

		public:
			using result = MetaConcat_t<
				typename RangeBuilder<min, midpt, midpt - min>::result,
				typename RangeBuilder<midpt, max, max - midpt>::result
			>;
		};

		template <auto x, auto y>
		struct RangeBuilder<x, y, 1>
		{
			using result = Range<x>;
		};

		template <auto x>
		struct RangeBuilder<x, x, 0>
		{
			using result = Range<>;
		};

	public:
		using result = typename RangeBuilder<start, limit, limit - start>::result;
	};

	template <auto limit, auto start = 0>
	using MetaMakeRange_t = typename MetaMakeRange<limit, start>::result;


	// Group
	// ========== ================================

	template <template <typename...> class Tuple, typename ... Ty>
	struct MetaGroup
	{
		using type = Tuple<Ty...>;
	};

	template <template <typename...> class Tuple, template <typename ...> class Set, unsigned ... Ns, typename ... Tys>
	struct MetaGroup<Tuple, Set<Indexer<Ns, Tys>...>>
	{
		using type = Tuple<Tys...>;
	};


	// Pop
	// ========== ================================

	template <typename T>
	struct MetaPopFront;

	template <template <typename ...> class Set, typename F, typename ... Ty>
	struct MetaPopFront<Set<F, Ty...>>
	{
		using type = Set<Ty...>;
	};

	template <typename T>
	struct MetaPopBack;

	template <template <typename ...> class Set, typename B, typename ... Ty>
	struct MetaPopBack<Set<Ty..., B>>
	{
		using type = Set<Ty...>;
	};

	template <typename T>
	using MetaPopBack_t = typename MetaPopBack<T>::type;

	template <typename T>
	using MetaPopFront_t = typename MetaPopFront<T>::type;
}



#endif		// INCLUDE GUARD

