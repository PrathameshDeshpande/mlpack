/**
 * @file sfinae_utility.hpp
 * @author Trironk Kiatkungwanglai, Kirill Mishchenko
 *
 * This file contains macro utilities for the SFINAE Paradigm. These utilities
 * determine if classes passed in as template parameters contain members at
 * compile time, which is useful for changing functionality depending on what
 * operations an object is capable of performing.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_SFINAE_UTILITY
#define MLPACK_CORE_SFINAE_UTILITY

#include <type_traits>

namespace mlpack {
namespace sfinae {

/*
 * MethodFormDetecter is a tool that helps to find out whether a given class has
 * a method of the requested form. For that purpose MethodFormDetecter defines
 * an operator() that accepts a class member pointer for the given class. If the
 * operator()(&Class::Method) call can be compiled, then the given class has a
 * method of the requested form. For any provided AdditionalArgsCount, the check
 * succeeds only if the given class has exactly one method of the requested form
 * with AdditionalArgsCount additional arguments.
 *
 * The tool is dedicated to be used in type functions (structs) generated by the
 * macro HAS_METHOD_FORM.
 *
 * @tparam MethodForm A template class member pointer type to a method of the
 *   form to look for.
 * @tparam Class A class in which a method of the requested form should be
 *   looked for.
 * @tparam AdditionalArgsCount A number of additional arguments.
 */
template<typename Class,
         template<typename...> class MethodForm,
         size_t AdditionalArgsCount>
struct MethodFormDetecter;

static const size_t MaxMFDAdditionalArgsCount = 7;

template<typename Class, template<typename...> class MethodForm>
struct MethodFormDetecter<Class, MethodForm, 0>
{
  void operator()(MethodForm<Class>);
};

template<typename Class, template<typename...> class MethodForm>
struct MethodFormDetecter<Class, MethodForm, 1>
{
  template<class T1>
  void operator()(MethodForm<Class, T1>);
};

template<typename Class, template<typename...> class MethodForm>
struct MethodFormDetecter<Class, MethodForm, 2>
{
  template<class T1, class T2>
  void operator()(MethodForm<Class, T1, T2>);
};

template<typename Class, template<typename...> class MethodForm>
struct MethodFormDetecter<Class, MethodForm, 3>
{
  template<class T1, class T2, class T3>
  void operator()(MethodForm<Class, T1, T2, T3>);
};

template<typename Class, template<typename...> class MethodForm>
struct MethodFormDetecter<Class, MethodForm, 4>
{
  template<class T1, class T2, class T3, class T4>
  void operator()(MethodForm<Class, T1, T2, T3, T4>);
};

template<typename Class, template<typename...> class MethodForm>
struct MethodFormDetecter<Class, MethodForm, 5>
{
  template<class T1, class T2, class T3, class T4, class T5>
  void operator()(MethodForm<Class, T1, T2, T3, T4, T5>);
};

template<typename Class, template<typename...> class MethodForm>
struct MethodFormDetecter<Class, MethodForm, 6>
{
  template<class T1, class T2, class T3, class T4, class T5, class T6>
  void operator()(MethodForm<Class, T1, T2, T3, T4, T5, T6>);
};

template<typename Class, template<typename...> class MethodForm>
struct MethodFormDetecter<Class, MethodForm, 7>
{
  template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  void operator()(MethodForm<Class, T1, T2, T3, T4, T5, T6, T7>);
};

/*
 * A type function that always produces true (if the template parameter can be
 * evaluated).
 */
template<typename>
struct True
{
  const static bool value = true;
};

/*
 * A type function that returns ResultType if T can be
 * evaluated (compiled).
 */
template<typename T, typename ResultType = void>
using EnableIfCompilable =
    typename std::enable_if<True<T>::value, ResultType>::type;

} // namespace sfinae
} // namespace mlpack

/*
 * Constructs a template supporting the SFINAE pattern.
 *
 * This macro generates a template struct that is useful for enabling/disabling
 * a method if the template class passed in contains a member function matching
 * a given signature with a specified name.
 *
 * The generated struct should be used in conjunction with std::enable_if_t.
 *
 * For general references, see:
 * http://stackoverflow.com/a/264088/391618
 *
 * For an mlpack specific use case, see /mlpack/core/util/prefixedoutstream.hpp
 * and /mlpack/core/util/prefixedoutstream_impl.hpp
 *
 * @param NAME the name of the struct to construct. For example: HasToString
 * @param FUNC the name of the function to check for. For example: ToString
 */
#define HAS_MEM_FUNC(FUNC, NAME)                                               \
template<typename T, typename sig>                                             \
struct NAME {                                                                  \
  typedef char yes[1];                                                         \
  typedef char no [2];                                                         \
  template<typename U, U> struct type_check;                                   \
  template<typename _1> static yes &chk(type_check<sig, &_1::FUNC> *);         \
  template<typename   > static no  &chk(...);                                  \
  static bool const value = sizeof(chk<T>(0)) == sizeof(yes);                  \
};

/*
 * HAS_METHOD_FORM generates a template that allows to check at compile time
 * whether a given class has a method of the requested form. For example, for
 * the following class
 *
 * class A
 * {
 *  public:
 *   ...
 *   Train(const arma::mat&, const arma::Row<size_t>&, double);
 *   ...
 * };
 *
 * and the following form of Train methods
 *
 * template<typename Class, typename...Ts>
 * using TrainForm =
 *     void(Class::*)(const arma::mat&, const arma::Row<size_t>&, Ts...);
 *
 * we can check whether the class A has a Train method of the specified form:
 *
 * HAS_METHOD_FORM(Train, HasTrain);
 * static_assert(HasTrain<TrainFrom, A>::value, "value should be true");
 *
 * The implementation is analogous to implementation of the macro HAS_MEM_FUNC.
 *
 * @param METHOD The name of the method to check for.
 * @param NAME The name of the struct to construct.
 */
#define HAS_METHOD_FORM(METHOD, NAME)                                          \
template<typename Class,                                                       \
         template<typename...> class MethodForm>                               \
struct NAME                                                                    \
{                                                                              \
  /* Making a short alias for MethodForm */                                    \
  template<typename C, typename...Args>                                        \
  using MF = MethodForm<C, Args...>;                                           \
                                                                               \
  /* Making short aliases for tools from mlpack::sfinae */                     \
  template<typename C, template<typename...> class MF, int N>                  \
  using MFD = mlpack::sfinae::MethodFormDetecter<C, MF, N>;                    \
  template<typename T, typename ResultType>                                    \
  using EnableIfComp = mlpack::sfinae::EnableIfCompilable<T, ResultType>;      \
  static const size_t MaxMFDAdditionalArgsCount =                              \
      mlpack::sfinae::MaxMFDAdditionalArgsCount;                               \
                                                                               \
  template<size_t N>                                                           \
  struct WithNAdditionalArgs                                                   \
  {                                                                            \
    using yes = char[1];                                                       \
    using no = char[2];                                                        \
                                                                               \
    template<typename C>                                                       \
    static EnableIfComp<decltype(MFD<C, MF, N>()(&C::METHOD)), yes&> chk(int); \
    template<typename>                                                         \
    static no& chk(...);                                                       \
                                                                               \
    static const bool value = sizeof(chk<Class>(0)) == sizeof(yes);            \
  };                                                                           \
                                                                               \
  template<size_t N>                                                           \
  struct WithGreaterOrEqualNumberOfAdditionalArgs                              \
  {                                                                            \
    using type = typename std::conditional<                                    \
        WithNAdditionalArgs<N>::value,                                         \
        std::true_type,                                                        \
        typename std::conditional<                                             \
            N < MaxMFDAdditionalArgsCount,                                     \
            WithGreaterOrEqualNumberOfAdditionalArgs<N + 1>,                   \
            std::false_type>::type>::type;                                     \
    static const bool value = type::value;                                     \
  };                                                                           \
                                                                               \
  static const bool value = WithGreaterOrEqualNumberOfAdditionalArgs<0>::value;\
};

#endif
