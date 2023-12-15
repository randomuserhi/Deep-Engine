#pragma once

#include "Deep.h"
#include <vector>

namespace Deep
{
	template<typename T>
	class Archetype
	{
	private:
		std::vector<T> component;

	public:
	};

	namespace Impl
	{
		template<typename ...types>
		class Impl_ArchetypeRecurrBase
		{
		};

		template<typename L, typename ...types>
		class Impl_ArchetypeRecurrBase
		{
		public:
			template <typename CL, typename... CArgs>
			Impl_ArchetypeRecurrBase(CL&& arg, CArgs &&... args) : _tuple_impl<CL>(std::forward<CL>(arg)),
				Impl_ArchetypeRecurrBase<types...>(std::forward<CArgs>(args)...)
			{
			}
		};
	}
}
