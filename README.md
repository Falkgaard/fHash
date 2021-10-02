# fHash

fHash is the stand-alone hashing sub-library/module of
the FALK ("Falk's Assorted Libraries, 'K?") super-library.

For now, it only provides fnv-1a hashing.

## Usage

The library provides the long-hand `falk::hash` namespace as well as
the shorthand `fhash` namespace (the two are functionally equivalent;
the latter will be used henceforth for brevity.)

### The functions provide include

----

`fhash::hash(X)` where `X` is a non-pointer primitive,
convertible to std::string\_view, or a user type that provides a
customization point specialiazation.

----

`fhash::hash_pack(X1, X2, ..., Xn)` where the parameters all satisfy
the same conditions as X above. They may be different types and
the order they're provided affects the resulting hash value.

----

`fhash::hash_type<T>()` which hashes the name of a type.
A pre-processor flag can be set to customize whether the resulting
value will be the same as a string containing the name of the type or not.

----

`fhash::hash_member( &T::member, T const &instance )` will hash the type `T`
and the member `member` of T instance `instance`. Mainly meant for PODs,
but will also work on public members and non-public members of friend classes.

----

In place of the Xs above it's also possible to use the following two wrappers:
`fhash::wrap_member(&T::member, instance)` and `fhash::wrap_type<T>` (mainly
meant for the `fhash::hash_pack(...)` function.)

----

And if one does either `using namespace falk::hash::literals;`,
`using namespace fhash::literals`, or `using namespace falk::literals;`
(note: the last one will import all literals from all include FALK headers);
thenone can use the user string literal `_h` to hash a string, e.g. `"text"_h`.
One case where this is particularly useful is when one wants to do a switch
statement on a string value:

```cpp
switch( fhash::hash(some_runtime_string) ) {
   case "CASE ONE"_h: /* do something */ break;   
   case "CASE TWO"_h: /* do something */ break;   
   // etc ...
   default: /* optional default case */
}
```

The hash functions also come with a "(name)_with_carry)" variant that takes a
specific initial hash value as the first parameter; this is to enable the
chaining that's used by many of the functions to produce sensible hashes for
sets of data or contextual data.

## TODO

Cover customization points.
