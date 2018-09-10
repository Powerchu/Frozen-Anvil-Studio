/* HEADER *********************************************************************************/
/*!
\file	MetaHelper.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Helper functions & structs for Meta.h

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _META_HELPER_H_
#define _META_HELPER_H_

#include "Utility\MetaDataStructures.h"

namespace Utility
{
	// Foward Decls
	template <bool, typename true_t, typename false_t>
	struct IfElse;

	template <typename T, typename U>
	struct IsSame;

	template <bool, typename>
	struct EnableIf;

	template <typename T>
	struct RemoveRef;

	namespace Helper
	{
		// If it has a member, we can instantiate type of member pointer
		template <typename T>
		constexpr char HasMember(T T::*)	{ return 1; }

		// Force false overload to have the lowest rank so that
		// it will only be picked if substution for "true" overload fails
		template <typename T>
		constexpr int HasMember(...)		{ return 0; }



	// =========================================== COMPILE TIME FIND =========================================== // 
	// Make result be false instead of compile error?

		template <typename Ty, typename ... Arr>
		struct MetaFinder
		{
			static constexpr bool value = false;
		};

		template <typename Ty, typename first, typename ... Arr>
		struct MetaFinder<Ty, first, Arr...> : public IfElse< IsSame<Ty, first>::value,
			MetaFinder<Ty, Ty>, MetaFinder<Ty, Arr...>>::type
		{
		};

		template <typename Ty, typename last>
		struct MetaFinder<Ty, last>
		{
			static constexpr bool value = IsSame<Ty, last>::value;
			using result = typename IfElse<value, last, NULL_TYPE>::type;
		};

		template <typename Ty, typename T, typename ... Ts, unsigned val, unsigned ... vals>
		struct MetaFinder<Ty, Indexer<val, T>, Indexer<vals, Ts>...> : public
			IfElse< IsSame<Ty, T>::value,
				MetaFinder<Ty, Indexer<val, T>>,
				MetaFinder<Ty, Indexer<vals, Ts>...>>::type
		{
		};

		template <typename Ty, typename T, unsigned val>
		struct MetaFinder<Ty, Indexer<val, T>>
		{
			static constexpr bool value = IsSame<Ty, T>::value;
			using result = typename IfElse<value, Indexer<val, Ty>, NULL_TYPE>::type;
		};



	// ============================================ TYPE LIST MAKER ============================================ // 

		template <template <typename...> typename Ret_t, typename ... Ty>
		struct TypeListMaker
		{
			using type = Ret_t<Ty...>;
		};

		template <template <typename...> typename Ret_t, unsigned ... vals, typename ... Ty>
		struct TypeListMaker<Ret_t, Indexer<vals, Ty>...>
		{
			using type = Ret_t<Ty...>;
		};



	// ========================================= COMPILE TIME PARTITION ======================================== // 

		template <typename T, template <typename, T, T> typename Op, T, typename ... Ty>
		struct MetaPartitionerValue;

		template <typename T, template <typename, T, T> typename Op, 
				  template <typename, T...> typename Set, T pivot, T next, T ... rest>
		struct MetaPartitionerValue<T, Op, pivot, Set<T, next, rest...>>
		{
			using type = typename MetaPartitionerValue<T, Op, pivot, Set<T, rest...>>::result;

			using result = Duplex <
				typename MetaConcat <typename IfElse < Op<T, next, pivot>::value, Set<T, next>, Set<T>>::type, typename type::lhs>::type,
				typename MetaConcat <typename IfElse <!Op<T, next, pivot>::value, Set<T, next>, Set<T>>::type, typename type::rhs>::type
			>;
		};

		template <typename T, template <typename, T, T> typename Op,
				  template <typename, T...> typename Set, T pivot, T next>
		struct MetaPartitionerValue<T, Op, pivot, Set<T, next>>
		{
			using result = Duplex <
				typename IfElse < Op<T, next, pivot>::value, Set<T, next>, Set<T> >::type,
				typename IfElse <!Op<T, next, pivot>::value, Set<T, next>, Set<T> >::type
			>;
		};

		template <template <typename, typename> typename Op, typename ... Ty>
		struct MetaPartitionerType;

		template <template <typename, typename> typename Op, template <typename...> typename Set,
				  typename pivot, typename ... rest>
		struct MetaPartitionerType<Op, Set<pivot, rest...>>
		{
			using result = Duplex <
				MetaConcat_t <typename IfElse < Op<rest, pivot>::value, Set<rest>, Set<>>::type ...>,
				MetaConcat_t <typename IfElse <!Op<rest, pivot>::value, Set<rest>, Set<>>::type ...>
			>;
		};



	// ============================================ COMPILE TIME SORT ========================================== // 

		template <template <typename, typename> typename Op, typename ... Arr>
		struct MetaSorterT;

		template <template <typename, typename> typename Op, template <typename...> typename Set, typename Pivot, typename ... Ty>
		struct MetaSorterT<Op, Set<Pivot, Ty...>>
		{
		private:
			using type = typename MetaPartitionerType<Op, Set<Pivot, Ty...>>::result;

			using lhs = typename MetaSorterT<Op, typename type::lhs>::result;
			using rhs = typename MetaSorterT<Op, typename type::rhs>::result;

		public:
			using result = typename MetaConcat<lhs, Set<Pivot>, rhs>::type;
		};

		template <template <typename, typename> typename Op, template <typename...> typename Set, typename Pivot>
		struct MetaSorterT<Op, Set<Pivot>>
		{
			using result = Set<Pivot>;
		};

		template <template <typename, typename> typename Op, template <typename...> typename Set>
		struct MetaSorterT<Op, Set<>>
		{
			using result = Set<>;
		};

		template <template <typename T, T, T> typename Op, typename ... Arr>
		struct MetaSorterV;
		 
		template <typename T, template <typename, T, T> typename Op, template <typename, T...> typename Set, T Pivot, T ... vals>
		struct MetaSorterV<Op, Set<T, Pivot, vals...>>
		{
		private:
			using type = typename MetaPartitionerValue<T, Op, Pivot, Set<T, vals...>>::result;

			using lhs = typename MetaSorterV<Op, typename type::lhs>::result;
			using rhs = typename MetaSorterV<Op, typename type::rhs>::result;

		public:
			using result = typename MetaConcat<lhs, Set<T, Pivot>, rhs>::type;
		};

		template <typename T, template <typename, T, T> typename Op, template <typename, T...> typename Set, T Pivot>
		struct MetaSorterV<Op, Set<T, Pivot>>
		{
			using result = Set<T, Pivot>;
		};

		template <typename T, template <typename, T, T> typename Op, template <typename, T...> typename Set>
		struct MetaSorterV<Op, Set<T>>
		{
			using result = Set<T>;
		};



	// ======================================= EXTRACT TYPE FROM TYPE LIST ===================================== // 

		template <unsigned N, typename T>
		struct MetaExtractor;

		template <unsigned N, template <typename...> typename Set, typename Ty, typename ... R>
		struct MetaExtractor <N, Set<Ty, R...>>
		{
			using result = typename MetaExtractor <N - 1, Set<R...>>::result;
		};

		template <template <typename...> typename Set, typename Ty, typename ... R>
		struct MetaExtractor <0, Set<Ty, R...>>
		{
			using result = Ty;
		};

		template <unsigned N, typename T, template <typename, T...> typename Set, T val, T ... rest>
		struct MetaExtractor <N, Set<T, val, rest...>>
		{
			using result = typename MetaExtractor <N - 1, Set<T, rest...>>::result;
		};

		template <typename T, template <typename, T...> typename Set, T val, T ... rest>
		struct MetaExtractor <0, Set<T, val, rest...>>
		{
			using result = Set<T, val>;
		};

		template <unsigned N, typename T>
		struct MetaExtractorV;

		template <unsigned N, typename T, T val, T ... rest,
			template <typename, T...> class Set>
		struct MetaExtractorV<N, Set<T, val, rest...>>
		{
			static constexpr T value = MetaExtractorV <N - 1, Set<T, rest...>>::value;
		};

		template <typename T, T val, T ... rest,
			template <typename, T...> class Set>
		struct MetaExtractorV<0, Set<T, val, rest...>>
		{
			static constexpr T value = val;
		};


		
	// ============================================== AUTO INDEXER ============================================= // 

		template <typename Index, typename ... Ty>
		struct MetaAutoIndexerMake;

		template <template <unsigned...> class Set, unsigned ... Ns, typename ... Ty>
		struct MetaAutoIndexerMake<Set<Ns...>, Ty...>
		{
			using result = Collection< Indexer<Ns, Ty>... > ;
		};


		
	// =========================================== CONVERSION SELECTOR ========================================= //
		
		// TODO: SFINAE Friendly?
		template <typename Ty>
		struct ConvertionOption
		{
			Ty operator () (typename Utility::RemoveRef<Ty>::type);
		};
		
		template <typename ... Ty>
		struct ConversionSelector : public ConvertionOption<Ty> ...
		{

		};
	}
}



#endif		// INCLUDE GUARD

